#pragma once

#include <memory>
#include <vector>

#include "argparse.h"
#include "body.h"

using namespace std;

void read_file(struct options_t* args,
               vector<body>& bodies);

void read_file2(struct options_t* args,
               double** bodies_array);

void write_file(struct options_t* args,
                vector<body>& bodies);