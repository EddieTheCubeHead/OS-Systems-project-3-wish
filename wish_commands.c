#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "imports.h"

Builtin get_builtin(char* command_name) {
    // EDIT THIS IF BUILTINS CHANGE
    char *all_builtins[3] = {"cd", "path", "exit"};

    for (int i = cd_cmd; i <= exit_cmd; i++) {
        if (!strcmp(all_builtins[i], command_name)) {
            return i;
        }
    }

    return invalid;
}

// Simple module-specific function for building a command path
// from two strings into the given destination string
void built_from_path(char *dest, char *command, char *path) {
    strcat(dest, path);
    strcat(dest, command);
}

StringNode *realloc_paths(StringNode *args, StringNode *paths) {
    args = args->pNext;
    if (args == NULL) {
        fprintf(stdout, "Please specify at least 1 path.\n");
        return paths;
    }

    fprintf(stdout, "Assigning new paths...\n");
    free_list(paths);
    StringNode *pNew = NULL;
    StringNode *pFirst = NULL;

    while (args != NULL) {
        pFirst = append(pFirst, args->string);
        if (pNew == NULL) {
            pNew = pFirst;
        }
        args = args->pNext;
    }

    fprintf(stdout, "All specified paths assigned.\n");

    return pNew;
}

char **build_from_list(int argc, StringNode *args) {
    char **argv = calloc((argc + 1), sizeof(char*));
    if (argv == NULL) {
        error_print("malloc error", 0);
        _exit(1);
    }

    int i = 0;
    while (args != NULL) {
        argv[i] = malloc(MAX_STRING * sizeof(char));
        strcpy(argv[i++], args->string);
        args = args->pNext;
    }

    return argv;
}

int change_dir(StringNode *pArgs) {
    if (pArgs->pNext == NULL) {
        error_print("You must specify the directory to move to", 0);
        return -1;
    }

    if (chdir(pArgs->pNext->string) == -1) {
        fprintf(stderr, "Error: something went wrong while trying to move ");
        fprintf(stderr, "to '%s'\n", pArgs->pNext->string);
        return -1;
    }

    return 0;
}

void execute_command(Command *command_data) {

    char *full_command = calloc(1, MAX_STRING * 2 * sizeof(char));
    int out_file = STDOUT_FILENO;

    if (full_command == NULL) {
        error_print("malloc error", 0);
    } else { 
        if (command_data->out_stream != NULL) {
            out_file = open(command_data->out_stream, O_RDWR | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR );
            if (out_file == -1) {
                fprintf(stderr, "Error opening file '%s' for writing.\n",
                        command_data->out_stream);
            }

            dup2(out_file, STDOUT_FILENO);
            dup2(out_file, STDERR_FILENO);
            close(out_file);
        }

        if (out_file != -1) {
            StringNode *try_path = command_data->paths;
            while (try_path != NULL) {
                built_from_path(full_command, command_data->args->string, try_path->string);
                if (access(full_command, X_OK) == 0) {
                    execv(full_command, build_from_list(command_data->argc, command_data->args));
                }
                full_command[0] = '\0';
                try_path = try_path->pNext;
            }
            fprintf(stdout, "Couldn't find command '%s' in any path.\n", 
                    command_data->args->string);
        }
    }

    free(full_command);
    free_command(command_data);
    _exit(0);
}
