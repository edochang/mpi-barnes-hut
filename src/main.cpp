#include <mpi.h>  // MPI libraries

#include <chrono>
#include <cstddef>
#include <iostream>
#include <memory>
#include <thread>

// Visualization
#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Custom Libraries
#include "argparse.h"
#include "body.h"
#include "helpers.h"
#include "io.h"
#include "treenode.h"

// namespaces
using namespace std;

// global variables

// main logic of the program
int main(int argc, char** argv) {
    // Local Variables
    struct options_t opts;
    GLFWwindow* window;
    double starttime, endtime;
    vector<body> bodies;

    // MPI variables
    int mpi_size, mpi_rank;
    int root = 0;

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

    // Define custom MPI Data Type
    MPI_Aint displacements[8] = {offsetof(body, index), offsetof(body, x_pos), offsetof(body, y_pos), offsetof(body, mass), offsetof(body, x_vel), offsetof(body, y_vel), offsetof(body, F_x), offsetof(body, F_y)};
    int block_lengths[8] = {1, 1, 1, 1, 1, 1, 1, 1};
    MPI_Datatype types[8] = {MPI_INT, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
    MPI_Datatype custom_body_dt;
    MPI_Type_create_struct(8, block_lengths, displacements, types, &custom_body_dt);
    MPI_Type_commit(&custom_body_dt);

    // printf("main: mpi-process rank(%d) and size(%d)\n", mpi_rank, mpi_size);  // debug statement

    // Parse args
    get_opts(argc, argv, &opts);

    // printf("main: mpi-process rank(%d) and steps(%d)\n", mpi_rank, opts.steps);  // debug statement

    if (mpi_rank == root) {
        /* Window setup */
        if (opts.visualization) {
            /* OpenGL window dims */
            int width = 600, height = 600;

            if (!glfwInit()) {
                fprintf(stderr, "Failed to initialize GLFW\n");
                return -1;
            }
            // Open a window and create its OpenGL context
            window = glfwCreateWindow(width, height, "Simulation", NULL, NULL);
            if (window == NULL) {
                fprintf(stderr, "Failed to open GLFW window.\n");
                glfwTerminate();
                return -1;
            }
            glfwMakeContextCurrent(window);  // Initialize GLEW
            if (glewInit() != GLEW_OK) {
                fprintf(stderr, "Failed to initialize GLEW\n");
                return -1;
            }
            // Ensure we can capture the escape key being pressed below
            glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
        }

        starttime = MPI_Wtime();

        // Read input data
        read_file(&opts, bodies);

        // printf("main: rank(%d), bodies: \n", mpi_rank);  // debug statement
        // printBodies(bodies);  // debug statement

        // Send number of bodies (records) to be processed by all non-root processes.
        if (mpi_size != 1) {
            // auto start = std::chrono::high_resolution_clock::now();

            // Send number of records.
            MPI_Bcast(&opts.records, 1, MPI_INT, root, MPI_COMM_WORLD);

            /*
            auto end = std::chrono::high_resolution_clock::now();
            auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            printf("%06ld\n", diff.count());
            */

            // auto start = std::chrono::high_resolution_clock::now();

            MPI_Bcast(bodies.data(), opts.records, custom_body_dt, root, MPI_COMM_WORLD);
            // printf("main: rank(%d): broadcast data \n", mpi_rank);  // debug statement

            /*
            auto end = std::chrono::high_resolution_clock::now();
            auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            printf("%06ld\n", diff.count());
            */
        }
    } else {
        //auto start = std::chrono::high_resolution_clock::now();

        // Non-root processes receive bodies
        MPI_Bcast(&opts.records, 1, MPI_INT, root, MPI_COMM_WORLD);
        // printf("main: rank(%d): opts.records: %d \n", mpi_rank, opts.records);  // debug statement

        /*
        auto end = std::chrono::high_resolution_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        printf("%06ld\n", diff.count());
        */

        bodies.resize(opts.records);
        // printf("main: rank(%d): bodies capacity (%d) \n", mpi_rank, (int)bodies.capacity());  // debug statement

        //auto start = std::chrono::high_resolution_clock::now();

        MPI_Bcast(bodies.data(), opts.records, custom_body_dt, root, MPI_COMM_WORLD);
        // printf("main: rank(%d): received bodies with size (%d) \n", mpi_rank, (int)bodies.size());  // debug statement
        // printf("main: rank(%d), bodies: \n", mpi_rank);  // debug statement
        // printBodies(bodies);  // debug statement
        
        /*
        auto end = std::chrono::high_resolution_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        printf("%06ld\n", diff.count());
        */
    }

    // this_thread::sleep_for(5000ms);  // Thread sleep to screen record.  Will comment out later.

    // printf("main: bodies: \n");  // debug statement
    // printBodies(bodies);         // debug statement

    int bodies_size = bodies.size();  // size of the bodies vector.
    // int bodies_size = opts.records;  // size of the bodies vector.
    // printf("main: rank(%d) bodies.size: %d \n", mpi_rank, bodies_size);  // debug statement

    for (int i = 0; i < opts.steps; ++i) {
        // Create root node for Barnes-Hut Tree
        unique_ptr<TreeNode> bhtree_root(new TreeNode(0, 4));

        //auto start = std::chrono::high_resolution_clock::now();

        // Insert bodies into Barnes-Hut Tree
        for (int j = 0; j < bodies_size; ++j) {
            // printf("main: insert body \n");  // debug statement
            bhtree_root->insertBody(bodies[j]);
        }

        /*
        auto end = std::chrono::high_resolution_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        printf("%06ld\n", diff.count());
        */

        if (mpi_size == 1) {
            //auto start = std::chrono::high_resolution_clock::now();
            
            // Calculate net force on bodies.
            for (int j = 0; j < bodies_size; ++j) {
                bodies[j].resetForce();
                bhtree_root->calculateNetForce(bodies[j], opts.theta);
            }

            /*
            auto end = std::chrono::high_resolution_clock::now();
            auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            printf("%06ld\n", diff.count());
            */

            //auto start = std::chrono::high_resolution_clock::now();

            // Calculate bodies new positions using Leap Frog Vertlet Integration
            for (int j = 0; j < bodies_size; ++j) {
                bodies[j].calculateLeapFrogVertletIntegration(opts.dt);
            }

            /*
            auto end = std::chrono::high_resolution_clock::now();
            auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            printf("%06ld\n", diff.count());
            */

        } else {  // multi-processor / non-sequential logic
            // Decompose the bodies for each processes to be responsible for.
            int bodies_per_process = bodies_size / mpi_size;
            int bodies_for_last_process;
            int last_rank = mpi_size - 1;

            if (bodies_per_process == 0) {
                bodies_per_process = 1;
                bodies_for_last_process = 1;
                last_rank = bodies_size - 1;
            } else {
                bodies_for_last_process = (bodies_size - (bodies_per_process * mpi_size)) + bodies_per_process;
            }

            // potential to improve with a unique pointer and allocate on stack.  This array may get too large for the stack.
            // int* recvcount = new int[last_rank + 1]{0};
            // int* displacements = new int[last_rank + 1]{0};
            int recvcount[last_rank + 1] = {0};
            int displacements[last_rank + 1] = {0};
            for (int j = 0; j < last_rank + 1; ++j) {
                displacements[j] = j * bodies_per_process;
                if (j == last_rank) {
                    recvcount[j] = bodies_for_last_process;
                } else {
                    recvcount[j] = bodies_per_process;
                }
            }

            /*
            if (mpi_rank == root and i == opts.records - 1) {
                printf("main: rank(%d), displacements: \n", mpi_rank);  // debug statement
                for (const auto& value : displacements) {
                    std::cout << value << ' ';
                }
                std::cout << endl;

                printf("main: rank(%d), recvcount: \n", mpi_rank);  // debug statement
                for (const auto& value : recvcount) {
                    std::cout << value << ' ';
                }
                std::cout << endl;
            }
            */

            if (mpi_rank <= last_rank) {
                // All process's calculated net force for their responsible bodies are all gather to root processor
                for (int p = 0; p < last_rank + 1; ++p) {
                    if (mpi_rank == p) {
                        int start_index = displacements[p];
                        int end_index = start_index + recvcount[p];

                        for (int j = start_index; j < end_index; ++j) {
                            bodies[j].resetForce();
                            bhtree_root->calculateNetForce(bodies[j], opts.theta);
                        }

                        // printf("main: rank(%d), bodies: \n", mpi_rank);  // debug statement
                        // printBodies(bodies, mpi_rank);  // debug statement

                        // int number_of_elements = end_index - start_index;
                        // printf("main: rank(%d), start_index(%d), end_index(%d), number_of_elements(%d) \n", mpi_rank, start_index, end_index, number_of_elements);  // debug statement

                        //auto start = std::chrono::high_resolution_clock::now();

                        MPI_Allgatherv(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, bodies.data(), recvcount, displacements, custom_body_dt, MPI_COMM_WORLD);

                        /*
                        auto end = std::chrono::high_resolution_clock::now();
                        auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                        printf("%06ld\n", diff.count());
                        */

                        // printf("main: rank(%d), complete allgatherv \n", mpi_rank);  // debug statement

                        // printf("main: rank(%d): received bodies with size (%d) \n", mpi_rank, (int)bodies.size());  // debug statement

                        // printf("main: rank(%d), bodies: \n", mpi_rank);  // debug statement
                        // printBodies(bodies, mpi_rank);  // debug statement
                    }
                }

                for (int p = 0; p < last_rank + 1; ++p) {
                    if (mpi_rank == p) {
                        int start_index = displacements[p];
                        int end_index = start_index + recvcount[p];

                        for (int j = start_index; j < end_index; ++j) {
                            bodies[j].calculateLeapFrogVertletIntegration(opts.dt);
                        }

                        //auto start = std::chrono::high_resolution_clock::now();

                        MPI_Allgatherv(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, bodies.data(), recvcount, displacements, custom_body_dt, MPI_COMM_WORLD);

                        /*
                        auto end = std::chrono::high_resolution_clock::now();
                        auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                        printf("%06ld\n", diff.count());
                        */
                    }
                }
            }

            // delete[] displacements;
            // delete[] recvcount;
        }

        if (opts.visualization && mpi_rank == root) {
            glClear(GL_COLOR_BUFFER_BIT);
            drawQuadTreeBounds2D(*bhtree_root);
            float colors[3] = {1.0f, 0.2f, 0.2f};
            for (int p = 0; p < bodies_size; p++) {
                colors[0] = bodies[p].mass / 4;
                drawParticle2D(getWindowPoint(bodies[p].x_pos), getWindowPoint(bodies[p].y_pos), 0.01, colors);
            }
            //  Swap buffers
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        /*
        if (i == opts.steps - 1 && mpi_rank == root) {
            printf("main: print Barnes-Hut Tree \n");  // debug statement
            bhtree_root->printTree();                  // debug statement
        }
        */
    }

    /*
    if (mpi_rank == 1) {
    printf("main: rank(%d), bodies: \n", mpi_rank);  // debug statement
    printBodies(bodies, mpi_rank);                   // debug statement
    }
    */

    MPI_Barrier(MPI_COMM_WORLD);  // barrier used to make sure all processors are synced before taking a time measurement.

    if (mpi_rank == root) {
        // printf("main: rank(%d), bodies: \n", mpi_rank);  // debug statement
        // printBodies(bodies, mpi_rank);                   // debug statement

        write_file(&opts, bodies);

        if (opts.visualization) {
            // Loop until the user closes the window
            while (!glfwWindowShouldClose(window)) {
                // spin and poll events until window closes.
                glfwPollEvents();
            }
        }

        /*  Your program should also output the elapsed time for the simulation in the following form
            xxxxxx
            Do not print out anything else (such as the unit). Make sure that the unit of the output time is second because it is assumed that you are using MPI_Wtime() to measure the time. You can also use other methods, but make sure to convert the result to seconds.
        */
        endtime = MPI_Wtime();
        printf("%06.0f\n", endtime - starttime);
    }

    MPI_Type_free(&custom_body_dt);

    MPI_Finalize();

    // printf("main: End of main() \n");  // debug statement
}