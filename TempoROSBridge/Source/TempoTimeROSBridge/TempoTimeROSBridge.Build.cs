using UnrealBuildTool;

public class TempoTimeROSBridge : ModuleRules
{
    public TempoTimeROSBridge(ReadOnlyTargetRules Target) : base(Target)
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
                "TempoTime",
                "TempoScriptingROSBridge",
                "TempoScripting",
                "TempoROS",
                "rclcpp",
                "TempoROSBridgeShared",
                "TempoCoreShared",
            }
        );
    }
}