#include "my_libs.h"


struct msgbuf {
  long mtype;                       /* type of message */
  char mtext[MSG_MAX_SIZE];         /* user-define message */
};

struct sigaction sa, sa_old;
struct msgbuf mybuf;
struct sembuf sops;
char buffer[128];
int ball=0;
int file=0;
int dim=0;
int queue=0;
short possession=0;

void handle_signal(int signal);

int main(int argc,char*argv[]){
  int continued = 0;
  ball = atoi(argv[0]);
  queue = atoi(argv[1]);
  int referee_pid = atoi(argv[2]);
  int appearence = atoi(argv[3]);
  sa.sa_handler = &handle_signal;
  sa.sa_flags = 0;
  if (sigaction(SIGTERM, &sa, &sa_old) == -1) {
    perror("Error: cannot handle SIGTERM"); // Should not happen
  }
  //provo a conquistare il possesso palla
  #ifdef DEBUG_MODE
    dprintf(1,"Sono il processo giocatore, e il mio pid è: %d\n",getpid());
  #endif
  //devo incrementare di 1 il semaforo se non è a posto

  while (1) {
    //controllo la palla
    sleep(1);
    srand(time(NULL));
    if(continued==0){
      sops.sem_num = 0;
      sops.sem_op = -1;
      sops.sem_flg = 0;
      if ((semop(ball, &sops, 1))==-1) exit(1);
      possession++;
    }
    //Hanlding play type
    int r = rand()% 3;
    mybuf.mtype = PLAYER_TO_FAITH; // MAGIC NUMBER , DON'T ASK ME
    int num_bytes = sprintf(mybuf.mtext,"%d %d %d",r,getpid(),getppid()) + 1; //considero anche il "/0"
    msgsnd(queue, &mybuf, num_bytes, 0);
    if ((num_bytes = msgrcv(queue, &mybuf, MSG_MAX_SIZE,FAITH_TO_PLAYER, 0)) == -1) {
      exit(-1);
      break;
    } else {
      if(r == GOAL && atoi(mybuf.mtext)==1){
        mybuf.mtype = PLAYER_TO_REF; // MAGIC NUMBER , DON'T ASK ME
        int num_bytes = sprintf(mybuf.mtext,"%d",getppid()) + 1; //considero anche il "/0"
        msgsnd(queue, &mybuf, num_bytes, 0);
        #ifndef DEBUG_MODE
          GRAFICA3
        #endif
        kill(referee_pid,SIGUSR1);
        //Releasing ball semaphore
        sops.sem_num = 0;
        sops.sem_op =  1;
        sops.sem_flg = 0;
        semop(ball, &sops, 1);
        continued=0;
        possession--;
      }else if (r == GOAL && atoi(mybuf.mtext)==0 ){
        //Releasing ball semaphore
        sops.sem_num = 0;
        sops.sem_op =  1;
        sops.sem_flg = 0;
        semop(ball, &sops, 1);
        continued=0;
        possession--;
      }else if (r == DRIBBLING && atoi(mybuf.mtext)==1 ){
        continued=1;
      }else if (r == DRIBBLING && atoi(mybuf.mtext)==0 ){
        //Releasing ball semaphore
        sops.sem_num = 0;
        sops.sem_op =  1;
        sops.sem_flg = 0;
        semop(ball, &sops, 1);
        continued=0;
        possession--;
      }else if (r == INJURY && atoi(mybuf.mtext)==1 ){
        //Activating injury system
        sops.sem_num = 0;
        sops.sem_op  = -1;
        sops.sem_flg = 0;
        semop(appearence, &sops, 1);
        mybuf.mtype = INJURY_SYSTEM; // MAGIC NUMBER , DON'T ASK ME
        int num_bytes = sprintf(mybuf.mtext,"%d",getpid()) + 1; //considero anche il "/0"
        msgsnd(queue, &mybuf, num_bytes, 0);
        continued=0;
        sops.sem_num = 0;
        sops.sem_op =  1;
        sops.sem_flg = 0;
        semop(ball, &sops, 1);
        possession--;
        kill(getppid(),SIGUSR2);
        exit(0);

      }else if (r == INJURY && atoi(mybuf.mtext)==0 ){
        continued=1;
      }
    }
  }
}


void handle_signal(int signal) {
  //Explicit Exit
  switch (signal){
    case SIGTERM :
    if(possession>0){
      semctl(ball,0,IPC_RMID); //remove the ball
      exit(0);
    }
    break;
  }
}
