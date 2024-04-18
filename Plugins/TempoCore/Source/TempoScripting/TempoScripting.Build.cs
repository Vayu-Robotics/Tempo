// Copyright Tempo Simulation, LLC. All Rights Reserved.

using UnrealBuildTool;

public class TempoScripting : ModuleRules
{
	public TempoScripting(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[]
			{
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[]
			{
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"gRPC",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// Unreal
				"CoreUObject",
				"Engine",
				// Tempo
				"TempoCoreShared",
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
		
		PrivateDefinitions.Add("GRPC_DLL_EXPORTS=1");
		PrivateDefinitions.Add("GRPCXX_DLL_EXPORTS=1");
		PrivateDefinitions.Add("GPR_DLL_EXPORTS=1");
		PrivateDefinitions.Add("PROTOBUF_USE_DLLS=1");
		PrivateDefinitions.Add("LIBPROTOBUF_EXPORTS=1");
		PrivateDefinitions.Add("ABSL_BUILD_DLL=1");
	}
}
