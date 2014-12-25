/***************************************************************************
 *                       Lanceur de rayons simplifie                       *
 ***************************************************************************
 * ENSERB, 3me annee Informatique                               1995-1996  *
 * Option AAMP, Module MP2                                                 *
 *                                                                         *
 ***************************************************************************
 * Author : Gilles Subrenat                                                *
 * Date   : 15 janvier 1996                                                *
 *                                                                         *
 * $Id$
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "img.h"
#include "queue.h"

#include "macro.h"
#include "const.h"
#include "type.h"
#include "type_spec.h"
#include "lanceur.h"
#include "lanceur_jouet.h"
#include "scn.h"
#include "bnd.h"
#include "cmr.h"

#include <mpi.h>
#include <pthread.h>

#define XCARREAU 8
#define YCARREAU 8

#define NBTHREADS 4

#define MSG_SIZE 3
#define FINALIZATION_TAG 100
#define REQUESTWORK_TAG 101 
#define TASK_TAG 102

typedef struct {
  COUPLE  Pixel;
} IMG_BASIC;

/* Parametres pour la version jouet */
int * __tasktime;
Distribution __dist;
int __numstasks;
int __numbtasks;
int __time;
int __steal_mode_jouet;

static IMG_BASIC  Img;

/* La queue des tâches ainsi que le tableau recevant les valeurs 
 * calculées est commun aux threads et déclarés global. */
COLOR  *TabColor;
Queue * tasks;
int finalization;
int __nosteal;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

BOOL
file_img (FILE *File)
{
  GET_COUPLE (Img.Pixel);
  return (TRUE);
}


static COLOR
pixel_basic (INDEX i, INDEX j)
{
  RAY    Ray;
  HIT    Hit;
  REAL   u, v;

  Ray.Level = 1;
  Ray.Color = White;
  u = 2.0 * i / Img.Pixel.i - 1.0;
  v = 2.0 * j / Img.Pixel.j - 1.0;
  cmr (&Ray, u, v);

  if (! hit_bnd (&Ray, &Hit))
    return (MainScn->Atm);
  get_scn (&Ray, &Hit);

  return (Ray.Color);
}

void (*core_func) (long tile_number);

/* index is the index of a tile
 * tpl is the number of tiles per line
 * x, and y are the location where the coordinates
 * of the first pixel will be stored.
 **/
void tile_fst_pixel(long index, int tpl, int * x, int * y ){
  int nb_ligne = index/tpl;
  *y = nb_ligne * YCARREAU;
  *x = (index%tpl) * XCARREAU; 
}

/* Une fonction qui s'occupe de la tuile de numéro tile_number */
void process_task(long tile_number){
    INDEX k, l;
    int fpx, fpy;
    int tpl = Img.Pixel.i/XCARREAU +1;
    tile_fst_pixel(tile_number, tpl, &fpx, &fpy);
    /*i correspond au pixel qu'on est en train de traiter*/
    for (k = 0; k < XCARREAU; ++k)
    {
      for (l = 0; l < YCARREAU; ++l)
      {
        int I= fpx+k, J = fpy+l;
        if(I < Img.Pixel.i && J< Img.Pixel.j){
          TabColor [J*Img.Pixel.i + I ] = pixel_basic(I, J);
        }
      }
    }
}

/* Une fonction qui s'occupe de la tuile de numéro tile_number */
void process_task_fake(long tile_number){
  usleep(__tasktime[tile_number]);
}

void* worker_f(void * args){
  while(!finalization){
    long task= -1;
    pthread_mutex_lock(&mutex);
    if(!queue_isEmpty(tasks)){
      task = queue_pop(tasks);
    }
    pthread_mutex_unlock(&mutex);
    if(task != -1)
      core_func(task);
  }
  return NULL;
}

void freeze_workers(){
}

void unfreeze_workers(){
}

void create_workers(int nbWorkers, pthread_t * threads){
  int i;
  for (i = 0; i < nbWorkers; ++i)
  {
    pthread_create(&threads[i], NULL, worker_f, NULL);
  }
}

void join_workers(int nbWorkers, pthread_t* threads){
  int i;
  for (i = 0; i < nbWorkers; ++i)
  {
    pthread_join(threads[i], NULL);
  }
}

