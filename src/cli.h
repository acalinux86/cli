#ifndef CLI_H
#define CLI_H

#define FASIC_IMPLEMENTATION
#include "fasic.h"

#define MAX_BUFFER_LEN 1024

typedef enum {
    PARAM_FULL,
    PARAM_SHORT,
} Param_Kind;

typedef struct Command Command;

typedef struct Param {
    const char *full_name;
    const char *short_name;
    Param_Kind kind;
} Param;

Param *new_param(const char *param);

typedef enum {
    VALUE_BOOLEAN,
    VALUE_FLOAT,
    VALUE_STRING,
} Value_Kind;

typedef union {
    bool boolean;
    float floating_point;
    const char *c_string;
} Value_Data;

typedef struct Value {
    Value_Data *data;
    Value_Kind kind;
} Value;

char *get_value_as_cstr(Value *value);
Value *new_value(const char *default_value);

typedef struct Description {
    const char *description;
} Description;

Description *new_description(const char *description_info);

// TODO: Abstract Away the command variable inside the add_command
// and take in cli instead

struct Command {
    Param *param;
    Value *value;
    Description *description;
};

typedef struct CLI {
    Command *items;
    unsigned int count;
    unsigned int capacity;
} CLI;
void add_command(CLI *cli, const char *param, const char *value, const char *description);
void dump_argv_to_cli(CLI *cli, const int argc, const char **argv);

// NOTE: Debug Purposes
void print_command(Command *command);
void usage(CLI *cli, const char *program);

#endif // CLI_H
