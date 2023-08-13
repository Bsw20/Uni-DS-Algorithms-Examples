#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#include <stdlib.h>


pid_t pid_first;
pid_t pid_second;
int bit_num = 31;
int result = 0;

int get_int()
{
    char s[50];
    int num;

    fgets(s, sizeof(s), stdin);
    while (sscanf(s, "%d", &num) != 1)
    {
        printf("Not int or out of int bound\n");
        fgets(s, sizeof(s), stdin);
    }
    return num;
}

void check()
{
    kill(pid_second, SIGUSR1);
    if (bit_num < 0)
    {
        printf("Received number %d\n", result);
        exit(0);
    }
}

void handler1(int signo)
{
    result += 1 << bit_num;
    printf("Recieved bit equals 1\n");
    bit_num--;
    check();
}

void handler0(int signo)
{
    if (pid_second < 0)
        return;
    printf("Received bit equals 0\n");
    bit_num--;
    check();
}

int main(void)
{

    (void) signal(SIGUSR1, handler1);
    (void) signal(SIGUSR2, handler0);

    pid_first = getpid();
    printf("Receiver ID - %d\n", (int) pid_first);
    printf("Input ID of the sender:\n");
    pid_second = get_int();
    printf("Reading is carried out from the beginning of the number\n");
    printf("Start receiving a message..\n");
    while (1);

    return 0;
}