void* negociator_f(void* args){
  int myrank, nb_others, requested = 0, res= 0;
  MPI_Status st;
  MPI_Request req;
  pthread_t workers[NBTHREADS];
  MPI_Comm_rank( MPI_COMM_WORLD, &myrank );
  MPI_Comm_size( MPI_COMM_WORLD, &nb_others);
  int next = (myrank +1) % nb_others;
  finalization = 0;
  long recv_msg[MSG_SIZE], send_msg[MSG_SIZE];
  create_workers(NBTHREADS, workers);
  if(__nosteal == 1)
  {
    while(1)
    {
      if(finalization)
        break;
      if(queue_isEmpty(tasks))
        finalization = 1;
    }
  }
  else
  {
    while(1){
      if(finalization)
      {
        break;
      }
      if(queue_isEmpty(tasks) && !requested)
      {
        freeze_workers();
        /*Ask for work*/
        send_msg[0] = myrank;
        MPI_Isend(&send_msg, MSG_SIZE, MPI_LONG, next,REQUESTWORK_TAG, MPI_COMM_WORLD, &req);
        requested = 1;
      }
      /* Listen for messages from others */
      MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &res, &st);
      /* There is some message to process, let's get it. */
      if(1 == res){
        MPI_Recv(&recv_msg, MSG_SIZE, MPI_LONG, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &st);
        /* If the message is a Finalization one */
        if(st.MPI_TAG == FINALIZATION_TAG){
          MPI_Isend(&recv_msg, MSG_SIZE, MPI_LONG, next, FINALIZATION_TAG, MPI_COMM_WORLD,&req);
          finalization = 1;
        }
        /* If the message is a request of work */
        else if(st.MPI_TAG == REQUESTWORK_TAG)
        {
          long who = recv_msg[0];
          /* First, I check wheteher it's my own message */
          if(who == myrank)
          {
            finalization = 1;
            send_msg[0]= myrank;
            MPI_Isend(&send_msg, MSG_SIZE, MPI_LONG, next,FINALIZATION_TAG, MPI_COMM_WORLD, &req);
          }
          /* If I have tasks to give */
          else if(queue_length(tasks) >= 1){
            long task = -1;
            pthread_mutex_lock(&mutex);
            if(!queue_isEmpty(tasks)){
              task = queue_pop(tasks);
            }
            pthread_mutex_unlock(&mutex);
            if(task != -1){
              send_msg[0] = task;
              MPI_Isend(&send_msg, MSG_SIZE, MPI_LONG, who,TASK_TAG,MPI_COMM_WORLD, &req);
            }
            else{
              MPI_Isend(&recv_msg, MSG_SIZE, MPI_LONG, next,REQUESTWORK_TAG, MPI_COMM_WORLD, &req);
            }
          }
          /* Else, I transfer the message to the next node*/
          else{
           MPI_Isend(&recv_msg, MSG_SIZE, MPI_LONG, next,REQUESTWORK_TAG, MPI_COMM_WORLD, &req);
          }
        }
        /* If the message is a task  */
        else if(st.MPI_TAG == TASK_TAG){
          long task = recv_msg[0];
          pthread_mutex_lock(&mutex);
          queue_push(tasks, task);
          pthread_mutex_unlock(&mutex);
          unfreeze_workers();
          requested = 0;
        }
      } /* End treatment of received message*/
    } /* End while */
  }/*end steal*/
  join_workers(NBTHREADS, workers);
  return NULL;
}

void init_tasks(int myrank, int nb_processes)
{
  INDEX j;
  /* Nombre de carreaux */
  int nb_carreaux = (Img.Pixel.i/XCARREAU +1) * (Img.Pixel.j / YCARREAU +1);

  /* Nombre de carreaux dont doit s'occuper chaque processus. */
  int q = (nb_carreaux + nb_processes-1)/ nb_processes; 

  int C = nb_carreaux;
  int N = C+1;
  int start_j = myrank * q ;
  int end_j = MIN( (myrank+1)* q -1, nb_carreaux-1);

  /* Mise en place de la queue de tâche partagée entre les threads*/
  tasks = queue_init(2*(end_j-start_j));
  for (j=start_j; j<= end_j; j++){
    long tile_number =  ((long) j * N) % C;
    queue_push(tasks, tile_number);
  }
}

