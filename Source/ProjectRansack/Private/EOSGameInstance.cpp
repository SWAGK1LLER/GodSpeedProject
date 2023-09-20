// Fill out your copyright notice in the Description page of Project Settings.


#include "EOSGameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include <Kismet/GameplayStatics.h>
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlinePartyInterface.h"

const FName FSessionName = FName("Lobby");
const FString FSearchName = FString("ProjectRansackLobby");

const FName FGameSessionName = FName("Match");
const FString FSearchGameSessionName = FString("ProjectRansackMatch");

UEOSGameInstance::UEOSGameInstance()
{

}

void UEOSGameInstance::Init()
{
	Super::Init();

	OnlineSubsystem = IOnlineSubsystem::Get();

	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SessionPtr->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &UEOSGameInstance::OnFriendAcceptInvite);
}

//Login
void UEOSGameInstance::Logout()
{
	if (!bIsLogin)
		return;

	if (!OnlineSubsystem)
		return;

	CloseParty();

	IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
	if (!Identity)
		return;

	Identity->OnLogoutCompleteDelegates->AddUObject(this, &UEOSGameInstance::logoutCompleted);
	Identity->Logout(0);
}

void UEOSGameInstance::logoutCompleted(int32 LocalUserNum, bool bWasSuccessful)
{
	if (!OnlineSubsystem)
		return;

	IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
	if (!Identity)
		return;

	Identity->ClearOnLogoutCompleteDelegates(0, this);
	bIsLogin = false;
	LogoutSuccessful();
}

void UEOSGameInstance::Login()
{
	if (bIsLogin)
	{
		LoginSuccessful();
		CreateParty();
		return;
	}

	TryLogin();
}

void UEOSGameInstance::TryLogin()
{
	if (!OnlineSubsystem)
		return;

	IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
	if (!Identity)
		return;

	FOnlineAccountCredentials Credentials;
	Credentials.Id = FString("");
	Credentials.Token = FString("");
	Credentials.Type = FString("persistentauth");

	Identity->OnLoginCompleteDelegates->AddUObject(this, &UEOSGameInstance::OnLoginCompleteTry);
	Identity->Login(0, Credentials);
}

void UEOSGameInstance::EOSLogin()
{
	if (!OnlineSubsystem)
		return;

	IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
	if (!Identity)
		return;

	FOnlineAccountCredentials Credentials;
	Credentials.Id = FString("");
	Credentials.Token = FString("");
	Credentials.Type = FString("accountportal");

	Identity->OnLoginCompleteDelegates->AddUObject(this, &UEOSGameInstance::OnLoginCompleteEOS);
	Identity->Login(0, Credentials);
}

void UEOSGameInstance::OnLoginCompleteTry(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	bIsLogin = bWasSuccessful;

	if (!bIsLogin)
	{
		if (Error.Equals(TEXT("Already logged in"), ESearchCase::IgnoreCase))
		{
			bIsLogin = true;
		}
	}

	if (!OnlineSubsystem)
		return;

	IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
	if (!Identity)
		return;

	Identity->ClearOnLoginCompleteDelegates(0, this);

	if (!bIsLogin)
		EOSLogin();
	else
	{
		LoginSuccessful();
		CreateParty();
	}
}

void UEOSGameInstance::OnLoginCompleteEOS(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	bIsLogin = bWasSuccessful;

	if (!OnlineSubsystem)
		return;

	IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
	if (!Identity)
		return;

	Identity->ClearOnLoginCompleteDelegates(0, this);

	if (bIsLogin)
	{
		LoginSuccessful();
		CreateParty();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("error %s"), *Error);
		LoginFail();
	}
}
//--------
//Party creation
void UEOSGameInstance::CreateParty(bool pTravel)
{
	if (!bIsLogin)
		return;

	if (!OnlineSubsystem)
		return;

	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bIsLANMatch = false;
	SessionSettings.NumPublicConnections = partyLimite;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bUseLobbiesVoiceChatIfAvailable = false;
	SessionSettings.bAllowInvites = true;
	
	SessionSettings.Set(SEARCH_KEYWORDS, FSearchName, EOnlineDataAdvertisementType::ViaOnlineService);

	if (pTravel)
		SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnCreateSessionComplete);
	else
		bHasSession = true;
	
	SessionPtr->CreateSession(0, FSessionName, SessionSettings);
}

void UEOSGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccess)
{
	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SessionPtr->ClearOnCreateSessionCompleteDelegates(this);
	GetWorld()->ServerTravel(FString("/Game/Maps/MainMenu?listen"), false);
	bHasSession = true;
}
//--------
//Party Initation
void UEOSGameInstance::OnFriendAcceptInvite(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult)
{
	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	if (bWasSuccessful)
	{	
		UE_LOG(LogTemp, Warning, TEXT("Accept invite"));

		PartyToJoin = InviteResult;

		SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnClosePartyAndJoin);
		SessionPtr->DestroySession(FSessionName);
	}
}

void UEOSGameInstance::OnClosePartyAndJoin(FName SessionName, bool bWasSuccess)
{
	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SessionPtr->ClearOnDestroySessionCompleteDelegates(this);
	bHasSession = false;
	
	SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnJoinPartyComplete);
	SessionPtr->JoinSession(0, FSessionName, PartyToJoin);
}

void UEOSGameInstance::OnJoinPartyComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SessionPtr->ClearOnJoinSessionCompleteDelegates(this);

	if (Result == EOnJoinSessionCompleteResult::AlreadyInSession)
		JoinPartyFail("Already in Party");
	else if (Result == EOnJoinSessionCompleteResult::SessionIsFull)
		JoinPartyFail("Party is full");
	else if (Result == EOnJoinSessionCompleteResult::UnknownError)
		JoinPartyFail("UnknownError error");
	else if (Result == EOnJoinSessionCompleteResult::Success)
	{
		ClientTravelToSession(0, FSessionName);
		return;
	}

	CreateParty(false);
}
//--------
//Party Quit
void UEOSGameInstance::QuitParty()
{
	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnPartyQuitComplete);
	SessionPtr->DestroySession(FSessionName);
}

void UEOSGameInstance::OnPartyQuitComplete(FName SessionName, bool bWasSuccess)
{
	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SessionPtr->ClearOnDestroySessionCompleteDelegates(this);

	CreateParty();
}
void UEOSGameInstance::CloseParty()
{
	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	//SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnPartyCloseComplete);
	SessionPtr->DestroySession(FSessionName);
}
//void UEOSGameInstance::OnPartyCloseComplete(FName SessionName, bool bWasSuccess)
//{
//	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
//	if (!SessionPtr)
//		return;
//
//	SessionPtr->ClearOnDestroySessionCompleteDelegates(this);
//
//	UGameplayStatics::OpenLevel(GetWorld(), "/Game/Maps/MainMenu", true);
//}
//--------
//Find Game
void UEOSGameInstance::CreateGame()
{
	if (!bIsLogin)
		return;

	if (!OnlineSubsystem)
		return;

	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bIsLANMatch = false;
	SessionSettings.NumPublicConnections = 6;
	SessionSettings.bAllowJoinInProgress = false;
	SessionSettings.bAllowJoinViaPresence = false;
	SessionSettings.bUsesPresence = false;
	SessionSettings.bUseLobbiesIfAvailable = false;
	SessionSettings.bUseLobbiesVoiceChatIfAvailable = false;
	SessionSettings.bAllowInvites = false;

	createCustomSettings(SessionSettings);

	SessionSettings.Set(SEARCH_KEYWORDS, FSearchGameSessionName, EOnlineDataAdvertisementType::ViaOnlineService);
	
	SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnCreateGameComplete);
	SessionPtr->CreateSession(0, FGameSessionName, SessionSettings);

	team = ETeam::A;
	setPlayerTeam(team);
}

