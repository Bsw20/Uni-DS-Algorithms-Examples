#include <signal.h>
#include <stdio.h>

void handler_1(int ns) {
    printf("\n Receive signal %d, CTRL-C pressed\n", ns);
}
void handler_2(int ns) {
    printf("\n Receive signal %d, CTRL-4 pressed\n", ns);
}


int main(void) {
    (void)signal(SIGINT, handler_1);
    (void)signal(SIGQUIT, handler_2);
    
    while(1);
    return 0;
}
