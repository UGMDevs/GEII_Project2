// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TP_WeaponComponent.h"
#include "TP_AssaultRifle.generated.h"


UCLASS()
class GEII_PROJECT2_API UTP_AssaultRifle : public UTP_WeaponComponent
{
	GENERATED_BODY()
	
public:
	UTP_AssaultRifle();

	virtual void HandleFire() override;

protected:
	UPROPERTY(EditAnywhere, Category = "Effects")
	class UParticleSystem* ExplosionEffect;
	//The damage type and damage that will be done by this projectile
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<class UDamageType> DamageType;
	//The damage dealt by this projectile.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponFire")
	int32 NumberOfFiringBullets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponFire")
	float SpreadAngle;

};
