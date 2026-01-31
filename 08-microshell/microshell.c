#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUF_SIZE 25000
    typedef struct {

        char *name;     /*variable name*/
        char *value;    /*variable value*/
        int exported;   /* flag to detect if its envir or local*/

} Variable;

Variable *vars=NULL; /*for dynamic allocation*/
int var_count=0;





void set_variable(const char *name,const char *value)
{
        /*to add a local variable to array*/

        for(int i =0;i<=var_count;i++)
        {
		/*search in dynamic array*/
                if(strcmp(vars[i].name,name)==0)
                {
                        free(vars[i].value);
                        vars[i].value=strdup(value);
                        return;
                }
        }
        /*if not found--> create a new variable */
        vars=realloc(vars,(var_count+1)*sizeof(Variable));
        vars[var_count].name=strdup(name);
        vars[var_count].value=strdup(value);
	vars[var_count].exported=0;
        var_count++;
}

char *get_variable(const char *name)
{
        /*search for a specific variable and return pointer to it (not dummy pointer)*/
       for(int i=0;i<var_count;i++)
       {
               if(strcmp(vars[i].name,name)==0)
               {
                       return vars[i].value;
               }
       }
       return NULL;
}

void export_variable(const char *name)
{   /*searching for a variable to be environmental variable*/
  for (int i = 0; i < var_count; i++)
  {
        if (strcmp(vars[i].name, name) == 0)
        {
            vars[i].exported = 1;/*set flag to be environ variable*/
            setenv(vars[i].name, vars[i].value, 1);/*3rd parameter =1--> overwrite*/
            return;
        }
    }
  /*if not found*/
    printf("Variable '%s' not found\n", name);
}


int is_valid_assignment(const char *cmd)
{   /*check if '=' is valid or not */

    if (strchr(cmd, '=') == NULL)
    {
            return 0;   /*if no '=' sign --> no need for this function*/
    }

    char temp[BUF_SIZE];
    strcpy(temp, cmd); /*copy of "cmd" to implement some operations on it */
    char *eq = strchr(temp, '=');/*pointer pointed to the location of '=' sign */

    if (eq == temp)
    {
            return 0; /* starts with '='*/
    }
    *eq = '\0';/*substitute '=' with '\0'
                ex: mohamed=5;
                *name=temp --> "mohamed\0",  char * value-->pointed to variable "5"*/
    char *name = temp;
    char *value = eq + 1;

    if (strlen(name) == 0 || strlen(value) == 0) /*=*/
    {
       return 0;
    }
    if (strchr(name, ' ') || strchr(value, ' '))/*x =5, x= 5*/
    {
        return 0;
    }
    return 1; /*if passed --> valid instruction */
}

void substitute_vars(char *args[], int count)
{
    for (int i = 0; i < count; i++) {
        char *dollar = strchr(args[i], '$');
        if (!dollar)
	{
	/*if not found--> to the next iteration*/
		continue;
	}

        char *var_name = dollar + 1;
        if (*var_name == '\0')
	{   /*if $ only--> means there is no replacement*/
		continue;
	}

        char *val = get_variable(var_name);
        if (!val)
	{
		val = getenv(var_name);
	}
        if (!val)
	{
		val = "";
	}

        /* build a new string */
        int prefix_len = dollar - args[i]; /*find the length after removing the text after '$'*/
        int new_len = prefix_len + strlen(val) + 1; /*adding new text +1 to add '\0'*/

        char *new_arg = malloc(new_len);

        strncpy(new_arg, args[i], prefix_len);
        new_arg[prefix_len] = '\0';
        strcat(new_arg, val);

        args[i] = new_arg;  /*replace argument*/
    }
}


int microshell_main(int argc, char *argv[]) {
    // Write your code here
char original_cwd[BUF_SIZE];
getcwd(original_cwd, sizeof(original_cwd));


    char buf[BUF_SIZE];
    int status = 0;   /* status of last command */

    while (1) {
        printf("Micro shell prompt > ");
        fflush(stdout);

        /* Handle EOF properly*/ 
        if (fgets(buf, BUF_SIZE, stdin) == NULL) {
           chdir(original_cwd);
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
	     chdir(original_cwd);
            exit(status);
        }

	      if (strncmp(buf, "export ", 7) == 0)
      {       /*hn compare awl 7 elements lw tmam a3ml add ll export var
           ex : export x ana kda bbasi buf[7] l hwa x*/
            export_variable(buf + 7);
            continue;
        }

           if (strchr(buf, '=') != NULL)
     {
         /*lw fih = sign ashof l awl hya k syntax maktopa sa7 wla la */
            if (!is_valid_assignment(buf))
            {
                printf("Invalid command\n");
                continue;
            }
            /*lw ktoba sa7 azbtha w adifha f l array b2a set_variable*/
            char *eq = strchr(buf, '=');
            *eq = '\0';
            char *name = buf;
            char *value = eq + 1;
            set_variable(name, value);
            continue;
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
            if (argcc >= capacity-1 ) {
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
      substitute_vars(argvv, argcc);


             /* pointer of char to pass it to open func*/
        char *inputfile= NULL;
        char *outputfile= NULL;
        char *errorfile= NULL;

        /*array of pointers bi4awr*/
        char *finalargs[10];
        int finalcount=0;
        for (int k = 0; k < argcc; k++)
   {

    /* compare each argument (after tokenization) with input redirection  "<"*/
    if (strcmp(argvv[k], "<") == 0)
      {
        /*if input file exists after "<"*/
        if (k + 1 < argcc)
        {

                inputfile = argvv[k+1];
                k++;
                continue;
        }
        else {
            printf("Error: missing input file after '<'\n");
	    status=1;
            break;
             }
    }

   /* compare each argument (after tokenization) with output redirection  ">"*/
    else if (strcmp(argvv[k], ">") == 0)
      {
        /* if output file exists after ">"*/
        if (k + 1 < argcc)
        {

                outputfile = argvv[k+1];
                k++;
                continue;
        }
        else {
            printf("Error: missing output file after '>'\n");
	        status=1;
            break;
             }
    }

/* compare each argument (after tokenization) with error redirection  "2>"*/

    else if (strcmp(argvv[k], "2>") == 0)
    {
        /* if error file exists after "2>"*/
        if (k + 1 < argcc)
        {

                errorfile = argvv[k+1];
                k++;
                continue;
        }
        else {
            printf("Error: missing error file after '2>'\n");
	        status=1;
            break;
             }
    }
    else
    {

            finalargs[finalcount++]=argvv[k];

    }
        }
    finalargs[finalcount]=NULL;

       /* ---------- External commands ---------- */
        pid_t pid = fork();
  if (pid == 0) {
    
    /* ---------- INPUT REDIRECTION ---------- */
    if (inputfile) {
        int fd = open(inputfile, O_RDONLY);
        if (fd < 0) {
            fprintf(stderr,
                    "cannot access %s: No such file or directory\n",
                    inputfile);
            exit(1);
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }

    /* ---------- OUTPUT REDIRECTION ---------- */
    if (outputfile) {
        int fd = open(outputfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            perror(outputfile);
            exit(1);
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    /* ---------- ERROR REDIRECTION ---------- */
    if (errorfile) {
        int fd = open(errorfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            perror(errorfile);
            exit(1);
        }
        dup2(fd, STDERR_FILENO);
        close(fd);
    }

    /* ---------- EXEC ---------- */
    execvp(finalargs[0], finalargs);

    fprintf(stderr, "%s: command not found\n", finalargs[0]);
    exit(1);
}
 else if (pid > 0) {
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
