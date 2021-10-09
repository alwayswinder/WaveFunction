// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WFCTest : ModuleRules
{
	public WFCTest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {            
                "GameplayAbilities",
                "GameplayTags",
                "GameplayTasks",
        });

        PrivateIncludePaths.AddRange(
            new string[] {
				// ... add other private include paths required here ...
				"WFCTest/Public/GameCore",
                "WFCTest/Public/Character",
                "WFCTest/Public/Character/Abilities",
                "WFCTest/Public/Character/Monster",
                "WFCTest/Public/Character/Player",

            }
            );
        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
