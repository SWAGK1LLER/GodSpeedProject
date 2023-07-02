// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "OnlineSessionSettings.h"
#include "EOSGameInstance.generated.h"


//USTRUCT(BlueprintType)
//struct FSessionResult
//{
//	GENERATED_USTRUCT_BODY()
//public:
//	FOnlineSessionSearchResult OnlineResult;
//	FString sessionID;
//
//	FSessionResult() {}
//};

namespace EOnJoinSessionCompleteResult { enum Type; }

UENUM(BlueprintType)
enum EOutcomePins
{
	Failure,
	Success
};

UCLASS()
class PROJECTRANSACK_API UEOSGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UEOSGameInstance();

public:
	virtual void Init() override;

	//Login
	UFUNCTION(BlueprintCallable)
	void Login();
	void TryLogin();
	void EOSLogin();
	void OnLoginCompleteTry(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	void OnLoginCompleteEOS(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	UFUNCTION(BlueprintImplementableEvent)
	void LoginSuccessful();
	UFUNCTION(BlueprintImplementableEvent)
	void LoginFail();
	//------------
	//Create Party
	UFUNCTION(BlueprintCallable)
	void CreateParty(bool pTravel = true);
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccess);
	//------------
	//Party Initation
	void OnFriendAcceptInvite(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult);
	void OnClosePartyAndJoin(FName SessionName, bool bWasSuccess);
	void OnJoinPartyComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION(BlueprintImplementableEvent)
	void JoinPartyFail(const FString& msg);
	//------------
	//Party Quit
	UFUNCTION(BlueprintCallable)
	void QuitParty();
	void OnPartyQuitComplete(FName SessionName, bool bWasSuccess);
	//------------
	//Find game
	UFUNCTION(BlueprintCallable)
	void FindGameSession(int pNumberOfSlotneeded);
	void OnFindGameSessionComplete(bool bWasSucess);
	UFUNCTION(BlueprintCallable)
	void CancelFindGame();
	UFUNCTION(BlueprintImplementableEvent)
	void SearchFail();
	UFUNCTION(BlueprintImplementableEvent)
	void SearchFinish();
	void OnJoinGameComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	//------------
	//Create game
	void CreateGame();
	void OnCreateGameComplete(FName SessionName, bool bWasSuccess);
	void createCustomSettings(FOnlineSessionSettings& settings);
	void createSearchCustomSettings();
	UFUNCTION(BlueprintCallable)
	void updateGameSettings(int teamA, int teamB);
	//----------












	UFUNCTION(BlueprintCallable)
	void DeleteSession();
	void OnDeleteSessionComplete(FName SessionName, bool bWasSuccess);
	UFUNCTION(BlueprintCallable)
	void FindSession();
	
	void OnFindSessionComplete(bool bWasSucess);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);


public:
	class IOnlineSubsystem* OnlineSubsystem;
	bool bIsLogin = false;
	TSharedPtr<class FOnlineSessionSearch> SearchSettings;

	FOnlineSessionSearchResult PartyToJoin;
	bool bHasSession = false;
	int numberSlotneeded = 1;
};
