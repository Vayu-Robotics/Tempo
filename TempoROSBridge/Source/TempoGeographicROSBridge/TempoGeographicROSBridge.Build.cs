// Copyright Tempo Simulation, LLC. All Rights Reserved

using UnrealBuildTool;

public class TempoGeographicROSBridge : ModuleRules
{
    public TempoGeographicROSBridge(ReadOnlyTargetRules Target) : base(Target)
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
                "TempoGeographic",
                "TempoScripting",
                "TempoScriptingROSBridge",
                "TempoROS",
                "rclcpp",
                "TempoROSBridgeShared",
            }
        );
    }
}