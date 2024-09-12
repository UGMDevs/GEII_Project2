// Copyright Epic Games, Inc. All Rights Reserved.

#include "GEII_Project2Character.h"
#include "GEII_Project2Projectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "TP_WeaponComponent.h"
#include "Engine/World.h"
#include "UObject/UObjectGlobals.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "Engine/DamageEvents.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AGEII_Project2Character

AGEII_Project2Character::AGEII_Project2Character()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	bReplicates = true;

	// Initialize the player's Health
	MaxHealth = 100.0f;
	CurrentHealth =	MaxHealth;
	
	CurrentWeaponComponent = CreateDefaultSubobject<UTP_WeaponComponent>(TEXT("StartingWeaponComponent"));
	CurrentWeaponComponent->SetupAttachment(GetMesh1P());

}

void AGEII_Project2Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void AGEII_Project2Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGEII_Project2Character::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGEII_Project2Character::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AGEII_Project2Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AGEII_Project2Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGEII_Project2Character::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
	if(!bHasRifle)
	{
		if(CurrentWeaponComponent)
		{
			CurrentWeaponComponent->DestroyComponent();
		}
	}
}

bool AGEII_Project2Character::GetHasRifle()
{
	return bHasRifle;
}

// New Code

void AGEII_Project2Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate character color
	DOREPLIFETIME(AGEII_Project2Character, CharacterColor);
	// Replicate current health
	DOREPLIFETIME(AGEII_Project2Character, CurrentHealth);
}

void AGEII_Project2Character::ChangeColor(FLinearColor NewColor)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		// Change color if it's server
		CharacterColor = NewColor;
		OnRep_CharacterColor();
	}
	else
	{
		// Ask the server to change the color
		Server_ChangeColor(NewColor);
	}
}

void AGEII_Project2Character::OnRep_CharacterColor()
{
	OnCharacterColorChange(CharacterColor);
}

void AGEII_Project2Character::Server_ChangeColor_Implementation(FLinearColor NewColor)
{
	// Change Color with authority
	ChangeColor(NewColor);
}

void AGEII_Project2Character::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}

void AGEII_Project2Character::OnHealthUpdate()
{
	// Client-specific functionality
	if (IsLocallyControlled())
	{
		FString healthMessage = FString::Printf(TEXT("You now have %f health remaining."), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
		if (CurrentHealth <= 0)
		{
			FString deathMessage = FString::Printf(TEXT("You have been killed."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);
		}
	}
	//Server-specific functionality
	if (GetLocalRole() == ROLE_Authority)
	{
		FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	}
	//Functions that occur on all machines.
	/*
	Any special functionality that should occur as a result of damage or death should
	be placed here.
	*/
}

void AGEII_Project2Character::SetCurrentHealth(float healthValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(healthValue, 0.f, MaxHealth);
		OnHealthUpdate();
	}
}

float AGEII_Project2Character::TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = (FPointDamageEvent*)&DamageEvent;

		// Get the hit location from the damage event
		FVector HitLocation = PointDamageEvent->HitInfo.ImpactPoint;

		// Check if we hit the head (using a tag or bone name)
		if (PointDamageEvent->HitInfo.BoneName == "head")
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("BOOOOM!!! Headshot!"));
			float randomHigherDamage = FMath::FRandRange(DamageTaken*2, MaxHealth);
			float damageApplied = CurrentHealth - randomHigherDamage;
			SetCurrentHealth(damageApplied);
		}
		else 
		{
			float damageApplied = CurrentHealth - DamageTaken;
			SetCurrentHealth(damageApplied);
			return damageApplied;
		}
	}
	else 
	{
		float damageApplied = CurrentHealth - DamageTaken;
		SetCurrentHealth(damageApplied);
		return damageApplied;
	}
	return CurrentHealth;
}

