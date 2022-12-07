#pragma once

struct options_t {
    char* input_filename;
    char* output_filename;
    int steps;
    double theta;
    double dt;
    bool visualization;
    int records;
};

void get_opts(int argc, char **argv, struct options_t *opts);