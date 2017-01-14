#include "my_libs.h"
#include "config.h"

struct msgbuf {
  long mtype;                       /* type of message */
  char mtext[MSG_MAX_SIZE];         /* user-define message */
};

struct msgbuf mybuf;
struct sigaction sa, sa_old;
int num_bytes = 0;
FILE* log_file;

void handle_signal(int signal);

int main(int argc,char*argv[]){
  srand(time(NULL));
  int queue = atoi(argv[1]);
  int ans = -1;
  sleep(1);
  dprintf(1,"\n[%s] Sono il processo FATO e ho il pid %d\n" ,__FILE__,getpid());
  sa.sa_handler = &handle_signal;
  sa.sa_flags = 0;
  if (sigaction(SIGTERM, &sa, &sa_old) == -1) {
    perror("Error: cannot handle SIGTERM"); // Should not happen
  }
  log_file = fopen("log.txt","a+");
  setvbuf(log_file, NULL, _IONBF, 0);
  tempo();
  fprintf(log_file,"\n%s [%s] Tutto e' pronto per iniziare,si comincia !\n",x,__FILE__);
  //aspetto che qualcuno scriva sulla coda
  while (1) {
    if ((num_bytes = msgrcv(queue, &mybuf, MSG_MAX_SIZE,PLAYER_TO_FAITH, 0)) == -1) {
      // Error means that the queue has been erased
      break;
    } else {
      //dprintf(1,"\n Ho letto il messaggio : %s\n",mybuf.mtext);

      int answer=0;
      int player=0;
      int team=0;
      sscanf(mybuf.mtext,"%d %d %d",&answer,&player,&team);
      tempo();
      fprintf(log_file, "\n--------------------------------------------------\n%s [%s] Giocata del giocatore %d figlio della squadra %d \r",x,__FILE__,player,team);
      #ifdef DEBUG_MODE
        dprintf(1,"\n----------------------------------\nINIZIO GIOCATA di %d , figlio della squadra %d\n----------------------------------\n",player,team);
        dprintf(1,"\nSono il giocatore %d e ho la palla",player);
      #endif

      switch(answer){
        int r=0;
        case GOAL :
        r = 1 + (rand() % 100);
        if(r<=PERC_GOAL){
          ans = 1;
          #ifdef DEBUG_MODE
            dprintf(1,"\nSono sempre il giocatore %d ,HO FATTO GOAL\n",player);
            dprintf(1,"\nFINE della giocata di %d \n---------------------\n\n",player);
          #endif
          fprintf(log_file,"Tipo di Giocata : GOAL ,permesso accordato dal fato\r\nHO FATTO GOAL\r");
          tempo();
          fprintf(log_file,"\n%s [%s] Fine Giocata\n-------------------------------------\r\n",x,__FILE__);
        }
        else {
          ans = 0 ;
          #ifdef DEBUG_MODE
            dprintf(1,"\nSono sempre il giocatore %d ,NO GOAL",player);
            dprintf(1,"\nFINE della giocata di %d \n---------------------\n\n",player);
          #endif
          fprintf(log_file,"\nTipo di Giocata : GOAL ,permesso negato dal fato\r\nNO GOAL\r");
          tempo();
          fprintf(log_file,"\n%s [%s] Fine Giocata\n-------------------------------------\r\n",x,__FILE__);

        }
        break;

        case DRIBBLING :
        r = 1 + (rand() % 100);
        if(r<=PERC_DRIBBLING) {
          ans = 1;
          #ifdef DEBUG_MODE
            dprintf(1,"\nSono sempre il giocatore %d ,Ho dribblato un avversario",player);
          #endif
          fprintf(log_file,"\nTipo di Giocata : DRIBBLING ,permesso accordato dal fato\r\nHo Dribblato con successo, proseguo l'azione\r");
          tempo();
          fprintf(log_file,"\n%s [%s] Fine Giocata\n-------------------------------------\r\n",x,__FILE__);
        }
        else {
          ans = 0 ;
          #ifdef DEBUG_MODE
            dprintf(1,"\nSono sempre il giocatore %d ,Ho perso palla, Dribblig fallito",player);
            dprintf(1,"\nFINE della giocata di %d \n---------------------\n\n",player);
          #endif
          fprintf(log_file,"\nTipo di Giocata : DRIBBLING ,permesso negato dal fato\r\nHo perso palla\r");
          tempo();
          fprintf(log_file,"\n%s [%s] Fine Giocata\n-------------------------------------\r\n",x,__FILE__);

        }
        break;

        case INJURY :
        r = 1 + (rand() % 100);
        if(r<=PERC_INJURY) {
          ans = 1;
          #ifdef DEBUG_MODE
            dprintf(1,"\nSono sempre il giocatore %d ,Mi sono infortunato",player);
            dprintf(1,"\nFINE della giocata di %d \n---------------------\n\n",player);
          #endif
          fprintf(log_file,"\nTipo di Giocata : INFORTUNIO ,il fato e' d'accordo\r\nOI OI OI, Infortunato\r");
          tempo();
          fprintf(log_file,"\n%s [%s] Fine Giocata\n-------------------------------------\r\n",x,__FILE__);

        }
        else {
          ans = 0 ;
          #ifdef DEBUG_MODE
            dprintf(1,"\nSono sempre il giocatore %d ,Mi sono salvato da un infortunio",player);
          #endif
          fprintf(log_file,"\nTipo di Giocata : INFORTUNIO ,graziato dal fato\r\nFIUUUU");
          tempo();
          fprintf(log_file,"\n%s [%s] Fine Giocata\n-------------------------------------\r\n",x,__FILE__);

        }
        break;
      }
      //ritorno quello che ho deciso;
      mybuf.mtype = FAITH_TO_PLAYER;
      int num_bytes = sprintf(mybuf.mtext,"%d",ans) + 1; //considero anche il "/0"
      msgsnd(queue, &mybuf, num_bytes, 0);
      //dprintf(1,"\nHo mandato il messaggio\n");
      sleep(1);
    }
  }

}


void handle_signal(int signal) {
  //se rivevo il segnale devo gestire il punteggio
  switch (signal){
    case SIGTERM :
    fclose(log_file);
    exit(0);
    break;
  }
}
