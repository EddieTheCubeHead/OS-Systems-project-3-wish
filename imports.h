///////////////////////////////////////////////////////////////////////////////
//                             Project wide defines
///////////////////////////////////////////////////////////////////////////////

// Faciliate easy editing of the piping symbol
#define PIPE_OUT ">"

// Restrict the max string size in string linked list
#define MAX_STRING 1000

#ifndef IMPORTS_H
#define IMPORTS_H

///////////////////////////////////////////////////////////////////////////////
//                        Project wide data structures
///////////////////////////////////////////////////////////////////////////////


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

// A function for printing in the shell
//
// Takes the current input stream and the desired text to print. Prints the 
// text to stdout if input stream is stdin.
void shell_print(FILE *in_stream, char *text);

// A function for printing errors
//
// Takes an error message and (boolean) integer value 0 or 1.
// Prints the error message to stderr and exits current thread
// if given a "TRUE" -value
void error_print(char* msg, int exit_thread);

// A function for trimming strings
//
// Takes a string and removes whitespace from left and right, including spaces,
// tabs, newlines and \r
char *trim(char *str);

// A function for appending to a linked list of strings
//
// Takes a member of the list, and returns a new node, created at the end
// of the list.
StringNode *append(StringNode *pNode, char* string);

// A function for freeing a string list
void free_list(StringNode *pFirst);

// A function for freeing a command data struct
void free_command(Command *command_data);

///////////////////////////////////////////////////////////////////////////////
//                      core functions (wish_core.c)
///////////////////////////////////////////////////////////////////////////////

// A function for parsing a line of command data
//
// Takes a line from the shell (or batch file) and parses it into possible
// parallel commands (separated by '&'). Runs parse_single_commands on
// separate threads on all found commands and executes the result. Returns
// 0 normally, 1 if the program should quit
int parse_command_line(char *command_string, StringNode **pPaths);

// A function for parsing a single command into command and args
//
// Takes a string containing a single command and a freshly malloced Command
// struct. Populates the struct with the command data, returns 0 during normal
// operation, -1 to indicate an error.
int parse_single_command(char *command_string, Command *command_data);

///////////////////////////////////////////////////////////////////////////////
//                   command functions (wish_commands.c)
///////////////////////////////////////////////////////////////////////////////

// A function for executing a command from a command data struct
void execute_command(Command *command_data);

// A function for getting a command's builtin type from it's name
//
// Takes the name of a command and retursn a Builtin enum telling whether the
// command is builtin or not. Note that non-builtin commands are of type
// "invalid" (-1).
Builtin get_builtin(char* command_name);

// A function for changing directory
//
// Takes a linked list of command arguments (including name). Returns -1 on
// failure and 0 on success
int change_dir(StringNode *pArgs);

// A function for assigning new paths to the shell.
//
// Takes a pointer to the command argument linked list and the current path 
// linked list. Returns a pointer to the new path linked list.
StringNode *realloc_paths(StringNode *args, StringNode *paths);

#endif