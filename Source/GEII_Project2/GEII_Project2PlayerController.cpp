// Copyright Epic Games, Inc. All Rights Reserved.


#include "GEII_Project2PlayerController.h"
#include "GameFramework/Gamemode.h"
#include "GEII_Project2GameMode.h"
#include "EnhancedInputSubsystems.h"
#include "GEII_Project2Character.h"

void AGEII_Project2PlayerController::BeginPlay()
{
	Super::BeginPlay();

	// get the enhanced input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// add the mapping context so we get controls
		Subsystem->AddMappingContext(InputMappingContext, 0);

		UE_LOG(LogTemp, Warning, TEXT("BeginPlay"));
	}

	// Ask the server to spawn a character for this controller
	if (IsLocalPlayerController())
	{
		Server_SpawnPlayer();
	}

}

void AGEII_Project2PlayerController::Server_SpawnPlayer_Implementation()
{
	if (HasAuthority())
	{
		AGEII_Project2GameMode* GameMode = Cast<AGEII_Project2GameMode>(GetWorld()->GetAuthGameMode());

		if (GameMode)
		{
			GameMode->SpawnPlayer(this);
		}
	}
}
