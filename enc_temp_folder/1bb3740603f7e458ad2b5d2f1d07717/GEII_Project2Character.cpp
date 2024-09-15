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
#include "Blueprint/UserWidget.h"
#include "GEII_Project2GameMode.h"
#include "GEII_Project2PlayerController.h"


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
	CurrentWeaponComponent->bOnlyOwnerSee = true;
	CurrentWeaponComponent->bOwnerNoSee = false;

	ThirdPersonCurrentWeapon = CreateDefaultSubobject<USkeletalMeshComponent>("ThirdPersonWeapon");
	ThirdPersonCurrentWeapon->SetupAttachment(GetMesh());
	ThirdPersonCurrentWeapon->SetIsReplicated(true);
	ThirdPersonCurrentWeapon->bOnlyOwnerSee = false;
	ThirdPersonCurrentWeapon->bOwnerNoSee = true;
	ThirdPersonCurrentWeapon->SetRelativeLocation(FVector( -6.f, 3.3f, -1.f));
	ThirdPersonCurrentWeapon->SetRelativeRotation(FRotator(9.f, 160.f, -11.f));
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	ThirdPersonCurrentWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("middle_01_r")));
}

void AGEII_Project2Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Check if we have a valid widget class
	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);

		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}



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

		// Bind SwitchWeapon input
		EnhancedInputComponent->BindAction(SwitchWeaponNextAction, ETriggerEvent::Started, this, &AGEII_Project2Character::SwitchWeaponNext);
		EnhancedInputComponent->BindAction(SwitchWeaponPreviousAction, ETriggerEvent::Started, this, &AGEII_Project2Character::SwitchWeaponPrevious);
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
	// Replicate current weapon
	DOREPLIFETIME(AGEII_Project2Character, CurrentWeaponComponent);
	// Replicate has Rifle
	DOREPLIFETIME(AGEII_Project2Character, bHasRifle);
	// Replicate the index
	DOREPLIFETIME(AGEII_Project2Character, CurrentIndex);
}

/////////////////////////////// ATTACHING AND SELECTING WEAPON LOGIC /////////////////////////////////////////////////

void AGEII_Project2Character::AddWeapon(TSubclassOf<UTP_WeaponComponent> NewWeapon)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		SetHasRifle(false);
		CurrentWeaponComponent = NewObject<UTP_WeaponComponent>(this, NewWeapon);
		CurrentIndex = WeaponsInventory.AddUnique(CurrentWeaponComponent->GetClass());
		EquipWeaponFromPickup(NewWeapon);
		OnRep_CurrentWeaponComponent();
	}
}

void AGEII_Project2Character::AttachCurrentWeapon()
{
	if (CurrentWeaponComponent)
	{
		// Server-side logic
		if (GetLocalRole() == ROLE_Authority)
		{
			
			CurrentWeaponComponent->SetOnlyOwnerSee(true);
			CurrentWeaponComponent->SetOwnerNoSee(false);
			
			ThirdPersonCurrentWeapon->SetSkeletalMesh(CurrentWeaponComponent->GetSkeletalMeshAsset());
			ThirdPersonCurrentWeapon->SetOnlyOwnerSee(false);
			ThirdPersonCurrentWeapon->SetOwnerNoSee(true);
		}
		// Client-side logic
		else
		{
			
			CurrentWeaponComponent->SetOnlyOwnerSee(true);
			CurrentWeaponComponent->SetOwnerNoSee(false);

			ThirdPersonCurrentWeapon->SetSkeletalMesh(CurrentWeaponComponent->GetSkeletalMeshAsset());
			ThirdPersonCurrentWeapon->SetOnlyOwnerSee(false);
			ThirdPersonCurrentWeapon->SetOwnerNoSee(true);
		}
	}
}

/// SWITCH WEAPON LOGIC ////

