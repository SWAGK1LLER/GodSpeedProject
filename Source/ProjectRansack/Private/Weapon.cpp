#include "Weapon.h"

UWeapon::UWeapon(const FObjectInitializer& ObjectInitializer) : UStaticMeshComponent(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWeapon::BeginPlay()
{
	Super::BeginPlay();
	CoolDownCurrentTime = 0;
}

void UWeapon::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CoolDownCurrentTime -= DeltaTime;
}