// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PortalProjectile.h"
#include "BlueProjectile.generated.h"

/**
 * 
 */
UCLASS()
class GEII_PROJECT2_API ABlueProjectile : public APortalProjectile
{
	GENERATED_BODY()
	
public:
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};
