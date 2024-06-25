// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TempoROSBridge : ModuleRules
{
	public TempoROSBridge(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
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
