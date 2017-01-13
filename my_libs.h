#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "config.h"

//Constants
#define NUM_PROC 3
#define NUM_PLAYER 5
#define NUM_MSG  2
#define MSG_MAX_SIZE 128
#define GOAL 0
#define DRIBBLING 1
#define INJURY 2
//Events
#define PLAYER_TO_REF 333 //SEGNALAZIONE GOAL
#define PLAYER_TO_FAITH 444 //TIPO DI GIOCATA
#define FAITH_TO_PLAYER 555 //PERMESSO DI EFFETTUARE UNA GIOCATA
#define INJURY_SYSTEM 666 // SEGNALAZIONE INFORTUNIO
//End


//#define DEBUG_MODE

#define GRAFICA3 int e;FILE *file2;file2 = fopen("goal.txt", "r");if (file2) {  while ((e = getc(file2)) != EOF) putchar(e);fflush(file2);fclose(file2);}


//--------------------------------------------------------------------
 char x[20];

 void tempo(){
 char buff[20];
 struct tm *sTm;
 time_t now = time(0);
 sTm = localtime (&now);
 strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
 strcpy(x,buff);
}
