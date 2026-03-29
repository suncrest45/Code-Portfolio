// ------------------------------------------------------------------------------
// File Name: FrontendLoadingScreenSubsystem.h
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2026-03-27
// Date Last Modified: 2026-03-29
// Copyright © 2026 Aidan Straker
// ------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FrontendLoadingScreenSubsystem.generated.h"

// A game instance subsystem that handles displaying and removing the loading screen
UCLASS()
class SSFRONTEND_API UFrontendLoadingScreenSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
	
public:
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadingReasonUpdatedDelegate, const FString&, CurrentLoadingReason);
	// A delegate broadcast when the reason for why the loading screen is active has changed
	UPROPERTY(BlueprintAssignable)
	FOnLoadingReasonUpdatedDelegate OnLoadingReasonUpdated;
	
	//~ Begin USubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface
	
	//~ Begin FTickableGameObject Interface
	virtual UWorld* GetTickableGameObjectWorld() const override;
	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	//~ End FTickableGameObject Interface
	
private:
	
	/// @brief A function called when the map starts loading
	/// @param WorldContext The world context for the map being loaded
	/// @param MapName The name of the map being loaded
	void OnMapPreLoaded(const FWorldContext& WorldContext, const FString& MapName);

	/// @brief Called when a map has been fully loaded
	/// @param LoadedWorld The loaded world
	void OnMapPostLoaded(UWorld* LoadedWorld);
	
	/// @brief Determines if the loading screen should be displayed
	void TryUpdateLoadingScreen();

	/// @brief Checks if a loading screen is already displayed
	/// @return If there is a load screen already being displayed
	bool IsPreLoadScreenActive() const;

	/// @brief Should the loading screen be displayed
	/// @return Whether to show the loading screen
	bool ShouldShowLoadingScreen();

	/// @brief A helper function to determine if a load screen is needed
	/// @return Is there even a need to show the loading screen
	bool CheckTheNeedToShowLoadingScreen();
	
	/// @brief Try to display the loading screen if none is present
	void TryDisplayLoadingScreenIfNone();
	/// @brief Attempt to remove the loading screen
	void TryRemoveLoadingScreen();

	/// @brief Informs whoever is listening that the loading screen is/isn't visible
	/// @param bIsVisible Is the loading screen visible right now
	void NotifyLoadingScreenVisibilityChanged(bool bIsVisible);
	
	// Is the map currently being loaded
	bool bIsCurrentlyLoadingMap = false;
	// Caches the starting time of when loading has finished
	float HoldLoadingScreenStartTime = -1.0f;
	// The message we display on the loading screen
	FString CurrentLoadingReason;
	// The loading screen cached as a slate widget
	TSharedPtr<SWidget> CachedCreatedLoadingScreenWidget;
};
