#include "body.h"

#include <iostream>

// namespaces
using namespace std;

void body::setXForce(double input_x_force) {
    F_x += input_x_force;
}

void body::setYForce(double input_y_force) {
    F_y += input_y_force;
}

void body::resetForce() {
    F_x = 0;
    F_y = 0;
}

void body::calculateLeapFrogVertletIntegration(double dt) {
    if (mass != -1) {
        double a_x = F_x/mass;  // calculate x acceleration
        double a_y = F_y/mass;  // calculate y acceleration

        //printf("body.calculateLeapFrogVertletIntegration(): dt (timestep): %f \n", dt);  // debug statement

        double new_x_pos = x_pos + (x_vel * dt) + (0.5 * a_x * (dt * dt));
        double new_y_pos = y_pos + (y_vel * dt) + (0.5 * a_y * (dt * dt));

        double new_x_vel = x_vel + (a_x * dt);
        double new_y_vel = y_vel + (a_y * dt);

        x_pos = new_x_pos;
        y_pos = new_y_pos;
        x_vel = new_x_vel;
        y_vel = new_y_vel;

        if (x_pos < 0 || x_pos > 4) {
            mass = -1;
        }

        if (y_pos < 0 || y_pos > 4) {
            mass = -1;
        }
    }
}

void body::printBody() {
    printf("body: (%d, %.6f, %.6f, %.6f, %.6f, %.6f) force: (%.6f, %.6f) \n", index, x_pos, y_pos, mass, x_vel, y_vel, F_x, F_y);
}

void body::printBodyNoSpace() {
    printf("body: (%d, %.6f, %.6f, %.6f, %.6f, %.6f) force: (%.6f, %.6f) +++ ", index, x_pos, y_pos, mass, x_vel, y_vel, F_x, F_y);
}

void body::printBodyRank(int rank) {
    printf("rank(%d) body: (%d, %.6f, %.6f, %.6f, %.6f, %.6f) force: (%.6f, %.6f) \n", rank, index, x_pos, y_pos, mass, x_vel, y_vel, F_x, F_y);
}