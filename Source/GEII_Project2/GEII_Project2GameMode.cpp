// Copyright Epic Games, Inc. All Rights Reserved.

#include "GEII_Project2GameMode.h"
#include "GEII_Project2Character.h"
#include "UObject/ConstructorHelpers.h"

AGEII_Project2GameMode::AGEII_Project2GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}

void AGEII_Project2GameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AllPlayerControllers.AddUnique(NewPlayer);

	UE_LOG(LogTemp, Warning, TEXT("Player has logged in: %s"), *NewPlayer->GetName());
}

void AGEII_Project2GameMode::SwapPlayerControllers(APlayerController* OldPC, APlayerController* NewPC)
{
	Super::SwapPlayerControllers(OldPC, NewPC);

	AllPlayerControllers.Remove(OldPC);
	AllPlayerControllers.AddUnique(NewPC);

	UE_LOG(LogTemp, Log, TEXT("Swapped PlayerControllers: %s -> %s"), *OldPC->GetName(), *NewPC->GetName());
}

void AGEII_Project2GameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	AllPlayerControllers.Add(Cast<APlayerController>(Exiting));

	UE_LOG(LogTemp, Log, TEXT("Player has logged out: %s"), *Exiting->GetName());
}

FTransform AGEII_Project2GameMode::FindRandomPlayerStart()
{
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

	if (PlayerStarts.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, PlayerStarts.Num() - 1);

		AActor* SelectedPlayerStart = PlayerStarts[RandomIndex];

		return SelectedPlayerStart->GetActorTransform();
	}

	return FTransform();
}

void AGEII_Project2GameMode::SpawnPlayer(APlayerController* PlayerController)
{
	if (!PlayerController)
	{
		return;
	}

	APawn* ControlledPawn = PlayerController->GetPawn();

	if (ControlledPawn)
	{
		ControlledPawn->Destroy();
	}

	FTransform PlayerStartTransform = FindRandomPlayerStart();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = PlayerController;

	AGEII_Project2Character* SpawnedCharacter = GetWorld()->SpawnActor<AGEII_Project2Character>(PlayerCharacterClass, PlayerStartTransform, SpawnParameters);

	if (SpawnedCharacter)
	{
		PlayerController->Possess(SpawnedCharacter);

		SetPlayerColor(PlayerController, SpawnedCharacter);
	}
}

void AGEII_Project2GameMode::SetPlayerColor(APlayerController* PlayerController, AGEII_Project2Character* PlayerCharacter)
{
	int32 PlayerIndex = AllPlayerControllers.Find(PlayerController);

	if (PlayerIndex != INDEX_NONE)
	{
		PlayerCharacter->ChangeColor(PlayerColorOptions[PlayerIndex]);
	}
}