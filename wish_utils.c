#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "imports.h"

// Print function that only prints if in shell mode
void shell_print(FILE *in_stream, char *text) {
    if (in_stream == stdin) {
        fprintf(stdout, "%s", text);
    }
}

// Extremely basic error handler for the sheel
void error_print(char* msg, int exit_thread) {
    if (msg == NULL) {
        fprintf(stderr, "An error has occurred\n"); 
    } else {
        fprintf(stderr, "ERROR: %s\n", msg);
    }
    
    if (exit_thread) {
        _exit(1);
    }
}

// Trim whitespace from both sides of input string
// Inspiration from here. I've explained it out to justify
// copy-pasting as the code was simple enought that meaningful
// changes couldn't be made.
// https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
char *trim(char *str) {
    char* end;

    // Convert the string into a single char and iterate
    // over the start while that char is space
    // This trims leading whitespace
    //
    // Note that isspace also catches other common whitespace characters
    while (isspace((unsigned char) *str)) {
        str++;
    }

    // Handling the edge-case of all-spaces string
    if (*str == 0) {
        return str;
    }

    // End pointer assigned to the last char of the string
    end = str + strlen(str) - 1;

    // Similiarly to trimming leading whitespace we trim
    // trailing whitespace with end-- iterations
    while (isspace((unsigned char) *end)) {
        end--;
    }
    
    // Need a new terminator char
    end[1] = '\0';

    return str;
}

// Can use any list member from first to last
StringNode *append(StringNode *pNode, char* string) {

    StringNode *pNew = calloc(1, sizeof(StringNode));

    if (pNew == NULL) {
        error_print("malloc error", 1);
    }

    pNew->pNext = NULL;
    pNew->string = malloc(sizeof(string));

    if (pNew->string == NULL) {
        error_print("malloc error", 1);
    }

    strcpy(pNew->string, string);

    // Make a new list if given null pointer
    if (pNode == NULL) {
        return pNew;
    }

    // Navigate to the end of the list
    while (pNode->pNext != NULL) {
        pNode = pNode->pNext;
    }

    pNode->pNext = pNew;
    return pNew;
}

void free_list(StringNode *pFirst) {
    StringNode *pNext;
    while (pFirst != NULL) {
        pNext = pFirst->pNext;
        free(pFirst->string);
        free(pFirst);
        pFirst = pNext;
    }
}

void free_command(Command *command_data) {
    if (command_data->out_stream != NULL) {
        free(command_data->out_stream);
    }
    free_list(command_data->args);
    free(command_data);
}