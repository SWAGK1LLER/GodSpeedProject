#include "Equipement.h"
#include "Gun.h"
#include "Baton.h"
#include "Base3C.h"
#include "SensorGadgetOfficerComponent.h"
#include "GrenadeTrajectory.h"
#include "CameraComp.h"
#include "Camera/CameraComponent.h"
#include "Decoy.h"
#include <Thief.h>

UEquipement::UEquipement()
{
	PrimaryComponentTick.bCanEverTick = true;

	StunWeapon = CreateDefaultSubobject<UGun>(TEXT("StunWeapon"));
	
	StunStick = CreateDefaultSubobject<UBaton>(TEXT("StunBaton"));
	
	sensorGadgetOfficer = CreateDefaultSubobject<USensorGadgetOfficerComponent>(TEXT("Sensor Gadget Component"));

	GrenateTrajectory = CreateDefaultSubobject<UGrenadeTrajectory>(TEXT("Grenate Trajectory"));
	
	decoyGadget = CreateDefaultSubobject<UDecoy>(TEXT("Decoy Gadget"));

	equippedWeapon = StunWeapon;
}

void UEquipement::FinishAttachement(class USceneComponent* root)
{
	StunWeapon->SetupAttachment(root);
	StunStick->SetupAttachment(root, FName("RightHandSocket"));
	GrenateTrajectory->FinishAttachment(root, Cast<ABase3C>(GetOwner())->cameraComponent->camera);
	decoyGadget->SetupComp(Cast<AThief>(GetOwner()));
}

void UEquipement::BeginPlay()
{
	Super::BeginPlay();
}

void UEquipement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	IWeapon::Execute_Tick(equippedWeapon->_getUObject(), DeltaTime);
}

void UEquipement::EquipWeapon(const TScriptInterface<IWeapon>& nextWeapon)
{
	if (nextWeapon.GetInterface() == equippedWeapon)
	{
		IWeapon::Execute_MUlToggleVisibility(equippedWeapon->_getUObject(), false);
		IWeapon::Execute_MUlToggleVisibility(StunWeapon->_getUObject(), true);

		equippedWeapon = StunWeapon;
	}
	else
	{
		IWeapon::Execute_MUlToggleVisibility(equippedWeapon->_getUObject(), false);
		IWeapon::Execute_MUlToggleVisibility(nextWeapon.GetObject(), true);

		equippedWeapon = nextWeapon.GetInterface();
	}

	IWeapon::Execute_UpdateUI(equippedWeapon->_getUObject());
};

void UEquipement::Fire()
{
	IWeapon::Execute_MUlFire(equippedWeapon->_getUObject());

	Cast<ABase3C>(GetOwner())->TryGeneratingOverlapEvent();
};