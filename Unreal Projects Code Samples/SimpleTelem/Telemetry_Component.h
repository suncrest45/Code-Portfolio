// ------------------------------------------------------------------------------
// File Name: FrontendUISubsystemSample.cpp
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: SSFrontend
// Date Created: 2024-10-10
// Date Last Modified: 2026-02-25
// Copyright © 2026 Aidan Straker
//
// Description: An actor component that uses a blackboard to handle telemetry and player modelling
// ------------------------------------------------------------------------------

#pragma once




#include "CoreMinimal.h"
#include "EngineMinimal.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/ActorComponent.h"
#include "Telemetry_Component.generated.h"


// This is the enum we use to determine what data type a blackboard key is
UENUM()
enum EKeyTpe : uint8
{
	Type_Unknown,
	Type_Int,
	Type_Float,
	Type_String,
	Type_Bool,
	Type_Enum,
	Type_Vector,
	Type_Rotator,
	Type_Name
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIMPLETELEM_API UTelemetry_Component : public UBlackboardComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTelemetry_Component();

private:
	/// @brief Get the type of blackboard key
	/// @param Key - The key inside the blackboard
	/// @return An enum indicating the data type of the key
	EKeyTpe GetKeyType(const FBlackboardEntry& Key) const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// The name of the file to write to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
	FString FileName;
	
	// This map relates the enum above to the class names of blackboard data types
	const TMap<FString, EKeyTpe> Telemetry_Map = {  {"BlackboardKeyType_Int",Type_Int          },
													{"BlackboardKeyType_Float", Type_Float     },
													{"BlackboardKeyType_Bool", Type_Bool       },
													{"BlackboardKeyType_Name", Type_Name       },
													{"BlackboardKeyType_String", Type_String   },
													{"BlackboardKeyType_Rotator", Type_Rotator },
													{"BlackboardKeyType_Vector", Type_Vector   },
													{"BlackboardKeyType_Enum", Type_Enum}      };

	// These are the headers of the CSV
	FString Telemetry_KeysString = ",";
	// These are the values of the string
	FString Telemetry_ValuesString;

	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// @brief Converts the keys in the blackboard to a string
	UFUNCTION(BlueprintCallable, Category = "Telemetry")
	void ConcatenateKeysAndValues();

	/// @brief Gets the headers string
	UFUNCTION(BlueprintCallable, Category = "Telemetry")
	FString GetHeadersString();

	UFUNCTION(BlueprintCallable, Category = "Telemetry")
	FString GetValuesString();

	/// @brief Add a key to the blackboard as an integer
	/// @param Key				  - The key we want to add to the blackboard
	/// @param CompoundAssignment - Add the value to the value being stored
	/// @param Value			  - The value of the key we want to add
	UFUNCTION(BlueprintCallable, Category = "Telemetry")
	void AddTrackersAsInt(const FString& Key, bool CompoundAssignment, int Value);

	/// @brief Add a key to the blackboard as a float
	/// @param Key				  - The key we want to add to the blackboard
	/// @param CompoundAssignment - Add the value to the value being stored
	/// @param Value			  - The value of the key we want to add
	UFUNCTION(BlueprintCallable, Category = "Telemetry")
	void AddTrackersAsFloat(const FString& Key, bool CompoundAssignment, float Value);

	/// @brief Add a key to the blackboard as a string
	/// @param Key	 - The key we want to add to the blackboard 
	/// @param Value - The value of the key we want to add
	UFUNCTION(BlueprintCallable, Category = "Telemetry")
	void AddTrackersAsString(const FString& Key, FString Value);

	/// @brief Add a key to the blackboard as a boolean
	/// @param Key	 - The key we want to add to the blackboard 
	/// @param Value - The value of the key we want to add
	UFUNCTION(BlueprintCallable, Category = "Telemetry")
	void AddTrackersAsBool(const FString& Key, bool Value);

	/// @brief Update a vector in the telemetry blackboard
	/// @param Key   - The name of the vector we are tracking
	/// @param Value - The actual vector we are tracking
	UFUNCTION(BlueprintCallable, Category = "Telemetry")
	void AddTrackersAsVector(const FString& Key, const FVector& Value);

	/// @brief Update a FName key in the blackboard
	/// @param Key - The name of the FName variable we are tracking
	/// @param Value - The FName we are tracking
	UFUNCTION(BlueprintCallable, Category = "Telemetry")
	void AddTrackersAsName(const FString& Key, const FName& Value);

	UFUNCTION(BlueprintCallable, Category = "Telemetry")
	void AddTrackersAsRotator(const FString& Key, const FRotator& Value);

	/// @brief Prints telemetry data to a CSV
	/// @param Append	   - Overwrite or add to the file
	/// @param MinuteStamp - Do or do not minute stamp the CSV file
	UFUNCTION(BlueprintCallable, Category = "Telemetry")
	void PrintTelemetry(bool Append, bool MinuteStamp);
};
