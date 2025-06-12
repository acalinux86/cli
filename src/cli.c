#include "cli.h"
#include <ctype.h>

const char *param_kind_as_cstr[] = {
    [PARAM_FULL]  = "PARAM_FULL",
    [PARAM_SHORT] = "PARAM_SHORT",
};

Param *new_param(const char *param)
{
    Param *parameter = (Param *)calloc(1, sizeof(Param));
    if (!parameter) return NULL;

    parameter->full_name = NULL;
    parameter->short_name = NULL;

    if (strlen(param) > 2) {
        parameter->full_name = strdup_custom(param);
        parameter->kind = PARAM_FULL;
    } else {
        parameter->short_name = strdup_custom(param);
        parameter->kind = PARAM_SHORT;
    }
    return parameter;
}

const char *value_kind_as_cstr[] = {
    [VALUE_BOOLEAN] =  "VALUE_BOOLEAN",
    [VALUE_FLOAT]   =  "VALUE_FLOAT",
    [VALUE_STRING]  =  "VALUE_STRING",
};

// FIX: Bug in this function, Not Priniting Correctly in Print_command
char *get_value_as_cstr(Value *value)
{
    Value_Data *data = value->data;
    char buffer[MAX_BUFFER_LEN];
    switch (value->kind) {
    case VALUE_BOOLEAN:
        snprintf(buffer, sizeof(buffer), "%s", data->boolean == true ? "true" : "false");
        break;

    case VALUE_FLOAT:
        snprintf(buffer, sizeof(buffer),"%f", data->floating_point);
        break;

    case VALUE_STRING:
        snprintf(buffer, sizeof(buffer),"%s", data->c_string);
        break;

    default:
        Log_Out(ERROR, "UnKnown Kind.\n");
        return NULL;
    }
    return strdup_custom(buffer);
}


Value *new_value(const char *default_value)
{
    Value *value = (Value *)calloc(1, sizeof(Value));
    if (!value) return NULL;

    value->data = (Value_Data *)calloc(1, sizeof(Value_Data));
    if (!value->data) return NULL;

    if (isdigit(*default_value)) {
        value->kind = VALUE_FLOAT;
        value->data->floating_point = atof(default_value);
    } else if (*default_value == 0 || *default_value == 1) {
        value->kind = VALUE_BOOLEAN;
        value->data->boolean = atoi(default_value);
    } else {
        value->kind = VALUE_STRING;
        value->data->c_string = default_value;
    }

    return value;
}

Description *new_description(const char *description_info)
{
    Description *description = (Description *)calloc(1, sizeof(Description));
    if (!description) return NULL;

    description->description = strdup_custom(description_info);

    return description;
}

// TODO: Abstract Away the command variable inside the add_command
// and take in cli instead
void add_command(CLI *cli, const char *param, const char *default_value, const char *description)
{
    Command command = {0};
    command.param = new_param(param);
    command.value = new_value(default_value);
    command.description = new_description(description);
    append_to_array(cli, command);
}

// NOTE: Debug Purposes
// TODO: fix bug in get_value_as_cstr
void print_command(Command *command)
{
    Log_Out(DEBUG, "%s(%s), %s, %s\n",
    command->param->kind == PARAM_FULL ? command->param->full_name : command->param->short_name,
    param_kind_as_cstr[command->param->kind],
    get_value_as_cstr(command->value->get_value(command)),
    command->description->get_description(command)
    );
}

// void usage(CLI *cli)
// {
//     // TODO: Store Both Short and Long Name
//     fprintf(stderr, "Usage: %s OPTIONS...\n", cli->name);
//     fprintf(stderr, "Commands: \n");
//     for (unsigned int i = 0; i < cli->count; ++i) {
//         Command *command = &cli->items[i];
//         fprintf(stderr, "    %s, %s.\n",
//         command->param->kind == PARAM_FULL ? command->param->full_name : command->param->short_name,
//         command->description->get_description(command)
//         );
//     }
// }

// TODO: Process CLI
// TODO: Default args
// TODO: Also check if default param match when comparing cli and argv
// TODO: cli to Fasic_cmd -> executed
