// ------------------------------------------------------------------------------
// File Name: OptionsDataRegistry.cpp
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2026-03-27
// Date Last Modified: 2026-03-29
// Copyright © 2026 Aidan Straker
// ------------------------------------------------------------------------------

#include "Widgets/Options/OptionsDataRegistry.h"

#include "Widgets/Options/OptionsDataInteractionHelper.h"
#include "FrontendSettings/FrontendGameUserSettings.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"
#include "Widgets/Options/DataObjects/ListDataObject_Scalar.h"
#include "Widgets/Options/DataObjects/ListDataObject_StringResolution.h"
#include "FrontendFunctionLibrary.h"
#include "SSFrontendGameplayTags.h"
#include "Internationalization/StringTableRegistry.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "FrontendDebugHelper.h"
#include "Widgets/Options/DataObjects/ListDataObject_KeyRemap.h"

// A macro that creates paths to getters and setters for user settings
#define MAKE_OPTIONS_DATA_CONTROL(SetterOrGetterFuncName) \
	MakeShared<FOptionsDataInteractionHelper>(GET_FUNCTION_NAME_STRING_CHECKED(UFrontendGameUserSettings, SetterOrGetterFuncName))

#define GET_DESCRIPTION(InKey) LOCTABLE("/Game/UI/StringTables/ST_OptionsScreenDescription.ST_OptionsScreenDescription", InKey)

void UOptionsDataRegistry::InitOptionsDataRegistry(ULocalPlayer* InLocalOwningPlayer)
{
	InitGameplayCollectionTab();
	InitAudioCollectionTab();
	InitVideoCollectionTab();
	InitControlCollectionTab(InLocalOwningPlayer);
	InitAccessibilityCollectionTab();
}

TArray<UListDataObject_Base*> UOptionsDataRegistry::GetListSourceItemsByTabID(const FName& InSelectedTabID) const
{
	// Search the array of registered tabs for a tab with a matching ID, this will give us a pointer to the tab
	UListDataObject_Collection* const* FoundTabCollectionPtr = RegisteredOptionsTabCollections.FindByPredicate(
		[InSelectedTabID](const UListDataObject_Collection* AvailableTabCollections)->bool
		{
			return AvailableTabCollections->GetDataID() == InSelectedTabID;
		}
	);
	
	// Perform a check to make sure the pointer to the tab is valid
	checkf(FoundTabCollectionPtr, TEXT("No valid tab found under the ID: %s"), *InSelectedTabID.ToString());
	
	// Dereference the pointer to the tab
	const UListDataObject_Collection* FoundTabCollection = *FoundTabCollectionPtr;
	
	TArray<UListDataObject_Base*> AllChildListItems;
	
	for (UListDataObject_Base* ChildListData : FoundTabCollection->GetAllChildListData())
	{
		if (!ChildListData) continue;
		
		// Get the surface level settings data objects (these could be individual settings or collections of settings)
		AllChildListItems.Add(ChildListData);
		// Check if the data objects have any children
		if (ChildListData->HasAnyChildListData())
		{
			FindChildListDataRecursively(ChildListData, AllChildListItems);
		}
	}
	
	return AllChildListItems;
}

void UOptionsDataRegistry::InitGameplayCollectionTab()
{
	UListDataObject_Collection* GameplayTabCollection = NewObject<UListDataObject_Collection>();
	GameplayTabCollection->SetDataID(FName("GameplayTabCollection"));
	GameplayTabCollection->SetDataDisplayName(FText::FromString(TEXT("Gameplay")));
	
	
	// Game Difficulty Setting
	{
		// Construct the data object that represents the game difficulty setting
		UListDataObject_String* GameDifficulty = NewObject<UListDataObject_String>();
		GameDifficulty->SetDataID(FName("GameDifficulty"));
		
		GameDifficulty->SetDataDisplayName(FText::FromString(TEXT("Difficulty")));
		
		GameDifficulty->SetDescriptionRichText(FText::FromString(TEXT(
			"Adjusts the difficulty of the game experience.\n\n<Bold>Easy:</> Focuses on the story experience. Provides the most relaxing combat experience.\n\n<Bold>Normal:</> Offers a slightly harder combat experience.\n\n<Bold>Hard:</> Offers a much more challenging combat experience.\n\n<Bold>Very Hard:</> Provides the most challenging combat experience. Not recommended for first playthroughs."
			)));
		
		// Create game difficulty options
		GameDifficulty->AddDynamicOptions(TEXT("Easy"), FText::FromString(TEXT("Easy")));
		GameDifficulty->AddDynamicOptions(TEXT("Normal"), FText::FromString(TEXT("Normal")));
		GameDifficulty->AddDynamicOptions(TEXT("Hard"), FText::FromString(TEXT("Hard")));
		GameDifficulty->AddDynamicOptions(TEXT("Very Hard"), FText::FromString(TEXT("Very Hard")));
		// Set the default value
		GameDifficulty->SetDefaultValueFromString(TEXT("Normal"));
		// Set the paths for the getter and setter
		GameDifficulty->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetCurrentGameDifficulty));
		GameDifficulty->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetCurrentGameDifficulty));
		GameDifficulty->SetShouldApplySettingsImmediately(true);
		
		GameplayTabCollection->AddChildListData(GameDifficulty);
	}
	
	// Test Item Setting
	{
		UListDataObject_String* TestItem = NewObject<UListDataObject_String>();
		TestItem->SetDataID(FName("TestItem"));
		TestItem->SetDataDisplayName(FText::FromString(TEXT("Test Image Item")));
		
		TestItem->SetSoftDescriptionImage(UFrontendFunctionLibrary::GetOptionsSoftImageByTag(SSFrontendGameplayTags::Frontend_Image_TestImage));
		
		GameplayTabCollection->AddChildListData(TestItem);
	}
	
	RegisteredOptionsTabCollections.Add(GameplayTabCollection);
}

