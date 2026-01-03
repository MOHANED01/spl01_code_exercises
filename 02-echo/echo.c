// Add the needed includes
#include <string.h>
#include <unistd.h>


int echo_main(int argc, char *argv[]) {
    // Write your code here
    for(int i=1;i<argc;i++)
            {

             if(write(1 ,argv[i] ,strlen (argv[i]))<0)
             {
               return -1;
              }
            
	     if(i<(argc-1))
	     {
		      if(write(1 ," " ,1)<0)
                         return -1;
	     }
	    }

           /*insert enter at the end of line*/

           if(write(1 ,"\n" ,1)<0)
             {
               return -1;

             }

    
    return 0;
}
