// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TP_WeaponComponent.h"
#include "TP_PickUpComponent.h"
#include "PickUpWeapon.generated.h"

UCLASS()
class GEII_PROJECT2_API APickUpWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	APickUpWeapon();

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	UTP_WeaponComponent* Weapon;

	UPROPERTY(BlueprintReadOnly, Category = "Pickup")
	UTP_PickUpComponent* PickUp;
};
