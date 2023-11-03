// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TcpDedicatedConn : ModuleRules
{
	public TcpDedicatedConn(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "Sockets", "Networking" });
	}
}