void UEOSGameInstance::OnCreateGameComplete(FName SessionName, bool bWasSuccess)
{
	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	TArray<FUniqueNetIdRef> Players;

	SessionPtr->ClearOnCreateSessionCompleteDelegates(this);
	GetWorld()->ServerTravel(FString("/Game/Maps/GameLobby?listen"), false);
}

void UEOSGameInstance::OnRegisterPlayersCompleteDelegates(FName SessionName, const TArray<TSharedRef<const FUniqueNetId>>& Players, bool bWasSuccessful)
{
	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SessionPtr->ClearOnRegisterPlayersCompleteDelegates(this);
}

void UEOSGameInstance::createCustomSettings(FOnlineSessionSettings& settings)
{
	settings.Set(FName("TeamOf3A"), true, EOnlineDataAdvertisementType::ViaOnlineService);
	settings.Set(FName("TeamOf2A"), true, EOnlineDataAdvertisementType::ViaOnlineService);
	settings.Set(FName("NoTeamA"), true, EOnlineDataAdvertisementType::ViaOnlineService);

	settings.Set(FName("TeamOf3B"), true, EOnlineDataAdvertisementType::ViaOnlineService);
	settings.Set(FName("TeamOf2B"), true, EOnlineDataAdvertisementType::ViaOnlineService);
	settings.Set(FName("NoTeamB"), true, EOnlineDataAdvertisementType::ViaOnlineService);
}

void UEOSGameInstance::createSearchCustomSettingsA()
{
	if (numberSlotneeded == 1)
		SearchSettings->QuerySettings.Set(FName("NoTeamA"), true, EOnlineComparisonOp::Equals);
	else if (numberSlotneeded == 2)
		SearchSettings->QuerySettings.Set(FName("TeamOf2A"), true, EOnlineComparisonOp::Equals);
	else if (numberSlotneeded == 3)
		SearchSettings->QuerySettings.Set(FName("TeamOf3A"), true, EOnlineComparisonOp::Equals);
}

void UEOSGameInstance::createSearchCustomSettingsB()
{
	if (numberSlotneeded == 1)
		SearchSettings->QuerySettings.Set(FName("NoTeamB"), true, EOnlineComparisonOp::Equals);
	else if (numberSlotneeded == 2)
		SearchSettings->QuerySettings.Set(FName("TeamOf2B"), true, EOnlineComparisonOp::Equals);
	else if (numberSlotneeded == 3)
		SearchSettings->QuerySettings.Set(FName("TeamOf3B"), true, EOnlineComparisonOp::Equals);
}

