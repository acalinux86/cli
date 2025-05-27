#include "cli.h"

int main(const int argc, const char **argv)
{
    CLI cli = {0};
    // dump_argv_to_cli(&cli, argc, argv);
    add_command(&cli, "-i", "src", "Input File", VALUE_STRING);
    add_command(&cli, "-o", "dest", "Output File", VALUE_STRING);
    add_command(&cli, "-count", "10", "Count: default is 10", VALUE_INTEGER);

    if (argc <= 1 && argc > (int)cli.count + 1) {
        usage(&cli, argv[0]);
        return 1;
    }

    for (unsigned int i = 0; i < cli.count; ++i) {
        print_command(&cli.items[i]);
    }

    bool quit = false;
    while (!quit) {
        unsigned int i = 0;
        while (i < cli.count) {
            Command *command = &cli.items[i];
            if (command->param->get_param(command)) {
                if (strcmp(command->param->get_param(command), argv[i + i + 1]) != 0) {
                    usage(&cli, argv[0]);
                    Log_Out(ERROR, "Unrecognized parameter `%s`.\n", argv[i + i + 1]);
                    return 1;
                }
            } else {
                Log_Out(ERROR, "Invalid Param.\n");
                return 1;
            }
            ++i;
            if (i == cli.count) quit = true;
        }
    }
    return 0;
}
