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
#include "img.h"

#include "macro.h"
#include "const.h"
#include "type.h"
#include "type_spec.h"

#include "lanceur.h"
#include "scn.h"
#include "bnd.h"
#include "cmr.h"
#include <mpi.h>


#define XCARREAU 8
#define YCARREAU 8
#define CARREAUSIZE XCARREAU*YCARREAU 

typedef struct {
  COUPLE  Pixel;
} IMG_BASIC;

static IMG_BASIC  Img;


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

void carreau_index_to_pixel(int index, int car_par_l, int * x, int * y ){
  int nb_ligne = index/car_par_l;
  *y = nb_ligne * YCARREAU;
  *x = (index%car_par_l) * XCARREAU; 
}

void
img (const char *FileNameImg)
{
  FILE   *FileImg;   
  COLOR	 *TabColor, *Color;
  STRING Name;
  INDEX	 i, j, k, l;
  BYTE Byte;

  /* Initialisation des constantes MPI */
  int myrank, nb_processes;
  MPI_Init( NULL, NULL ); 
  MPI_Comm_rank( MPI_COMM_WORLD, &myrank ); 
  MPI_Comm_size( MPI_COMM_WORLD, &nb_processes);

  if(myrank == 0)
  {
    strcpy (Name, FileNameImg);
    strcat (Name, ".ppm");
    INIT_FILE (FileImg, Name, "w");
    fprintf (FileImg, "P6\n%d %d\n255\n", Img.Pixel.i, Img.Pixel.j);
  }
  /* Nombre de carreaux */
  int nb_carreaux = Img.Pixel.i * Img.Pixel.j / CARREAUSIZE; 

  /* Nombre de carreaux dont doit s'occuper chaque processus*/
  int q = (nb_carreaux + nb_processes-1)/ nb_processes; 
  INIT_MEM (TabColor, Img.Pixel.i, COLOR);
  int C = nb_carreaux;
  int N = C+1;

  int start_j = myrank * q ;
  int end_j = MIN( (myrank+1)* q -1, nb_carreaux-1);
  /* j correspond au carreau qu'on est en train de traiter*/
  for (j=start_j; j<= end_j; j++){
    int carreau_courant = j * N % C;
    int fstpixel_x, fstpixel_y;
    carreau_index_to_pixel(carreau_courant, Img.Pixel.i/XCARREAU, &fstpixel_x, &fstpixel_y );
    /*i correspond au pixel qu'on est en train de traiter*/
    i = 0;
    for (k = 0; k < XCARREAU; ++k)
    {
      for (l = 0; l < YCARREAU; ++l)
      {
        TabColor [(j-start_j)*CARREAUSIZE + i] = pixel_basic (fstpixel_x+k, fstpixel_y+l);
        i++;
      }
    }
  }


  MPI_Reduce(TabColor, TabColor, Img.Pixel.i*Img.Pixel.j*3, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);


  // Si je suis le root, j'écris dans le fichier
  if(myrank == 0)
  {
    for (j = 0; j < Img.Pixel.j; j++) {
      for (i = 0, Color = TabColor; i < Img.Pixel.i*Img.Pixel.j; i++, Color++) {
        Byte = Color->r < 1.0 ? 255.0*Color->r : 255.0;
        putc (Byte, FileImg);
        Byte = Color->g < 1.0 ? 255.0*Color->g : 255.0;
        putc (Byte, FileImg);
        Byte = Color->b < 1.0 ? 255.0*Color->b : 255.0;
        putc (Byte, FileImg);
      }
      fflush (FileImg);
    }
  }
  EXIT_MEM (TabColor);
  EXIT_FILE (FileImg);
  
  MPI_Finalize();

}
