// ------------------------------------------------------------------------------
// File Name: FrontendLoadingScreenSettings.h
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2026-03-27
// Date Last Modified: 2026-03-29
// Copyright © 2026 Aidan Straker
// ------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "FrontendLoadingScreenSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig)
class SSFRONTEND_API UFrontendLoadingScreenSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	
	/// @brief Converts the soft widget class into something we cna use in the viewport
	/// @return The soft widget class converted to a subclass
	TSubclassOf<UUserWidget> GetLoadingScreenWidgetClassChecked() const;
	
	// The widget we will be using as a loading screen
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen | Settings")
	TSoftClassPtr<UUserWidget> SoftLoadingScreenWidgetClass;
	
	// How long to keep the loading screen on screen after loading has finished
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen | Settings")
	float HoldLoadingScreenExtraSeconds = 3.0f;
	
	// Should the loading screen be displayed in editor
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen | Settings")
	bool bShowLoadingScreenInEditor = false;
	
};
