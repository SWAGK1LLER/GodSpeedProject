#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "Grenade.h"
#include "GrenadeTrajectory.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTRANSACK_API UGrenadeTrajectory : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AGrenade> GrenadeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UNiagaraComponent* niagara = nullptr;

	TArray<FVector> PredictionPositions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector throwVelocity = FVector(800, 800, 300);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float simulationDuree = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float radius = 10;

	float refreshCounter = -1;
	FVector throwingVelo;

	UGrenadeTrajectory();

	void FinishAttachment(USceneComponent* root);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void PredictGrenade(float cameraRot);
	void ThrowGrenade();
};