void AGEII_Project2Character::SwitchWeapon(bool bNext)
{
	if (HasAuthority())
	{
		// Server logic
		if (bNext)
		{
			SetHasRifle(false);
			Server_SelectWeapon();
		}
		else
		{
			SetHasRifle(false);
			Server_SelectPreviousWeapon();
		}
	}
	else
	{
		// Client logic
		if (bNext)
		{
			SetHasRifle(false);
			Server_SelectWeapon();
		}
		else
		{
			SetHasRifle(false);
			Server_SelectPreviousWeapon();
		}
	}
}

void AGEII_Project2Character::SwitchWeaponNext()
{
	UE_LOG(LogTemp, Log, TEXT("Switching to next weapon."));
	SwitchWeapon(true);
}

void AGEII_Project2Character::SwitchWeaponPrevious()
{
	UE_LOG(LogTemp, Log, TEXT("Switching to previous weapon."));
	SwitchWeapon(false);
}

void AGEII_Project2Character::Server_SelectPreviousWeapon_Implementation()
{
	// Ensure the server logic for weapon switching
	int32 InventoryLength = WeaponsInventory.Num();

	if (InventoryLength > 0)
	{
		// Cycle through the inventory backward
		if (CurrentIndex <= 0)
		{
			CurrentIndex = InventoryLength - 1; // Go to the last weapon if we are at the beginning
		}
		else
		{
			CurrentIndex = FMath::Clamp(CurrentIndex - 1, 0, InventoryLength - 1);
		}

		// Fetch the weapon class from the inventory
		TSubclassOf<UTP_WeaponComponent> WeaponClass = WeaponsInventory[CurrentIndex];

		if (WeaponClass != nullptr)
		{
			// Create the new weapon component
			UTP_WeaponComponent* NewWeaponComponent = NewObject<UTP_WeaponComponent>(this, WeaponClass);
			if (NewWeaponComponent)
			{
				// Register and replicate the new weapon component
				NewWeaponComponent->RegisterComponent();

				// Destroy the old weapon component
				if (CurrentWeaponComponent)
				{
					CurrentWeaponComponent->DestroyComponent();
				}

				// Set the new weapon component and attach it
				CurrentWeaponComponent = NewWeaponComponent;
				CurrentWeaponComponent->AttachWeapon(this);

				// Replicate the new weapon component
				OnRep_CurrentWeaponComponent();
			}
		}
	}
}



void AGEII_Project2Character::AttachServerWeapon()
{
	if (CurrentWeaponComponent)
	{
		// Attach the weapon using its own method, which will handle both logic and animation switching
		CurrentWeaponComponent->AttachWeapon(this);
	}
}

void AGEII_Project2Character::Server_SelectWeapon_Implementation()
{
	// Inventory Length
	int32 InventoryLength = WeaponsInventory.Num();

	if (InventoryLength > 0)
	{
		// Cycle through the inventory
		if (CurrentIndex >= InventoryLength - 1)
		{
			CurrentIndex = 0; // Goes back to the first weapon if we are at the end
		}
		else
		{
			CurrentIndex = FMath::Clamp(CurrentIndex + 1, 0, InventoryLength - 1);
		}

		// Fetches the weapon class from the inventory
		TSubclassOf<UTP_WeaponComponent> WeaponClass = WeaponsInventory[CurrentIndex];

		if (WeaponClass != nullptr)
		{
			// Creates a new weapon component with the pickup class
			UTP_WeaponComponent* NewWeaponComponent = NewObject<UTP_WeaponComponent>(this, WeaponClass);
			if (NewWeaponComponent)
			{
				// Registers and replicates the new weapon component
				NewWeaponComponent->RegisterComponent();

				if (CurrentWeaponComponent)
				{
					CurrentWeaponComponent->DestroyComponent();
				}

				// Setting of the new weapon component and attaching it
				CurrentWeaponComponent = NewWeaponComponent;
				CurrentWeaponComponent->AttachWeapon(this);

				// Replicate the new weapon component
				OnRep_CurrentWeaponComponent();
				
			}
		}
	}
}

