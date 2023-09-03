#pragma once

#define TEAM_A_STR "TeamA"
#define TEAM_B_STR "TeamB"

UENUM(BlueprintType)
enum class ETeam : uint8
{
	A,
	B,
	NONE
};