// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GridNode.h"
#include "CoreMinimal.h"

/**
 * 
 */
class FIT3094_A1_CODE_API PathGenerator
{
public:
	// Sets default values for this pawn's properties
	PathGenerator();
	//~PathGenerator();

	static const int GRID_SIZE = 40;
	static const int OFFSET = 50;
	static const int GRID_WORLD_SIZE = 100;

	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> StartActorEntity;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> GoalActorEntity;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> PathActor;

protected:
	int SpawnGrid[GRID_SIZE][GRID_SIZE];
	GridNode* GridNodes[GRID_SIZE][GRID_SIZE];

	TArray<AActor*> Path;
	

	int SearchCount = 0;
	float TotalHeuristicCost = 0;
	bool IsPathCalculated;
	GridNode* StartNode;
	GridNode* GoalNode;

	TArray<GridNode*> GeneratePath();

	void CalculateBFS();
	void CalculateAStar();
	void ValidateNode(GridNode* &tempNode, GridNode* &currentNode, TArray<GridNode*> &closedNodes,
		TArray<GridNode*> &NodesToVisit);
	float CalculateHeuristicDist(GridNode* GoalNode, GridNode* CurrentNode);
	TArray<GridNode*> CalculateDFS();
	void ResetPath();
	void RenderPath();

	TArray<GridNode*> GenerateNodeArray();

};
