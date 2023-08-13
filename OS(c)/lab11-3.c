#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LAST_MESSAGE 255

int main(void)
{
  long mypid = (long)getpid();
  int msqid;
  char pathname[]="09-3.c";
  key_t  key;
  int i, len, maxlen;

  struct informativestruct
  {
	long pid;
	float number;
  };

  struct mymsgbuf // Custom structure for the message
  {
    long mtype;
    struct informativestruct information;
  } mybuf;


  if ((key = ftok(pathname,0)) < 0) {
    printf("Can\'t generate key\n");
    exit(-1);
  }

  if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
    printf("Can\'t get msqid\n");
    exit(-1);
  }

  while (1) {
    //
    // In an infinite loop, accept messages of type 1 in FIFO order
    //
    maxlen = sizeof(struct informativestruct);

    if (( len = msgrcv(msqid, (struct msgbuf *) &mybuf, maxlen, 1, 0)) < 0) {
      printf("Can\'t receive message from queue\n");
      printf("message type = %ld, pid = %ld, number = %f\n", mybuf.mtype, mybuf.information.pid, mybuf.information.number);
      printf("len = %d, maxlen = %d\n", len, maxlen);
      exit(-1);
    }

    printf("message type = %ld, pid = %ld, number = %f\n", mybuf.mtype, mybuf.information.pid, mybuf.information.number);

    //
    // then terminate and remove the message queue from the system.
    // Otherwise, print the text of the received message.
    // 
    //
    // Fill in the structure for the message and
    // determine the length of the informative part.
    //
    mybuf.mtype = mybuf.information.pid;
	mybuf.information.number = mybuf.information.number * mybuf.information.number;
    mybuf.information.pid = mypid;
    len = sizeof(mybuf.information);
    //
    // Send the message. If there is an error,
    // report it and delete the message queue from the system.
    //
    

    if (msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0) {
      printf("Can\'t send message to queue\n");
      msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
      exit(-1);
    }



    
  }

  return 0;
}
