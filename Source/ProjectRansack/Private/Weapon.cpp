#include "Weapon.h"

UWeapon::UWeapon(const FObjectInitializer& ObjectInitializer) : UStaticMeshComponent(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	MaxAmmo = ammo;
}

void UWeapon::BeginPlay()
{
	Super::BeginPlay();

}

void UWeapon::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	currentTime += DeltaTime;
}