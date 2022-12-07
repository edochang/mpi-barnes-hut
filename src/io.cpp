#include <io.h>

#include <fstream>
#include <iostream>
#include <iomanip>

void read_file(struct options_t* args,
               vector<body>& bodies) {
    // Open file
    std::ifstream in;
    in.open(args->input_filename);
    // Get num vals
    in >> args->records;

    // Alloc input and output arrays.  Each record in the input file contains a record ID and 5 other dimensions.
    /* 	x_position
        y_position
        mass
        x_velocity
        y_velocity
    */
    int records_dims = args->records;

    // printf("io: records_dims: %d \n", records_dims);  // debug statement

    // Read input vals
    for (int i = 0; i < records_dims; ++i) {
        int indx;
        double x_pos, y_pos, mass, x_vel, y_vel;
        for (int j = 0; j < 6; ++j) {
            switch (j) {
                case 0:
                    in >> indx;
                    break;
                case 1:
                    in >> x_pos;
                    break;
                case 2:
                    in >> y_pos;
                    break;
                case 3:
                    in >> mass;
                    //printf("mass: %.7f \n", mass);  // debug statement
                    break;
                case 4:
                    in >> x_vel;
                    break;
                case 5:
                    in >> y_vel;
                    break;
            }
        }
        bodies.push_back(body(indx, x_pos, y_pos, mass, x_vel, y_vel));
    }

    // printf("io: bodies.size: %d \n", static_cast<int>(bodies.size()));  //debug statement
}

void read_file2(struct options_t* args,
                double** bodies_array) {
    // Open file
    std::ifstream in;
    in.open(args->input_filename);
    // Get num vals
    in >> args->records;

    // Alloc input and output arrays.  Each record in the input file contains a record ID and 5 other dimensions.
    /* 	x_position
        y_position
        mass
        x_velocity
        y_velocity
    */
    int records_dims = args->records * 6;
    *bodies_array = (double *)malloc(records_dims * sizeof(double));

    //printf("io: records_dims: %d \n", records_dims);  // debug statement

    // Read input vals
    double temp;
    for (int i = 0; i < records_dims; ++i) {
        in >> temp;

        (*bodies_array)[i] = temp;
    }
}

void write_file(options_t* args,
                vector<body>& bodies) {
     // Open file
	std::ofstream out;
	out.open(args->output_filename, std::ofstream::trunc);

	// Write solution to output file
    out.precision(6);
    out << setprecision(6) << fixed << scientific;
    out << (int)bodies.size() << endl;
	for (int i = 0; i < (int)bodies.size(); ++i) {
		out << bodies[i].index << "\t" << bodies[i].x_pos << "\t" << bodies[i].y_pos << "\t" << bodies[i].mass << "\t" << bodies[i].x_vel << "\t" << bodies[i].y_vel << std::endl;
		//printf("io: Writing from i(%d) the value: %d \n", i, opts->output_vals[i]); // debug statement
	}

	out.flush();
	out.close();
}