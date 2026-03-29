// ------------------------------------------------------------------------------
// File Name: FrontendLoadingScreenSubsystem.cpp
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2026-03-27
// Date Last Modified: 2026-03-29
// Copyright © 2026 Aidan Straker
// ------------------------------------------------------------------------------

#include "Subsystems/FrontendLoadingScreenSubsystem.h"
#include "PreLoadScreenManager.h"
#include "Blueprint/UserWidget.h"
#include "FrontendSettings/FrontendLoadingScreenSettings.h"
#include "Interfaces/FrontendLoadingScreenInterface.h"

bool UFrontendLoadingScreenSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// Check to see if the subsystem already exists
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> FoundClasses;
		GetDerivedClasses(GetClass(), FoundClasses);
		
		// If the found classes is empty we should create the subsystem
		return FoundClasses.IsEmpty();
	}
	
	return false;
}

void UFrontendLoadingScreenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	// On initialisation, bind the pre- and post-load delegates
	FCoreUObjectDelegates::PreLoadMapWithContext.AddUObject(this, &ThisClass::OnMapPreLoaded);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::OnMapPostLoaded);
}

void UFrontendLoadingScreenSubsystem::Deinitialize()
{
	// On shutdown, unbind the pre- and post-load delegates
	FCoreUObjectDelegates::PreLoadMapWithContext.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
}

UWorld* UFrontendLoadingScreenSubsystem::GetTickableGameObjectWorld() const
{
	// Used to determine the specific world this object is associated with. 
	// If this returns a valid world it will tick during that world's level tick. 
	if (const UGameInstance* OwningGameInstance = GetGameInstance())
	{
		return OwningGameInstance->GetWorld();
	}
	
	// If this returns null, it will tick during the general engine tick after all world ticks.
	return nullptr;
}

void UFrontendLoadingScreenSubsystem::Tick(float DeltaTime) { TryUpdateLoadingScreen(); }

ETickableTickType UFrontendLoadingScreenSubsystem::GetTickableTickType() const
{
	// Check if our subsystem is a template
	if (IsTemplate())
	{
		// We do not want to tick a template type
		return ETickableTickType::Never;
	}
	// Use IsTickable to determine if this should be ticked
	return ETickableTickType::Conditional;
}

bool UFrontendLoadingScreenSubsystem::IsTickable() const
{
	// If the game instance is valid, and it has a valid viewport, tick this game instance
	return GetGameInstance() && GetGameInstance()->GetGameViewportClient();
}

TStatId UFrontendLoadingScreenSubsystem::GetStatId() const
{
	// Return the tick performance of this class
	RETURN_QUICK_DECLARE_CYCLE_STAT(UFrontendLoadingScreenSubsystem, STATGROUP_Tickables);
}


void UFrontendLoadingScreenSubsystem::OnMapPreLoaded(const FWorldContext& WorldContext, const FString& MapName)
{
	// Make sure this is the correct game instance
	if (WorldContext.OwningGameInstance != GetGameInstance()) return;
	// Enable ticking 
	SetTickableTickType(ETickableTickType::Conditional);
	bIsCurrentlyLoadingMap = true;
	
	TryUpdateLoadingScreen();
}

void UFrontendLoadingScreenSubsystem::OnMapPostLoaded(UWorld* LoadedWorld)
{
	// if the loaded world is valid and has the same game instance, then loading has finished
	if (LoadedWorld && LoadedWorld->GetGameInstance() == GetGameInstance())
	{
		bIsCurrentlyLoadingMap = false;
	}
}

void UFrontendLoadingScreenSubsystem::TryUpdateLoadingScreen()
{
	// Check if there is any start up loading screen that is currently active
	if (IsPreLoadScreenActive()) return;
	
	// Check If we should show the loading screen
	if (ShouldShowLoadingScreen())
	{
		// Try to display loading screen
		TryDisplayLoadingScreenIfNone();
		// Notify the widget blueprints of the current loading reason
		OnLoadingReasonUpdated.Broadcast(CurrentLoadingReason);
	}
	else
	{
		// Try to remove the curren loading screen
		TryRemoveLoadingScreen();
		HoldLoadingScreenStartTime = -1.0f;
		// Notify that loading is complete
		NotifyLoadingScreenVisibilityChanged(false);
		// Disable Ticking
		SetTickableTickType(ETickableTickType::Never);
	}
}

bool UFrontendLoadingScreenSubsystem::IsPreLoadScreenActive() const
{
	// check if there are any Preload screens active
	if (FPreLoadScreenManager* PreLoadScreenManager = FPreLoadScreenManager::Get())
	{
		return PreLoadScreenManager->HasValidActivePreLoadScreen();
	}
	return false;
}

