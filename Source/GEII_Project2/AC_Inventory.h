// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TP_WeaponComponent.h"
#include "AC_Inventory.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GEII_PROJECT2_API UAC_Inventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAC_Inventory();

	
	void AddWeapon(TSubclassOf<UTP_WeaponComponent> WeaponClass);

	void SwitchWeapon(int32 WeaponIndex);

	void DropCurrentWeapon();

	void SetStartingWeapon(UTP_WeaponComponent* StartingWeapon);

	// Get the list of weapons in the inventory
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    const TArray<TSubclassOf<UTP_WeaponComponent>>& GetWeapons() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	


private:

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TArray<TSubclassOf<UTP_WeaponComponent>> Weapons;

	UPROPERTY(VisibleAnywhere)
	UTP_WeaponComponent* CurrentWeapon;
	
	int32 CurrentWeaponIndex;
		
};
