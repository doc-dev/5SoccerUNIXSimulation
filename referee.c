#include "header.h"

//Functions setup
void handle_signal(int signal);
void handle_sigalrm(int signal);
//Setup End
char buffer[64];
FILE *a;

int main(void){
  sa.sa_handler = &handle_signal;
  sa.sa_flags = 0;
  child_pid=1;
  //Let's create first semaphore
  int ball = semget(IPC_PRIVATE, 1, 0600 | IPC_CREAT);
  semctl(ball, 0, SETVAL, 1);
  //Put key value into my structure
  char buf[32];
  sprintf (buf, "%d", ball);
  args[3] = strdup (buf);
  //done

  sync1 = semget(IPC_PRIVATE, 1, 0600 | IPC_CREAT);
  semctl(sync1, 0, SETVAL, 0);

  //Let's create the message queue
  q_id = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0600);

  //Put other key values into my structure
  sprintf (buf,"%d",q_id);
  args[4] = strdup (buf);
  sprintf (buf,"%d",getpid());
  args[5] = strdup (buf);
  sprintf (buf,"%d",sync1);
  args[6] = strdup (buf);
  //done

  tempo();
  #ifdef DEBUG_MODE
    dprintf(1,"\n\n%s [%s] L'arbitro fa scendere le 2 squadre sul terreno di gioco\n",x,__FILE__);
    sleep(1);
  #else
    sleep(2);
    GRAFICA
  #endif

  a = fopen("log.txt","a+");
  fprintf(a,"%s [%s] L'arbitro fa scendere le 2 squadre sul terreno di gioco\n",x,__FILE__);
  fflush(a);
  fclose(a);

  //LET'S FORK A BIT
  for (int i=0; i<NUM_PROC && child_pid!=0; i++){
    switch (child_pid = fork()){
      case -1:
      perror("Errore nella fork");
      case 0:
      //Preparing child pids for my structure
      sprintf (buf, "%d", getpid());
      args[i] = strdup (buf);
      //done

      //Finally let's start team program with my structure as argument
      execve("team", args,NULL);
      break;

      default:
      team_pid[i] = child_pid;
      if (sigaction(SIGUSR1, &sa, &sa_old) == -1) {
        perror("Error: cannot handle SIGUSR1"); // Should not happen
      }
      sa1.sa_handler = &handle_sigalrm;
      sa1.sa_flags = 0;
      if (sigaction(SIGALRM, &sa1, &sa_old) == -1) {
        perror("Error: cannot handle SIGALRM"); // Should not happen
      }
    }
  }
  #ifdef DEBUG_MODE
    sleep(1);
    dprintf(1,"\n\n[%s] Array Con i PIDs delle squadre %d %d %d\n",__FILE__,team_pid[0],team_pid[1],team_pid[2]);
  #else
    sleep(2);
    GRAFICA2
  #endif
  //Release team sync semaphore,so we can start with player generation
  sops.sem_num = 0;
  sops.sem_flg = 0;
  sops.sem_op =  2;
  semop(sync1, &sops, 1);
  //Done

  //Time setup
  alarm(MATCH_LENGTH);
  //End

  while(1){
    /* Action performed if SIGALRM or SIGUSR1 are received */
    pause();
    if(alrm==1){
      semctl(sync1,0,IPC_RMID); //remove sync
      tempo();
      dprintf(1,"\n%s [%s] Tempo scaduto",x,__FILE__);

      for(int j=0;j<NUM_PROC;j++){
        kill(team_pid[j],SIGTERM);
      }
      //Wait teams and faith for security reason
      waitpid(-1, NULL, 0);
      //OK

      //Closing the log file
      a = fopen("log.txt", "a+");
      if(a==NULL) perror("\nErrore File");
      sleep(1);
      tempo();
      fprintf(a, "\n______________\n %s [%s] TEMPO SCADUTO : GAME OVER, tempo giocato %d secondi\n______________\r\n\r",x,__FILE__,MATCH_LENGTH);
      tempo();
      fprintf(a,"\n%s [%s]Punteggio Finale A-B %d-%d \n",x,__FILE__,home,visitors);
      fclose(a);
      //Done

      semctl(ball,0,IPC_RMID); //remove the ball

        sleep(1);
        dprintf(1,"\n\n\nGAME OVER, L'ARBITRO MANDA TUTTI A PRENDERE UN TE CALDO \n");
        dprintf(1,"\nPunteggio Finale A-B %d-%d \n-------------------\n\n",home,visitors);

      exit(EXIT_SUCCESS);
    }


    if(rete==1){
      while ((num_bytes = msgrcv(q_id, &mybuf, MSG_MAX_SIZE,PLAYER_TO_REF, 0)) == -1) {}

      //Who scored ?
      int ppid = atoi(mybuf.mtext);
      if(ppid == team_pid[0]) home++;
      else if (ppid == team_pid[1]) visitors++;
      //OK, Undestood

      //Updating the log file with the final score
      a = fopen("log.txt", "a+");
      tempo();
      fprintf(a, "\n______________\n%s [%s] PUNTEGGIO AGGIORNATO : A-B %d-%d\n--------------------\r\n\r",x,__FILE__,home,visitors);
      fclose(a);
      //Done
      #ifdef DEBUG_MODE
      dprintf(1,"\n%s [%s]PUNTEGGIO AGGIORNATO : A-B %d-%d\n",x,__FILE__,home,visitors);
      #endif
      rete=0;
    }
  }


}

void handle_signal(int signal) {
  switch (signal){
    case SIGUSR1 :
    rete=1;
    if (sigaction(SIGUSR1, &sa, &sa_old) == -1) {
      perror("Error: cannot handle SIGUSR1"); // Should not happen
    }
    break;
  }
}

void handle_sigalrm(int signal) {
  switch(signal){
    case SIGALRM :
    alrm=1;
    break;
  }

}
