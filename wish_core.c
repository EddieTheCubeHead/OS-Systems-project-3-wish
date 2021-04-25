#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

#include "imports.h"

int parse_command_line(char* command_string, StringNode** pPaths) {

    const char divider[2] = "&";
    char *save_ptr;

    // strtok_r instead of strtok to maintain thread safety
    char *command_token = strtok_r(command_string, divider, &save_ptr);
    int parse_result;

    while (command_token != NULL) {

        parse_result = 0;
        Command *command_data = malloc(sizeof(Command));
        StringNode *new_paths = malloc(sizeof(StringNode));
        parse_result = parse_single_command(trim(command_token), command_data);

        command_token = strtok_r(NULL, divider, &save_ptr);

        if (parse_result == -1) {
            continue;
        }

        // Checking for builtins beforehand
        Builtin builtin_command = get_builtin(command_data->args->string);

        switch (builtin_command) {
            case invalid:
                command_data->paths = *pPaths;
                if (fork() == 0) {
                    execute_command(command_data);
                }
                break;

            case cd_cmd:
                // Implement cd
                break;

            case path_cmd:
                new_paths = realloc_paths(command_data->args, *pPaths);
                *pPaths = new_paths;
                fprintf(stdout, "Now using path");
                StringNode *pPrint = *pPaths;
                if (pPrint->pNext != NULL) {
                    fprintf(stdout, "s: ");
                    while (pPrint->pNext != NULL) {
                        fprintf(stdout, "'%s' ", pPrint->string);
                        pPrint = pPrint->pNext;
                    }
                    fprintf(stdout, "and '%s'\n", pPrint->string);
                } else {
                    fprintf(stdout, ": '%s'\n", pPrint->string);
                }

                break;

            case exit_cmd:
                return 1;

        }
    }

    // Inspiration from here:
    // https://stackoverflow.com/questions/19461744/how-to-make-parent-wait-for-all-child-processes-to-finish
    int wait_status = 0;
    pid_t wait_pid;
    while((wait_pid = wait(&wait_status)) > 0);

    return 0;
}

// Parser for a single command, assigns args to the given destination and
// returns the user desired output file (stdout by default)
int parse_single_command(char *command_string, Command *command_data) {

    // Preparing the variables for piping parsing
    int parse_pipe = 0;
    char *out_stream_name;

    int argument_count = 0;

    const char divider[2] = " ";
    char *save_ptr;

    char *arg_token = strtok_r(command_string, divider, &save_ptr);

    StringNode *pFirst = NULL;

    while (arg_token != NULL) {

        arg_token = trim(arg_token);

        if (strlen(arg_token) == 0) {
            arg_token = strtok_r(NULL, divider, &save_ptr);
            continue;
        }

        if (!strcmp(PIPE_OUT, arg_token)) {
            if (parse_pipe) {
                error_print("multiple pipe symbols detected", FALSE);
                return -1;
            }

            parse_pipe++;
        } else if (parse_pipe == 1) {
            parse_pipe++;
            out_stream_name = malloc(sizeof(arg_token));
            strcpy(out_stream_name, arg_token);
        } else if (parse_pipe > 1) {
            error_print("output redirection should be the last argument", 
                        FALSE);
            free(out_stream_name);
            return -1;
        } else {
            pFirst = append(pFirst, arg_token);
            if (command_data->args == NULL) {
                command_data->args = pFirst;
            }
            argument_count++;
        }

        arg_token = strtok_r(NULL, divider, &save_ptr);
    }

    if (parse_pipe) {
        command_data->out_stream = malloc(sizeof(out_stream_name));
        strcpy(command_data->out_stream, out_stream_name);
    }

    command_data->argc = argument_count;
    return 0;
}