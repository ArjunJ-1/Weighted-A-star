// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GridNode.h"
#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Ship.generated.h"

class ALevelGenerator;

UCLASS()
class FIT3094_A1_CODE_API AShip : public AActor
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, Category = "Stats")
		float MoveSpeed;
	UPROPERTY(EditAnywhere, Category = "Stats")
		float Tolerance;
	UPROPERTY(EditAnywhere, Category = "Mesh")
		UBoxComponent* BoxCollision;

	FVector Heading;

public:	
	// Sets default values for this actor's properties
	AShip();
	bool GeneratePath = true;
	TArray<GridNode*> Path;
	ALevelGenerator* Level;
	int Morale = 50;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Virtual function to check for collision
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
