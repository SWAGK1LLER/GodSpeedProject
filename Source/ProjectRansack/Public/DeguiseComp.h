#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon.h"
#include "Animation/AnimBlueprint.h"
#include "DeguiseComp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTRANSACK_API UDeguiseComp : public UActorComponent, public IWeapon
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float duration = 5;

	bool abilityUsed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMesh* thiefMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation)
	UAnimBlueprint* thiefAnim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMesh* officerMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation)
	UAnimBlueprint* officerAnim = nullptr;

	class USkeletalMesh* originalMesh = nullptr;
	UAnimBlueprint* originalAnim = nullptr;

	UDeguiseComp();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void finishAttachement();

	void MUlFire_Implementation() override;

	void MUlToggleVisibility_Implementation(bool visible) override;

	void UpdateUI_Implementation() override;

	void SetMesh(class USkeletalMesh* mesh);
};
