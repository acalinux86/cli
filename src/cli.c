#include "cli.h"

const char *param_kind_as_cstr[] = {
    [PARAM_FULL]  = "PARAM_FULL",
    [PARAM_SHORT] = "PARAM_SHORT",
};

const char *get_param(Command *command)
{
    Param *return_param = command->param;
    return return_param->kind == PARAM_FULL ? return_param->full_name : return_param->short_name;
}

void set_param(Command *command, const char *parameter)
{
    unsigned int param_len = strlen(parameter);
    if (param_len > 2) {
        command->param->kind = PARAM_FULL;
        command->param->full_name = strdup_custom(parameter);
        command->param->short_name = NULL;
    } else {
        command->param->kind = PARAM_SHORT;
        command->param->short_name = strdup_custom(parameter);
        command->param->full_name = NULL;
    }
}

Param *new_param()
{
    Param *parameter = (Param *)calloc(1, sizeof(Param));
    if (!parameter) return NULL;

    parameter->get_param = get_param;
    parameter->set_param = set_param;
    parameter->full_name = NULL;
    parameter->short_name = NULL;
    return parameter;
}

const char *value_kind_as_cstr[] = {
    [VALUE_BOOLEAN] =  "VALUE_BOOLEAN",
    [VALUE_INTEGER] =  "VALUE_INTEGER",
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

    case VALUE_INTEGER:
        snprintf(buffer, sizeof(buffer), "%s", data->c_string);
        break;

    case VALUE_FLOAT:
        snprintf(buffer, sizeof(buffer),"%f", data->floating_point);
        break;

    case VALUE_STRING:
        snprintf(buffer, sizeof(buffer),"%d", data->integer);
        break;

    default:
        Log_Out(ERROR, "UnKnown Kind.\n");
        return NULL;
    }
    return strdup_custom(buffer);
}

Value *get_value(Command *command)
{
    return command->value;
}

void set_value(Command *command, const char *new_value, Value_Kind kind)
{
    char *end;
    Value *value = command->value;
    if (kind == VALUE_FLOAT) {
        float float_value = strtof(new_value, &end);
        if (end != new_value) {
            Log_Out(ERROR, "Conversion failed for `%s`: %s.\n", value_kind_as_cstr[kind], new_value);
            return;
        }
        value->data->floating_point = float_value;
    } else if (kind == VALUE_INTEGER) {
        int integer_value = atoi(new_value);
        value->data->integer = integer_value;
    } else if (kind == VALUE_STRING) {
        value->data->c_string = strdup_custom(new_value);
    } else if (kind == VALUE_BOOLEAN) {
        bool boolean = (bool)atoi(new_value);
        value->data->boolean = boolean;
    } else {
        Log_Out(ERROR, "UnKnown Kind.\n");
        return;
    }
}

Value *new_value()
{
    Value *value = (Value *)malloc(sizeof(Value));
    if (!value) return NULL;

    value->get_value = get_value;
    value->set_value = set_value;

    Value_Data *data = (Value_Data *)malloc(sizeof(Value_Data));
    if (!data) return NULL;

    value->data = data;

    return value;
}

void set_description(Command *command, const char *new_description)
{
    command->description->description = strdup_custom(new_description);
}

const char *get_description(Command *command)
{
    return command->description->description;
}

Description *new_description()
{
    Description *description = (Description *)malloc(sizeof(Description));
    if (!description) return NULL;

    description->get_description = get_description;
    description->set_description = set_description;
    description->description = NULL;

    return description;
}

// TODO: Abstract Away the command variable inside the add_command
// and take in cli instead
void add_command(CLI *cli, const char *param, const char *value, const char *description, Value_Kind kind)
{
    Command command = {0};
    command.param = new_param();
    command.value = new_value();
    command.description = new_description();
    command.param->set_param(&command, param);
    command.value->set_value(&command, value, kind);
    command.description->set_description(&command, description);

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

void usage(CLI *cli, const char *program)
{
    // TODO: Store Both Short and Long Name
    fprintf(stderr, "Usage: %s OPTIONS...\n", program);
    fprintf(stderr, "Args: \n");
    for (unsigned int i = 0; i < cli->count; ++i) {
        Command *command = &cli->items[i];
        fprintf(stderr, "    %s, %s.\n",
        command->param->kind == PARAM_FULL ? command->param->full_name : command->param->short_name,
        command->description->get_description(command)
        );
    }
}

// TODO: Process CLI
// TODO: Default args
// TODO: Also check if default param match when comparing cli and argv
// TODO: cli to Fasic_cmd -> executed
