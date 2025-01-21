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
		
		PublicIncludePaths.AddRange(
			new string[]
			{
				// ... add public include paths required here ...
			}
		);


		PrivateIncludePaths.AddRange(
			new string[]
			{
				// ... add other private include paths required here ...
			}
		);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
		);
		AddEngineThirdPartyPrivateStaticDependencies(Target, "ICU");

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"ApplicationCore",
				"CoreUObject",
				"EditorFramework",
				"SceneOutliner",
				"Engine",
				"InputCore",
				"Projects",
				"PropertyEditor",
				"Slate",
				"SlateCore",
				"ToolMenus",
				"ToolWidgets",
				"UnrealEd",
				"Json", // required for importer
				"DeveloperSettings",
			}
		);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
		PublicDefinitions.Add("UDATA_DEBUG=1");
	}
}