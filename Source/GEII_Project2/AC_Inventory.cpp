// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_Inventory.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UAC_Inventory::UAC_Inventory()
{
	
	CurrentWeaponIndex = -1;
	
}


// Called when the game starts
void UAC_Inventory::BeginPlay()
{
	Super::BeginPlay();

	// Initialize the inventory with a starting weapon
	if (Weapons.Num() > 0)
	{
		SwitchWeapon(0);
	}
	
}

void UAC_Inventory::AddWeapon(TSubclassOf<UTP_WeaponComponent> WeaponClass)
{
	Weapons.Add(WeaponClass);
}

void UAC_Inventory::SwitchWeapon(int32 WeaponIndex)
{
	if (WeaponIndex >= 0 && WeaponIndex < Weapons.Num())
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->DestroyComponent();
		}

		CurrentWeaponIndex = WeaponIndex;
		UWorld* World = GetWorld();
		if (World)
		{
			// Cast the owner to ACharacter
			ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
			if (CharacterOwner)
			{
				CurrentWeapon = NewObject<UTP_WeaponComponent>(GetOwner(), Weapons[WeaponIndex]);
				CurrentWeapon->RegisterComponent();
				CurrentWeapon->AttachToComponent(CharacterOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("GripPoint")));
			}
		}
	}
}

void UAC_Inventory::DropCurrentWeapon()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->DestroyComponent();
		CurrentWeapon = nullptr;
		CurrentWeaponIndex = -1;
	}
}

void UAC_Inventory::SetStartingWeapon(UTP_WeaponComponent* StartingWeapon)
{
	if (StartingWeapon)
	{
		AddWeapon(StartingWeapon->GetClass()); // Add the weapon class to inventory

		SwitchWeapon(0); // Set it as the current weapon
	}
}

const TArray<TSubclassOf<UTP_WeaponComponent>>& UAC_Inventory::GetWeapons() const
{
	return Weapons;
}


