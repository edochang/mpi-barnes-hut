#include "treenode.h"

#include <iostream>

// Custom Libraries
#include "helpers.h"

/* Global Variables / Constants */
// Universal Gravitational Constant
const double G = 0.0001;

TreeNode::TreeNode(int input_level, double input_space_length, double input_x, double input_y) {
    level = input_level;
    x = input_x;
    y = input_y;
    space_length = input_space_length;
    leaf = true;
    node_body_ptr = nullptr;
    total_mass = 0;
    com_x_sum = 0;
    com_y_sum = 0;
    com_x = 0;
    com_y = 0;
    children.resize(0);
}

TreeNode::~TreeNode() {
    // printf("Deconstructing TreeNode \n");
    node_body_ptr = nullptr;
    free(node_body_ptr);
}

double TreeNode::getXPosition() {
    return x;
}

double TreeNode::getYPosition() {
    return y;
}

double TreeNode::getSpaceLength() {
    return space_length;
}

vector<shared_ptr<TreeNode>>* TreeNode::getChildren() {
    return &children;
}

bool TreeNode::isLeaf() {
    return leaf;
}

void TreeNode::insertBody(body& body) {
    /*  Check the following conditions...
        1. if the node is a leaf.
        2. if the node has a body or not.
        3. if x and y is within the space coordinates of the node.
    */

    if (body.mass != -1) {
        if (leaf == true) {
            if (node_body_ptr == nullptr) {
                node_body_ptr = &body;
            } else {
                leaf = false;
                this->subdivide();
            }
        }

        if (leaf == false) {
            if (node_body_ptr != nullptr) {
                com_x_sum += node_body_ptr->x_pos * node_body_ptr->mass;
                com_y_sum += node_body_ptr->y_pos * node_body_ptr->mass;
                total_mass += node_body_ptr->mass;
                com_x = com_x_sum / total_mass;
                com_y = com_y_sum / total_mass;
                dispatchInsertBody(*node_body_ptr);
                node_body_ptr = nullptr;
            }
            com_x_sum += body.x_pos * body.mass;
            com_y_sum += body.y_pos * body.mass;
            total_mass += body.mass;
            com_x = com_x_sum / total_mass;
            com_y = com_y_sum / total_mass;
            dispatchInsertBody(body);

            // printf("com_x_sum: %.9f \n", com_x_sum);
        }
    }
}

void TreeNode::subdivide() {
    double new_space_length = space_length / 2;

    // printf("TreeNode.insertBody: Creates child TreeNodes \n");                                   // debug statement
    children.push_back(make_shared<TreeNode>(TreeNode(level + 1, new_space_length, x, y + new_space_length)));                     // index 0 - NW
    children.push_back(make_shared<TreeNode>(TreeNode(level + 1, new_space_length, x + new_space_length, y + new_space_length)));  // index 1 - NE
    children.push_back(make_shared<TreeNode>(TreeNode(level + 1, new_space_length, x, y)));                                        // index 2 - SW
    children.push_back(make_shared<TreeNode>(TreeNode(level + 1, new_space_length, x + new_space_length, y)));                     // index 3 - SE
    // printf("TreeNode: Children vector size is: %d \n", static_cast<int>(children.size()));       // debug statement
}

void TreeNode::dispatchInsertBody(body& body) {
    for (int i = 0; i < (int)children.size(); ++i) {
        double lower_x = children[i]->x;
        double upper_x = children[i]->x + children[i]->space_length;
        double lower_y = children[i]->y;
        double upper_y = children[i]->y + children[i]->space_length;

        if ((body.x_pos >= lower_x && body.x_pos <= upper_x) && (body.y_pos >= lower_y && body.y_pos <= upper_y)) {
            children[i]->insertBody(body);
            break;
        }
    }
}

