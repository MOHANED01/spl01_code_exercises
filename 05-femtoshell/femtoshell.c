#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define SIZE 30000
int femtoshell_main(int argc, char *argv[])
{
    int status = 0;
    char buf[SIZE];
  
    while(1){
        
	    printf("Femto shell prompt > ");
	    if(fgets(buf,SIZE,stdin)!=0)
         {

            if(strcmp(buf,"exit\n")==0)
	    {
                printf("Good Bye\n");
                exit(status);
                break;
            }
            else if(strncmp(buf,"echo ",5)==0)
	    {
                printf("%s",buf+5);
            }
            else if(strcmp(buf,"\n")==0)
	    {
	    continue;
            }
            else
	    {
                
                printf("Invalid command\n");
                status = 1 ;
            }
        }
        else
	{
            exit(status);
            break;
        }
    }
    return 0;

}
