// Copyright Epic Games, Inc. All Rights Reserved.


#include "GEII_Project2PlayerController.h"
#include "GameFramework/Gamemode.h"
#include "GEII_Project2GameMode.h"
#include "EnhancedInputSubsystems.h"
#include "GEII_Project2Character.h"
#include "../../../../../../../Source/Runtime/Engine/Public/Net/UnrealNetwork.h"

AGEII_Project2PlayerController::AGEII_Project2PlayerController()
{
	RespawnAfterDeath = 2.f;
}

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

void AGEII_Project2PlayerController::RespawnPlayer()
{
	GetWorld()->GetTimerManager().SetTimer(RespawnTimer, this, &AGEII_Project2PlayerController::Server_SpawnPlayer, RespawnAfterDeath, false);
}


// STILL TO BE FIGURED OUT
void AGEII_Project2PlayerController::AddKill()
{
	Kills++;
}

void AGEII_Project2PlayerController::AddDeath()
{
	Deaths++;
}
//////////////////////////////////////////////


void AGEII_Project2PlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGEII_Project2PlayerController, Kills);
	DOREPLIFETIME(AGEII_Project2PlayerController, Deaths);
}

void AGEII_Project2PlayerController::Server_SpawnPlayer_Implementation()
{
	AGEII_Project2GameMode* GameMode = Cast<AGEII_Project2GameMode>(GetWorld()->GetAuthGameMode());
	if (HasAuthority())
	{
		if (GameMode)
		{
			GameMode->SpawnPlayer(this);
		}
	}
}
