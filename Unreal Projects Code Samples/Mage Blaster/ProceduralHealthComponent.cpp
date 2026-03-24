// ------------------------------------------------------------------------------
// File Name: ProceduralHealthComponent.cpp
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: TDS_Procedural
// Date Created: 2025-10-27
// Date Last Modified: 2026-02-25
// Copyright © 2025 Aidan Straker
// ------------------------------------------------------------------------------

#include "Character/ProceduralHealthComponent.h"

#include "ProceduralGameplayTags.h"
#include "ProceduralBaseCharacter.h"
#include "GameModes/TDS_ProceduralGameMode.h"
#include "AbilitySystem/ProceduralAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ProceduralHealthSet.h"
#include "System/ProceduralGameData.h"
#include "System/ProceduralGameInstance.h"


UProceduralHealthComponent::UProceduralHealthComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	AbilitySystemComponent = nullptr;
}

void UProceduralHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifeTimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifeTimeProps);
}

void UProceduralHealthComponent::InitialiseWithAbilitySystem(UProceduralAbilitySystemComponent* InASC)
{
	const AActor* Owner = GetOwner();
	check(Owner);
	
	if (AbilitySystemComponent)
	{
		UE_LOG(LogTemp,Error, TEXT("ProceduralHealthComponent: Health component for owner [%s] has already been initialized with an ability system."), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("ProceduralHealthComponent: Cannot initialize health component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
		return;
	}

	HealthSet = AbilitySystemComponent->GetSet<UProceduralHealthSet>();
	if (!HealthSet)
	{
		UE_LOG(LogTemp, Error, TEXT("ProceduralHealthComponent: Cannot initialize health component for owner [%s] with NULL health set on the ability system."), *GetNameSafe(Owner));
		return;
	}

	// Register to listen for attribute changes
	HealthSet->OnHealthChanged.AddUObject(this, &ThisClass::HandleHealthChange);
	HealthSet->OnMaxHealthChanged.AddUObject(this, &ThisClass::HandleMaxHealthChange);
	HealthSet->OnOutOfHealth.AddUObject(this, &ThisClass::HandleOutOfHealth);
	
	ClearGameplayTags();
	
	OnHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
	OnMaxHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
}

void UProceduralHealthComponent::UninitialiseFromAbilitySystem()
{
	ClearGameplayTags();

	if (HealthSet)
	{
		HealthSet->OnHealthChanged.RemoveAll(this);
		HealthSet->OnMaxHealthChanged.RemoveAll(this);
		HealthSet->OnOutOfHealth.RemoveAll(this);
	}

	HealthSet = nullptr;
	AbilitySystemComponent = nullptr;
}

float UProceduralHealthComponent::GetHealth() const
{
	return (HealthSet ? HealthSet->GetHealth() : 0.0f);
}

float UProceduralHealthComponent::GetMaxHealth() const
{
	return (HealthSet ? HealthSet->GetMaxHealth() : 0.0f);
}

float UProceduralHealthComponent::GetHealthNormalised() const
{
	if (HealthSet)
	{
		const float Health = HealthSet->GetHealth();
		const float MaxHealth = HealthSet->GetMaxHealth();
		return ((MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f);
	}

	return 0.0f;
}


void UProceduralHealthComponent::DamageSelfDestruct(bool bFellOutOfWorld)
{
	if (AbilitySystemComponent)
	{
		const TObjectPtr<UProceduralGameData> DefaultGameData = Cast<AProceduralBaseCharacter>(GetOwner())->GetProceduralGameInstance()->GetGlobalGameData();
		const TSubclassOf<UGameplayEffect> DamageGE = DefaultGameData->DamageGameplayEffect_SetByCaller;
		if (!DamageGE)
		{
			UE_LOG(LogTemp, Error, TEXT("ProceduralHealthComponent: DamageSelfDestruct failed for owner [%s]. Unable to find gameplay effect [%s]."), *GetNameSafe(GetOwner()),
				*GetNameSafe(DamageGE));
			return;
		}

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DamageGE, 1.0f, AbilitySystemComponent->MakeEffectContext());
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

		if (!Spec)
		{
			UE_LOG(LogTemp, Error, TEXT("ProceduralHealthComponent: DamageSelfDestruct failed for owner [%s]. Unable to make outgoing spec for [%s]."), *GetNameSafe(GetOwner()), *GetNameSafe(DamageGE));
			return;
		}

		Spec->AddDynamicAssetTag(TAG_Gameplay_DamageSelfDestruct);

		if (bFellOutOfWorld)
		{
			Spec->AddDynamicAssetTag(TAG_Gameplay_FellOutOfWorld);
		}

		const float DamageAmount = GetMaxHealth();
		Spec->SetSetByCallerMagnitude(ProceduralGameplayTags::SetByCaller_Damage, DamageAmount);
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec);
	}
}

// Called when the game starts
void UProceduralHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UProceduralHealthComponent::OnUnregister()
{
	UninitialiseFromAbilitySystem();
	
	Super::OnUnregister();
}

void UProceduralHealthComponent::ClearGameplayTags()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(ProceduralGameplayTags::Status_Death_Dying, 0);
		AbilitySystemComponent->SetLooseGameplayTagCount(ProceduralGameplayTags::Status_Death_Dead, 0);
	}
}

void UProceduralHealthComponent::HandleHealthChange(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	OnHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void UProceduralHealthComponent::HandleMaxHealthChange(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	OnMaxHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void UProceduralHealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	OnOutOfHealth.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

// Called every frame
void UProceduralHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

