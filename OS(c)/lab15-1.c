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
int num1;

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


void send()
{
    if (num1 & (1 << bit_num))
        kill(pid_second, SIGUSR1);
    else
        kill(pid_second, SIGUSR2);
    --bit_num;
}

void check(int sig_num)
{
    if (bit_num < 0)
    {
        printf("Transaction has finished!\n");
        exit(0);
    }
    else
    {
        send();
    }
}

int main(void)
{
    pid_first = getpid();
    printf("Sender ID - %d\n", (int) pid_first);
    printf("Input reciever ID :\n");
    pid_second = get_int();

    printf("Enter the number for sending:\n");
    num1 = get_int();
    (void) signal(SIGUSR1, check);

    printf("The very beginnig of the sending..\n");

    send();
    while(1);

    return 0;
}

