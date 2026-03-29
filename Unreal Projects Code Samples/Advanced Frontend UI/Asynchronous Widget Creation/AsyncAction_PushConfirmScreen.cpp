// ------------------------------------------------------------------------------
// File Name: AsyncAction_PushConfirmScreen.cpp
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2026-02-04
// Date Last Modified: 2026-03-22
// Copyright © 2026 Aidan Straker
// ------------------------------------------------------------------------------

#include "AsyncActions/AsyncAction_PushConfirmScreen.h"
#include "Subsystems/FrontendUISubsystem.h"


UAsyncAction_PushConfirmScreen* UAsyncAction_PushConfirmScreen::PushConfirmScreen(const UObject* WorldContextObject,
                                                                                  EConfirmScreenType ScreenType, 
                                                                                  FText InScreenTitle, 
                                                                                  FText InScreenMessage)
{
	if (GEngine)
	{
		// Get the world context object
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			UAsyncAction_PushConfirmScreen* Node = NewObject<UAsyncAction_PushConfirmScreen>();
			Node->CachedOwningWorld = World;
			Node->CachedScreenType = ScreenType;
			Node->CachedScreenTitle = InScreenTitle;
			Node->CachedScreenMessage = InScreenMessage;
			
			Node->RegisterWithGameInstance(World);
			return Node;
		}
	}
	
	return nullptr;
}

void UAsyncAction_PushConfirmScreen::Activate()
{
	// Get the UI subsystem and create the widget
	UFrontendUISubsystem::Get(CachedOwningWorld.Get())->PushConfirmScreenToModalStackAsync(
		CachedScreenType, 
		CachedScreenTitle, 
		CachedScreenMessage, 
		// This lambda broadcasts the event associated with an option on the pop-up screen
		[this](EConfirmScreenButtonType ClickedButtonType)
		{
			OnButtonClicked.Broadcast(ClickedButtonType);
			
			SetReadyToDestroy();
		}
	);
}