void UEOSGameInstance::updateGameSettings(int teamA, int teamB)
{
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bIsLANMatch = false;
	SessionSettings.NumPublicConnections = 6;
	SessionSettings.bAllowJoinInProgress = false;
	SessionSettings.bAllowJoinViaPresence = false;
	SessionSettings.bUsesPresence = false;
	SessionSettings.bUseLobbiesIfAvailable = false;
	SessionSettings.bUseLobbiesVoiceChatIfAvailable = false;
	SessionSettings.bAllowInvites = false;

	SessionSettings.Set(FName("TeamOf3A"), false, EOnlineDataAdvertisementType::ViaOnlineService);
	SessionSettings.Set(FName("TeamOf2A"), false, EOnlineDataAdvertisementType::ViaOnlineService);
	SessionSettings.Set(FName("NoTeamA"), false, EOnlineDataAdvertisementType::ViaOnlineService);

	SessionSettings.Set(FName("TeamOf3B"), false, EOnlineDataAdvertisementType::ViaOnlineService);
	SessionSettings.Set(FName("TeamOf2B"), false, EOnlineDataAdvertisementType::ViaOnlineService);
	SessionSettings.Set(FName("NoTeamB"), false, EOnlineDataAdvertisementType::ViaOnlineService);

	if (teamA == 0)
	{
		SessionSettings.Set(FName("TeamOf3A"), true, EOnlineDataAdvertisementType::ViaOnlineService);
		SessionSettings.Set(FName("TeamOf2A"), true, EOnlineDataAdvertisementType::ViaOnlineService);
		SessionSettings.Set(FName("NoTeamA"), true, EOnlineDataAdvertisementType::ViaOnlineService);
	}
	else if (teamA == 1)
	{
		SessionSettings.Set(FName("TeamOf2A"), true, EOnlineDataAdvertisementType::ViaOnlineService);
		SessionSettings.Set(FName("NoTeamA"), true, EOnlineDataAdvertisementType::ViaOnlineService);
	}
	else if (teamA == 2)
	{
		SessionSettings.Set(FName("NoTeamA"), true, EOnlineDataAdvertisementType::ViaOnlineService);
	}
	//---------------
	if (teamB == 0)
	{
		SessionSettings.Set(FName("TeamOf3B"), true, EOnlineDataAdvertisementType::ViaOnlineService);
		SessionSettings.Set(FName("TeamOf2B"), true, EOnlineDataAdvertisementType::ViaOnlineService);
		SessionSettings.Set(FName("NoTeamB"), true, EOnlineDataAdvertisementType::ViaOnlineService);
	}
	else if (teamB == 1)
	{
		SessionSettings.Set(FName("TeamOf2B"), true, EOnlineDataAdvertisementType::ViaOnlineService);
		SessionSettings.Set(FName("NoTeamB"), true, EOnlineDataAdvertisementType::ViaOnlineService);
	}
	else if (teamB == 2)
	{
		SessionSettings.Set(FName("NoTeamB"), true, EOnlineDataAdvertisementType::ViaOnlineService);
	}

	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SessionPtr->UpdateSession(FGameSessionName, SessionSettings);
}

void UEOSGameInstance::FindGameSession(int pNumberOfSlotneeded)
{
	numberSlotneeded = pNumberOfSlotneeded;

	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SearchSettings = MakeShareable(new FOnlineSessionSearch());
	SearchSettings->QuerySettings.SearchParams.Empty();
	SearchSettings->MaxSearchResults = 100;
	SearchSettings->QuerySettings.Set(SEARCH_KEYWORDS, FSearchGameSessionName, EOnlineComparisonOp::Equals);
	createSearchCustomSettingsA();

	SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnFindGameSessionCompleteA);
	SessionPtr->FindSessions(0, SearchSettings.ToSharedRef());
}

void UEOSGameInstance::FindGameSessionB(int pNumberOfSlotneeded)
{
	numberSlotneeded = pNumberOfSlotneeded;

	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SearchSettings = MakeShareable(new FOnlineSessionSearch());
	SearchSettings->QuerySettings.SearchParams.Empty();
	SearchSettings->MaxSearchResults = 100;
	SearchSettings->QuerySettings.Set(SEARCH_KEYWORDS, FSearchGameSessionName, EOnlineComparisonOp::Equals);
	createSearchCustomSettingsB();

	SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnFindGameSessionCompleteB);
	SessionPtr->FindSessions(0, SearchSettings.ToSharedRef());
}

void UEOSGameInstance::OnFindGameSessionCompleteA(bool bWasSucess)
{
	if (!bWasSucess)
	{
		SearchFail();
		return;
	}

	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SessionPtr->ClearOnFindSessionsCompleteDelegates(this);

	if (SearchSettings->SearchResults.Num() == 0)
	{
		FindGameSessionB(numberSlotneeded);
		return;
	}

	//Get team
	//setTeam(SearchSettings->SearchResults[0]);
	setPlayerTeam(ETeam::A);

	SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnJoinGameComplete);
	SessionPtr->JoinSession(0, FGameSessionName, SearchSettings->SearchResults[0]);
}

