// ------------------------------------------------------------------------------
// File Name: TDS_DungeonGenerator.h
// Author: Aidan Straker(aidan.straker@gmail.com)
// Project: TDS_Procedural
// Date Created: 2025-11-30
// Date Last Modified: 2026-02-25
// Copyright © 2025 Aidan Straker
// ------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Telemetry/Recording/Telemetry_Component.h"
#include "TDS_DungeonGenerator.generated.h"

enum class ELevelState : uint8;
class UInstancedStaticMeshComponent;

class ATDS_DungeonGenerator;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGenerationFinished, ATDS_DungeonGenerator*, Generator, FVector, StartPosition, FVector, EndPosition);

UENUM()
enum ECorridorDirection : uint8
{
	Up_Right = 0,
	Right_Up = 1,
	Up_Left = 2,
	Left_Up = 3,
	Down_Right = 4,
	Right_Down = 5,
	Down_Left = 6,
	Left_Down = 7,
};

UENUM()
enum ECorridorType
{
	Normal = 0 UMETA(DisplayName = "Normal"),
	Alcove = 1 UMETA(DisplayName = "Alcove"),
};

UCLASS()
class TDS_PROCEDURAL_API ATDS_DungeonGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATDS_DungeonGenerator();

	/// @brief Gte the coordinates to teleport the player to
	/// @return The coordinates to teleport the player to
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "PCG|Spawn")
	FVector GetPlayerSpawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called when the game ends 
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	/// @brief Generates the dungeon 
	void GenerateDungeon();

	/// @brief Given a location, formulates the extents of the room and coordinates of the tiles in the room
	/// @param Location The root location of the room
	/// @param ConnectedTiles An array representing the coordinates of the tiles that make up the room
	/// @return The X and Y extents of the room as a vector
	FIntVector Room_MakeFloor(const FIntVector& Location, TArray<FIntVector>& ConnectedTiles);

	/// @brief iterates through the floor tiles array, spawning a mesh at each coordinate
	void Dungeon_SpawnTiles();

	/// @brief Determines the position of the next room
	/// @param NewLocation The location of the next room
	/// @return Whether a valid position for a new room was found
	bool Room_FindNextLocation(FIntVector& NewLocation) const;

	void Room_MakeRoom();

	/// @brief Determine which neighbours are suitable for a tile
	/// @param ReferenceLocation The starting point
	/// @param X Horizontal coordinate for neighbour
	/// @param Y Vertical Coordinate for its neighbour
	/// @param OutLocation The coordinates of the location that another tile cna be placed
	/// @return Is this location a valid coordinates to place a tile
	bool TestRelativeTileLocation(FIntVector ReferenceLocation, int32 X, int32 Y, FIntVector& OutLocation) const;

	/// @brief If a location cannot be found branch off of an existing room
	/// @return A randomly selected (based on the stream) previous room
	FIntVector Room_BranchingPath();

	/// @brief Create an instanced static mesh
	/// @param MeshFilePath File path of the mesh used for the instanced static mesh component
	/// @param IMCName The name of the instanced static mesh component
	/// @return A pointer to the newly created instanced static mesh
	TObjectPtr<UInstancedStaticMeshComponent> CreateStaticMesh(const TCHAR* MeshFilePath, const TCHAR* IMCName);

	/// @brief Given a tile determine if an outer corner should be placed and if so place it
	/// @param Tile The tile we are checking to see where to place the outer corners
	/// @param Rotation The rotation we should use to place the outer corners
	/// @param Diagonal The diagonal neighbours to the specified tile
	/// @param Neighbour_One
	/// @param Neighbour_Two
	void Room_GenerateOuterCorners(const FIntVector& Tile, const FRotator& Rotation, const FIntPoint& Diagonal, const FIntPoint& Neighbour_One, const FIntPoint& Neighbour_Two);

	/// @brief Determine which side and on which two rooms we want to connect are
	/// @param RoomA The room we want the corridor to start from
	/// @param RoomB The room we want the corridor to go to
	/// @return An array of vectors representing the corridor tiles
	TArray<FIntVector> Dungeon_MapCorridors(const FIntVector RoomA, const FIntVector RoomB);

	/// @brief Make the Y path of the corridor
	/// @param From The location to start generating the corridor from 
	/// @param To The location to end the corridor generation at
	TArray<FIntVector> Corridor_Make_Y(FIntVector From, FIntVector To);

	/// @brief Make the X path of the corridor
	/// @param From The location to start generating the corridor from 
	/// @param To The location to end the corridor generation at
	TArray<FIntVector> Corridor_Make_X(FIntVector From, FIntVector To);

	/// @brief Try to generate a corridor between two given rooms
	/// @param bIsXAxis Are the rooms parallel to each other on the X axis
	/// @param StartA The start point of Room A
	/// @param EndA The end point of Room A
	/// @param StartB The start point of Room B
	/// @param EndB The end point of Room B
	/// @return The coordinates to place the tiles for the hallway
	TArray<FIntVector> Dungeon_MakeCorridor_Parallel(const bool bIsXAxis,
	                                           const FIntVector& StartA, const FIntVector& EndA,
	                                           const FIntVector& StartB, const FIntVector& EndB);
	

	/// @brief Generate a perpendicular corridor between PointA and PointB, with PointC serving as the 90 degree bend
	/// @param Direction How are the rooms perpendicular to each other
	/// @param PointAX The X Coordinate to start generating the Corridor from RoomA
	/// @param PointAY The Y Coordinate to start generating the Corridor from RoomA
	/// @param PointBX The X Coordinate the corridor from PointC attaches to
	/// @param PointBY The Y Coordinate the corridor from PointC attaches to
	/// @param PointCX The X Coordinate the corridor from RoomA attaches to and to start generating the corridor from PointC
	/// @param PointCY The Y Coordinate the corridor from RoomA attaches to and to start generating the corridor from PointC
	/// @param Z All rooms are on the same plane so the value is the same for all points
	/// @return An array of tiles representing the corridor between PointA and PointB
	TArray<FIntVector> Dungeon_MakeCorridor_Perpendicular(ECorridorDirection Direction, int PointAX, int PointAY, int PointBX,
		int PointBY, int PointCX, int PointCY, int Z);

	/// @brief Tests for overlapping corridors when placing a room 
	/// @param ZAxis The ZAxis all other tiles are on
	/// @return Would this room overlap with a corridor
	bool Dungeon_OverlappingCorridors(const int ZAxis) const;
	
