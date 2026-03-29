// ------------------------------------------------------------------------------
// File Name: FrontendFunctionLibrary.cpp
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2026-03-13
// Date Last Modified: 2026-03-22
// Copyright © 2026 Aidan Straker
// ------------------------------------------------------------------------------

#include "FrontendFunctionLibrary.h"

#include "FrontendGameInstance.h"
#include "FrontendSettings/FrontendGameUserSettings.h"
#include "FrontendSettings/FrontendDeveloperSettings.h"

TSoftClassPtr<UWidget_ActivatableWidgetBase> UFrontendFunctionLibrary::GetFrontendSoftWidgetClassByTag(
	UPARAM(meta = (Categories = "Frontend.Widget")) FGameplayTag InWidgetTag)
{
	// Retrieve our developer settings
	const UFrontendDeveloperSettings* FrontendDeveloperSettings = GetDefault<UFrontendDeveloperSettings>();
	
	checkf(FrontendDeveloperSettings->FrontendWidgetMap.Contains(InWidgetTag), TEXT("GetFrontendSoftWidgetClassByTag: Could not find the corresponding widget under the tag %s "), *InWidgetTag.ToString());
	
	return FrontendDeveloperSettings->FrontendWidgetMap.FindRef(InWidgetTag);
}

TSoftObjectPtr<UTexture2D> UFrontendFunctionLibrary::GetOptionsSoftImageByTag(FGameplayTag InImageTag)
{
	// Retrieve our developer settings
	const UFrontendDeveloperSettings* FrontendDeveloperSettings = GetDefault<UFrontendDeveloperSettings>();
	
	checkf(FrontendDeveloperSettings->OptionsScreenSoftImageMap.Contains(InImageTag), TEXT("Could not find an image associated with tag: %s"), *InImageTag.ToString());
	
	return FrontendDeveloperSettings->OptionsScreenSoftImageMap.FindRef(InImageTag);
}
