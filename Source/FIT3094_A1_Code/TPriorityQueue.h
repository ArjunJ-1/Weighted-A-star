// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

template <typename ElementType>
struct TPriorityQueueNode
{
	float Priority;
	ElementType Element;

	TPriorityQueueNode()
	{
	}

	TPriorityQueueNode(ElementType Element_Type, float Element_Priority)
	{
		Element = Element_Type;
		Priority = Element_Priority;
	}

	bool operator < (const TPriorityQueueNode<ElementType> Other) const
	{
		return Priority < Other.Priority;
	}
	
};

template <typename ElementType>
class FIT3094_A1_CODE_API TPriorityQueue
{
public:
	TPriorityQueue();
	
	// Always check if IsEmpty() before Pop-ing!
	ElementType Pop();

	TPriorityQueueNode<ElementType> PopNode();

	void Push(ElementType Element, float Priority);

	bool IsEmpty() const;

	float Num();

	auto Contains(ElementType Element, float Priority) -> bool;

private:
	TArray<TPriorityQueueNode<ElementType>> Array;
};