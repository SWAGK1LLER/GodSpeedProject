#include "Equipement.h"
#include "Gun.h"
#include "Baton.h"
#include "Base3C.h"
#include "SensorGadgetOfficerComponent.h"
#include "GrenadeTrajectory.h"
#include "InvisibleCloak.h"
#include "CameraComp.h"
#include "Camera/CameraComponent.h"
#include "Decoy.h"
#include "GamePlayerController.h"
#include "EnhancedInputComponent.h"
#include "Engine/Texture2D.h"
#include "DeguiseComp.h"
#include "EMP.h"

UEquipement::UEquipement()
{
	PrimaryComponentTick.bCanEverTick = true;

	StunWeapon = CreateDefaultSubobject<UGun>(TEXT("StunWeapon"));
	StunStick = CreateDefaultSubobject<UBaton>(TEXT("StunBaton"));
	sensorGadgetOfficer = CreateDefaultSubobject<USensorGadgetOfficerComponent>(TEXT("Sensor Gadget Component"));
	GrenateTrajectory = CreateDefaultSubobject<UGrenadeTrajectory>(TEXT("Grenate Trajectory"));
	decoyGadget = CreateDefaultSubobject<UDecoy>(TEXT("Decoy Gadget"));
	invisibleCloak = CreateDefaultSubobject<UInvisibleCloak>(TEXT("Invisible Cloak"));
	deguisement = CreateDefaultSubobject<UDeguiseComp>(TEXT("Deguisement"));
	emp = CreateDefaultSubobject<UEMP>(TEXT("Emp"));

	equippedWeapon = StunWeapon;
}

void UEquipement::FinishAttachement(class USceneComponent* root)
{
	pawn = Cast<ABase3C>(root->GetOwner());
	StunWeapon->pawn = pawn;
	StunStick->pawn = pawn;
	sensorGadgetOfficer->pawn = pawn;
	GrenateTrajectory->pawn = pawn;
	decoyGadget->pawn = pawn;
	invisibleCloak->pawn = pawn;
	deguisement->pawn = pawn;
	emp->pawn = pawn;

	StunWeapon->AttachToComponent(root, FAttachmentTransformRules::KeepRelativeTransform);
	StunStick->AttachToComponent(root, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("RightHandSocket"));
	decoyGadget->FinishSetup();
	GrenateTrajectory->FinishAttachment(root, pawn->cameraComponent->camera);
	deguisement->finishAttachement();
}

void UEquipement::SetController(AGamePlayerController* pController)
{
	controller = pController;
	StunWeapon->controller = pController;
	StunStick->controller = pController;
	sensorGadgetOfficer->controller = pController;
	GrenateTrajectory->controller = pController;
	decoyGadget->controller = pController;
	invisibleCloak->controller = pController;
	deguisement->controller = pController;
	emp->controller = pController;
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

void UEquipement::EquipWeapon_Implementation(const TScriptInterface<IWeapon>& nextWeapon)
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
};

void UEquipement::UpdateUI()
{
	IWeapon::Execute_UpdateUI(equippedWeapon->_getUObject());
}

void UEquipement::Fire()
{
	IWeapon::Execute_MUlFire(equippedWeapon->_getUObject());

	Cast<ABase3C>(GetOwner())->TryGeneratingOverlapEvent();
};

void UEquipement::BindBeltKey(UEnhancedInputComponent* input, FBase3CTable* table)
{
	input->BindAction(table->Belt1Action, ETriggerEvent::Started, this, &UEquipement::EquipeBelt1);
	input->BindAction(table->Belt2Action, ETriggerEvent::Started, this, &UEquipement::EquipeBelt2);
	input->BindAction(table->Belt3Action, ETriggerEvent::Started, this, &UEquipement::EquipeBelt3);
}

void UEquipement::EquipeBelt1()
{
	if (!(utilityBelt.Num() >= 1))
	{
		if (controller != nullptr)
		{
			controller->SREquipWeapon(this, StunWeapon);
		}
		return;
	}

	const TScriptInterface<IWeapon>& weapon = GetWeaponFromEnum(utilityBelt[0]);
	if (weapon == equippedWeapon)
	{
		if ((weapon == GrenateTrajectory && !GrenateTrajectory->IsSameGrenadeClass()) || (weapon == GrenateTrajectory && *GrenateTrajectory->currentAmmo == 0))
		{
			IWeapon::Execute_UpdateUI(weapon.GetObject());
		}
		else
		{
			IWeapon::Execute_UpdateUI(StunWeapon->_getUObject());
			GrenateTrajectory->CurrentGrenadeClass = GrenadeType::None;

			if (controller != nullptr)
			{
				controller->SREquipWeapon(this, StunWeapon);
			}
		}

		return;
	}
	else
	{
		IWeapon::Execute_UpdateUI(weapon.GetObject());

		if (controller != nullptr)
		{
			controller->SREquipWeapon(this, weapon);
		}
	}
}

