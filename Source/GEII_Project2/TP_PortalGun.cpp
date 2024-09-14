// Fill out your copyright notice in the Description page of Project Settings.


#include "TP_PortalGun.h"
#include "GEII_Project2Character.h"
#include "Portal.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"
#include "PortalProjectile.h"

// Define custom trace channels
#define ECC_PortalTraceChannel ECC_GameTraceChannel2

// Sets default values for this component's properties
UTP_PortalGun::UTP_PortalGun()
{
	ObjectTypes.Empty();
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PortalTraceChannel));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

	bLastTraceHitPortalWall = false;
}

void UTP_PortalGun::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundPortals;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APortal::StaticClass(), FoundPortals);

	for (AActor* Actor : FoundPortals)
	{
		APortal* Portal = Cast<APortal>(Actor);
		if (Portal)
		{
			// Check if this is the Blue Portal
			if (Portal->ActorHasTag(TEXT("BluePortal")))
			{
				SpawnedBluePortal = Portal;
			}
			// Check if this is the Orange Portal
			else if (Portal->ActorHasTag(TEXT("OrangePortal")))
			{
				SpawnedOrangePortal = Portal;
			}
		}
	}

}

void UTP_PortalGun::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Perform line trace on tick
	PerformLineTrace();
}

void UTP_PortalGun::SetupActionBindings(AGEII_Project2Character* TargetCharacter)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(TargetCharacter->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 0);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			UE_LOG(LogTemp, Warning, TEXT("Binding inputs for %s"), *TargetCharacter->GetName());

			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_PortalGun::FireBlueProjectile);

			// Reload
			EnhancedInputComponent->BindAction(SecondFireAction, ETriggerEvent::Triggered, this, &UTP_PortalGun::FireOrangeProjectile);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Input component not found for %s"), *TargetCharacter->GetName());
		}
	}
}

void UTP_PortalGun::FireBlueProjectile()
{
	FirePortal(BlueProjectile);
}

void UTP_PortalGun::FireOrangeProjectile()
{
	FirePortal(OrangeProjectile);
}

void UTP_PortalGun::FirePortal(TSubclassOf<class APortalProjectile> PortalProjectile)
{
	if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		if (!bIsFiringWeapon)
		{
			bIsFiringWeapon = true;
			UWorld* World = GetWorld();
			World->GetTimerManager().SetTimer(FiringTimer, this, &UTP_WeaponComponent::StopFire, FireRate, false);

			if (Character == nullptr || Character->GetController() == nullptr)
			{
				return;
			}

			// Check if the last trace hit a portal wall
			if (!bLastTraceHitPortalWall)
			{
				return; // Don't fire if the last trace didn't hit a portal wall
			}

			// Try and fire a projectile
			if (PortalProjectile != nullptr)
			{
				if (World != nullptr)
				{
					APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
					const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
					// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
					const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

					//Set Spawn Collision Handling Override
					FActorSpawnParameters ActorSpawnParams;
					ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

					// Spawn Projectile
					APortalProjectile* ShotProjectile = World->SpawnActor<APortalProjectile>(PortalProjectile, SpawnLocation, SpawnRotation, ActorSpawnParams);
					ShotProjectile->SetWeaponThatShot(this);
				}
			}

			// Try and play the sound if specified
			if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
			}

			// Try and play a firing animation if specified
			if (FireAnimation != nullptr)
			{
				// Get the animation object for the arms mesh
				UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
				if (AnimInstance != nullptr)
				{
					AnimInstance->Montage_Play(FireAnimation, 1.f);
				}
			}
		}
	}
	else
	{
		Server_FirePortal(PortalProjectile);
	}
}

void UTP_PortalGun::Server_FirePortal_Implementation(TSubclassOf<class APortalProjectile> PortalProjectile)
{
	FirePortal(PortalProjectile);
}