void init_tasks_fake(int myrank, int nb_processes)
{
  INDEX j;
  /* Nombre de carreaux */
  int nb_carreaux = __numstasks + __numbtasks;
  __tasktime = calloc(nb_carreaux, sizeof(double));

  /* Nombre de carreaux dont doit s'occuper chaque processus. */
  int q = (nb_carreaux + nb_processes-1)/ nb_processes; 

  /* Nombre de gros carreaux dont doit s'occuper chaque processus. */
  int bq;
  if(__dist == UNIFORM)
  {
    bq = (__numbtasks + nb_processes-1)/nb_processes;
  }
  else
  {
    bq = MIN(q, __numbtasks - myrank*q);
  }

  int start_j = myrank * q ;
  int end_j = MIN( (myrank+1)* q -1, nb_carreaux-1);

  /* Mise en place de la queue de tâche partagée entre les threads*/
  tasks = queue_init(2*(end_j-start_j));
  for (j=start_j; j<= end_j; j++){
    long tile_number =  j;
    if(bq > 0){
      __tasktime[tile_number] = 10*__time;
      bq--;
    }
    else
      __tasktime[tile_number] = __time;
    queue_push(tasks, tile_number);
    MPI_Allreduce(MPI_IN_PLACE, __tasktime, nb_carreaux, MPI_INT,MPI_MAX, MPI_COMM_WORLD);
  }
}

void write_file(const char * FileNameImg)
{
  INDEX  i;
  BYTE Byte;
  STRING Name;
  FILE   *FileImg;   
  COLOR  *Color;
  strcpy (Name, FileNameImg);
  strcat (Name, ".ppm");
  INIT_FILE (FileImg, Name, "w");
  fprintf (FileImg, "P6\n%d %d\n255\n", Img.Pixel.i, Img.Pixel.j);
  for (i = 0, Color = TabColor; i < Img.Pixel.i*Img.Pixel.j; i++, Color++) {
    Byte = Color->r < 1.0 ? 255.0*Color->r : 255.0;
    putc (Byte, FileImg);
    Byte = Color->g < 1.0 ? 255.0*Color->g : 255.0;
    putc (Byte, FileImg);
    Byte = Color->b < 1.0 ? 255.0*Color->b : 255.0;
    putc (Byte, FileImg);
  }
  fflush (FileImg);
  EXIT_FILE (FileImg);
}

void process_scene(const char *FileNameImg, int img_size, int myrank, int nb_processes)
{
  pthread_t negociator;

  double start, end, elapsed, elapsed_max;
  start = MPI_Wtime();

  pthread_create(&negociator, NULL, negociator_f, NULL);
  pthread_join(negociator, NULL);
  
  if (myrank==0)
  {
    MPI_Reduce(MPI_IN_PLACE, TabColor, img_size, 
      MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
  }
  else
  {
    MPI_Reduce(TabColor, NULL, img_size, 
      MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD); 
  }
    
  end = MPI_Wtime();
  elapsed = end - start;
  MPI_Reduce(&elapsed, &elapsed_max, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
 
  if(myrank == 0)
  {
    FILE* timefile = fopen("time.dat", "a");
    if(timefile != NULL)
    {
      fprintf(timefile, "%d %g \n", nb_processes, elapsed_max);
      fclose(timefile);
    }
    printf("Benchmark time : %gs\n", elapsed_max);
  }
}

void img (const char *FileNameImg)
{
  int myrank, nb_processes;
  MPI_Init( NULL, NULL ); 
  MPI_Comm_rank( MPI_COMM_WORLD, &myrank ); 
  MPI_Comm_size( MPI_COMM_WORLD, &nb_processes);

  if(FileNameImg == NULL)
  {
    __nosteal = __steal_mode_jouet;
    init_tasks_fake(myrank, nb_processes);
    core_func = process_task_fake;
    process_scene(NULL, 0, myrank, nb_processes);
    free(__tasktime);
  }
  else
  {
    INIT_MEM (TabColor, Img.Pixel.i*Img.Pixel.j, COLOR);
    int img_size = Img.Pixel.i*Img.Pixel.j*3;
    __nosteal = 0;

    init_tasks(myrank, nb_processes);
    core_func = process_task;
    process_scene(FileNameImg, img_size, myrank, nb_processes);

    if(myrank == 0)
    {
      write_file(FileNameImg);
    }
    EXIT_MEM (TabColor);  
  }

  queue_delete(tasks);
  
  MPI_Finalize();
}