void UOptionsDataRegistry::InitAudioCollectionTab()
{
	UListDataObject_Collection* AudioTabCollection = NewObject<UListDataObject_Collection>();
	AudioTabCollection->SetDataID(FName("AudioTabCollection"));
	AudioTabCollection->SetDataDisplayName(FText::FromString(TEXT("Audio")));
	
	// Volume Category
	{
		// Create an audio settings category called volume
		UListDataObject_Collection* VolumeCollection = NewObject<UListDataObject_Collection>();
		VolumeCollection->SetDataID(FName("VolumeCollection"));
		VolumeCollection->SetDataDisplayName(FText::FromString(TEXT("Volume")));
		
		// Add the volume collection as a child of the audio tab collection
		AudioTabCollection->AddChildListData(VolumeCollection);
		
		// Overall Volume
		{
			UListDataObject_Scalar* OverallVolume = NewObject<UListDataObject_Scalar>();
			OverallVolume->SetDataID(FName("OverallVolume"));
			OverallVolume->SetDataDisplayName(FText::FromString(TEXT("Overall Volume")));
			OverallVolume->SetDescriptionRichText(FText::FromString(TEXT("This is the description for the Overall Volume")));
			
			OverallVolume->SetDisplayValueRange(TRange<float>(0.0f, 1.f));
			OverallVolume->SetOutputValueRange(TRange<float>(0.f, 2.f));
			OverallVolume->SetSliderStepSize(0.01f);
			
			OverallVolume->SetDefaultValueFromString(LexToString(1.f));
			
			OverallVolume->SetDisplayNumericType(ECommonNumericType::Percentage);
			
			OverallVolume->SetNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());
			
			// Set the data dynamic getter and setter for the scalar object
			OverallVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetOverallVolume));
			OverallVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetOverallVolume));
			
			OverallVolume->SetShouldApplySettingsImmediately(true);
			
			VolumeCollection->AddChildListData(OverallVolume);
		}
		
		// Music Volume
		{
			UListDataObject_Scalar* MusicVolume = NewObject<UListDataObject_Scalar>();
			MusicVolume->SetDataID(FName("MusicVolume"));
			MusicVolume->SetDataDisplayName(FText::FromString(TEXT("Music Volume")));
			MusicVolume->SetDescriptionRichText(FText::FromString(TEXT("This is the description for the Music Volume")));
			
			MusicVolume->SetDisplayValueRange(TRange<float>(0.0f, 1.f));
			MusicVolume->SetOutputValueRange(TRange<float>(0.f, 2.f));
			MusicVolume->SetSliderStepSize(0.01f);
			
			MusicVolume->SetDefaultValueFromString(LexToString(1.f));
			
			MusicVolume->SetDisplayNumericType(ECommonNumericType::Percentage);
			
			MusicVolume->SetNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());
			
			// Set the data dynamic getter and setter for the scalar object
			MusicVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetMusicVolume));
			MusicVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetMusicVolume));
			
			MusicVolume->SetShouldApplySettingsImmediately(true);
			
			VolumeCollection->AddChildListData(MusicVolume);
		}
		
		// SFX Volume
		{
			UListDataObject_Scalar* SFXVolume = NewObject<UListDataObject_Scalar>();
			SFXVolume->SetDataID(FName("SFXVolume"));
			SFXVolume->SetDataDisplayName(FText::FromString(TEXT("SFX Volume")));
			SFXVolume->SetDescriptionRichText(FText::FromString(TEXT("This is the description for the SFX Volume")));
			
			SFXVolume->SetDisplayValueRange(TRange<float>(0.0f, 1.f));
			SFXVolume->SetOutputValueRange(TRange<float>(0.f, 2.f));
			SFXVolume->SetSliderStepSize(0.01f);
			
			SFXVolume->SetDefaultValueFromString(LexToString(1.f));
			
			SFXVolume->SetDisplayNumericType(ECommonNumericType::Percentage);
			
			SFXVolume->SetNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());
			
			// Set the data dynamic getter and setter for the scalar object
			SFXVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetSFXVolume));
			SFXVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetSFXVolume));
			
			SFXVolume->SetShouldApplySettingsImmediately(true);
			
			VolumeCollection->AddChildListData(SFXVolume);
		}
		
		// Voice Volume
		{
			UListDataObject_Scalar* VoiceVolume = NewObject<UListDataObject_Scalar>();
			VoiceVolume->SetDataID(FName("VoiceVolume"));
			VoiceVolume->SetDataDisplayName(FText::FromString(TEXT("Voice Volume")));
			VoiceVolume->SetDescriptionRichText(FText::FromString(TEXT("This is the description for the Voice Volume")));
			
			VoiceVolume->SetDisplayValueRange(TRange<float>(0.0f, 1.f));
			VoiceVolume->SetOutputValueRange(TRange<float>(0.f, 2.f));
			VoiceVolume->SetSliderStepSize(0.01f);
			
			VoiceVolume->SetDefaultValueFromString(LexToString(1.f));
			
			VoiceVolume->SetDisplayNumericType(ECommonNumericType::Percentage);
			
			VoiceVolume->SetNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());
			
			// Set the data dynamic getter and setter for the scalar object
			VoiceVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetVoiceVolume));
			VoiceVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetVoiceVolume));
			
			VoiceVolume->SetShouldApplySettingsImmediately(true);
			
			VolumeCollection->AddChildListData(VoiceVolume);
		}
	}
	
	// Sound Category
	{
		// A new category, under audio, that contains audio settings not related to volume
		UListDataObject_Collection* SoundCollection = NewObject<UListDataObject_Collection>();
		SoundCollection->SetDataID(FName("SoundCollection"));
		SoundCollection->SetDataDisplayName(FText::FromString(TEXT("Sound")));
		
		// Add the sound collection to the audio tab
		AudioTabCollection->AddChildListData(SoundCollection);
		
		// Allow Background Audio Setting
		{
			UListDataObject_StringBool* AllowBackgroundAudioSetting = NewObject<UListDataObject_StringBool>();
			AllowBackgroundAudioSetting->SetDataID(FName("AllowBackgroundAudio"));
			AllowBackgroundAudioSetting->SetDataDisplayName(FText::FromString(TEXT("Allow Background Audio")));
			AllowBackgroundAudioSetting->OverrideTrueDisplayText(FText::FromString(TEXT("Enabled")));
			AllowBackgroundAudioSetting->OverrideFalseDisplayText(FText::FromString(TEXT("Disabled")));
			AllowBackgroundAudioSetting->SetDescriptionRichText(FText::FromString(TEXT("<Bold>Enabled:</> Hear game audio while the game is not focused.\n\n<Bold>Disabled:</> Prevents game audio from playing while the game is not focused.")));
			AllowBackgroundAudioSetting->SetFalseAsDefaultValue();
			AllowBackgroundAudioSetting->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetAllowBackGroundAudio));
			AllowBackgroundAudioSetting->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetAllowBackGroundAudio));
			AllowBackgroundAudioSetting->SetShouldApplySettingsImmediately(true);
			
			SoundCollection->AddChildListData(AllowBackgroundAudioSetting);
		}
		
		// Use HDR Audio Setting
		{
			UListDataObject_StringBool* UseHDRAudioModeSetting = NewObject<UListDataObject_StringBool>();
			UseHDRAudioModeSetting->SetDataID(FName("UseHDRAudioMode"));
			UseHDRAudioModeSetting->SetDataDisplayName(FText::FromString(TEXT("Use HDR Audio Mode")));
			UseHDRAudioModeSetting->OverrideTrueDisplayText(FText::FromString(TEXT("Enabled")));
			UseHDRAudioModeSetting->OverrideFalseDisplayText(FText::FromString(TEXT("Disabled")));
			UseHDRAudioModeSetting->SetDescriptionRichText(FText::FromString(TEXT("<Bold>Enabled:</> The game will use High Dynamic Range Audio.\n\n<Bold>Disabled:</> The game will not use High Dynamic Range Audio.")));
			UseHDRAudioModeSetting->SetFalseAsDefaultValue();
			UseHDRAudioModeSetting->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetUseHDRAudioMode));
			UseHDRAudioModeSetting->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetUseHDRAudioMode));
			UseHDRAudioModeSetting->SetShouldApplySettingsImmediately(true);
			
			SoundCollection->AddChildListData(UseHDRAudioModeSetting);
		}
	}
	
	RegisteredOptionsTabCollections.Add(AudioTabCollection);
}

