#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUF_SIZE 15000

int picoshell_main(int argc, char *argv[]) {
    // Write your code here


    char buf[BUF_SIZE];
    int status = 0;   /* status of last command */

    while (1) {
        printf("Pico shell prompt > ");
        fflush(stdout);

        /* Handle EOF properly*/ 
        if (fgets(buf, BUF_SIZE, stdin) == NULL) {
            exit(status);
        }
	
        /* replace enter with null */
        buf[strcspn(buf, "\n")] = '\0';

        /* Press Enter only */
        if (buf[0] == '\0') {
            continue;
        }

        /* exit command */
        if (strcmp(buf, "exit") == 0) {
            printf("Good Bye\n");
            exit(status);
        }

        /* ---------- Parse arguments ---------- */
        int capacity = 8;
        int argcc = 0;
        char **argvv = (char **)malloc(capacity * sizeof(char *));
        if (!argvv) {
            perror("malloc");
            status = 1;
            continue;
        }

        char *token = strtok(buf, " ");
        while (token) {
            if (argcc > capacity ) {
                capacity *= 2;
                char **tmp = (char **)realloc(argvv, capacity * sizeof(char *));
                if (!tmp) {
                    perror("realloc");
                    free(argvv);
                    status = 1;
                //    goto next_loop;
		continue;
                }
                argvv = tmp;
            }
            argvv[argcc++] = token;
            token = strtok(NULL, " ");
        }
        argvv[argcc] = NULL;

        /* ---------- Built-in commands ---------- */

        /* echo */
        if (strcmp(argvv[0], "echo") == 0)
       	{
            for (int i = 1; argvv[i]!=NULL; i++) {
                printf("%s", argvv[i]);
                if (argvv[i + 1]!=NULL)
                    printf(" ");
            }
            printf("\n");
            status = 0;
           // goto next_loop;
	    continue;
        }

        /* pwd */
        if (strcmp(argvv[0], "pwd") == 0)
       	{
            char cwd[BUF_SIZE];
            if (getcwd(cwd, sizeof(cwd)))
                printf("%s\n", cwd);
            else
                perror("pwd");
            status = 0;
	    continue;
       //     goto next_loop;
        }

        /* cd */
        if (strcmp(argvv[0], "cd") == 0)
       	{
            if (argcc!=2||(chdir(argvv[1]) != 0))
	    {
                perror("cd: /invalid_directory");
                status = 1;
            } else {
                status = 0;
            }
	    continue;
           // goto next_loop;
        }

        /* ---------- External commands ---------- */
        pid_t pid = fork();
        if (pid == 0) {
            execvp(argvv[0], argvv);
	    printf("%s: command not found\n", argvv[0]);
	    status=1;
            exit(1);
        } else if (pid > 0) {
            int wstatus;
            waitpid(pid, &wstatus, 0);
            if (WIFEXITED(wstatus))
                status = WEXITSTATUS(wstatus);
            else
                status = 1;
		
        } else {
            perror("fork error");
            status = 1;
        }

   // next_loop:
       free(argvv);
    }
    return status;

    // Do not write a main() function. Instead, deal with picoshell_main() as the main function of your program.
}
moh
