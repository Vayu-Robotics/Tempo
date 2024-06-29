// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class TempoROSBridge : ModuleRules
{
	public TempoROSBridge(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		// string Private = Path.Combine(ModuleDirectory, "Private", "ROSGenerated");
		// System.IO.Directory.CreateDirectory(Private);
		// PublicIncludePaths.AddRange(
		// 	new string[]
		// 	{
		// 		Private
		// 	}
		// );
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// Unreal
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// Tempo
				"TempoROS",
				"TempoSensors",
				"TempoGeographic",
				"TempoTime",
				"TempoVehicles",
				"TempoCamera",
				"TempoScripting",
				"rclcpp",
			}
			);
	}
}
