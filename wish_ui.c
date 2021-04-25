#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "imports.h"

int run_cli_shell(FILE *in_stream, StringNode *pPaths) {

    if (in_stream == NULL) {
        in_stream = stdin;
    }

    int do_exit = 0;
    char *command_buffer;

    ssize_t read_size = 0;
    size_t buffer_size = 0;

    shell_print(in_stream, "wish> ");

    while ((read_size = getline(&command_buffer, 
                                &buffer_size, 
                                in_stream)) >= 0) {

        command_buffer[strcspn(command_buffer, "\n")] = 0;
        do_exit = parse_command_line(command_buffer, &pPaths);

        if (do_exit) {
            break;
        }

        shell_print(in_stream, "wish> ");
    }

    free(command_buffer);

    shell_print(in_stream, "Closing wish shell...\n");

    if (in_stream != stdin) {
        fclose(in_stream);
    }

    return do_exit;
}

int main(int argc, char *argv[]) {

    int do_exit = 0;
    StringNode *path_list = calloc(1, sizeof(StringNode));

    if (path_list == NULL) {
        error_print("malloc error", 1);
    }

    path_list->pNext = NULL;
    path_list->string = malloc(sizeof("/bin/"));

    if (path_list->string == NULL) {
        error_print("malloc error", 1);
    }

    strcpy(path_list->string, "/bin/");

    // If no batch mode start the loop
    if (argc == 1) {
        run_cli_shell(NULL, path_list);
    } else {
        FILE *in_stream;
        for (int i = 1; i < argc; i++) {
            in_stream = fopen(argv[i], "r");

            if (in_stream == NULL) {
                fprintf(stderr, "Error: failed to open file '%s'", argv[i]);
                exit(1);
            }

            do_exit = run_cli_shell(in_stream, path_list);

            if (do_exit) {
                fprintf(stdout, "Closing shell...\n");
                break;
            }
        }
    }
}