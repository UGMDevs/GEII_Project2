// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GEII_Project2 : ModuleRules
{
	public GEII_Project2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