void UEquipement::EquipeBelt2()
{
	if (!(utilityBelt.Num() >= 2))
	{
		if (controller != nullptr)
		{
			controller->SREquipWeapon(this, StunWeapon);
		}
		return;
	}

	const TScriptInterface<IWeapon>& weapon = GetWeaponFromEnum(utilityBelt[1]);
	if (weapon == equippedWeapon)
	{
		if ((weapon == GrenateTrajectory && !GrenateTrajectory->IsSameGrenadeClass()) || (weapon == GrenateTrajectory && *GrenateTrajectory->currentAmmo == 0))
		{
			IWeapon::Execute_UpdateUI(weapon.GetObject());
		}
		else
		{
			IWeapon::Execute_UpdateUI(StunWeapon->_getUObject());
			GrenateTrajectory->CurrentGrenadeClass = GrenadeType::None;
			if (controller != nullptr)
			{
				controller->SREquipWeapon(this, StunWeapon);
			}
		}
	}
	else
	{
		IWeapon::Execute_UpdateUI(weapon.GetObject());
		if (controller != nullptr)
		{
			controller->SREquipWeapon(this, weapon);
		}
	}
}

void UEquipement::EquipeBelt3()
{
	if (!(utilityBelt.Num() >= 3))
	{
		if (controller != nullptr)
		{
			controller->SREquipWeapon(this, StunWeapon);
		}
		return;
	}

	const TScriptInterface<IWeapon>& weapon = GetWeaponFromEnum(utilityBelt[2]);
	if (weapon == equippedWeapon)
	{
		if ((weapon == GrenateTrajectory && !GrenateTrajectory->IsSameGrenadeClass()) || (weapon == GrenateTrajectory && *GrenateTrajectory->currentAmmo == 0))
		{
			IWeapon::Execute_UpdateUI(weapon.GetObject());
		}
		else
		{
			IWeapon::Execute_UpdateUI(StunWeapon->_getUObject());
			GrenateTrajectory->CurrentGrenadeClass = GrenadeType::None;
			if (controller != nullptr)
			{
				controller->SREquipWeapon(this, StunWeapon);
			}
		}
	}
	else
	{
		IWeapon::Execute_UpdateUI(weapon.GetObject());
		if (controller != nullptr)
		{
			controller->SREquipWeapon(this, weapon);
		}
	}
}

const TScriptInterface<IWeapon> UEquipement::GetWeaponFromEnum(EquipementPossibility weapon)
{
	switch (weapon)
	{
		case _None : return StunWeapon;

		case SmokeGrenade:	
							GrenateTrajectory->previousGrenadeClass = GrenateTrajectory->CurrentGrenadeClass;
							GrenateTrajectory->CurrentGrenadeClass = GrenadeType::Smoke;
							GrenateTrajectory->uiTexture = AllGrenade[GrenadeType::Smoke].uiImage;
							controller->SetGrenade(this, GrenadeType::Smoke);
							return GrenateTrajectory;

		case StunGrenade:	
							GrenateTrajectory->previousGrenadeClass = GrenateTrajectory->CurrentGrenadeClass;
							GrenateTrajectory->CurrentGrenadeClass = GrenadeType::Stun;
							GrenateTrajectory->uiTexture = AllGrenade[GrenadeType::Stun].uiImage;
							controller->SetGrenade(this, GrenadeType::Stun);
							return GrenateTrajectory;

		case HoloDecoy: return decoyGadget;
		case InvisibleCloak: return invisibleCloak;
		case Deguisement: return deguisement;
		case EMP: return emp;
		default: return StunWeapon;
	}
}

void UEquipement::SetGrenadeType_Implementation(GrenadeType grenadeType)
{
	GrenateTrajectory->GrenadeClass = &AllGrenade[grenadeType].type;
	GrenateTrajectory->currentAmmo = &AllGrenade[grenadeType].ammo;
}