void UEOSGameInstance::OnFindGameSessionCompleteB(bool bWasSucess)
{
	if (!bWasSucess)
	{
		SearchFail();
		return;
	}

	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SessionPtr->ClearOnFindSessionsCompleteDelegates(this);

	if (SearchSettings->SearchResults.Num() == 0)
	{
		CreateGame();
		return;
	}

	//Get team
	//setTeam(SearchSettings->SearchResults[0]);
	setPlayerTeam(ETeam::B);

	SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnJoinGameComplete);
	SessionPtr->JoinSession(0, FGameSessionName, SearchSettings->SearchResults[0]);
}

void UEOSGameInstance::OnJoinGameComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SessionPtr->ClearOnJoinSessionCompleteDelegates(this);

	FString ConnectionInfo = FString();
	SessionPtr->GetResolvedConnectString(FGameSessionName, ConnectionInfo);
	if (ConnectionInfo.IsEmpty())
		return;

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* Controller = Cast<APlayerController>(Iterator->Get());
		if (Controller == nullptr)
			continue;

		Controller->ClientTravel(ConnectionInfo, ETravelType::TRAVEL_Absolute);
	}
}

void UEOSGameInstance::registerPlayerToGameSession(APlayerController* InPlayerController)
{
	check(IsValid(InPlayerController));

	FUniqueNetIdRepl UniqueNetIdRepl;
	if (InPlayerController->IsLocalPlayerController())
	{
		ULocalPlayer* LocalPlayer = InPlayerController->GetLocalPlayer();
		if (IsValid(LocalPlayer))
		{
			UniqueNetIdRepl = LocalPlayer->GetPreferredUniqueNetId();
		}
		else
		{
			UNetConnection* RemoteNetConnection = Cast<UNetConnection>(InPlayerController->Player);
			check(IsValid(RemoteNetConnection));
			UniqueNetIdRepl = RemoteNetConnection->PlayerId;
		}
	}
	else
	{
		UNetConnection* RemoteNetConnection = Cast<UNetConnection>(InPlayerController->Player);
		check(IsValid(RemoteNetConnection));
		UniqueNetIdRepl = RemoteNetConnection->PlayerId;
	}

	TSharedPtr<const FUniqueNetId> UniqueNetId = UniqueNetIdRepl.GetUniqueNetId();
	if (UniqueNetId == nullptr)
		return;

	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SessionPtr->RegisterPlayer(FGameSessionName, *UniqueNetId, false);
}

void UEOSGameInstance::unregisterPlayerToGameSession(APlayerController* InPlayerController)
{
	check(IsValid(InPlayerController));

	FUniqueNetIdRepl UniqueNetIdRepl;
	if (InPlayerController->IsLocalPlayerController())
	{
		ULocalPlayer* LocalPlayer = InPlayerController->GetLocalPlayer();
		if (IsValid(LocalPlayer))
		{
			UniqueNetIdRepl = LocalPlayer->GetPreferredUniqueNetId();
		}
		else
		{
			UNetConnection* RemoteNetConnection = Cast<UNetConnection>(InPlayerController->Player);
			if (RemoteNetConnection == nullptr)
				return;

			UniqueNetIdRepl = RemoteNetConnection->PlayerId;
		}
	}
	else
	{
		UNetConnection* RemoteNetConnection = Cast<UNetConnection>(InPlayerController->Player);
		if (RemoteNetConnection == nullptr)
			return;
		UniqueNetIdRepl = RemoteNetConnection->PlayerId;
	}

	TSharedPtr<const FUniqueNetId> UniqueNetId = UniqueNetIdRepl.GetUniqueNetId();
	if (UniqueNetId == nullptr)
		return;

	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SessionPtr->UnregisterPlayer(FGameSessionName, *UniqueNetId);
}

void UEOSGameInstance::CloseGame()
{
	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnGameCloseComplete);
	SessionPtr->DestroySession(FGameSessionName);
}

void UEOSGameInstance::OnGameCloseComplete(FName SessionName, bool bWasSuccess)
{
	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SessionPtr->ClearOnDestroySessionCompleteDelegates(this);

	UGameplayStatics::OpenLevel(GetWorld(), "/Game/Maps/MainMenu", true);
}

