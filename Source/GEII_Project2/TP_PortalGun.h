// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TP_WeaponComponent.h"
#include "GEII_Project2GameMode.h"
#include "TP_PortalGun.generated.h"

/**
 * 
 */
UCLASS()
class GEII_PROJECT2_API UTP_PortalGun : public UTP_WeaponComponent
{
	GENERATED_BODY()
	
public:
	UTP_PortalGun();

	virtual void BeginPlay() override;

	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

	virtual void SetupActionBindings(AGEII_Project2Character* TargetCharacter) override;

	/** Make the weapon Fire Blue Projectile */
	UFUNCTION(BlueprintCallable, Category="PortalGun|Projectile")
	void FireBlueProjectile();

	/** Make the weapon Fire Orange Projectile */
	UFUNCTION(BlueprintCallable, Category="PortalGun|Projectile")
	void FireOrangeProjectile();

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void FirePortal(TSubclassOf<class APortalProjectile> PortalProjectile);

	UFUNCTION(Server, Reliable)
	void Server_FirePortal(TSubclassOf<class APortalProjectile> PortalProjectile);

	/** Spawn blue portal*/
	UFUNCTION(BlueprintCallable, Category = "PortalGun|Portal")
	void SpawnBluePortal();

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "PortalGun|Portal")
	void Server_SpawnBluePortal();

	UFUNCTION(BlueprintCallable, Category = "PortalGun|Portal")
	void SpawnOrangePortal();

	/** Spawn orange portal*/
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "PortalGun|Portal")
	void Server_SpawnOrangePortal();

protected:
	/** Perform a LineTrace */
	UFUNCTION(BlueprintCallable, Category="PortalGun|LineTrace")
	bool PerformLineTrace();

	/** Returns the Object that is currently being hit **/
	UPROPERTY(BlueprintReadWrite, Category = "PortalGun|LineTrace")
	bool bLastTraceHitPortalWall;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	/** BlueProjectile To Spawn */
	UPROPERTY(EditDefaultsOnly, Category = "PortalGun|Projectile")
	TSubclassOf<class APortalProjectile> BlueProjectile;

	/** OrangeProjectile To spawn */
	UPROPERTY(EditDefaultsOnly, Category = "PortalGun|Projectile")
	TSubclassOf<class APortalProjectile> OrangeProjectile;

	/** Right Click Input Action **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* SecondFireAction;

	FHitResult LastTraceHit;
	
	AGEII_Project2GameMode* GameMode;

	AGEII_Project2Character* OwnerCharacter;
};
