// Add the needed includes
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define COUNT 100

int mv_main(int argc, char *argv[]) {
    // Write your code here
    int buf[COUNT];
    int fd1 = open (argv[1],O_RDONLY);
        if(fd1<0)
        {
                return -1;
        }

        int fd2 =open(argv[2], O_RDWR|O_CREAT|O_TRUNC ,0644);

        /*
	 O_RDWR: for read and write ,
	 O_TRUNC: if overwrite to old file
         0644: for permission --> (0) octal, (6) read & write for owner, (4) read only for group, (4) read only for others rw-r--r--"default permission for regular files"
	 */

        if(fd2<0)
        {
                return -2;
        }

        int num_read;


          while((num_read=read(fd1 , buf , COUNT))>0)
          {
            if(write(fd2 , buf , num_read)!=(ssize_t)num_read)
            {
                 return -3;
             }

           }

	  /*delete source from file system*/
         if(unlink(argv[1])!=0)
	 {
		 return -4;
         }



    return 0;
}
