// Copyright Tempo Simulation, LLC. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class TempoTarget : TargetRules
{
	public TempoTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("Tempo");
		
		if (Platform == UnrealTargetPlatform.Win64)
		{
			ToolChainName = "TempoVCToolChain";
		}
		else if (Platform == UnrealTargetPlatform.Mac)
		{
			ToolChainName = "TempoMacToolChain";
		}
		else if (Platform == UnrealTargetPlatform.Linux)
		{
			ToolChainName = "TempoLinuxToolChain";
		}
	}
}
