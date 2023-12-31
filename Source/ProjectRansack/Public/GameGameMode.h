#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "EnumTeam.h"
#include <Thief.h>
#include "GameGameMode.generated.h"

#define MINUTE 60
#define SECOND 60
#define MAX_PLAYER 6
#define MAX_ROUND 2

DECLARE_DELEGATE(FOnCustomTimerFinish);
DECLARE_DELEGATE(FOnCustomTimerTick);

struct CustomTimer
{
	FOnCustomTimerTick onTick;
	FOnCustomTimerFinish onFinish;
	float currentTime = 0;
	bool used = false;
};

UCLASS()
class PROJECTRANSACK_API AGameGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int connectionWaiter = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RoundTimer = 10 * MINUTE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int warmupTimer = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int EndTimer = 5;

	int spawnedPlayer = 0;

	CustomTimer eventTimer;
	int TotalRound = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ScoreTeamA = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ScoreTeamB = 0;

	bool roundStarted = false;

	int TotalPlayerToStart = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	ETeam debugSpawnedTeam = ETeam::A;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool debugEnabled = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool debugForceSpawnBothSide = false;
	static int teamCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABase3C> ThiefClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABase3C> OfficerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class APlayerController*> PC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class ABase3C*> TeamA; //Thief

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class ABase3C*> TeamB; //Security

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<class APlayerController*, ETeam> PlayerTeams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class AGamePlayerStart*> PlayerSpawn;
	
	FString extractSpawnTag = TEAM_A_STR;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void PostLogin(class APlayerController* NewPlayer) override;

	virtual void Logout(class AController* ExitPlayer) override;

	void SpawnPlayer(ETeam team, class APlayerController* NewPlayer);

	void FindSpawn(class APlayerController* NewPlayer, const ETeam& team, FTransform& Location, TSubclassOf<ABase3C>& ActorClass);

	void SendPlayerName();

	void SendTeamData();

	void UpdateTeamDuffleBag();

	void SpawnParticle(class UParticleSystem* particleEffect, const FTransform& position, const float& duration);

	void beginArrestThief(bool pArrest, AThief* pThief, AOfficer* pOfficer);

	UFUNCTION()
	void StartWaitingConnection();

	UFUNCTION()
	void StartWarmup();

	UFUNCTION()
	void StartRound();

	UFUNCTION(BlueprintCallable)
	void EndRound();

	UFUNCTION()
	void RestartRound();

	UFUNCTION()
	void EndGame();

	UFUNCTION(BlueprintCallable)
	FString getRemainingTimeText();
	FString convertTimeToText();

	void AddToScore(int pValue, int& pScore);
	void RemoveToScore(int pValue, int& pScore);

	void FreezeInput(float duration, ABase3C* actor, FVector DamageActorLocation);

	void FreezeCamera(AActor* actor);

	UFUNCTION(BlueprintCallable)
	void ArrestThief(ABase3C* other);

	void SetCustomTimerCallback(float time, const FOnCustomTimerTick& onTick, const FOnCustomTimerFinish& onFinish);

	void PingThiefToAllOfficer(class ASensorGadget* sensor, class AThief* thief);

	void stealMagnetCard(class AThief* thief, class AOfficer* officer);
};
