// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "TP_WeaponComponent.generated.h"

class AGEII_Project2Character;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GEII_PROJECT2_API UTP_WeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AGEII_Project2Projectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* NoAmmoSound;
	
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector MuzzleOffset;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* FireMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Reload;

	/** Sets default values for this component's properties */
	UTP_WeaponComponent();

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void AttachWeapon(AGEII_Project2Character* TargetCharacter);

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category="Weapon|WeaponFire")
	void Fire();

	UFUNCTION(Server, Reliable)
	void Server_Fire();

	UFUNCTION()
	void VerifyAmmo();

	UFUNCTION(Server, Reliable)
	void Server_Reload();

	/** Function to reload the weapon */
	UFUNCTION()
	virtual void ReloadWeapon();

	/** Function increment bullets */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void IncrementAmmo();

	/** Function for ending weapon fire. Once this is called, the player can use
	StartFire again.*/
	UFUNCTION(BlueprintCallable, Category = "Weapon|WeaponFire")
	void StopFire();

	UFUNCTION(Server, Reliable)
	virtual void HandleFire();

protected:
	/** Delay between shots in seconds. Used to control fire rate for your test
	projectile, but also to prevent an overflow of server functions from binding
	SpawnProjectile directly to input.*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|WeaponFire")
	float FireRate;

	/** If true, you are in the process of firing projectiles. */
	bool bIsFiringWeapon;

	/** A timer handle used for providing the fire rate delay in-between spawns.*/
	FTimerHandle FiringTimer;

public:
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon|Ammo")
	int MaxTotalAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon|Ammo")
	int MaxClipAmmo;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Weapon|Ammo")
	int TotalAmmo;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Weapon|Ammo")
	int ClipAmmo;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Weapon|Ammo")
	int BulletsToIncrement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon|Ammo")
	float ReloadTime;


protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** The Character holding this weapon*/
	AGEII_Project2Character* Character;
};
