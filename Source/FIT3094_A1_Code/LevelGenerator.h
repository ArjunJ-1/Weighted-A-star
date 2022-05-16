// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Ship.h"
#include "CoreMinimal.h"
#include "Gold.h"
#include "GameFramework/Actor.h"
#include "GridNode.h"
#include "LevelGenerator.generated.h"

UCLASS()
class FIT3094_A1_CODE_API ALevelGenerator : public AActor
{
	GENERATED_BODY()

	// Maximum Size for World Map
	static const int MAX_MAP_SIZE = 255;

public:

	// Grid Size in World Units
	static const int GRID_SIZE_WORLD = 100;
	
	// Sets default values for this actor's properties
	ALevelGenerator();

	UPROPERTY(BlueprintReadOnly)
		int MapSizeX;
	UPROPERTY(BlueprintReadOnly)
		int MapSizeY;
	UPROPERTY()
		TArray<FVector2D> GoldArray;
	UPROPERTY()
		TArray<AActor*> PathDisplayActors;

	// This is a 2D Array for holding nodes for each part of the world
	GridNode* WorldArray[MAX_MAP_SIZE][MAX_MAP_SIZE];

	UPROPERTY()
		TArray<AGold*> GoldActors;

	// Actors for spawning into the world
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> PathDisplayBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> DeepBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> LandBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> ShallowBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> GoldBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> ShipBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		AActor* Camera;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Generate and spawn the map in the world
	void SpawnWorldActors(char Grid[MAX_MAP_SIZE][MAX_MAP_SIZE]);
	void GenerateNodeGrid(char Grid[MAX_MAP_SIZE][MAX_MAP_SIZE]);
	void ResetAllNodes();

	// calculate manhattan distance
	float CalculateDistanceBetween(GridNode* first, GridNode* second);
	float CalculateHeuristicDist(GridNode* GoalNode, GridNode* CurrentNode);

	// spawn gold on the map
	void SpawnNextGold();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void GenerateWorldFromFile(TArray<FString> WorldArray);
	

	int SearchCount = 0;
	float HeuristicCost = 0;
	int GCost = 0;
	bool IsPathCalculated;
	GridNode* StartNode;
	GridNode* GoalNode;
	AShip* Ship;

	// Pathfinding algorithms and utility functions
	void CalculateBFS();
	void CalculateAStar();
	void RenderPath();
	void DetailPath();
	void ResetPath();
	void CollectGold();
	void RelaxValidNodes(GridNode* &tempNode, GridNode* &currentNode, TArray<GridNode*> &closedNodes,
		TArray<GridNode*> &NodesToVisit);
};