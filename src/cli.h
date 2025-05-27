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
    const char *(*get_param)(Command *);
    void (*set_param)(Command *, const char *);
    Param_Kind kind;
} Param;

const char *get_param(Command *command);
void set_param(Command *command, const char *parameter);
Param *new_param();

typedef enum {
    VALUE_BOOLEAN,
    VALUE_INTEGER,
    VALUE_FLOAT,
    VALUE_STRING,
} Value_Kind;

typedef union {
    bool boolean;
    int  integer;
    float floating_point;
    const char *c_string;
} Value_Data;

typedef struct Value {
    struct Value *(*get_value)(Command *);
    void (*set_value)(Command *, const char *, Value_Kind);
    Value_Data *data;
    Value_Kind kind;
} Value;

char *get_value_as_cstr(Value *value);
Value *get_value(Command *command);
void set_value(Command *command, const char *value, Value_Kind kind);
Value *new_value();

typedef struct Description {
    const char *description;
    void (*set_description)(Command *, const char *);
    const char *(*get_description)(Command *);
} Description;

void set_description(Command *command, const char *new_description);
const char *get_description(Command *command);
Description *new_description();

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
void add_command(CLI *cli, const char *param, const char *value, const char *description, Value_Kind kind);
void dump_argv_to_cli(CLI *cli, const int argc, const char **argv);

// NOTE: Debug Purposes
void print_command(Command *command);
void usage(CLI *cli, const char *program);

#endif // CLI_H
