// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GEII_Project2PlayerController.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "GEII_Project2GameMode.generated.h"

UCLASS(minimalapi)
class AGEII_Project2GameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AGEII_Project2GameMode();

// New Code

protected:
	/** All player controllers conrrently connected*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<APlayerController*> AllPlayerControllers;

	/** All player starts in the world*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly);
	TArray<AActor*> PlayerStarts;

	/** Player starts where a character has already spawned in*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly);
	TArray<AActor*> AllSelectedPlayerStarts;

	/** Player colors for each player index*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FLinearColor> PlayerColorOptions = {
			FLinearColor::White,
			FLinearColor::Red,
			FLinearColor::Blue,
			FLinearColor::Green
	};

	/** Player character to be spawned for the player controllers*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AGEII_Project2Character> PlayerCharacterClass;

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void SwapPlayerControllers(APlayerController* OldPC, APlayerController* NewPC) override;
	virtual void Logout(AController* Exiting) override;
	
	/** Find a random player start in the world*/
	UFUNCTION(BlueprintPure, Category = "Player Spawning")
	FTransform FindRandomPlayerStart();

	/** Change the color of a character related to a player controller*/
	UFUNCTION(BlueprintCallable, Category = "Player Spawning")
	void SetPlayerColor(APlayerController* PlayerController, AGEII_Project2Character* PlayerCharacter);

public:
	/** Spawn a character for the player controller*/
	UFUNCTION(BlueprintCallable, Category = "Player Spawning")
	void SpawnPlayer(APlayerController* PlayerController);

};



