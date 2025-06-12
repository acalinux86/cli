#include "cli.h"

int main(const int argc, const char **argv)
{
    CLI cli = {0};
    add_command(&cli, "-i", "src", "Input File");
    add_command(&cli, "-o", "dest", "Output File");
    add_command(&cli, "-count", "10", "Count: default is 10");

    if (argc < 3) {
        Log_Out(ERROR, "Few Args Provided");
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        Log_Out(INFO, "%s\n", argv[i]);
    }

    for (unsigned int i = 0; i < cli.count; ++i) {
        print_command(&cli.items[i]);
    }
    return 0;
}
