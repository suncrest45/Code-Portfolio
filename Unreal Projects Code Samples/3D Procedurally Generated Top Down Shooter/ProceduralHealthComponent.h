// ------------------------------------------------------------------------------
// File Name: ProceduralHealthComponent.h
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: TDS_Procedural
// Date Created: 2025-10-27
// Date Last Modified: 2026-02-25
// Copyright © 2025 Aidan Straker
// ------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProceduralHealthComponent.generated.h"

class UProceduralHealthComponent;

class UProceduralAbilitySystemComponent;
class UProceduralHealthSet;
class UObject;
struct FGameplayEffectSpec;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FProceduralHealth_AttributeChanged, UProceduralHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);

/**
 * UProceduralHealthComponent
 *
 * An actor component used to handle anything related to health
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class TDS_PROCEDURAL_API UProceduralHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProceduralHealthComponent(const FObjectInitializer& ObjectInitializer);

	/**
	 * Returns the health component if one exists on a given actor
	 * @param Actor Specified actor
	 * @return NULL or the actor's health component
	 */
	UFUNCTION(BlueprintPure, Category = "Procedural|Health")
	static UProceduralHealthComponent* FindHealthComponent(const AActor* Actor) {return (Actor ? Actor->FindComponentByClass<UProceduralHealthComponent>() : nullptr); }

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * Initialise the component using an ability system
	 * @param InASC The ability system to initialise this component with
	 */
	UFUNCTION(BlueprintCallable, Category = "Procedural|Health")
	void InitialiseWithAbilitySystem(UProceduralAbilitySystemComponent* InASC);

	/**
	 * Uninitialise the component, clearing any references to the ability system
	 */
	UFUNCTION(BlueprintCallable, Category = "Procedural|Health")
	void UninitialiseFromAbilitySystem();

	/**
	 * Gets the current health value
	 * @return The current health value
	 */
	UFUNCTION(BlueprintCallable, Category = "Procedural|Health")
	float GetHealth() const;

	/**
	 * Get the current max health value
	 * @return The current max health value
	 */
	UFUNCTION(BlueprintCallable, Category = "Procedural|Health")
	float GetMaxHealth() const;

	/**
	 * Returns the current health as a percentage
	 * @return The current health in the range of 0.0 - 1.0
	 */
	UFUNCTION(BlueprintCallable, Category = "Procedural|Health")
	float GetHealthNormalised() const;

	
	/**
	 * @brief Applies enough damage to kill the owner
	 * @param bFellOutOfWorld Did the actor fall out of the world
	 */
	virtual void DamageSelfDestruct(bool bFellOutOfWorld = false);


public:

	// Delegate fired when the health value has changed. This is called on the client but the instigator may not be valid
	UPROPERTY(BlueprintAssignable)
	FProceduralHealth_AttributeChanged OnHealthChanged;

	// Delegate fired when the max health value has changed. This is called on the client but the instigator may not be valid
	UPROPERTY(BlueprintAssignable)
	FProceduralHealth_AttributeChanged OnMaxHealthChanged;
	
	// Delegate fired when the health value reaches 0. This is called on the client but the instigator may not be valid
	UPROPERTY(BlueprintAssignable)
	FProceduralHealth_AttributeChanged OnOutOfHealth;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void OnUnregister() override;
	
	void ClearGameplayTags();

	
	virtual void HandleHealthChange(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleMaxHealthChange(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	

protected:

	// Ability system used by this component
	UPROPERTY()
	TObjectPtr<UProceduralAbilitySystemComponent> AbilitySystemComponent;

	// Health set used by this component
	UPROPERTY()
	TObjectPtr<const UProceduralHealthSet> HealthSet;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