void UEOSGameInstance::CancelFindGame()
{
	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SessionPtr->CancelFindSessions();
}

void UEOSGameInstance::setTeam(FOnlineSessionSearchResult search)
{
	FOnlineSessionSettings SessionSettings = search.Session.SessionSettings;

	bool value;
	if (numberSlotneeded == 1)
	{
		SessionSettings.Get("NoTeamA", value);
		team = value? ETeam::A : ETeam::B;
	}
	else if (numberSlotneeded == 2)
	{
		SessionSettings.Get("TeamOf2A", value);
		team = value ? ETeam::A : ETeam::B;
	}
	else if (numberSlotneeded == 3)
	{
		SessionSettings.Get("TeamOf3A", value);
		team = value ? ETeam::A : ETeam::B;
	}
}
//--------
//Start Game
void UEOSGameInstance::StartGame()
{
	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SessionPtr->StartSession(FGameSessionName);
}
//--------
//Friend
void UEOSGameInstance::ShowFriendOverlay()
{
	IOnlineExternalUIPtr ExternalUI = OnlineSubsystem->GetExternalUIInterface();
	if (!ExternalUI.IsValid())
		return;

	ExternalUI->ShowFriendsUI(0);
}
//--------
//SaveGame / saveSettings

void UEOSGameInstance::SaveSettings()
{
	UGameplayStatics::SaveGameToSlot(SettingsGameSlot.saveGame, SettingsGameSlot.fileName, SettingsGameSlot.slotIdx);
}

void UEOSGameInstance::LoadSaveSettings()
{
	if (SettingsGameSlot.saveGame != nullptr)
		return;

	USaveGame* aSaveGame = UGameplayStatics::LoadGameFromSlot(SettingsGameSlot.fileName, SettingsGameSlot.slotIdx);
	if (aSaveGame == nullptr)
	{
		//New account, no file on server
		SettingsGameSlot.saveGame = (UPlayerSetting*)(UGameplayStatics::CreateSaveGameObject(UPlayerSetting::StaticClass()));
		SaveSettings();
	}
	else
		SettingsGameSlot.saveGame = (UPlayerSetting*)aSaveGame;	

	SettingsGameSlot.saveGame->InitializeSettings();
}

void UEOSGameInstance::SaveGame()
{
	if (ServerGameSlot.saveGame == nullptr)
		return;

	UGameplayStatics::SaveGameToSlot(ServerGameSlot.saveGame, ServerGameSlot.fileName, ServerGameSlot.slotIdx);
	UploadPlayerData(ConvertSaveGameToUint());
}

void UEOSGameInstance::LoadSaveGame()
{
	isLoadingSaveGame = true;
	GetPlayerData();
}

TArray<uint8> UEOSGameInstance::ConvertSaveGameToUint()
{
	TArray<uint8> data;
	if (ServerGameSlot.saveGame == nullptr)
		return data;

	UGameplayStatics::SaveGameToMemory(ServerGameSlot.saveGame, data);
	return data;
}

USaveGame* UEOSGameInstance::ConvertUintToSaveGame(TArray<uint8> pData)
{
	USaveGame* aSaveGame = nullptr;

	if (pData.IsEmpty())
		return aSaveGame;

	return UGameplayStatics::LoadGameFromMemory(pData);
}

void UEOSGameInstance::UploadPlayerData(TArray<uint8> pData)
{
	if (pData.IsEmpty())
	{
		SaveGameFinish.Broadcast();
		return;
	}

	IOnlineIdentityPtr identityPointerRef = OnlineSubsystem->GetIdentityInterface();
	if (!identityPointerRef)
	{
		SaveGameFinish.Broadcast();
		return;
	}

	IOnlineUserCloudPtr cloundPointerRef = OnlineSubsystem->GetUserCloudInterface();
	if (!cloundPointerRef)
	{
		SaveGameFinish.Broadcast();
		return;
	}
		
	TSharedPtr<const FUniqueNetId> userIdRef = identityPointerRef->GetUniquePlayerId(0);
	if (userIdRef == nullptr)
	{
		SaveGameFinish.Broadcast();
		return;
	}

	cloundPointerRef->OnWriteUserFileCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnWritePlayerDataCompleted);
	cloundPointerRef->WriteUserFile(*userIdRef, ServerGameSlot.fileName, pData);
}

