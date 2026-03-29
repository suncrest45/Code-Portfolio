// ------------------------------------------------------------------------------
// File Name: FrontendLoadingScreenInterface.h
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2026-03-28
// Date Last Modified: 2026-03-29
// Copyright © 2026 Aidan Straker
// ------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FrontendLoadingScreenInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, MinimalAPI)
class UFrontendLoadingScreenInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SSFRONTEND_API IFrontendLoadingScreenInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	UFUNCTION(BlueprintNativeEvent)
	void OnLoadingScreenActivated();
	//virtual void OnLoadingScreenActivated_Implementation();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnLoadingScreenDeactivated();
	//virtual void OnLoadingScreenDeactivated_Implementation();
};
