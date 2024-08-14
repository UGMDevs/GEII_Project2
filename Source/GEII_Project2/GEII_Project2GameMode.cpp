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
