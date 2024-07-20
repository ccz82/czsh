#include <stdio.h>
#include <unistd.h>

/* Changes the working directory of the shell to the path provided. */
int
czsh_cd(char **args)
{
    if (args[1] == NULL)
    {

        fprintf(stderr, "czsh: expected path to cd\n");
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("czsh: ");
        }
    }

    return 1;
}

/* Prints out help information to stdout. */
int
czsh_help(char **args)
{
    printf("czsh, a command-line interpreter.\n");

    return 1;
}

/* Exits the shell. */
int
czsh_exit(char **args)
{
    return 0;
}
