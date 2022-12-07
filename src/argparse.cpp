#include <argparse.h>

// https://www.gnu.org/software/libc/manual/html_node/Getopt.html
#include <getopt.h>
#include <iostream>
#include <map>

// namespaces
using namespace std;

// Flag set by --long_options
//static int visualization_flag;

void printOptionInstructions() {
    std::cout << "Usage:" << std::endl;
    std::cout << "\t[Required] --inputfilename or -i <input file name (char *)>" << std::endl;
    std::cout << "\t[Required]--outputfilename or -o <output file name (char *)>" << std::endl;
    std::cout << "\t[Required]--steps or -s <number of iterations (int)>" << std::endl;
    std::cout << "\t[Required]--theta or -t <threshold for MAC (double)>" << std::endl;
    std::cout << "\t[Required]--dt or -d <timestep (double)>" << std::endl;
    std::cout << "\t[Optional] -v <flag to turn on visualization window>" << std::endl;
    exit(0);
}

void get_opts(int               argc,
              char              **argv,
              struct options_t  *opts)
{
    if (argc == 1)
    {
        printOptionInstructions();
    }

    // Set flag values.
    opts->visualization = false;

    // Set required options.
    map<string, bool> required_options_map{
        { "i", false },
        { "o", false },
        { "s", false },
        { "t", false },
        { "d", false }
    };

    struct option l_opts[] = {
        {"inputfilename", required_argument, NULL, 'i'},
        {"outputfilename", required_argument, NULL, 'o'},
        {"steps", required_argument, NULL, 's'},
        {"theta", required_argument, NULL, 't'},
        {"dt", required_argument, NULL, 'd'},
        {"v", no_argument, NULL, 'v'}
    };

    int ind, c;
    while ((c = getopt_long(argc, argv, "i:o:s:t:d:v", l_opts, &ind)) != -1)
    {
        //printf("argparse: s value: %d \n", s);  // debug statement
        switch (c)
        {
        case 0:
            break;
        case 'i':
            opts->input_filename = (char *)optarg;
            required_options_map["i"] = true;
            break;
        case 'o':
            opts->output_filename = (char *)optarg;
            required_options_map["o"] = true;
            break;
        case 's':
            opts->steps = atoi((char *)optarg);
            required_options_map["s"] = true;
            break;
        case 't':
            opts->theta = atof((char *)optarg);
            required_options_map["t"] = true;
            //printf("argparse: theta: %f \n", opts->theta);  // debug statement
            break;
        case 'd':
            opts->dt = atof((char *)optarg);
            required_options_map["d"] = true;
            //printf("argparse: dt: %f \n", opts->dt);  // debug statement
            break;
        case 'v':
            opts->visualization = true;
            break;
        case ':':
            std::cerr << argv[0] << ": option -" << (char)optopt << " requires an argument." << std::endl;
            exit(0);
        case '?':
            std::cerr << argv[0] << ": option -" << (char)optopt << " is not defined in the program." << std::endl;
            exit(0);
        }
    }

    for (const auto& [key, value] : required_options_map) {
        if (!value) {
            std::cerr << argv[0] << ": option -" << key << " is a required option." << std::endl;
            printOptionInstructions();
            exit(0);
        }
    }
}