bool UFrontendLoadingScreenSubsystem::ShouldShowLoadingScreen()
{
	const UFrontendLoadingScreenSettings* LoadingScreenSettings = GetDefault<UFrontendLoadingScreenSettings>();
	
	// If the setting for showing in editor is false, we do not display the loading screen
	if (GIsEditor && !LoadingScreenSettings->bShowLoadingScreenInEditor)
	{
		return false;
	}
	
	// Check if the objects in the world need a loading screen
	if (CheckTheNeedToShowLoadingScreen())
	{
		GetGameInstance()->GetGameViewportClient()->bDisableWorldRendering = true;
		return true;
	}
	
	// If we clear all the other checks then the last one is just waiting for textures to fully load
	CurrentLoadingReason = TEXT("Waiting for Texture Streaming");
	
	// There is no need to show the loading screen, allow the world to be rendered to our viewport
	GetGameInstance()->GetGameViewportClient()->bDisableWorldRendering = false;
	
	const float CurrentTime = FPlatformTime::Seconds();
	
	// the value has never been set
	if (HoldLoadingScreenStartTime < 0.0f)
	{
		HoldLoadingScreenStartTime = CurrentTime;
	}
	
	// Calculate the amount of time that has elapsed
	const float ElaspedTime = CurrentTime - HoldLoadingScreenStartTime;
	
	// If the elapsed time is less than the extra time we give to display the loading screen, keep showing the screen
	if (ElaspedTime < LoadingScreenSettings->HoldLoadingScreenExtraSeconds)
	{
		return true;
	}
	
	return false;
}

bool UFrontendLoadingScreenSubsystem::CheckTheNeedToShowLoadingScreen()
{
	if (bIsCurrentlyLoadingMap)
	{
		CurrentLoadingReason = TEXT("Loading Level");
		return true;
	}
	// Get the current active world
	UWorld* OwningWorld = GetGameInstance()->GetWorld();
	
	// If there is none, then it is still being loaded
	if (!OwningWorld)
	{
		CurrentLoadingReason = TEXT("Loading World");
		return true;
	}
	
	// The world has not started playing yet
	if (!OwningWorld->HasBegunPlay())
	{
		CurrentLoadingReason = TEXT("World has not begun play yet");
		return true;
	}
	
	// Check if the player controller is ready
	if (!OwningWorld->GetFirstPlayerController())
	{
		CurrentLoadingReason = TEXT("Player controller is not ready yet");
		return true;
	}
	
	return false;
}

void UFrontendLoadingScreenSubsystem::TryDisplayLoadingScreenIfNone()
{
	// If there is already an active loading screen
	if (CachedCreatedLoadingScreenWidget) return;
	
	const UFrontendLoadingScreenSettings* LoadingScreenSettings = GetDefault<UFrontendLoadingScreenSettings>();
	// Get the loading screen widget
	TSubclassOf<UUserWidget> LoadedWidgetClass = LoadingScreenSettings->GetLoadingScreenWidgetClassChecked();
	// Since we are actively loading in a level/world, there is no player controller, so we cannot
	// Use the FrontendUISubsystem to create the widget
	UUserWidget* CreatedWidget = UUserWidget::CreateWidgetInstance(*GetGameInstance(), LoadedWidgetClass, NAME_None);
	// Make sure the new widget is valid
	check(CreatedWidget);
	
	CachedCreatedLoadingScreenWidget = CreatedWidget->TakeWidget();
	
	// This function takes in a slate widget, so we have to convert the widget we just created
	// Use a large ZOrder to make sure the loading screen is at the top
	GetGameInstance()->GetGameViewportClient()->AddViewportWidgetContent(
		CachedCreatedLoadingScreenWidget.ToSharedRef(), 1000);
	// The visibility of the loading screen has changed
	NotifyLoadingScreenVisibilityChanged(true);
}

void UFrontendLoadingScreenSubsystem::TryRemoveLoadingScreen()
{
	// If there is no cached loading screen widget then there is nothing to remove
	if (!CachedCreatedLoadingScreenWidget) return;
	// Remove the loading screen
	GetGameInstance()->GetGameViewportClient()->RemoveViewportWidgetContent(CachedCreatedLoadingScreenWidget.ToSharedRef());
	// Free the memory allocated to this shared pointer
	CachedCreatedLoadingScreenWidget.Reset();
}

void UFrontendLoadingScreenSubsystem::NotifyLoadingScreenVisibilityChanged(bool bIsVisible)
{
	for (ULocalPlayer* ExistingLocalPlayer : GetGameInstance()->GetLocalPlayers())
	{
		if (!ExistingLocalPlayer) continue;
		
		// Get the player controller
		if (APlayerController* PC = ExistingLocalPlayer->GetPlayerController(GetGameInstance()->GetWorld()))
		{
			// Query if the player controller implements the interface
			// Use the interface to call the function to notify the controller of the loading screen status
			if (PC->Implements<UFrontendLoadingScreenInterface>())
			{
				// Use the interface to notify the player controllers of the status of the loading screen
				if (bIsVisible)
				{
					IFrontendLoadingScreenInterface::Execute_OnLoadingScreenActivated(PC);
				}
				else
				{
					IFrontendLoadingScreenInterface::Execute_OnLoadingScreenDeactivated(PC);
				}
			}
			
			// Check if the owning pawn implements the interface, and notify it of the status of the loading screen
			if (APawn* OwningPawning = PC->GetPawn())
			{
				if (OwningPawning->Implements<UFrontendLoadingScreenInterface>())
				{
					// Use the interface to notify the owning pawn of the status of the loading screen
					if (bIsVisible)
					{
						IFrontendLoadingScreenInterface::Execute_OnLoadingScreenActivated(PC);
					}
					else
					{
						IFrontendLoadingScreenInterface::Execute_OnLoadingScreenDeactivated(PC);
					}
				}
			}
		}
		// The code for notifying other objects in the world goes here
	}
}