void UOptionsDataRegistry::InitVideoCollectionTab()
{
	UListDataObject_Collection* VideoTabCollection = NewObject<UListDataObject_Collection>();
	VideoTabCollection->SetDataID(FName("VideoTabCollection"));
	VideoTabCollection->SetDataDisplayName(FText::FromString("Video"));
	
	UListDataObject_StringEnum* CreatedWindowMode = nullptr;
	UListDataObject_StringInteger* CreatedOverallQuality = nullptr;
	
	// A collection of display settings (window, resolution, v-sync)
	{
		// A collection of settings for the display
		UListDataObject_Collection* DisplayCollection = NewObject<UListDataObject_Collection>();
		DisplayCollection->SetDataID(FName("DisplayCollection"));
		DisplayCollection->SetDataDisplayName(FText::FromString(TEXT("Display")));
		
		VideoTabCollection->AddChildListData(DisplayCollection);
		
		// A condition that disables changing a setting if we are in the editor
		FOptionsDataEditConditionDescriptor PackagedBuildOnlyCondition;
		PackagedBuildOnlyCondition.SetEditConditionFunc(
			[]()->bool
			{
				const bool bIsInEditor = GIsEditor || GIsPlayInEditorWorld;
				
				return !bIsInEditor;
			}
		);
		
		PackagedBuildOnlyCondition.SetDisabledRichReason(TEXT("\n\n<Disabled>This setting can only be adjusted in a packaged build.</>"));
		
		// Window Mode setting
		{
			UListDataObject_StringEnum* WindowModeSetting = NewObject<UListDataObject_StringEnum>();
			WindowModeSetting->SetDataID(FName("WindowMode"));
			WindowModeSetting->SetDataDisplayName(FText::FromString(TEXT("Window Mode")));
			WindowModeSetting->SetDescriptionRichText(GET_DESCRIPTION("WindowModeDescKey"));
			WindowModeSetting->AddEnumOption(EWindowMode::Fullscreen, FText::FromString(TEXT("Fullscreen")));
			WindowModeSetting->AddEnumOption(EWindowMode::WindowedFullscreen, FText::FromString(TEXT("Borderless Window")));
			WindowModeSetting->AddEnumOption(EWindowMode::Windowed, FText::FromString(TEXT("Windowed")));
			WindowModeSetting->SetDefaultValueFromEnumOption(EWindowMode::WindowedFullscreen);
			WindowModeSetting->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetFullscreenMode));
			WindowModeSetting->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetFullscreenMode));
			WindowModeSetting->SetShouldApplySettingsImmediately(true);
			
			// The window mode should only be modifiable in a packaged build
			WindowModeSetting->AddEditCondition(PackagedBuildOnlyCondition);
			
			DisplayCollection->AddChildListData(WindowModeSetting);
			CreatedWindowMode = WindowModeSetting;
		}
		
		// Screen Resolution
		{
			UListDataObject_StringResolution* ScreenResolutionSetting = NewObject<UListDataObject_StringResolution>();
			ScreenResolutionSetting->SetDataID(FName("ScreenResolution"));
			ScreenResolutionSetting->SetDataDisplayName(FText::FromString(TEXT("Resolution")));
			ScreenResolutionSetting->SetDescriptionRichText(GET_DESCRIPTION("ScreenResolutionsDescKey"));
			ScreenResolutionSetting->InitResolutionValues();
			ScreenResolutionSetting->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetScreenResolution));
			ScreenResolutionSetting->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetScreenResolution));
			ScreenResolutionSetting->SetShouldApplySettingsImmediately(true);
			
			// The resolution should not be modifiable outside a packaged build
			ScreenResolutionSetting->AddEditCondition(PackagedBuildOnlyCondition);
			
			// The resolution should not be modifiable if the game is in borderless window mode
			FOptionsDataEditConditionDescriptor WindowModeEditCondition;
			WindowModeEditCondition.SetEditConditionFunc(
				[CreatedWindowMode]()->bool
				{
					const bool bIsBorderlessWindow = CreatedWindowMode->GetCurrentValueAsEnum<EWindowMode::Type>() == EWindowMode::WindowedFullscreen;
					
					return !bIsBorderlessWindow;
				}
			);
			
			WindowModeEditCondition.SetDisabledRichReason(TEXT("\n\n<Disabled>Screen resolution is not adjustable when the 'Window Mode' is set to Borderless Window. The value must match the native desktop resolution</>"));
			WindowModeEditCondition.SetDisabledForcedStringValue(ScreenResolutionSetting->GetMaximumAllowedResolution());
			
			ScreenResolutionSetting->AddEditCondition(WindowModeEditCondition);
			// Set the window mode setting as a dependency for the screen resolution
			ScreenResolutionSetting->AddEditDependencyData(CreatedWindowMode);
			
			DisplayCollection->AddChildListData(ScreenResolutionSetting);
		}
		
		// Display Gamma Setting - Adjusts Brightness
		{
			UListDataObject_Scalar* GammaSetting = NewObject<UListDataObject_Scalar>();
			GammaSetting->SetDataID(FName("Gamma"));
			GammaSetting->SetDataDisplayName(FText::FromString(TEXT("Brightness")));
			GammaSetting->SetDescriptionRichText(FText::FromString(TEXT("Adjusts the brightness.")));
			GammaSetting->SetDisplayValueRange(TRange<float>(0.f, 1.f));
			GammaSetting->SetOutputValueRange(TRange<float>(1.7f, 2.7f)); // The default value is 2.2f
			GammaSetting->SetSliderStepSize(0.01f);
			GammaSetting->SetDisplayNumericType(ECommonNumericType::Percentage);
			GammaSetting->SetNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());
			GammaSetting->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetCurrentDisplayGamma));
			GammaSetting->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetCurrentDisplayGamma));
			GammaSetting->SetDefaultValueFromString(LexToString(2.2f));
			
			DisplayCollection->AddChildListData(GammaSetting);
		}
	}
	
	// A collection of graphics settings
	{
		UListDataObject_Collection* GraphicsCollection = NewObject<UListDataObject_Collection>();
		GraphicsCollection->SetDataID(FName("GraphicsCollection"));
		GraphicsCollection->SetDataDisplayName(FText::FromString(TEXT("Graphics")));
		
		VideoTabCollection->AddChildListData(GraphicsCollection);
		
		// Overall Scalability Setting
		{
			UListDataObject_StringInteger* OverallScalabilitySetting = NewObject<UListDataObject_StringInteger>();
			OverallScalabilitySetting->SetDataID(FName("GraphicsQualityPresets"));
			OverallScalabilitySetting->SetDataDisplayName(FText::FromString(TEXT("Graphics Quality")));
			OverallScalabilitySetting->SetDescriptionRichText(GET_DESCRIPTION("OverallQualityDescKey"));
			// Add the graphics quality preset options
			OverallScalabilitySetting->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			OverallScalabilitySetting->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			OverallScalabilitySetting->AddIntegerOption(2, FText::FromString(TEXT("High")));
			OverallScalabilitySetting->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
			OverallScalabilitySetting->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			// Set the dynamic accessors
			OverallScalabilitySetting->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetOverallScalabilityLevel));
			OverallScalabilitySetting->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetOverallScalabilityLevel));
			OverallScalabilitySetting->SetShouldApplySettingsImmediately(true);
			
			GraphicsCollection->AddChildListData(OverallScalabilitySetting);
			CreatedOverallQuality = OverallScalabilitySetting;
		}
		
		// Resolution Scale
		{
			UListDataObject_Scalar* ResolutionScale = NewObject<UListDataObject_Scalar>();
			ResolutionScale->SetDataID(FName("ResolutionScale"));
			ResolutionScale->SetDataDisplayName(FText::FromString(TEXT("3D Resolution")));
			ResolutionScale->SetDescriptionRichText(GET_DESCRIPTION("ResolutionScaleDescKey"));
			ResolutionScale->SetDisplayValueRange(TRange<float>(0.f, 1.f));
			ResolutionScale->SetOutputValueRange(TRange<float>(0.f, 1.f));
			ResolutionScale->SetSliderStepSize(0.01f);
			ResolutionScale->SetDisplayNumericType(ECommonNumericType::Percentage);
			ResolutionScale->SetNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());
			ResolutionScale->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetResolutionScaleNormalized));
			ResolutionScale->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetResolutionScaleNormalized));
			ResolutionScale->SetShouldApplySettingsImmediately(true);
			// Set the overall scalability as a dependency
			ResolutionScale->AddEditDependencyData(CreatedOverallQuality);
			GraphicsCollection->AddChildListData(ResolutionScale);
		}
		
		// Global Illumination Setting
		{
			UListDataObject_StringInteger* GISetting = NewObject<UListDataObject_StringInteger>();
			GISetting->SetDataID(FName("GlobalIlluminationQuality"));
			GISetting->SetDataDisplayName(FText::FromString(TEXT("Global Illumination")));
			GISetting->SetDescriptionRichText(GET_DESCRIPTION("GlobalIlluminationQualityDescKey"));
			GISetting->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			GISetting->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			GISetting->AddIntegerOption(2, FText::FromString(TEXT("High")));
			GISetting->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
			GISetting->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			GISetting->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetGlobalIlluminationQuality));
			GISetting->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetGlobalIlluminationQuality));
			GISetting->SetShouldApplySettingsImmediately(true);
			
			// GI, like many other graphical settings, has a two-way dependency with overall quality
			GISetting->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(GISetting);
			
			GraphicsCollection->AddChildListData(GISetting);
		}
		
		// Shadow Quality Setting
		{
			UListDataObject_StringInteger* ShadowQualitySetting = NewObject<UListDataObject_StringInteger>();
			ShadowQualitySetting->SetDataID(FName("ShadowQualitySetting"));
			ShadowQualitySetting->SetDataDisplayName(FText::FromString(TEXT("Shadows")));
			ShadowQualitySetting->SetDescriptionRichText(GET_DESCRIPTION("ShadowQualityDescKey"));
			ShadowQualitySetting->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			ShadowQualitySetting->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			ShadowQualitySetting->AddIntegerOption(2, FText::FromString(TEXT("High")));
			ShadowQualitySetting->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
			ShadowQualitySetting->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			ShadowQualitySetting->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetShadowQuality));
			ShadowQualitySetting->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetShadowQuality));
			ShadowQualitySetting->SetShouldApplySettingsImmediately(true);
			
			ShadowQualitySetting->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(ShadowQualitySetting);
			
			GraphicsCollection->AddChildListData(ShadowQualitySetting);
		}
		
		// Anti-Aliasing Quality
		{
			UListDataObject_StringInteger* AAQualitySetting = NewObject<UListDataObject_StringInteger>();
			AAQualitySetting->SetDataID(FName("AAQualitySetting"));
			AAQualitySetting->SetDataDisplayName(FText::FromString(TEXT("Anti-Aliasing")));
			AAQualitySetting->SetDescriptionRichText(GET_DESCRIPTION("AntiAliasingDescKey"));
			AAQualitySetting->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			AAQualitySetting->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			AAQualitySetting->AddIntegerOption(2, FText::FromString(TEXT("High")));
			AAQualitySetting->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
			AAQualitySetting->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			AAQualitySetting->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetAntiAliasingQuality));
			AAQualitySetting->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetAntiAliasingQuality));
			AAQualitySetting->SetShouldApplySettingsImmediately(true);
			
			AAQualitySetting->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(AAQualitySetting);
			
			GraphicsCollection->AddChildListData(AAQualitySetting);
		}
		
		// View Distance Setting
		{
			UListDataObject_StringInteger* ViewDistanceSetting = NewObject<UListDataObject_StringInteger>();
			ViewDistanceSetting->SetDataID(FName("ViewDistanceSetting"));
			ViewDistanceSetting->SetDataDisplayName(FText::FromString(TEXT("View Distance")));
			ViewDistanceSetting->SetDescriptionRichText(GET_DESCRIPTION("ViewDistanceDescKey"));
			ViewDistanceSetting->AddIntegerOption(0, FText::FromString(TEXT("Near")));
			ViewDistanceSetting->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			ViewDistanceSetting->AddIntegerOption(2, FText::FromString(TEXT("Far")));
			ViewDistanceSetting->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
			ViewDistanceSetting->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			ViewDistanceSetting->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetViewDistanceQuality));
			ViewDistanceSetting->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetViewDistanceQuality));
			ViewDistanceSetting->SetShouldApplySettingsImmediately(true);
			
			ViewDistanceSetting->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(ViewDistanceSetting);
			
			GraphicsCollection->AddChildListData(ViewDistanceSetting);
		}
		
		// Texture Quality Setting
		{
			UListDataObject_StringInteger* TextureQualitySetting = NewObject<UListDataObject_StringInteger>();
			TextureQualitySetting->SetDataID(FName("TextureQualitySetting"));
			TextureQualitySetting->SetDataDisplayName(FText::FromString(TEXT("Textures")));
			TextureQualitySetting->SetDescriptionRichText(GET_DESCRIPTION("TextureQualityDescKey"));
			TextureQualitySetting->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			TextureQualitySetting->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			TextureQualitySetting->AddIntegerOption(2, FText::FromString(TEXT("High")));
			TextureQualitySetting->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
			TextureQualitySetting->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			TextureQualitySetting->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetTextureQuality));
			TextureQualitySetting->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetTextureQuality));
			TextureQualitySetting->SetShouldApplySettingsImmediately(true);
			
			TextureQualitySetting->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(TextureQualitySetting);
			
			GraphicsCollection->AddChildListData(TextureQualitySetting);
		}
		
		// Visual Effects Quality Setting
		{
			UListDataObject_StringInteger* Setting = NewObject<UListDataObject_StringInteger>();
			Setting->SetDataID(FName("VisualEffectQuality"));
			Setting->SetDataDisplayName(FText::FromString(TEXT("Effects")));
			Setting->SetDescriptionRichText(GET_DESCRIPTION("VisualEffectQualityDescKey"));
			Setting->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			Setting->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			Setting->AddIntegerOption(2, FText::FromString(TEXT("High")));
			Setting->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
			Setting->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			Setting->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetVisualEffectQuality));
			Setting->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetVisualEffectQuality));
			Setting->SetShouldApplySettingsImmediately(true);
			
			Setting->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(Setting);
			
			GraphicsCollection->AddChildListData(Setting);
		}
		
		// Reflection Quality Setting
		{
			UListDataObject_StringInteger* Setting = NewObject<UListDataObject_StringInteger>();
			Setting->SetDataID(FName("ReflectionQuality"));
			Setting->SetDataDisplayName(FText::FromString(TEXT("Reflections")));
			Setting->SetDescriptionRichText(GET_DESCRIPTION("ReflectionQualityDescKey"));
			Setting->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			Setting->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			Setting->AddIntegerOption(2, FText::FromString(TEXT("High")));
			Setting->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
			Setting->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			Setting->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetReflectionQuality));
			Setting->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetReflectionQuality));
			Setting->SetShouldApplySettingsImmediately(true);
			
			Setting->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(Setting);
			
			GraphicsCollection->AddChildListData(Setting);
		}
		
		// Post-Processing Quality Setting
		{
			UListDataObject_StringInteger* Setting = NewObject<UListDataObject_StringInteger>();
			Setting->SetDataID(FName("PostProcessingQuality"));
			Setting->SetDataDisplayName(FText::FromString(TEXT("Post-Processing")));
			Setting->SetDescriptionRichText(GET_DESCRIPTION("PostProcessingQualityDescKey"));
			Setting->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			Setting->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			Setting->AddIntegerOption(2, FText::FromString(TEXT("High")));
			Setting->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
			Setting->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			Setting->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetPostProcessingQuality));
			Setting->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetPostProcessingQuality));
			Setting->SetShouldApplySettingsImmediately(true);
			
			Setting->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(Setting);
			
			GraphicsCollection->AddChildListData(Setting);
		}
	}
	
	// A collection of advanced graphics settings
	{
		UListDataObject_Collection* AdvancedGraphicsCollection = NewObject<UListDataObject_Collection>();
		AdvancedGraphicsCollection->SetDataID(FName("AdvancedGraphicsCollection"));
		AdvancedGraphicsCollection->SetDataDisplayName(FText::FromString(TEXT("Advanced Graphics")));
		
		VideoTabCollection->AddChildListData(AdvancedGraphicsCollection);
		
		// V-Sync Setting
		{
			UListDataObject_StringBool* Setting = NewObject<UListDataObject_StringBool>();
			Setting->SetDataID(FName("VerticalSync"));
			Setting->SetDataDisplayName(FText::FromString(TEXT("Vertical Sync")));
			Setting->SetDescriptionRichText(GET_DESCRIPTION("VerticalSyncDescKey"));
			Setting->OverrideTrueDisplayText(FText::FromString(TEXT("Enabled")));
			Setting->OverrideFalseDisplayText(FText::FromString(TEXT("Disabled")));
			Setting->SetFalseAsDefaultValue();
			Setting->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(IsVSyncEnabled));
			Setting->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetVSyncEnabled));
			Setting->SetShouldApplySettingsImmediately(true);
			
			// Vertical Sync only works in Fullscreen mode
			FOptionsDataEditConditionDescriptor WindowModeEditCondition;
			WindowModeEditCondition.SetEditConditionFunc(
				[CreatedWindowMode]()->bool
				{
					return CreatedWindowMode->GetCurrentValueAsEnum<EWindowMode::Type>() == EWindowMode::Fullscreen;
				}
			);
			
			WindowModeEditCondition.SetDisabledRichReason(TEXT("\n\n<Disabled>This feature only works if 'Window Mode' is set to 'Fullscreen'.</>"));
			WindowModeEditCondition.SetDisabledForcedStringValue(TEXT("False"));

			Setting->AddEditCondition(WindowModeEditCondition);
			AdvancedGraphicsCollection->AddChildListData(Setting);
		}
		
		// Frame Rate Limit Setting
		{
			UListDataObject_String* Setting = NewObject<UListDataObject_String>();
			Setting->SetDataID(FName("FrameRateLimit"));
			Setting->SetDataDisplayName(FText::FromString(TEXT("Frame Rate Limit")));
			Setting->SetDescriptionRichText(GET_DESCRIPTION("FrameRateLimitDescKey"));
			Setting->AddDynamicOptions(LexToString(30.f), FText::FromString(TEXT("30 FPS")));
			Setting->AddDynamicOptions(LexToString(60.f), FText::FromString(TEXT("60 FPS")));
			Setting->AddDynamicOptions(LexToString(90.f), FText::FromString(TEXT("90 FPS")));
			Setting->AddDynamicOptions(LexToString(120.f), FText::FromString(TEXT("120 FPS")));
			Setting->AddDynamicOptions(LexToString(0.f), FText::FromString(TEXT("Unlimited")));
			Setting->SetDefaultValueFromString(LexToString(0.f));
			Setting->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetFrameRateLimit));
			Setting->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetFrameRateLimit));
			Setting->SetShouldApplySettingsImmediately(true);
			
			AdvancedGraphicsCollection->AddChildListData(Setting);
		}
	}
	
	RegisteredOptionsTabCollections.Add(VideoTabCollection);
}

