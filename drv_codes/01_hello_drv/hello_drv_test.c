#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char **argv)
{
    int fd;
    char buf[1024];
    int len;
    /* Check input arguments */
    if (argc < 2)
    {
        printf("Usage: %s -w <string>\n", argv[0]);
        printf("       %s -r\n", argv[0]);
        return -1;
    }

    /* Open hello file */
    fd = open("/dev/hello", O_RDWR);
    if (fd == -1)
    {
        printf("Can not open file /dev/hello\n");
        return -1;
    }

    /* Read or write file */
    if (!strcmp(argv[1], "-w") && argc == 3)
    {
        len = strlen(argv[2]) + 1;
        len = len < 1024 ? len : 1024;
        write(fd, argv[2], len);
    }
    else
    {
        len = read(fd, buf, 1024);
        buf[1023] = '\0';
        printf("App read: %s\n", buf);
    }
    close(fd);

    return 0;
}




