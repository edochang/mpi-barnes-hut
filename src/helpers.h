#pragma once

#include <vector>

// Custom Libraries
#include "body.h"
#include "treenode.h"

// namespaces
using namespace std;

// Calculates the distance between two points with their x and y points.
double calculateDistance(double x1, double y1, double x2, double y2);

double calculateAxisDistance(double p1, double p2);

// Returns the absolute double value by doing a sign multiplication.
double absoluteFunction(double input_value);

// Return whether the double value is a negative number or not.
bool isNegative(double input);

// Print the vector of bodies.
void printBodies(vector<body>& bodies);
void printBodies(vector<body>& bodies, int rank);
void printBodiesNoSpace(vector<body>& bodies);

// Print the array of bodies.
void printBodies_Array_Debug(double* bodies_array, int records);

// Convert a point in the 0 to 4 coordiante system into a -1 to 1 coordinate system.
double getWindowPoint(double point);

// Convert space length from 0 to 4 coordinate space to -1 to 1 coordinate space.
double getWindowSpaceLength(double space_length);

// Draw the QuadTree bounds onto the glfw window.
void drawQuadTreeBounds2D(TreeNode& node);

// Draw the particle body onto the glfw window.
void drawParticle2D(double x_window, double y_window,
                    double radius,
                    float* colors);