#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>

//Когда значение семафора 0 - работает родитель
//Когда 1 - работает ребенок
//Когда 2 - промежуточное значение

int main()
{
  int     fd[2], result;
  int n;
  size_t size;
  char  resstring[14];
  int    semid;        // IPC descriptor for an array of IPC semaphores
  char   pathname[]="09-1b.c"; // The file name used to generate the key.
                               // A file with this name must exist in the current directory
  key_t  key;          // IPC key
  struct sembuf mybuf; // Structure for specifying operations on a semaphore.

  
  

  if (pipe2(fd, O_NONBLOCK) < 0) {
    printf("Can\'t open pipe\n");
    exit(-1);
  }

  if ((key = ftok(pathname,0)) < 0) {
    printf("Can\'t generate key\n");
    exit(-1);
  }

  if ((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0) {
    printf("Can\'t create semaphore set\n");
    exit(-1);
  }
  
  printf("Input n:\n");
  scanf("%d", &n);

  result = fork();

  

  if (result < 0) {
    printf("Can\'t fork child\n");
    exit(-1);
  } else if (result > 0) {

   /* Parent process */

    while (n != 0) {

        
    size = write(fd[1], "Hello, child!", 14);

    if (size != 14) {
      printf("Can\'t write all string to pipe\n");
      exit(-1);
    }
    
    mybuf.sem_op = 2;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;
    if(semop(semid, &mybuf, 1) < 0){
      printf("Can\'t add 2 to semaphore\n");
      exit(-1);
    }

    mybuf.sem_op = 0;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;
    if(semop(semid, &mybuf, 1) < 0){
      printf("Can\'t decrease 1 to semaphore\n");
      exit(-1);
    }
    
    size = read(fd[0], resstring, 14);

    if (size < 0) {
      printf("Can\'t read string from pipe\n");
      exit(-1);
    }

    printf("Parent: %s\n", resstring);

    n--;
    }

  } else {

    /* Child process */
    while (1) {
    
    
    mybuf.sem_op = -1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;
    if(semop(semid, &mybuf, 1) < 0){
      printf("Can\'t decrease 1 to semaphore\n");
      exit(-1);
    }
    
    
    size = read(fd[0], resstring, 14);

    if (size < 0) {
      printf("Can\'t read string from pipe\n");
      exit(-1);
    }

    printf("Child: %s\n", resstring);
    
    size = write(fd[1], "Hello,parent!", 14);
    
    if (size != 15) {
      printf("Can\'t write all string to pipe\n");
      exit(-1);
    }

    mybuf.sem_op = -1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;
    if(semop(semid, &mybuf, 1) < 0){
      printf("Can\'t decrease 1 to semaphore\n");
      exit(-1);
    }
  }
  }

  return 0;
}

