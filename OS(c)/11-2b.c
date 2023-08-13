#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  int msqid;
  char pathname[]="11-2a.c";
  key_t  key;
  int i, len, maxlen;

  struct mymsgbuf // Custom structure for the message
  {
    long mtype;

    struct {
    char mtext[81];
    float ind;
    } info;
  } mybuf;

  if ((key = ftok(pathname,0)) < 0) {
    printf("Can\'t generate key\n");
    exit(-1);
  }

  if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
    printf("Can\'t get msqid\n");
    exit(-1);
  }

  float ind = 0;
  for (i = 1; i <= 10; i++) {
    ind -= 1.1;
    mybuf.mtype = 2;
    mybuf.info.ind = ind;
    strcpy(mybuf.info.mtext, "2 type");
    len = sizeof(mybuf.info);
    if (msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0) {
      printf("Can\'t send message to queue\n");
      msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
      exit(-1);
    }
  }

  for (int i = 0; i < 5; ++i) {
    maxlen = sizeof(mybuf.info);

    if (( len = msgrcv(msqid, (struct msgbuf *) &mybuf, maxlen, 1, 0)) < 0) {
      printf("Can\'t receive message from queue\n");
      exit(-1);
    }
    printf("message type = %ld, info = %s, ind = %f\n", mybuf.mtype, mybuf.info.mtext, mybuf.info.ind);}

  return 0;
}