bool UTP_PortalGun::PerformLineTrace()
{
	if (Character == nullptr)
	{
		return false;
	}

	// Get player controller
	APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	if (PlayerController == nullptr)
	{
		return false;
	}

	// Get camera location and rotation
	FVector CameraLocation;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	// Calculate end location for line trace
	FVector EndLocation = CameraLocation + (CameraRotation.Vector() * 4000.0f);

	// Perform line trace using custom trace channel 
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Character);
	bool bHit = GetWorld()->LineTraceSingleByObjectType(HitResult, CameraLocation, EndLocation, FCollisionObjectQueryParams(ObjectTypes), CollisionParams);

	// Update the member variable based on the result
	bLastTraceHitPortalWall = bHit && HitResult.GetActor() && HitResult.GetActor()->GetRootComponent()->GetCollisionObjectType() == ECC_PortalTraceChannel;

	if (bLastTraceHitPortalWall)
	{
		LastTraceHit = HitResult;
	}
	return bLastTraceHitPortalWall;
}

void UTP_PortalGun::SpawnPortal(TSubclassOf<class APortal> PortalToSpawn)
{
	UWorld* const World = GetWorld();
	if (World)
	{
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FVector LocationToSpawn = LastTraceHit.Location + LastTraceHit.Normal;
		if (PortalToSpawn == BluePortal)
		{
			SpawnedBluePortal = World->SpawnActor<APortal>(PortalToSpawn, FVector(0, 0, 0), FRotator(0, 0, 0), ActorSpawnParams);
			SpawnedBluePortal->SetCurrentWall(LastTraceHit.GetActor());
			SpawnedBluePortal->PlacePortal(LocationToSpawn, UKismetMathLibrary::MakeRotFromX(LastTraceHit.Normal));
			if (SpawnedOrangePortal->IsValidLowLevel())
			{
				SpawnedBluePortal->SetPortalToLink(SpawnedOrangePortal);
				SpawnedOrangePortal->SetPortalToLink(SpawnedBluePortal);
				SpawnedBluePortal->SetupLinkedPortal();
				SpawnedOrangePortal->SetupLinkedPortal();
			}
		}
		else if (PortalToSpawn == OrangePortal)
		{
			SpawnedOrangePortal = World->SpawnActor<APortal>(PortalToSpawn, LocationToSpawn, UKismetMathLibrary::MakeRotFromX(LastTraceHit.Normal), ActorSpawnParams);
			SpawnedOrangePortal->SetCurrentWall(LastTraceHit.GetActor());
			SpawnedOrangePortal->PlacePortal(LocationToSpawn, UKismetMathLibrary::MakeRotFromX(LastTraceHit.Normal));
			if (SpawnedBluePortal->IsValidLowLevel())
			{
				SpawnedOrangePortal->SetPortalToLink(SpawnedBluePortal);
				SpawnedBluePortal->SetPortalToLink(SpawnedOrangePortal);
				SpawnedOrangePortal->SetupLinkedPortal();
				SpawnedBluePortal->SetupLinkedPortal();
			}
		}
	}
}

void UTP_PortalGun::ChangePortalLocation(APortal* PortalToChangeLocation, FVector NewLocation, FRotator NewRotation)
{
	PortalToChangeLocation->PlacePortal(NewLocation, NewRotation);
}

void UTP_PortalGun::SpawnBluePortal()
{
	if (SpawnedBluePortal == nullptr)
	{
		SpawnPortal(BluePortal);
	}
	else
	{
		SpawnedBluePortal->SetCurrentWall(LastTraceHit.GetActor());
		ChangePortalLocation(SpawnedBluePortal, LastTraceHit.Location, UKismetMathLibrary::MakeRotFromX(LastTraceHit.Normal));
	}
}

void UTP_PortalGun::SpawnOrangePortal()
{
	if (SpawnedOrangePortal == nullptr)
	{
		SpawnPortal(OrangePortal);
	}
	else
	{
		SpawnedOrangePortal->SetCurrentWall(LastTraceHit.GetActor());
		ChangePortalLocation(SpawnedOrangePortal, LastTraceHit.Location, UKismetMathLibrary::MakeRotFromX(LastTraceHit.Normal));
	}
}

