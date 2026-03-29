// ------------------------------------------------------------------------------
// File Name: Widget_PrimaryLayout.cpp
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2026-01-25
// Date Last Modified: 2026-03-22
// Copyright © 2026 Aidan Straker
// ------------------------------------------------------------------------------

#include "Widgets/Widget_PrimaryLayout.h"

UCommonActivatableWidgetContainerBase* UWidget_PrimaryLayout::FindWidgetStackByTag(const FGameplayTag& InTag) const
{
	checkf(RegisteredWidgetStackMap.Contains(InTag), TEXT("Can not find the widget stack by the tag %s"), *InTag.ToString());
	return RegisteredWidgetStackMap.FindRef(InTag);
}

void UWidget_PrimaryLayout::RegisterWidgetStack(UPARAM(meta = (Categories = "Frontend.WidgetStack")) FGameplayTag InStackTag, UCommonActivatableWidgetContainerBase* InStack)
{
	// We do not want to add containers while in editor
	if (!IsDesignTime())
	{
		// We do not want duplicates of the stack inside the map
		if (!RegisteredWidgetStackMap.Contains(InStackTag))
		{
			RegisteredWidgetStackMap.Add(InStackTag, InStack);
		}
	}
}
