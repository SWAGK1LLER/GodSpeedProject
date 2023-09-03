// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSetting.h"
#include "GameFramework/GameUserSettings.h"

void UPlayerSetting::InitializeSettings()
{
	UGameUserSettings* userSettings = UGameUserSettings::GetGameUserSettings();

	FString resX;
	FString resY;
	resolution.Split(FString("X"), &resX, &resY);

	FIntPoint point;
	point.X = FCString::Atoi(*resX);
	point.Y = FCString::Atoi(*resY);

	userSettings->SetScreenResolution(point);
	userSettings->ApplyResolutionSettings(false);

	userSettings->SetTextureQuality(GetEnumNumber(texture));
	userSettings->ApplySettings(false);

	userSettings->SetTextureQuality(GetEnumNumber(shadow));
	userSettings->ApplySettings(false);
}

int UPlayerSetting::GetEnumNumber(const FString& pValue)
{
	if (pValue == "High")
		return 2;
	else if (pValue == "Medium")
		return 1;
	else if (pValue == "Low")
		return 0;
	return 2;
}