public:

	// The seed of this generation, useful for recreating it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Settings")
	int Seed;

	// The number of rooms we want in this generation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Settings")
	int Room_Count;

	// How big should the biggest room be in tiles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Settings")
	int Room_SizeMax;

	// How small the smallest room should be in tiles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Settings")
	int Room_SizeMin;

	// Allows for merging rooms together 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Settings")
	bool bIsMerging;

	// The least amount of tiles we can cull from a room, to create interesting patterns
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Settings")
	int Floor_CullMin;

	// The most amount of tiles we can cull from a room, to create interesting patterns
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Settings")
	int Floor_CullMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Settings")
	bool bIsFloorCulling;

	// A safety check that allows us to backtrack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Settings")
	bool bIsBranching;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Settings")
	int Branching_Threshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Settings")
	int Last_Branch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Settings")
	float Branching_Chance;

	// Should room spawning ignore the existence of room tiles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Settings")
	bool bIgnoreCorridors;

	// Prevent corridors from overlapping with each other
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Settings")
	bool bTruncateCorridors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Settings")
	ELevelState DungeonState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInstancedStaticMeshComponent> Floor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInstancedStaticMeshComponent> Walls;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInstancedStaticMeshComponent> OuterCorner;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInstancedStaticMeshComponent> InnerCorner;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInstancedStaticMeshComponent> Doors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Editor Tools")
	int MaxLoops;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Editor Tools")
	float Scale;

	// In editor this allows us to generate a new seed 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Editor Tools")
	bool NewSeed;

	// A random generator for the seed
	UPROPERTY(BlueprintReadOnly, Category = "Editor Tools")
	FRandomStream Stream;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Editor Tools")
	FIntVector PreviousLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Editor Tools")
	TArray<FIntVector> FloorTiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Editor Tools")
	TArray<FIntVector> HallwayTiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Editor Tools")
	TMap<FIntVector, FIntVector> RoomsMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Editor Tools")
	FIntVector Dungeon_NextRoomLocation;
	
	UPROPERTY(BlueprintAssignable, Category = "Spawning")
	FOnGenerationFinished OnGenerationFinished;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
	TObjectPtr<UTelemetry_Component> Dungeon_Telemetry;

private:

	FVector PlayerSpawnLocation;
	FRotator PlayerSpawnRotation;
};