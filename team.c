#include "my_libs.h"

char*data[4];
void player_forker(int proc_num);
void handle_signal(int signal);
void handle_sigterm(int signal);

struct msgbuf{
  long mtype;                       /* type of message */
  char mtext[MSG_MAX_SIZE];         /* user-define message */
};

struct sigaction sa, sa_old,sa1,sa1_old;
int players_pid[NUM_PLAYER];
struct sembuf sops;
struct msgbuf mybuf;
int child_pid;
char buf[32];
int q_id;
int nproc;
int broken=0;

int main(int argc,char*argv[]){
  //Preparing my new data structure for players
  data[0] = argv[3];
  data[1]= argv[4];
  data[2]= argv[5];
  q_id = atoi(data[0]);
  if(getpid()==atoi(argv[2])) execve("faith",data,NULL);

  //Try to reserve sync semaphore (i'll wait the referee)
  sops.sem_num = 0;     // check the 0-th semaphore
  sops.sem_flg = 0;     // no flag
  sops.sem_op = -1;
  semop(atoi(argv[6]), &sops, 1);

  //Handlers setup
  sa.sa_handler = &handle_signal;
  sa.sa_flags = 0;

  sa1.sa_handler = &handle_sigterm;
  sa1.sa_flags = 0;
  //Done


  //My semaphore for the injury system
  nproc = semget(IPC_PRIVATE, 1, 0600 | IPC_CREAT);
  semctl(nproc, 0, SETVAL, 1);
  sprintf (buf,"%d",nproc);
  data[3] = strdup (buf);
  //Done

  if (sigaction(SIGUSR2, &sa, &sa_old) == -1) {
    perror("Error: cannot handle SIGUSR1"); // Should not happen
  }
  if (sigaction(SIGTERM, &sa1, &sa1_old) == -1) {
    perror("Error: cannot handle SIGTERM"); // Should not happen
  }
  //LET'S FORK A BIT
  player_forker(NUM_PLAYER);

  while (1) {
    pause();
    if(broken==1){
      int injured=0;
      int com =0;
      int pid = 0;
      pid = wait(0);
      injured = pid;
      com=injured;

      for(int i=0;i<NUM_PLAYER;i++){
        if(players_pid[i]==injured) injured = i ;
      }


      int sub =1;
      for (int i=0; i<1 && sub!=0; i++){
        switch (sub = fork()){
          case -1:
          /* Handle error */
          perror("Errore nella fork");
          break;
          case 0:
          /* Perform actions specific to team A and team B */
          execve("player",data, NULL);
          break;
        }
      }
      players_pid[injured] = sub;
      FILE* file = fopen("log.txt", "a+");
      setvbuf(file, NULL, _IONBF, 0);
      tempo();
      #ifdef DEBUG_MODE
        dprintf(1, "\n________________\n%s [%s] [INFORMAZIONE DI SERVIZIO]:Il giocatore %d viene sostituito dal giocatore %d \n________________\n",x,__FILE__, com,sub);
      #endif
      fprintf(file, "\n--------------\n%s [%s] INFORTUNIO, il giocatore %d viene sostituito dal giocatore %d \n-----------------\n",x,__FILE__, com,sub);
      fflush(file);
      fclose(file);
      if (sigaction(SIGUSR2, &sa, &sa_old) == -1) {
        perror("Error: cannot handle SIGUSR2"); // Should not happen
      }
      //Release injury system
      sops.sem_num = 0;
      sops.sem_op  = 1;
      sops.sem_flg = 0;
      semop(nproc, &sops, 1);
      broken=0;

    }
  }

}




void player_forker(int proc_num){
  child_pid=1;
  for (int i=0; i<proc_num && child_pid!=0; i++){
    switch (child_pid = fork()){
      case -1:
      /* Handle error */
      perror("Errore nella fork");
      break;
      case 0:
      /* Perform actions specific to players */
      execve("player",data, NULL);
      break;

      default:
      players_pid[i] = child_pid;
      break;
    }
  }

  #ifdef DEBUG_MODE
  sleep(1);
  dprintf(1,"\n\n [%s] Array con i PIDs dei giocatori: %d %d %d %d %d\n",__FILE__,players_pid[0],players_pid[1],players_pid[2],players_pid[3],players_pid[4] );
  #endif
}

void handle_signal(int signal) {
  //se rivevo il segnale devo gestire l'infortunio
  switch (signal){
    case SIGUSR2 :
    broken=1;
    break;
  }
}

void handle_sigterm(int signal){

  switch (signal){
    case SIGTERM:
    msgctl(atoi(data[1]),IPC_RMID,NULL); //remove the queue
    for(int j=0;j<NUM_PLAYER;j++){
      kill(players_pid[j],SIGTERM);
    }
    //Wait players for security reasons
    waitpid(-1, NULL, 0);
    semctl(nproc,0,IPC_RMID); // remove the injury system
    kill(getpid(),SIGKILL);
    break;
  }

}
