#include "Weapon.h"

UWeapon::UWeapon()
{
	PrimaryComponentTick.bCanEverTick = true;

	/*mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	mesh->RegisterComponent();*/
}

void UWeapon::BeginPlay()
{
	Super::BeginPlay();

}

void UWeapon::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}