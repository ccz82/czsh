#define _GNU_SOURCE

#define CZSH_TOK_BUFSIZE 64
#define CZSH_TOK_DELIM " \t\r\n\a"

#include "builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* Builtin commands, followed by their corresponding functions. */
char *builtin_str[] = {
    "cd",
    "help",
    "exit",
};

int (*builtin_func[])(char **) = {
    &czsh_cd,
    &czsh_help,
    &czsh_exit,
};

/*
 * Returns the length of the builtin_str array, which is
 * the total number of builtin shell commands.
 */
size_t
czsh_num_builtins()
{
    return sizeof(builtin_str) / sizeof(char *);
}

/* Reads a line from stdin. */
char *
czsh_read_line()
{
    char *line = NULL;
    size_t length = 0;

    /*
     * getline() returns -1 on failure to read a line, which includes reaching
     * the end-of-file or any other error.
     */
    if (getline(&line, &length, stdin) == -1)
    {
        if (feof(stdin))
        {
            exit(EXIT_SUCCESS);
        }
        else
        {
            perror("czsh: error calling getline()");
            exit(EXIT_FAILURE);
        }
    }

    return line;
}

/* Parse a line by splitting it into multiple string(s). */
char **
czsh_split_line(char *line)
{
    int bufsize = CZSH_TOK_BUFSIZE;
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token;

    if (!tokens)
    {
        fprintf(stderr, "czsh: allocation error!\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, CZSH_TOK_DELIM);
    while (token != NULL)
    {
        tokens[position++] = token;

        if (position >= bufsize)
        {
            bufsize += CZSH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize);

            if (!tokens)
            {
                fprintf(stderr, "czsh: allocation error!\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, CZSH_TOK_DELIM);
    }

    tokens[position] = NULL;
    return tokens;
}

/* Launch a new process, passing in the arguments required. */
int
czsh_launch(char **args)
{
    pid_t child_pid, wait_pid;
    int status;

    child_pid = fork();

    if (child_pid < 0)
    {
        perror("czsh: error calling fork()");
        exit(EXIT_FAILURE);
    }
    else if (child_pid == 0)
    {
        /* The exec() functions return only if an error has occurred. */
        if (execvp(args[0], args) == -1)
        {
            perror("czsh");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        do
        {
            wait_pid = waitpid(child_pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int
czsh_execute(char **args)
{
    /* Check for empty input. */
    if (args[0] == NULL)
    {
        return 1;
    }

    for (int i = 0; i < czsh_num_builtins(); i++)
    {
        if (strcmp(args[0], builtin_str[i]) == 0)
        {
            return (*builtin_func[i])(args);
        }
    }

    return czsh_launch(args);
}

void
czsh_loop()
{
    char *line;
    char **args;
    int status;

    do
    {
        printf("> ");
        line = czsh_read_line();
        args = czsh_split_line(line);
        status = czsh_execute(args);

        free(line);
        free(args);
    } while (status);
}

int
main(int argc, char *argv[])
{
    /* Main program loop. */
    czsh_loop();

    return EXIT_SUCCESS;
}
