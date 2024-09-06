// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Templates/SharedPointer.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "FindSessionsCallbackProxy.h"
#include "UObject/SoftObjectPtr.h"

#include "GEII_Project2GameInstance.generated.h"


UCLASS()
class GEII_PROJECT2_API UGEII_Project2GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
 public:
 	UGEII_Project2GameInstance();


     //Create Session
 public:
 
 	// Function to create a session
     UFUNCTION(BlueprintCallable, Category = "Multiplayer")
     void CreateMPSession(int32 NumPublicConnections, bool bIsLAN);
 
 protected:
     // Delegate for session creation
     void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
 
 private:
     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
     TSoftObjectPtr<UWorld> LobbyLevel;
 
 private:
 
     // Pointer to the session interface
     IOnlineSessionPtr SessionInterface;
 
     TSharedPtr<FOnlineSessionSettings> SessionSettings;
 
 
 
 
     //Find Sessions
 public:
     // Function to find multiplayer sessions
     UFUNCTION(BlueprintCallable, Category = "Sessions")
     void FindMPSessions(bool bUseLAN, int32 MaxResults = 6);
 
 private:
 
     // Callback function for when the session search is complete
     void OnFindSessionsComplete(bool bWasSuccessful);
 
     // Function to initialize the session interface
     void  InitSessionInterface();
 
     // Search handle to store results
     TSharedPtr<FOnlineSessionSearch> SessionSearch;
 
};