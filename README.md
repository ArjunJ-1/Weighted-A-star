# Weighted A-Star Algorithm with Binary Heap
 A* Weight can be changed by multiplying the heuristic function with a constant > 1


# About A*
A* path-finding algorithm: LevelGenerator.cpp

A* as compared to any other algorithm in its family, like Best first search and Dijkstra, expands as few nodes as possible and still returns the shortest possible route to the goal.
This is even better when we introduce heaps into the A* algorithm to reduce the time complexity of finding the node with minimum F value to O(1), the construction however take
O(LogN) and is done N times so total complexity of O(NLogN). N is the F distance between the start and the goal node.

On top of being greedy, A* makes use of manhattan distance in this instance. This distances, when added with the G cost gives us F cost which is the cost of expanding the
neighbor. This makes it so that the nodes that are of higher G cost and that are in other direction have a higher cost, relative to the cost of nodes close to the goal node. Giving
us a short, and direct route to the goal that has the least cost possible.

BFS and dijkstra were not used sice they expand nodes in all direction to give the shortest path. BFS does not account for the cost of the path and dijkstra, even though gives the
best possible route, is blind and expands all neighbors around it. A* on the other hand makes use of the cost and heuristic distance to be as optimal as possible.

Rotation of ship: Ship.cpp

The rotation is implement using a RInterpTo function that takes in the current rotation, heading, delta time and speed. Using Heading which is simple the direction vector from 
our current location to the next checkpoint on the path, we can find the rotation of the vector and use it to rotate the ship smoothly.
