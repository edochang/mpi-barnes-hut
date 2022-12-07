#pragma once

#include <vector>

// Custom Libraries
#include "argparse.h"

/*  The duple is in the following structure.
        index (int): the index of each particle is considered as the particle's name.  Therefore, in the output file, the order of particles does not matter since each particle will be tracked by its index. You should keep the index of each particle safely with the particle because the autograder will check the correctness of the output file based on the index of each particle.
        x_position (double)
        y_position (double)
        mass (double)
        x_velocity (double)
        y_velocity (double)
*/
struct body {
    int index;  // index identifier of the body.
    double x_pos, y_pos;  // x and y coordinate positions of a body.
    double mass;  // The mass of the body.
    double x_vel, y_vel;  // The velocity of the body.
    double F_x, F_y;  // Projected x or y force on the body.

    body(int input_index = 0, double input_x_pos = 0, double input_y_pos = 0, double input_mass = 0, double input_x_vel = 0, double input_y_vel = 0, double input_F_x = 0, double input_F_y = 0) : index(input_index), x_pos(input_x_pos), y_pos(input_y_pos), mass(input_mass), x_vel(input_x_vel), y_vel(input_y_vel), F_x(input_F_x), F_y(input_F_y) {}

    // Resets the x and y forces to 0, to recalculate the acting forces on the body.
    void resetForce();

    // Set's the force acting on the body's x-axis.
    void setXForce(double input_x_force);
    
    // Set's the force acting on the body's y-axis.
    void setYForce(double input_y_force);

    // Calculates the new position and velocity of the body given its current position and velocity based on the total x and y forces acting on the body.
    void calculateLeapFrogVertletIntegration(double dt);

    // Prints the data contained in a body in teh following format:
    // body: (index, x, y, mass, x_velociy, y_velocity) force: (x_force, y_force)
    void printBody();
    void printBodyRank(int rank);
    void printBodyNoSpace();
};