void UEOSGameInstance::OnWritePlayerDataCompleted(bool bWasSuccessful, const FUniqueNetId& user, const FString& FileName)
{
	if (bWasSuccessful)
	{
		SaveGameFinish.Broadcast();
		UE_LOG(LogTemp, Warning, TEXT("Save success"));
	}
	else
	{
		SaveGameFinish.Broadcast();
		UE_LOG(LogTemp, Warning, TEXT("Faile to save data"));
	}
}

void UEOSGameInstance::GetPlayerData()
{
	IOnlineIdentityPtr identityPointerRef = OnlineSubsystem->GetIdentityInterface();
	if (!identityPointerRef)
	{
		isLoadingSaveGame = false;
		return;
	}

	IOnlineUserCloudPtr cloundPointerRef = OnlineSubsystem->GetUserCloudInterface();
	if (!cloundPointerRef)
	{
		isLoadingSaveGame = false;
		return;
	}

	TSharedPtr<const FUniqueNetId> userIdRef = identityPointerRef->GetUniquePlayerId(0);
	if (!userIdRef.IsValid())
	{
		//In editor
		ServerGameSlot.saveGame = (UPlayerSaveGame*)(UGameplayStatics::CreateSaveGameObject(UPlayerSaveGame::StaticClass()));
		isLoadingSaveGame = false;
		return;
	}

	cloundPointerRef->OnReadUserFileCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnGetPlayerDataCompleted);
	cloundPointerRef->ReadUserFile(*userIdRef, ServerGameSlot.fileName);
}

void UEOSGameInstance::OnGetPlayerDataCompleted(bool bWasSuccessful, const FUniqueNetId& user, const FString& FileName)
{
	if (bWasSuccessful)
	{
		ReadPlayerData(FileName);
	}
	else
	{
		isLoadingSaveGame = false;
		//New account, no file on server
		ServerGameSlot.saveGame = (UPlayerSaveGame*)(UGameplayStatics::CreateSaveGameObject(UPlayerSaveGame::StaticClass()));
		SaveGame();
		LoadSaveGameFinished();
		LoadSaveGameSuccessful(ServerGameSlot.saveGame->GetPercent(), ServerGameSlot.saveGame->level);
	}
}

void UEOSGameInstance::ReadPlayerData(const FString& FileName)
{
	IOnlineIdentityPtr identityPointerRef = OnlineSubsystem->GetIdentityInterface();
	if (!identityPointerRef)
	{
		isLoadingSaveGame = false;
		return;
	}

	IOnlineUserCloudPtr cloundPointerRef = OnlineSubsystem->GetUserCloudInterface();
	if (!cloundPointerRef)
	{
		isLoadingSaveGame = false;
		return;
	}

	TSharedPtr<const FUniqueNetId> userIdRef = identityPointerRef->GetUniquePlayerId(0);
	if (!userIdRef.IsValid())
	{
		isLoadingSaveGame = false;
		return;
	}


	TArray<uint8> data;
	cloundPointerRef->GetFileContents(*userIdRef, FileName, data);

	isLoadingSaveGame = false;

	ServerGameSlot.saveGame = Cast<UPlayerSaveGame>(ConvertUintToSaveGame(data));

	LoadSaveGameFinished();
	LoadSaveGameSuccessful(ServerGameSlot.saveGame->GetPercent(), ServerGameSlot.saveGame->level);
}

UPlayerSetting* UEOSGameInstance::GetPlayerSettings()
{
	return SettingsGameSlot.saveGame;
}

UPlayerSaveGame* UEOSGameInstance::GetPlayerSaveGame()
{
	return ServerGameSlot.saveGame;
}