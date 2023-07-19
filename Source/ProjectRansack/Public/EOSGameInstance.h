// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "OnlineSessionSettings.h"
#include "EnumTeam.h"
#include "EOSGameInstance.generated.h"

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
	void FindGameSessionB(int pNumberOfSlotneeded);
	void OnFindGameSessionCompleteA(bool bWasSucess);
	void OnFindGameSessionCompleteB(bool bWasSucess);
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
	void OnRegisterPlayersCompleteDelegates(FName SessionName, const TArray<TSharedRef<const FUniqueNetId>>& Players, bool bWasSuccessful);
	void createCustomSettings(FOnlineSessionSettings& settings);
	void createSearchCustomSettingsA();
	void createSearchCustomSettingsB();
	UFUNCTION(BlueprintCallable)
	void updateGameSettings(int teamA, int teamB);
	UFUNCTION(BlueprintImplementableEvent)
	void setPlayerTeam(ETeam aTeam);
	void setTeam(FOnlineSessionSearchResult search);
	UFUNCTION(BlueprintCallable)
	void registerPlayerToGameSession(class APlayerController* InPlayerController);
	//------------
	//start Game
	UFUNCTION(BlueprintCallable)
	void StartGame();

	UFUNCTION(BlueprintCallable)
	void SaveGame();
	UFUNCTION(BlueprintCallable)
	void LoadGame();

	TArray<uint8> ConvertSaveGameToUint();
	class USaveGame* ConvertUintToSaveGame(TArray<uint8> pData);

	void UploadPlayerData(TArray<uint8> pData);
	void OnWritePlayerDataCompleted(bool bWasSuccessful, const FUniqueNetId& user, const FString& FileName);

	void GetPlayerData();
	void OnGetPlayerDataCompleted(bool bWasSuccessful, const FUniqueNetId& user, const FString& FileName);

	void ReadPlayerData(const FString& FileName);

public:
	class IOnlineSubsystem* OnlineSubsystem;
	bool bIsLogin = false;
	TSharedPtr<class FOnlineSessionSearch> SearchSettings;

	FOnlineSessionSearchResult PartyToJoin;
	bool bHasSession = false;
	int numberSlotneeded = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETeam team = ETeam::A;

	UPROPERTY(BlueprintReadOnly)
	FString SaveGameSlotName = "SaveGame.sav";

	class UPlayerSaveGame* saveGame = nullptr;
};