void AGEII_Project2Character::OnRep_CurrentWeaponComponent()
{
	if (CurrentWeaponComponent)
	{
		AttachCurrentWeapon();
		CurrentWeaponComponent->AttachWeapon(this);
	}
}



bool AGEII_Project2Character::Server_SelectWeapon_Validate()
{
	return true;
}

bool AGEII_Project2Character::Server_SelectPreviousWeapon_Validate()
{
	return true;
}


void AGEII_Project2Character::EquipWeaponFromPickup(TSubclassOf<UTP_WeaponComponent> NewWeaponClass)
{
	if (HasAuthority())
	{
		// Server logic
		EquipWeaponServer(NewWeaponClass);
	}
	else
	{
		// Client logic
		// Call server function to equip the weapon
		Server_EquipWeapon(NewWeaponClass);
	}
}

void AGEII_Project2Character::Server_EquipWeapon_Implementation(TSubclassOf<UTP_WeaponComponent> NewWeaponClass)
{
	EquipWeaponServer(NewWeaponClass);
	OnRep_CurrentWeaponComponent();
}




void AGEII_Project2Character::EquipWeaponServer_Implementation(TSubclassOf<UTP_WeaponComponent> NewWeaponClass)
{
	if (NewWeaponClass)
	{
		// Set the current weapon class from pickup
		SetHasRifle(false); // Reset the previous weapon

		// Create and attach the new weapon component
		UTP_WeaponComponent* NewWeaponComponent = NewObject<UTP_WeaponComponent>(this, NewWeaponClass);
		if (NewWeaponComponent)
		{
			NewWeaponComponent->RegisterComponent();
			NewWeaponComponent->AttachWeapon(this); // Custom function to attach the weapon to the character

			// Clean up the old weapon component if it exists
			if (CurrentWeaponComponent)
			{
				CurrentWeaponComponent->DestroyComponent();
			}

			// Update the current weapon component
			CurrentWeaponComponent = NewWeaponComponent;
			SetHasRifle(true);

			// Notify clients of the weapon change
			OnRep_CurrentWeaponComponent();
		}
	}
}





/////////////////////////////// END SWITCH WEAPON LOGIC //////////////////////////////////////////



/////////////////////////////// CHARACTER COLOR + HEALTH CHANGE //////////////////////////////////////////

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
		if (CurrentHealth <= 0)
		{
			AGEII_Project2GameMode* GameMode = Cast<AGEII_Project2GameMode>(GetWorld()->GetAuthGameMode());
			GameMode->AddPlayerDeath(Cast<APlayerController>(GetController()));
			AddKillToDamageCauser();
		}
	}
	//Server-specific functionality
	if (GetLocalRole() == ROLE_Authority)
	{
		if(CurrentHealth <= 0)
		{
			AGEII_Project2PlayerController* PlayerController = Cast<AGEII_Project2PlayerController>(GetController());
			PlayerController->RespawnPlayer();
			Destroy();
		}
	}
	//Functions that occur on all machines.
	/*
	Any special functionality that should occur as a result of damage or death should be placed here.
	*/
}

void AGEII_Project2Character::AddKillToDamageCauser()
{
	if (LastDamageCauser)
	{
		AGEII_Project2GameMode* GameMode = Cast<AGEII_Project2GameMode>(GetWorld()->GetAuthGameMode());
		GameMode->AddPlayerKill(Cast<APlayerController>(LastDamageCauser->GetController()));
	}
}

void AGEII_Project2Character::SetCurrentHealth(float healthValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(healthValue, 0.f, MaxHealth);
		OnHealthUpdate();
	}
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

float AGEII_Project2Character::TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	LastDamageCauser = Cast<AGEII_Project2Character>(DamageCauser);
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

/////////////////////////////// END CHARACTER COLOR + HEALTH CHANGE //////////////////////////////////////////