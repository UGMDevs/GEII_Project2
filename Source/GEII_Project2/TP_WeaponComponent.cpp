// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "GEII_Project2Character.h"
#include "GEII_Project2Projectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "Net/UnrealNetwork.h"
#include "EnhancedInputSubsystems.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{

	SetIsReplicatedByDefault(true);

	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);

	// Initialize fire rate
	FireRate = 0.25f;
	bIsFiringWeapon = false;

	MaxTotalAmmo = 64;
	MaxClipAmmo = 12;
	TotalAmmo = 64;
	ClipAmmo = 12;
	ReloadTime = 1.0f;
	BulletsToIncrement = 30;
}

void UTP_WeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTP_WeaponComponent, ClipAmmo);
	DOREPLIFETIME(UTP_WeaponComponent, TotalAmmo);
}


void UTP_WeaponComponent::Fire()
{
	if(GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		if (!bIsFiringWeapon)
		{
			bIsFiringWeapon = true;
			UWorld* World = GetWorld();
			World->GetTimerManager().SetTimer(FiringTimer, this, &UTP_WeaponComponent::StopFire, FireRate, false);
			VerifyAmmo();
		}
	}
	else
	{
		Server_Fire();
	}
}

void UTP_WeaponComponent::Server_Fire_Implementation()
{
	Fire();
}

void UTP_WeaponComponent::VerifyAmmo()
{
	if (ClipAmmo > 0)
	{
		HandleFire();
		ClipAmmo--;
	}
	else
	{
		// Try and play the sound if specified
		if (NoAmmoSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, NoAmmoSound, Character->GetActorLocation());
		}
	}
}



void UTP_WeaponComponent::StopFire()
{
	bIsFiringWeapon = false;
}

void UTP_WeaponComponent::ReloadWeapon()
{
	if (Character == nullptr)
	{
		return;
	}

	if (Character->GetLocalRole() == ROLE_Authority)
	{
		// Only handle reload logic on the server
		if (ClipAmmo != MaxClipAmmo)
		{
			if (TotalAmmo - (MaxClipAmmo - ClipAmmo) >= 0)
			{
				TotalAmmo -= (MaxClipAmmo - ClipAmmo);
				ClipAmmo = MaxClipAmmo;
			}
			else
			{
				ClipAmmo += TotalAmmo;
				TotalAmmo = 0;
			}
		}
	}
	else
	{
		Server_Reload(); // Request server to handle reloading
	}
}

void UTP_WeaponComponent::IncrementAmmo()
{
	// Calculate the new potential total ammo
	int newTotalAmmo = TotalAmmo + BulletsToIncrement;

	// Check if adding the new bullets would exceed the MaxTotalAmmo
	if (newTotalAmmo > MaxTotalAmmo)
	{
		// If it exceeds, cap it at MaxTotalAmmo
		TotalAmmo = MaxTotalAmmo;
	}
	else
	{
		// Add the full bullets
		TotalAmmo = newTotalAmmo;
	}
}

void UTP_WeaponComponent::Server_Reload_Implementation()
{
	ReloadWeapon();
}

void UTP_WeaponComponent::HandleFire_Implementation()
{
	// Try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			ActorSpawnParams.Owner = Character;
			ActorSpawnParams.Instigator = Character;

			// Spawn the projectile at the muzzle
			UE_LOG(LogTemp, Warning, TEXT("Spawning Projectile"));
			World->SpawnActor<AGEII_Project2Projectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}

	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}

	if (Character->IsLocallyControlled())
	{
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

void UTP_WeaponComponent::AttachWeapon(AGEII_Project2Character* TargetCharacter)
{
	Character = TargetCharacter;

	// Check that the character is valid, and has no rifle yet
	if (Character == nullptr || Character->GetHasRifle())
	{
		return;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	this->SetupAttachment(Character->GetMesh1P());
	
	// switch bHasRifle so the animation blueprint can switch to another animation set
	Character->SetHasRifle(true);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 0);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			UE_LOG(LogTemp, Warning, TEXT("Binding inputs for %s"), *Character->GetName());

			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);

			// Reload
			EnhancedInputComponent->BindAction(Reload, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::ReloadWeapon);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Input component not found for %s"), *Character->GetName());
		}
	}
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}