// Add the needed includes
#include <unistd.h>
#include <limits.h>
#include <string.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif


int pwd_main(int argc,char * argv[])
{
  	// Write your code here

char buffer[PATH_MAX];

    if (getcwd(buffer, sizeof(buffer)) == NULL)
    {
        return -1;
    }

    size_t len = strlen(buffer);

    /* Write path */
    if (write(1, buffer, len) != (ssize_t)len) 
    {
        return -1;
    }

    /* Write newline */
    if (write(1, "\n", 1) != 1)
    {
        return -1;
    }

   

    return 0;
}

