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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* camera;

	float refreshCounter = -1;
	FVector throwingVelo;

	bool refresh = false;

	float timer = 0;

	UGrenadeTrajectory();

	void FinishAttachment(USceneComponent* root, class UCameraComponent* pCamera);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void PredictGrenade(float cameraRot);
	void ThrowGrenade();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MUlToggleVisibility(bool visible);
	void MUlToggleVisibility_Implementation(bool visible);
};
