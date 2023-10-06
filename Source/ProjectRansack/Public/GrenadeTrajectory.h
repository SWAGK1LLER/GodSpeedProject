#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "Grenade.h"
#include "Weapon.h"
#include "GrenadeTrajectory.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTRANSACK_API UGrenadeTrajectory : public UActorComponent, public IWeapon
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* mesh = nullptr;

	TSubclassOf<AGrenade>* GrenadeClass;
	enum GrenadeType previousGrenadeClass;
	enum GrenadeType CurrentGrenadeClass;

	class UTexture2D* uiTexture = nullptr;

	int* currentAmmo = nullptr;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FVector predictionPathOffset = FVector(0, 0, 90);

	FVector throwPosition;

	float refreshCounter = -1;
	FVector throwingVelo;

	bool refresh = false;

	float timer = 0;

	bool isThrowing = false;

	bool isLocalComp = false;

	class ABase3C* owner = nullptr;
	class AGamePlayerController* pcCache = nullptr;

	UGrenadeTrajectory();

	void SetIslocalController(bool local);

	void FinishAttachment(USceneComponent* root, class UCameraComponent* pCamera);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void EndThrow();

	void PredictGrenade();
	void ThrowGrenade();

	//UFUNCTION(Client, Reliable, BlueprintCallable)
	void CLTogglePredictPath(bool visible);
	//void CLTogglePredictPath_Implementation(bool visible);

	UFUNCTION(NetMulticast, Reliable, NotBlueprintable)
	virtual void MUlToggleVisibility(bool visible);
	virtual void MUlToggleVisibility_Implementation(bool visible);

	UFUNCTION(NetMulticast, Reliable, NotBlueprintable)
	virtual void MUlFire();
	virtual void MUlFire_Implementation();

	void UpdateUI_Implementation() override;

	bool IsSameGrenadeClass();
};
