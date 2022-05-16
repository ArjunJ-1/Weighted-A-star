// Fill out your copyright notice in the Description page of Project Settings.


#include "TPriorityQueue.h"

template <typename ElementType>
TPriorityQueue<ElementType>::TPriorityQueue()
{
	Array.Heapify();
}

template <typename ElementType>
ElementType TPriorityQueue<ElementType>::Pop()
{
	TPriorityQueueNode<ElementType> Node;
	Array.HeapPop(Node);
	return Node.Element;
}

// HeapPop the node and return the element with it's priority
template <typename ElementType>
TPriorityQueueNode<ElementType> TPriorityQueue<ElementType>::PopNode()
{
	TPriorityQueueNode<ElementType> Node;
	Array.HeapPop(Node);
	return Node;
}

// Push element onto the queue
template <typename ElementType>
void TPriorityQueue<ElementType>::Push(ElementType Element, float Priority)
{
	Array.HeapPush(TPriorityQueueNode<ElementType>(Element, Priority));
}

// True if the queue is empty
template <typename ElementType>
bool TPriorityQueue<ElementType>::IsEmpty() const
{
	return Array.Num() == 0;
}

// Return the Size of the queue
template <typename ElementType>
float TPriorityQueue<ElementType>::Num()
{
	return Array.Num();
}

template <typename ElementType>
bool TPriorityQueue<ElementType>::Contains(ElementType Element, float Priority)
{
	return Array.Contains(TPriorityQueueNode<ElementType>(Element, Priority));
}
