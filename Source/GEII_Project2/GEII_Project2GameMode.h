// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GEII_Project2PlayerController.h"
#include "GameFramework/Actor.h"
#include "GEII_Project2GameMode.generated.h"

UCLASS(minimalapi)
class AGEII_Project2GameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AGEII_Project2GameMode();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<APlayerController*> AllPlayerControllers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FLinearColor> PlayerColorOptions = {
			FLinearColor::White,
			FLinearColor::Red,
			FLinearColor::Blue,
			FLinearColor::Green
	};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AGEII_Project2Character> PlayerCharacterClass;

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void SwapPlayerControllers(APlayerController* OldPC, APlayerController* NewPC) override;
	virtual void Logout(AController* Exiting) override;
	
	UFUNCTION(BlueprintPure, Category = "Player Spawning")
	FTransform FindRandomPlayerStart();

	UFUNCTION(BlueprintCallable, Category = "Player Spawning")
	void SetPlayerColor(APlayerController* PlayerController, AGEII_Project2Character* PlayerCharacter);

public:
	UFUNCTION(BlueprintCallable, Category = "Player Spawning")
	void SpawnPlayer(APlayerController* PlayerController);

};



