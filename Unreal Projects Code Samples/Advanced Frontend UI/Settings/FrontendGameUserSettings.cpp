// ------------------------------------------------------------------------------
// File Name: FrontendGameUserSettings.cpp
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2026-03-07
// Date Last Modified: 2026-03-29
// Copyright © 2026 Aidan Straker
// ------------------------------------------------------------------------------

#include "FrontendSettings/FrontendGameUserSettings.h"

#include "FrontendSettings/FrontendDeveloperSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"

UFrontendGameUserSettings::UFrontendGameUserSettings()
	: OverallVolume(1.f)
	, MusicVolume(1.f)
	, SFXVolume(1.f)
	, VoiceVolume(1.f)
	, bAllowBackgroundAudio(false)
	, bUseHDRAudioMode(false)
{
}

UFrontendGameUserSettings* UFrontendGameUserSettings::Get()
{
	// If the engine is valid
	if (GEngine)
	{
		// Get the game user settings
		return CastChecked<UFrontendGameUserSettings>(GEngine->GetGameUserSettings());
	}
	
	return nullptr;
}

void UFrontendGameUserSettings::SetOverallVolume(float InNewVolume)
{
	// Create local variables for the world object and the developer settings
	UWorld* InAudioWorld = nullptr;
	const UFrontendDeveloperSettings* FrontendDeveloperSettings = GetDefault<UFrontendDeveloperSettings>();
	
	// Attempt to get the world object
	if (GEngine)
	{
		InAudioWorld = GEngine->GetCurrentPlayWorld();
	}
	
	// If the world object or developer settings are invalid, do nothing
	if (!InAudioWorld || !FrontendDeveloperSettings) return;
	
	// Attempt to load the master sound class and default sound mix
	USoundClass* MasterSoundClass = nullptr;
	if (UObject* LoadedObject = FrontendDeveloperSettings->MasterSoundClass.TryLoad())
	{
		MasterSoundClass = CastChecked<USoundClass>(LoadedObject);
	}
	
	USoundMix* DefaultSoundMix = nullptr;
	
	if (UObject* LoadedObject = FrontendDeveloperSettings->DefaultSoundMix.TryLoad())
	{
		DefaultSoundMix = CastChecked<USoundMix>(LoadedObject);
	}
	
	// Set the new value for the overall volume
	OverallVolume = InNewVolume;
	
	// Update the sound value for every sound in the level
	UGameplayStatics::SetSoundMixClassOverride(
		InAudioWorld,
		DefaultSoundMix,
		MasterSoundClass,
		OverallVolume,
		1.f,
		0.2f
	);
	
	UGameplayStatics::PushSoundMixModifier(InAudioWorld, DefaultSoundMix);
}

void UFrontendGameUserSettings::SetMusicVolume(float InNewVolume) { MusicVolume = InNewVolume; }

void UFrontendGameUserSettings::SetSFXVolume(float InNewVolume) { SFXVolume = InNewVolume; }

void UFrontendGameUserSettings::SetVoiceVolume(float InNewVolume) { VoiceVolume = InNewVolume; }

void UFrontendGameUserSettings::SetAllowBackGroundAudio(bool IsAllowed) { bAllowBackgroundAudio = IsAllowed; }

void UFrontendGameUserSettings::SetUseHDRAudioMode(bool IsEnabled) { bUseHDRAudioMode = IsEnabled; }

float UFrontendGameUserSettings::GetCurrentDisplayGamma() const
{
	// get the brightness value using the provided getter
	if (GEngine)
	{
		return GEngine->GetDisplayGamma();
	}
	
	return 0.0f;
}

void UFrontendGameUserSettings::SetCurrentDisplayGamma(float InNewDisplayGamma)
{
	if (GEngine)
	{
		GEngine->DisplayGamma = InNewDisplayGamma;
	}
}
