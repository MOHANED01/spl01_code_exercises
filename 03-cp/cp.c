// Add the needed includes
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define COUNT 100

int cp_main(int argc, char *argv[]) {
    // Write your code here
       char buf[COUNT];     
       int num_read;

       int fd1 = open (argv[1],O_RDONLY);
        if(fd1<0)
        {
           return -1;
        }


        int fd2 =open(argv[2], O_RDWR|O_CREAT|O_TRUNC ,0644);

        if(fd2<0)
        {
               return -1;
        }


               while((num_read=read(fd1 , buf , COUNT))>0)
          {
            if(write(fd2 , buf , num_read)<0)
            {
         return -2;
	    }
	  }

    return 0;
}
