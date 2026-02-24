// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CricketApex : ModuleRules
{
	public CricketApex(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				"CricketApex/Public"
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
				"CricketApex/Private",
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"EnhancedInput",
				"PhysicsCore",
				"Niagara",
				"MetahumanSDK",
				"OnlineSubsystem",
				"OnlineSubsystemUtils",
				"AIModule",
				"GameplayTasks",
				"AnimGraphRuntime",
				"Slate",
				"SlateCore",
				"UMG"
			}
		);

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				"OnlineSubsystemSteam",
			}
		);

		if (Target.bBuildEditor == true)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"UnrealEd",
					"EditorStyle",
					"PropertyEditor"
				}
			);
		}
	}
}
