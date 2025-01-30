// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UnicodeBrowser : ModuleRules
{
	public UnicodeBrowser(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		// these should be disabled when debugging with debugger
		OptimizeCode = CodeOptimization.Always;
		OptimizationLevel = OptimizationMode.Speed;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
		);
		AddEngineThirdPartyPrivateStaticDependencies(Target, "ICU");

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"ApplicationCore",
				"CoreUObject",
				"DeveloperSettings",
				"EditorFramework",
				"Engine",
				"InputCore",
				"Json", // required for importer
				"Projects",
				"PropertyEditor",
				"SceneOutliner",
				"Slate",
				"SlateCore",
				"ToolMenus",
				"ToolWidgets",
				"UnrealEd",
			}
		);
	}
}