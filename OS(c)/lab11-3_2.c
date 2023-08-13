#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LAST_MESSAGE 255 // Message type for termination of program 11-1b.c

int main(void)
{
  float value;
  long mypid;
  int msqid;      // IPC descriptor for the message queue
  char pathname[]="09-3.c"; // The file name used to generate the key.
                             // A file with this name must exist in the current directory.
  key_t  key;     // IPC key
  int i,len, maxlen;      // Cycle counter and the length of the informative part of the message

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
  //
  // Trying to get access by key to the message queue, if it exists,
  // or create it, with read & write access for all users.
  //
  if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
    printf("Can\'t get msqid\n");
    exit(-1);
  }

  /* Send information */
  printf("Input float value:\n");
  scanf("%f", &value);
  mypid = (long)getpid();
  mybuf.mtype = 1;
  mybuf.information.number = value;
  mybuf.information.pid = mypid;
  len = sizeof(mybuf.information);
  if (msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0) {
      printf("Can\'t send message to queue\n");
      msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
      exit(-1);
  }

  while (1) {
    //
    // In an infinite loop, accept messages of any type in FIFO order
    //
    maxlen = sizeof(struct informativestruct);

    if (( len = msgrcv(msqid, (struct msgbuf *) &mybuf, maxlen, mypid, 0)) < 0) {
      printf("Can\'t receive message from queue\n");
      exit(-1);
    }
    //
    // If the received message is of type LAST_MESSAGE,
    // then terminate and remove the message queue from the system.
    // Otherwise, print the text of the received message.
    //
    printf("message type = %ld, pid = %ld, number = %f\n", mybuf.mtype, mybuf.information.pid, mybuf.information.number);

    if (mybuf.mtype == mypid) {
      exit(0);
    }

    
  }



  return 0;
}