void TreeNode::calculateNetForce(body& body, double theta) {
    bool run_calculation = false;
    double s_over_d = 0;
    double node_x_pos = 0;
    double node_y_pos = 0;
    double node_mass = 0;

    // printf("treenode.calculateNetForce(): theta(%f) \n", theta);  // debug statement
    // printf("treenode.calculateNetforce(): children.size(): %d \n", static_cast<int>(children.size()));  // debug statement

    double body_x_pos = body.x_pos;
    double body_y_pos = body.y_pos;
    double body_mass = body.mass;

    // Set calculation variables
    // Run calculations for internal space nodes containing childrens.
    if (leaf == false) {
        node_x_pos = com_x;
        node_y_pos = com_y;
        node_mass = total_mass;
        run_calculation = true;
    } else {
        // Run calculations for external leaf nodes that contain a body.
        if (leaf == true && node_body_ptr != nullptr) {
            // check if the body is not itself
            if (node_body_ptr->index != body.index) {
                // check if the body is not lost
                if (node_body_ptr->mass != -1) {
                    node_x_pos = node_body_ptr->x_pos;
                    node_y_pos = node_body_ptr->y_pos;
                    node_mass = node_body_ptr->mass;
                    run_calculation = true;
                }
            } else {
                // printf("treenode.calculateNetForce(): Body(%d) skips itself in Net Force Calculation with node_body(%d). \n", body.getIndex(), node_body_ptr->getIndex());  // debug statement
                // printf("treenode.calculateNetForce(): run_calculation: %s \n", run_calculation ? "true" : "false");  // debug statement
            }
        }
    }

    if (run_calculation) {
        double distance = calculateDistance(body.x_pos, body.y_pos, node_x_pos, node_y_pos);
        if (leaf == false) {
            s_over_d = space_length / distance;
            if (s_over_d < theta) {
                double d_3 = distance * distance * distance;

                // double d_x = node_x_pos - body_x_pos;
                double d_x = calculateAxisDistance(body_x_pos, node_x_pos);
                double F_x = (G * body_mass * node_mass * d_x) / d_3;
                body.setXForce(F_x);

                // double d_y = node_y_pos - body_y_pos;
                double d_y = calculateAxisDistance(body_y_pos, node_y_pos);
                double F_y = (G * body_mass * node_mass * d_y) / d_3;
                body.setYForce(F_y);

                // printf("BodyIdx(%d) COM(%.6f) G(%.6f), body_mass(%.6f), node_mass(%.6f), d_3(%.6f), d_x(%.6f), F_x(%.6f), d_y(%.6f), F_y(%.6f) \n", body.getIndex(), s_over_d, G, body_mass, node_mass, d_3, d_x, F_x, d_y, F_y);  // debug statement
            } else {
                for (int i = 0; i < static_cast<int>(children.size()); ++i) {
                    children[i]->calculateNetForce(body, theta);
                }
            }
        } else {
            if (node_body_ptr != nullptr) {
                if (body.index != node_body_ptr->index) {
                    double d_3 = distance * distance * distance;

                    // double d_x = node_x_pos - body_x_pos;
                    double d_x = calculateAxisDistance(body_x_pos, node_x_pos);
                    double F_x = (G * body_mass * node_mass * d_x) / d_3;
                    body.setXForce(F_x);

                    // double d_y = node_y_pos - body_y_pos;
                    double d_y = calculateAxisDistance(body_y_pos, node_y_pos);
                    double F_y = (G * body_mass * node_mass * d_y) / d_3;
                    body.setYForce(F_y);

                    // printf("BodyIdx(%d) NodeBody(%d), G(%.6f), body_mass(%.6f), node_mass(%.6f), d_3(%.6f), d_x(%.6f), F_x(%.6f), d_y(%.6f), F_y(%.6f) \n", body.getIndex(), node_body_ptr->getIndex(), G, body_mass, node_mass, d_3, d_x, F_x, d_y, F_y);  // debug statement
                } else {
                    printf("treenode.calculateNetForce(): Body(%d) skips itself in Net Force Calculation with node_body(%d). \n", body.index, node_body_ptr->index);  // debug statement
                }
            }
        }
    }
}

void TreeNode::printTree() {
    if (leaf == false) {
        string bread_crumb = "|-";
        printf("Node Space level(%d), x(%.8f), y(%.8f), space_length(%f), leaf(%s): Center of Mass - com_x(%.8f), com_y(%.8f), total_mass(%.8f) \n", level, x, y, space_length, leaf ? "true" : "false", com_x, com_y, total_mass);
        for (int i = 0; i < level; ++i) {
            bread_crumb.append("-");
        }
        for (int i = 0; i < static_cast<int>(children.size()); ++i) {
            printf("%schild %d ", bread_crumb.c_str(), i);
            children[i]->printTree();
        }
    } else {
        if (node_body_ptr != nullptr) {
            printf("Node Space level(%d), x(%.8f), y(%.8f), space_length(%f), leaf(%s): ", level, x, y, space_length, leaf ? "true" : "false");
            node_body_ptr->printBody();
        } else {
            printf("Node Space level(%d), Empty Leaf \n", level);
        }
    }
}
