#include "helpers.h"

#include <math.h>
#include <iostream>
#include <memory>

// Visualization
#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/* Global Variables / Constants */
// If the distance between two bodies are shorter than rlimit, then rlimit is used as the distance between the two bodies
const double rlimit = 0.03;

double calculateDistance(double x1, double y1, double x2, double y2) {
    double x_dif = x2 - x1;
    double y_dif = y2 - y1;

    double distance = sqrt((x_dif * x_dif) + (y_dif * y_dif));

    // When two bodies are too close, the force computed using the above formula can be infinitely large. To avoid infinity values, set a rlimit value such that if the distance between two bodies are shorter than rlimit, then rlimit is used as the distance between the two bodies.
    if (distance < rlimit) {
        distance = rlimit;
    }

    return distance;
}

double calculateAxisDistance(double p1, double p2) {
    double distance = p2 - p1;

    /*
    if (absoluteFunction(distance) < rlimit) {
        if (isNegative(distance)) {
            distance = rlimit * -1;
        } else {
            distance = rlimit;
        }
    }
    */
    return distance;
}

double absoluteFunction(double input_value) {
    return (isNegative(input_value)) ? input_value * -1 : input_value;
}

bool isNegative(double input) {
    return input < 0;
}

void printBodies(vector<body>& bodies) {
    int bodies_size = bodies.size();
    for (int i = 0; i < bodies_size; ++i) {
        bodies[i].printBody();
    }
}

void printBodies(vector<body>& bodies, int rank) {
    int bodies_size = bodies.size();
    for (int i = 0; i < bodies_size; ++i) {
        bodies[i].printBodyRank(rank);
    }
}

void printBodiesNoSpace(vector<body>& bodies) {
    int bodies_size = bodies.size();
    for (int i = 0; i < bodies_size; ++i) {
        bodies[i].printBodyNoSpace();
    }
    printf("\n");
}

void printBodies_Array_Debug(double* bodies_array, int bodies_array_length) {
    printf("bodies_array:");
    for (int i = 0; i < bodies_array_length; ++i) {
        printf(" %.6f", bodies_array[i]);
    }
    printf("\n");
}

// Convert a point in the 0 to 4 coordiante system into a -1 to 1 coordinate system.
double getWindowPoint(double point) {
    return (2 * point / 4.0) - 1;
}

// Convert space length from 0 to 4 coordinate space to -1 to 1 coordinate space.
double getWindowSpaceLength(double space_length) {
    return space_length / 2;
}

// Draw the QuadTree bounds onto the glfw window.
void drawQuadTreeBounds2D(TreeNode& node) {
    // int i;

    /*
    if (!node || node is external)
        return;
    */
    if (node.isLeaf()) {
        return;
    }

    double space_length = getWindowSpaceLength(node.getSpaceLength());

    double h_start_x = getWindowPoint(node.getXPosition());
    double h_start_y = getWindowPoint(node.getYPosition()) + space_length / 2;
    double h_end_x = h_start_x + space_length;
    double h_end_y = h_start_y;
    double v_start_x = getWindowPoint(node.getXPosition()) + space_length / 2;
    double v_start_y = getWindowPoint(node.getYPosition());
    double v_end_x = v_start_x;
    double v_end_y = v_start_y + space_length;

    glBegin(GL_LINES);
    // set the color of lines to be white
    glColor3f(1.0f, 1.0f, 1.0f);
    // specify the start point's coordinates
    glVertex2f(h_start_x, h_start_y);
    // specify the end point's coordinates
    glVertex2f(h_end_x, h_end_y);
    // do the same for verticle line
    glVertex2f(v_start_x, v_start_y);
    glVertex2f(v_end_x, v_end_y);
    glEnd();

    /* for each subtree of node
       drawOctreeBounds2D(subtree); */
    vector<shared_ptr<TreeNode>>* children = node.getChildren();
    int children_size = children->size();
    for (int i = 0; i < children_size; ++i) {
        drawQuadTreeBounds2D(*(*children)[i]);
    }
}

// Draw the particle body onto the glfw window.
void drawParticle2D(double x_window, double y_window,
                    double radius,
                    float* colors) {
    int k = 0;
    float angle = 0.0f;
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(colors[0], colors[1], colors[2]);
    glVertex2f(x_window, y_window);
    for (k = 0; k < 20; k++) {
        angle = (float)(k) / 19 * 2 * 3.141592;
        glVertex2f(x_window + radius * cos(angle),
                   y_window + radius * sin(angle));
    }
    glEnd();
}