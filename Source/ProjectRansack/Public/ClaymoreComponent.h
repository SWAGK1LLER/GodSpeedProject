#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon.h"
#include "ClaymoreComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTRANSACK_API UClaymoreComponent : public UActorComponent, public IWeapon
{
	GENERATED_BODY()

public:	
	FVector MeshNormal;
	FVector Location;
	FRotator Rotation;

	UPROPERTY(editAnywhere, BlueprintReadWrite)
	float viewReach = 1000;

	UPROPERTY(editAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* preview;

	UPROPERTY(editAnywhere, BlueprintReadWrite)
	TSubclassOf<class AClaymore> ClaymoreTospawn;

	UClaymoreComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(NetMulticast, Reliable, NotBlueprintable)
	virtual void MUlToggleVisibility(bool visible);
	virtual void MUlToggleVisibility_Implementation(bool visible);

	UFUNCTION(NetMulticast, Reliable, NotBlueprintable)
	virtual void MUlFire();
	virtual void MUlFire_Implementation();

	void UpdateUI_Implementation() override;

	void updatePosing(FVector CamLocation, FVector CamForward);
};
