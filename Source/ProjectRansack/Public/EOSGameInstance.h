// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionSettings.h"
#include "EnumTeam.h"
#include "PlayerSetting.h"
#include "PlayerSaveGame.h"
#include "EOSGameInstance.generated.h"

namespace EOnJoinSessionCompleteResult { enum Type; }


template <typename T> 
struct FSaveGameSlot
{
	FString fileName = "";
	int slotIdx = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	T* saveGame = nullptr;
};

UENUM(BlueprintType)
enum EOutcomePins
{
	Failure,
	Success
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveGameFinish);

UCLASS()
class PROJECTRANSACK_API UEOSGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UEOSGameInstance();

public:
	virtual void Init() override;

	//Login
	UFUNCTION(BlueprintCallable)
	void Logout();
	void logoutCompleted(int32 LocalUserNum, bool bWasSuccessful);
	UFUNCTION(BlueprintImplementableEvent)
	void LogoutSuccessful();
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
	UFUNCTION(BlueprintCallable)
	void CloseParty();
	void OnPartyCloseComplete(FName SessionName, bool bWasSuccess);
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
	UFUNCTION(BlueprintCallable)
	void unregisterPlayerToGameSession(class APlayerController* InPlayerController);
	UFUNCTION(BlueprintCallable)
	void CloseGame();
	void OnGameCloseComplete(FName SessionName, bool bWasSuccess);
	//------------
	//start Game
	UFUNCTION(BlueprintCallable)
	void StartGame();
	//------------
	//Friend
	UFUNCTION(BlueprintCallable)
	void ShowFriendOverlay();
	//-------------
	//SaveGame / saveSettings
	UFUNCTION(BlueprintCallable)
	void SaveSettings();
	UFUNCTION(BlueprintCallable)
	void LoadSaveSettings();

	UFUNCTION(BlueprintCallable)
	void SaveGame();
	UFUNCTION(BlueprintCallable)
	void LoadSaveGame();

	UFUNCTION(BlueprintImplementableEvent)
	void LoadSaveGameSuccessful(float percent, int level);

	TArray<uint8> ConvertSaveGameToUint();
	class USaveGame* ConvertUintToSaveGame(TArray<uint8> pData);

	void UploadPlayerData(TArray<uint8> pData);
	void OnWritePlayerDataCompleted(bool bWasSuccessful, const FUniqueNetId& user, const FString& FileName);

	void GetPlayerData();
	void OnGetPlayerDataCompleted(bool bWasSuccessful, const FUniqueNetId& user, const FString& FileName);

	void ReadPlayerData(const FString& FileName);

	UFUNCTION(BlueprintCallable)
	UPlayerSetting* GetPlayerSettings();

	UFUNCTION(BlueprintCallable)
	UPlayerSaveGame* GetPlayerSaveGame();

public:
	class IOnlineSubsystem* OnlineSubsystem;
	bool bIsLogin = false;
	TSharedPtr<class FOnlineSessionSearch> SearchSettings;

	FOnlineSessionSearchResult PartyToJoin;
	bool bHasSession = false;
	int numberSlotneeded = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETeam team = ETeam::A;


	FSaveGameSlot<class UPlayerSaveGame> ServerGameSlot = { "SaveGame.sav", 0, nullptr };
	FOnSaveGameFinish SaveGameFinish;

	FSaveGameSlot<class UPlayerSetting> SettingsGameSlot = { "Settings.sav", 1, nullptr };
};
