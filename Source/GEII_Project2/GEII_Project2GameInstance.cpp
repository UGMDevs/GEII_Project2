// Fill out your copyright notice in the Description page of Project Settings.


#include "GEII_Project2GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

 UGEII_Project2GameInstance::UGEII_Project2GameInstance()
 {
     InitSessionInterface();
 }
 
  void UGEII_Project2GameInstance::InitSessionInterface()
  {
      // Get the online subsystem and session interface
      if(IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
      {
          SessionInterface = Subsystem->GetSessionInterface();
      }
 }
 
 //Create Session
 
 void UGEII_Project2GameInstance::CreateMPSession(int32 NumPublicConnections, bool bIsLAN)
 {
    if (!SessionInterface.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Session Interface is invalid."));
        return;
    }
 
    // Bind delegate to handle session creation
    SessionInterface->OnCreateSessionCompleteDelegates.RemoveAll(this);
    SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UGEII_Project2GameInstance::OnCreateSessionComplete);
 
    // Generate a unique session name using a player ID or username
    FString playerID = GetFirstGamePlayer()->GetNickname(); // Example for unique player name
    FString uniqueSessionName = FString::Printf(TEXT("Session_%s"), *playerID);
 
    // Initialize session settings
    SessionSettings = MakeShareable(new FOnlineSessionSettings());
    SessionSettings->bAllowJoinInProgress = true;
    SessionSettings->bIsDedicated = false;
    SessionSettings->bIsLANMatch = bIsLAN;
    SessionSettings->NumPublicConnections = NumPublicConnections;
    SessionSettings->bShouldAdvertise = true;
    SessionSettings->bUsesPresence = true;
 
    // Create session
    UE_LOG(LogTemp, Warning, TEXT("Attempting creation of %s ..."), *uniqueSessionName);
    const ULocalPlayer* LocalPlayer = GetFirstGamePlayer();
    SessionInterface->CreateSession(0, FName("TesteServer"), *SessionSettings);
 }
 
 void UGEII_Project2GameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
 {
 
     if (bWasSuccessful)
     {
         UE_LOG(LogTemp, Warning, TEXT("Session created successfully on Host!"));
 
         // Open the lobby level with the "listen" option for hosting
         UE_LOG(LogTemp, Warning, TEXT("Opening World: %s"), *LobbyLevel.GetAssetName());
         UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), LobbyLevel, true, "listen");
     }
     else
     {
         UE_LOG(LogTemp, Warning, TEXT("Failed to create session"));
     }
 }
 
 // Find Sessions
 
 void UGEII_Project2GameInstance::FindMPSessions(bool bUseLAN, int32 MaxResults)
 {
     // Check if the session interface is valid
      if (!SessionInterface.IsValid())
      {
          UE_LOG(LogTemp, Warning, TEXT("Session Interface is invalid."));
          return;
      }
 
     UE_LOG(LogTemp, Warning, TEXT("Binding delegate"));
 
     // Bind the delegate to be called when the session search completes
	 SessionInterface->OnFindSessionsCompleteDelegates.RemoveAll(this);
	 SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UGEII_Project2GameInstance::OnFindSessionsComplete);
 
 
     // Initialize the session search
     SessionSearch = MakeShareable(new FOnlineSessionSearch());
     SessionSearch->bIsLanQuery = bUseLAN;
     SessionSearch->MaxSearchResults = MaxResults;
     SessionSearch->QuerySettings.Set("SEARCH_PRESENCE", true, EOnlineComparisonOp::Equals);
 
     UE_LOG(LogTemp, Warning, TEXT("Start Search"));
     // Start searching for sessions
     SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
     
 }
 
 // This function is called when the session search completes
 void UGEII_Project2GameInstance::OnFindSessionsComplete(bool bWasSuccessful)
 {
      UE_LOG(LogTemp, Warning, TEXT("Session search Complete"));
      //TArray<FBlueprintSessionResult> blueprintResults;
 
      if (bWasSuccessful && SessionSearch.IsValid())
      {
          TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;

          for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
          {
              // Convert FOnlineSessionSearchResult to FBlueprintSessionResult
              FBlueprintSessionResult BPResult;
              BPResult.OnlineResult = SearchResult;
              blueprintResults.Add(BPResult); 
              UE_LOG(LogTemp, Warning, TEXT("Sessions were found."));
          }
         
      }
      else
      {
          UE_LOG(LogTemp, Warning, TEXT("No sessions found or session search failed."));
      }
 }