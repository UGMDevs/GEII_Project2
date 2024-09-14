// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GEII_Project2PlayerController.generated.h"

class UInputMappingContext;

/**
 *
 */
UCLASS()
class GEII_PROJECT2_API AGEII_Project2PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AGEII_Project2PlayerController();

protected:

	/** Input Mapping Context to be used for player input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;

protected:

	virtual void BeginPlay() override;

protected:
	/** Server function for spawning the character */
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_SpawnPlayer();

	FTimerHandle RespawnTimer;

	UPROPERTY(EditDefaultsOnly, Category = "Respawn")
	float RespawnAfterDeath;

public:
	UFUNCTION(BlueprintCallable)
	void RespawnPlayer();
};
