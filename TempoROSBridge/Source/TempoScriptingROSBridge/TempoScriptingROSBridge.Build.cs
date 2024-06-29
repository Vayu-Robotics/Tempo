using UnrealBuildTool;

public class TempoScriptingROSBridge : ModuleRules
{
    public TempoScriptingROSBridge(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                // Unreal
                "Core",
                // Tempo
                "TempoScripting",
                "TempoROS",
                "rclcpp",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
            }
        );
    }
}