#define FASIC_IMPLEMENTATION
#include "thirdparty/fasic.h"

#define BUILD_DIR "build/"
#define SRC_DIR   "src/"

int main(void)
{
    if (!remove_directory  (BUILD_DIR)) return 1;
    if (!make_new_directory(BUILD_DIR)) return 1;

    Fasic_Cmd cmd = {0};

    const char *build_cli[] = {"gcc", "-Wall", "-Werror", "-Wextra", "-I./thirdparty/","-ggdb", "-D_DEFAULT_SOURCE", "-o", BUILD_DIR"cli", SRC_DIR"cli.c", SRC_DIR"cli_program.c",NULL};
    append_to_array_many(&cmd, build_cli);
    if (!build_cmd(&cmd)) return 1;

    return 0;
}
