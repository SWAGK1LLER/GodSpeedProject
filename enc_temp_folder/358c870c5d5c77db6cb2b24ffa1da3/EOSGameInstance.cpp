// Fill out your copyright notice in the Description page of Project Settings.


#include "EOSGameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include <Kismet/GameplayStatics.h>
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlinePartyInterface.h"

const FName FSessionName = FName("Test Session");

UEOSGameInstance::UEOSGameInstance()
{

}

void UEOSGameInstance::Init()
{
	Super::Init();

	OnlineSubsystem = IOnlineSubsystem::Get();
}

//Login
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
		LoginFail();
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
	SessionSettings.NumPublicConnections = 3;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bUseLobbiesVoiceChatIfAvailable = true;
	SessionSettings.bAllowInvites = true;

	SessionSettings.Set(SEARCH_KEYWORDS, FString("ProjectRansackLobby"), EOnlineDataAdvertisementType::ViaOnlineService);

	if (pTravel)
		SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnCreateSessionComplete);
	else
		bHasSession = true;
	
	SessionPtr->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &UEOSGameInstance::OnFriendAcceptInvite);
	SessionPtr->CreateSession(0, FSessionName, SessionSettings);

}

void UEOSGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccess)
{
	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SessionPtr->ClearOnCreateSessionCompleteDelegates(this);
	GetWorld()->ServerTravel(FString("/Game/Maps/Lobby?listen"), false);
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
//--------



void UEOSGameInstance::DeleteSession()
{
	if (!bIsLogin)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot Destroy Session: Not Logged In"));
		return;
	}

	if (!OnlineSubsystem)
		return;

	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnDeleteSessionComplete);
	SessionPtr->DestroySession(FSessionName);
}

void UEOSGameInstance::OnDeleteSessionComplete(FName SessionName, bool bWasSuccess)
{
	if (!OnlineSubsystem)
		return;

	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SessionPtr->ClearOnDestroySessionCompleteDelegates(this);
}

void UEOSGameInstance::FindSession()
{
	if (!bIsLogin)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot Find Session: Not Logged In"));
		return;
	}

	if (!OnlineSubsystem)
		return;

	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SearchSettings = MakeShareable(new FOnlineSessionSearch()); 
	SearchSettings->MaxSearchResults = 100;
	SearchSettings->QuerySettings.Set(SEARCH_KEYWORDS, FString("ProjectRansackLobby"), EOnlineComparisonOp::Equals);
	SearchSettings->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

	SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnFindSessionComplete);
	SessionPtr->FindSessions(0, SearchSettings.ToSharedRef());
}

void UEOSGameInstance::OnFindSessionComplete(bool bWasSucess)
{
	UE_LOG(LogTemp, Warning, TEXT("Search was : %d"), bWasSucess);

	if (!bWasSucess)
		return;
	
	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SessionPtr->ClearOnFindSessionsCompleteDelegates(this);

	UE_LOG(LogTemp, Warning, TEXT("Found %d lobbies"), SearchSettings->SearchResults.Num());

	if (SearchSettings->SearchResults.Num() == 0)
		return;

	SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnJoinSessionComplete);
	SessionPtr->JoinSession(0, FSessionName, SearchSettings->SearchResults[0]);
}

void UEOSGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{

	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (!SessionPtr)
		return;

	SessionPtr->ClearOnJoinSessionCompleteDelegates(this);

	FString ConnectionInfo = FString();
	SessionPtr->GetResolvedConnectString(FSessionName, ConnectionInfo);

	if (ConnectionInfo.IsEmpty())
		return;

	APlayerController* pc = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!pc)
		return;

	pc->ClientTravel(ConnectionInfo, ETravelType::TRAVEL_Absolute);
}
