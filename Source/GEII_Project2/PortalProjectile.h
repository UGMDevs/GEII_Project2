// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GEII_Project2Projectile.h"
#include "TP_PortalGun.h"
#include "PortalProjectile.generated.h"

/**
 * 
 */
UCLASS()
class GEII_PROJECT2_API APortalProjectile : public AGEII_Project2Projectile
{
	GENERATED_BODY()

public:
	APortalProjectile();

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponThatShot(UTP_PortalGun* Weapon);

protected:
	UTP_PortalGun* WeaponThatShotMe;
};
