// Project wide defines:

// Doing dynamic data structures with realloc
// this specifies the interval of reallocing an array
#define REALLOC_INTERVAL 5

// Faciliate easy editing of the piping symbol
#define PIPE_OUT ">"

// Mandatory fake booleans (personal preference)
#define TRUE 1
#define FALSE 0

// Restrict the max string size in string linked list
#define MAX_STRING 1000

#ifndef IMPORTS_H
#define IMPORTS_H

// String list struct (for argument and path listing)
struct StringNode {
    char *string;
    struct StringNode *pNext;
} typedef StringNode;

// Command data struct
struct Command {
    int argc;
    char *out_stream;
    StringNode *args;
    StringNode *paths;
} typedef Command;

// Enum for handling builtins
// Note the cmd to differentiate from system functions
enum Builtin {
    invalid = -1,
    cd_cmd = 0,
    path_cmd = 1,
    exit_cmd = 2
} typedef Builtin;

///////////////////////////////////////////////////////////////////////////////
//                     utility functions (wish_utils.c)
///////////////////////////////////////////////////////////////////////////////

// Takes the current input stream and the desired text to print.
// Prints the text to stdout if input stream is stdin
void shell_print(FILE *in_stream, char *text);

// Takes an error message and (boolean) integer value 0 or 1.
// Prints the error message to stderr and exits current thread
// if given a "TRUE" -value
void error_print(char* msg, int exit_thread);

// A simple utility function that takes a string and returns a trimmed version
// (removes whitespace from left and right)
char *trim(char *str);

// Appending to string list
StringNode *append(StringNode *pNode, char* string);

// Freeing a string list
void free_list(StringNode *pFirst);

// Freeing a command data struct
void free_command(Command *command_data);

///////////////////////////////////////////////////////////////////////////////
//                      core functions (wish_core.c)
///////////////////////////////////////////////////////////////////////////////

// Takes a line from the shell (or batch file) and parses it into possible
// parallel commands (separated by '&'). Runs parse_single_commands on
// separate threads on all found commands and executes the result
int parse_command_line(char *command_string, StringNode **pPaths);

// Takes a string containing a single command and a freshly malloced Command
// struct. Populates the struct with the command data but instead of populating
// the field, returns the out_stream, as NULL can be used to indicate an error
int parse_single_command(char *command_string, Command *command_data);

///////////////////////////////////////////////////////////////////////////////
//                   command functions (wish_commands.c)
///////////////////////////////////////////////////////////////////////////////

// Takes a struct containing all command data (arg count, arg array and output)
// and executes the command detailed by the struct.
void execute_command(Command *command_data);

// Takes the name of a command and retursn a Builtin enum telling whether the
// command is builtin or not.
Builtin get_builtin(char* command_name);

// A function that takes the new and current path count and paths and reallocs
// the new paths into the new paths
StringNode *realloc_paths(StringNode *args, StringNode *paths);

#endif