// Copyright Epic Games, Inc. All Rights Reserved.

#include "GEII_Project2GameMode.h"
#include "GEII_Project2Character.h"
#include "GEII_Project2PlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

AGEII_Project2GameMode::AGEII_Project2GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	World = GetWorld();
}

void AGEII_Project2GameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGEII_Project2GameMode, SpawnedBluePortal);
	DOREPLIFETIME(AGEII_Project2GameMode, SpawnedOrangePortal);
}

// New Code

// Network Handling
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

// Spawn Handling

FTransform AGEII_Project2GameMode::FindRandomPlayerStart()
{
	// Get all player starts in the world if the array is empty so that it only does this once.
	if (PlayerStarts.IsEmpty())
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);
	}

	// Verify if there are player starts in the world before getting a random one
	if (PlayerStarts.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, PlayerStarts.Num() - 1);

		AActor* SelectedPlayerStart = PlayerStarts[RandomIndex];

		if (!AllSelectedPlayerStarts.Contains(SelectedPlayerStart))
		{

			AllSelectedPlayerStarts.AddUnique(SelectedPlayerStart);

			PlayerStarts.Remove(SelectedPlayerStart);

			if (PlayerStarts.IsEmpty())
			{
				AllSelectedPlayerStarts.Empty();
			}
		}

		// Debug to check player starts availability
		UE_LOG(LogTemp, Warning, TEXT("Player Starts available: %d"), PlayerStarts.Num());
		UE_LOG(LogTemp, Warning, TEXT("Player Starts Unavailable: %d"), AllSelectedPlayerStarts.Num());

		return SelectedPlayerStart->GetActorTransform();
	}

	return FTransform();
}


void AGEII_Project2GameMode::SpawnPlayer(APlayerController* PlayerController)
{
	// Check if player controller is valid
	if (!PlayerController)
	{
		return;
	}

	APawn* ControlledPawn = PlayerController->GetPawn();

	//Destroy controlled pawn if it's valid, we want to spawn a new one and not use the previous Pawn
	if (ControlledPawn)
	{
		ControlledPawn->Destroy();
	}

	// Get a random player start
	FTransform PlayerStartTransform = FindRandomPlayerStart();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = PlayerController;

	AGEII_Project2Character* SpawnedCharacter = GetWorld()->SpawnActor<AGEII_Project2Character>(PlayerCharacterClass, PlayerStartTransform, SpawnParameters);

	// Make the Player Controlled possess the new Spawned Character and set its color
	if (SpawnedCharacter)
	{
		// Set the player controllers' direction
		PlayerController->ClientSetRotation(PlayerStartTransform.Rotator());
		PlayerController->Possess(SpawnedCharacter);

		SetPlayerColor(PlayerController, SpawnedCharacter);
	}
}

void AGEII_Project2GameMode::SetPlayerColor(APlayerController* PlayerController, AGEII_Project2Character* PlayerCharacter)
{
	int32 PlayerIndex = AllPlayerControllers.Find(PlayerController);

	// Change the character's color for its index's corresponding color
	if (PlayerIndex != INDEX_NONE)
	{
		PlayerCharacter->ChangeColor(PlayerColorOptions[PlayerIndex]);
	}
}

void AGEII_Project2GameMode::SpawnBluePortal(FHitResult TraceHit)
{
	if(!SpawnedBluePortal->IsValidLowLevel())
	{
		if (!BluePortal)
		{
			return;
		}
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FVector LocationToSpawn = TraceHit.Location + TraceHit.Normal;
		SpawnedBluePortal = World->SpawnActor<APortal>(BluePortal, LocationToSpawn, UKismetMathLibrary::MakeRotFromX(TraceHit.Normal), ActorSpawnParams);
		SpawnedBluePortal->SetCurrentWall(TraceHit.GetActor());
		SpawnedBluePortal->PlacePortal(LocationToSpawn, UKismetMathLibrary::MakeRotFromX(TraceHit.Normal));
		if (SpawnedOrangePortal->IsValidLowLevel())
		{
			SpawnedBluePortal->SetPortalToLink(SpawnedOrangePortal);
			SpawnedOrangePortal->SetPortalToLink(SpawnedBluePortal);
			SpawnedBluePortal->SetupLinkedPortal();
		}
	}
	else
	{
		SpawnedBluePortal->SetCurrentWall(TraceHit.GetActor());
		ChangePortalLocation(SpawnedBluePortal, TraceHit.Location, UKismetMathLibrary::MakeRotFromX(TraceHit.Normal));
	}
	

}

void AGEII_Project2GameMode::SpawnOrangePortal(FHitResult TraceHit)
{
	if (!SpawnedOrangePortal->IsValidLowLevel())
	{
		if (!OrangePortal)
		{
			return;
		}
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FVector LocationToSpawn = TraceHit.Location + TraceHit.Normal;
		SpawnedOrangePortal = World->SpawnActor<APortal>(OrangePortal, LocationToSpawn, UKismetMathLibrary::MakeRotFromX(TraceHit.Normal), ActorSpawnParams);
		SpawnedOrangePortal->SetCurrentWall(TraceHit.GetActor());
		SpawnedOrangePortal->PlacePortal(LocationToSpawn, UKismetMathLibrary::MakeRotFromX(TraceHit.Normal));
		if (SpawnedBluePortal->IsValidLowLevel())
		{
			SpawnedOrangePortal->SetPortalToLink(SpawnedBluePortal);
			SpawnedBluePortal->SetPortalToLink(SpawnedOrangePortal);
			SpawnedOrangePortal->SetupLinkedPortal();
		}
	}
	else
	{
		SpawnedOrangePortal->SetCurrentWall(TraceHit.GetActor());
		ChangePortalLocation(SpawnedOrangePortal, TraceHit.Location, UKismetMathLibrary::MakeRotFromX(TraceHit.Normal));
	}
}

void AGEII_Project2GameMode::ChangePortalLocation(APortal* PortalToChangeLocation, FVector NewLocation, FRotator NewRotation)
{
	PortalToChangeLocation->PlacePortal(NewLocation, NewRotation);
}
