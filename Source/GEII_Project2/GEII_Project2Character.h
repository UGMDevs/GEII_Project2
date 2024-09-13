// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "TP_WeaponComponent.h"
#include "AC_Inventory.h"
#include "GEII_Project2Character.generated.h"


class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class AGEII_Project2Character : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

public:
	AGEII_Project2Character();

protected:
	virtual void BeginPlay();

public:

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface


public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

public:
	/** Property replication*/
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Color")
	void ChangeColor(FLinearColor NewColor);

	// New Code

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TArray<TSubclassOf<UTP_WeaponComponent>> WeaponsInventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	TArray<int32> WeaponsAmmo;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeaponComponent, VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	UTP_WeaponComponent* CurrentWeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	USkeletalMeshComponent* ThirdPersonCurrentWeapon;

	UFUNCTION()
	void OnRep_CurrentWeaponComponent();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Inventory")
	void AddWeapon(TSubclassOf<UTP_WeaponComponent> NewWeapon);

	UFUNCTION(BlueprintCallable, Category = "Weapon|Inventory")
	void AttachCurrentWeapon();

	// The function clients will call to switch weapons
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SwitchWeapon();

protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Weapon|Inventory");
	int32 CurrentIndex;

	// ---- Begin Change Character Color ---- //

protected:
	/** Character's current color */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CharacterColor, Category = "Color")
	FLinearColor CharacterColor;

	/** RepNotify for changes made to the character's color*/
	UFUNCTION()
	void OnRep_CharacterColor();

	/** Called when the character's color change */
	UFUNCTION(BlueprintImplementableEvent, Category = "Color")
	void OnCharacterColorChange(FLinearColor NewColor);

	/** Server function for changing color */
	UFUNCTION(Server, Reliable)
	void Server_ChangeColor(FLinearColor NewColor);

	// This function runs on the server
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_SelectWeapon();

    // Attach the weapon on both server and client
    void AttachServerWeapon();

	bool Server_SelectWeapon_Validate();

	// ---- End Change Character Color ---- //


	// ---- Begin Health System ---- //
protected:
	/** The player's maximum health. This is the highest value of their health can be.
	This value is a value of the player's health, which starts at when spawned.*/
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth;
	/** The player's current health. When reduced to 0, they are considered dead.*/
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
	float CurrentHealth;
	/** RepNotify for changes made to current health.*/
	UFUNCTION()
	void OnRep_CurrentHealth();

	/** Response to health being updated. Called on the server immediately after modification, and on clients in response to a RepNotify*/
	void OnHealthUpdate();

	public:
	/** Getter for Max Health.*/
	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	/** Getter for Current Health.*/
	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	/** Setter for Current Health. Clamps the value between 0 and MaxHealth and calls
	OnHealthUpdate. Should only be called on the server.*/
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetCurrentHealth(float healthValue);
	/** Event for taking damage. Overridden from APawn.*/
	UFUNCTION(BlueprintCallable, Category = "Health")
	float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// ---- End Health System ---- //
		
};