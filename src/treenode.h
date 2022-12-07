#pragma once

#include <vector>
#include <memory>

// Custom Libraries
#include "argparse.h"
#include "body.h"

using namespace std;

class TreeNode {
   public:
    /* Public Functions */
    // Creates a TreeNode for a Barnes-Hut Tree.
    TreeNode(int input_level, double input_space_length, double = 0, double = 0);
    ~TreeNode();

    // Returns the TreeNode's x position.
    double getXPosition();
    
    // Returns the TreeNode's y position.
    double getYPosition();

    // Returns the TreeNode's space length.
    double getSpaceLength();

    // Returns a pointer reference to the TreeNode's children vector.
    vector<shared_ptr<TreeNode>>* getChildren();

    // True if the node is a leaf or False if it isn't.
    bool isLeaf();

    // Inserts a body into a TreeNode.
    void insertBody(body& body);

    // Caculates the center of mass for the TreeNodes in a Barnes-Hut Tree.
    void calculateCenterOfMass();

    /*  Calculate the net force onto a body using the following calculations:
        Fx = G*M0*M1*dx / d^3
        Fy = G*M0*M1*dy / d^3
    */
    void calculateNetForce(body& body, double theta);

    // Recursively traverses the tree from the root to print out the internal space nodes and external space nodes.
    void printTree();

   private:
    int level;
    double x, y, space_length;
    vector<shared_ptr<TreeNode>> children;
    bool leaf;
    body* node_body_ptr;
    double com_x_sum, com_y_sum;  // Pre-Center of Mass (com) summation before dividing by total mass
    double com_x, com_y;  // Center of Mass (com)
    double total_mass;

    /* Private Functions */
    // Creates children TreeNodes for NW, NE, SW, SE spaces.
    void subdivide();

    // Determines which children the body needs to be dispatced to, to insert the body into the appropriate TreeNode.
    void dispatchInsertBody(body& body);
};