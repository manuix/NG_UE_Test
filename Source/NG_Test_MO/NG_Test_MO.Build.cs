// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NG_Test_MO : ModuleRules
{
	public NG_Test_MO(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "SlateCore" });
	}
}