void UOptionsDataRegistry::InitControlCollectionTab(ULocalPlayer* InLocalOwningPlayer)
{
	UListDataObject_Collection* ControlTabCollection = NewObject<UListDataObject_Collection>();
	ControlTabCollection->SetDataID(FName("ControlTabCollection"));
	ControlTabCollection->SetDataDisplayName(FText::FromString("Control"));
	
	// Get a pointer to the enhanced input subsystem, crash if it is invalid
	UEnhancedInputLocalPlayerSubsystem* EISubsystem = InLocalOwningPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(EISubsystem);
		
	UEnhancedInputUserSettings* EIUserSettings = EISubsystem->GetUserSettings();
	check(EIUserSettings);
	
	// Mouse and Keyboard Category
	{
		UListDataObject_Collection* MKCollection = NewObject<UListDataObject_Collection>();
		MKCollection->SetDataID(FName("MouseKeyboardCollection"));
		MKCollection->SetDataDisplayName(FText::FromString("Mouse & Keyboard"));
		
		ControlTabCollection->AddChildListData(MKCollection);
		
		// Mouse and Keyboard inputs
		{
			// Use this to filter the input, so we only get the mouse and keyboard options
			FPlayerMappableKeyQueryOptions MKQueryOptions;
			MKQueryOptions.KeyToMatch = EKeys::S;
			MKQueryOptions.bMatchBasicKeyTypes = true;
			// Use this to filter the input, so we only get the gamepad options
			//FPlayerMappableKeyQueryOptions GamepadQueryOptions;
			//GamepadQueryOptions.KeyToMatch = EKeys::Gamepad_FaceButton_Bottom;
			//GamepadQueryOptions.bMatchBasicKeyTypes = true;
			
			// This for loop is for iterating through all the Input Player Profiles this project has
			for (const TPair<FString, UEnhancedPlayerMappableKeyProfile*>& ProfilePair : EIUserSettings->GetAllAvailableKeyProfiles())
			{
				// In this project, there is only one profile
				UEnhancedPlayerMappableKeyProfile* MappableKeyProfile = ProfilePair.Value;
				check(MappableKeyProfile);
				
				// This for loop gets all the inputs in the profile
				for (const TPair<FName, FKeyMappingRow>& MappingRowPair : MappableKeyProfile->GetPlayerMappingRows())
				{
					// Each input has multiple mappings (gamepad/M&K)
					for (const FPlayerKeyMapping& KeyMapping : MappingRowPair.Value.Mappings)
					{
						if (MappableKeyProfile->DoesMappingPassQueryOptions(KeyMapping, MKQueryOptions))
						{
							// Initialise the remappable input data object
							UListDataObject_KeyRemap* KeyRemapDataObject = NewObject<UListDataObject_KeyRemap>();
							KeyRemapDataObject->SetDataID(KeyMapping.GetMappingName());
							KeyRemapDataObject->SetDataDisplayName(KeyMapping.GetDisplayName());
							KeyRemapDataObject->InitKeyRemapData(EIUserSettings, MappableKeyProfile, ECommonInputType::MouseAndKeyboard, KeyMapping);
							
							MKCollection->AddChildListData(KeyRemapDataObject);
						}
					}
				}
			}
		}
		
	}
	
	// Gamepad Category
	{
		UListDataObject_Collection* GamepadCollection = NewObject<UListDataObject_Collection>();
		GamepadCollection->SetDataID(FName("GamepadCollection"));
		GamepadCollection->SetDataDisplayName(FText::FromString("Gamepad"));
		
		ControlTabCollection->AddChildListData(GamepadCollection);
		
		{
			// Use this to filter the input, so we only get the gamepad options
			FPlayerMappableKeyQueryOptions GamepadQueryOptions;
			GamepadQueryOptions.KeyToMatch = EKeys::Gamepad_FaceButton_Bottom;
			GamepadQueryOptions.bMatchBasicKeyTypes = true;
			
			
			// This for loop is for iterating through all the Input Player Profiles this project has
			for (const TPair<FString, UEnhancedPlayerMappableKeyProfile*>& ProfilePair : EIUserSettings->GetAllAvailableKeyProfiles())
			{
				// In this project, there is only one profile
				UEnhancedPlayerMappableKeyProfile* MappableKeyProfile = ProfilePair.Value;
				check(MappableKeyProfile);
				
				// This for loop gets all the inputs in the profile
				for (const TPair<FName, FKeyMappingRow>& MappingRowPair : MappableKeyProfile->GetPlayerMappingRows())
				{
					// Each input has multiple mappings (gamepad/M&K)
					for (const FPlayerKeyMapping& KeyMapping : MappingRowPair.Value.Mappings)
					{
						if (MappableKeyProfile->DoesMappingPassQueryOptions(KeyMapping, GamepadQueryOptions))
						{
							// Initialise the remappable input data object
							UListDataObject_KeyRemap* KeyRemapDataObject = NewObject<UListDataObject_KeyRemap>();
							KeyRemapDataObject->SetDataID(KeyMapping.GetMappingName());
							KeyRemapDataObject->SetDataDisplayName(KeyMapping.GetDisplayName());
							KeyRemapDataObject->InitKeyRemapData(EIUserSettings, MappableKeyProfile, ECommonInputType::Gamepad, KeyMapping);
							
							GamepadCollection->AddChildListData(KeyRemapDataObject);
						}
					}
				}
			}
		}
	}
	
	RegisteredOptionsTabCollections.Add(ControlTabCollection);
}

void UOptionsDataRegistry::InitAccessibilityCollectionTab()
{
	UListDataObject_Collection* AccessibilityCollection = NewObject<UListDataObject_Collection>();
	AccessibilityCollection->SetDataID(FName("AccessibilityCollection"));
	AccessibilityCollection->SetDataDisplayName(FText::FromString("Accessibility"));
	
	RegisteredOptionsTabCollections.Add(AccessibilityCollection);
}

void UOptionsDataRegistry::FindChildListDataRecursively(UListDataObject_Base* InParentData,
	TArray<UListDataObject_Base*>& OutFoundChildListData) const
{
	// If the parent data is invalid, or it has no children. Exit this function
	if (!InParentData || !InParentData->HasAnyChildListData()) return;
	
	// Add every sub-child to list
	for (UListDataObject_Base* SubChildListData : InParentData->GetAllChildListData())
	{
		if (!SubChildListData) continue;
		
		OutFoundChildListData.Add(SubChildListData);
		
		// if any of the sub-children have children. Use recursion to add them as well
		if (SubChildListData->HasAnyChildListData())
		{
			FindChildListDataRecursively(SubChildListData, OutFoundChildListData);
		}
	}
}
