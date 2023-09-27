// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectRansack : ModuleRules
{
	public ProjectRansack(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "OnlineSubsystem", "OnlineSubsystemEOS", "UMG", "SlateCore" });
	}
}
