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


private:
    // Handle for the respawn timer
    FTimerHandle RespawnTimerHandle;

    // Time in seconds before the pickup respawns
    UPROPERTY(EditDefaultsOnly, Category = "Pickup")
    float RespawnTime = 5.0f;

    // Function to reactivate the pickup
    void ReactivatePickup();

    // Function to initialize the pickup's initial state
    void InitializePickup();

    // Function to hide the pickup (runs on the server)
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_HidePickup();

    // Function to reactivate the pickup (runs on the server)
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_ReactivatePickup();

    // Implementation of the server functions
    void Server_HidePickup_Implementation();
    bool Server_HidePickup_Validate();

    void Server_ReactivatePickup_Implementation();
    bool Server_ReactivatePickup_Validate();
	


public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	UTP_WeaponComponent* Weapon;

	UPROPERTY(BlueprintReadOnly, Category = "Pickup")
	UTP_PickUpComponent* PickUp;

	// Function to be called when the pickup is collected or destroyed
	UFUNCTION(BlueprintCallable, Category = "Respawn")
	void OnPickupCollected();
};
