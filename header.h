#include "my_libs.h"

struct msgbuf {
  long mtype;                       /* type of message */
  char mtext[MSG_MAX_SIZE];         /* user-define message */
};

char* args[] = {
  "PID TEAM A",
  "PID TEAM B",
  "PID FATO",
  "BALL",
  "QUEUE",
  "referee_pid",
  "SYNC REF-TEAM",
  NULL
};

struct sembuf sops;
//Global variables here
int home = 0;
int visitors = 0;
int alrm = 0;
int rete = 0;
int q_id;
int child_pid;
int i,status, num_bytes;
struct msgbuf mybuf;
struct sigaction sa,sa1, sa_old,sa1_old;
int team_pid[NUM_PROC];
int sync1;
//End of globals


#define GRAFICA int c;FILE *file;file = fopen("intro.txt", "r");if (file) {  while ((c = getc(file)) != EOF) putchar(c);fflush(file);fclose(file);}

#define GRAFICA2 int d;FILE *file1;file1 = fopen("squadre.txt", "r");if (file1) {  while ((d = getc(file1)) != EOF) putchar(d);fflush(file1);fclose(file1);}
