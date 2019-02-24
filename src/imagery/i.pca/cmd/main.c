/***** Principal Component Transformation of Satellite Data *****/
/*

             Center for Space Research
             WRW 402
             University of Texas
             Austin, TX 78712-1085

             (512) 471-6824

*/

#define MAIN

#include "gis.h"
#include "globals.h"

main(argc,argv)
int argc;
char *argv[];
{
  /* Global variable & function declarations */

  int i,j,k;                    /* Loop control variables */
  long rows,cols;               /* Number of rows & columns */
  long bands;                   /* Number of image bands */
  long NN;                      /* Total number of data points */
  double *DATA[MX];             /* Data structure containing image data values*/
  double mu[MX];                /* Mean vector for image bands */
  double covar[MX+1][MX+1];     /* Covariance Matrix */
  double eigval[MX+1];
  double eigmat[MX+1][MX+1];
  int result, infd, outfd;
  char tempbuf[100], *mapset;
  CELL *rowbuf;
  CELL outbandmax[MX+1];      /* will hold the maximums found in the out maps*/
  struct Colors color_tbl;

  struct Option *opt1, *opt2 ;

  /* Define the different options */

  opt1 = G_define_option() ;
  opt1->key        = "input";
  opt1->type       = TYPE_STRING;
  opt1->required   = YES;
  opt1->multiple   = YES;
  opt1->gisprompt  = "old,cell,raster" ;
  opt1->description= "input layer name" ;

  opt2 = G_define_option() ;
  opt2->key        = "output";
  opt2->type       = TYPE_STRING;
  opt2->required   = YES;
  opt2->gisprompt  = "new,cell,raster" ;
  opt2->description= "output layer name";

  /***** Start of main *****/
  G_gisinit(argv[0]);

  if (G_parser(argc, argv) < 0)
    exit(-1);

  /* check output file */
  if (strlen(opt2->answer) >=13)
    G_fatal_error("The output cell map name can not be longer than 12 characters.");

  rows = G_window_rows();
  cols = G_window_cols();
  NN = rows * cols;

  rowbuf = G_allocate_cell_buf();

  for(bands=0; opt1->answers[bands] != NULL; bands++)
    ;

  if (bands < 2)
    G_fatal_error("Sorry, you must provide at least 2 input bands") ;
  if (bands >= MX)
    G_fatal_error("Sorry, you must provide fewer than 8 input bands") ;

  allocate(DATA,NN,bands);      /* Allocate structures */

  /* input the data */
  for (i=0 ; i<bands ; i++)
    {
      if ((mapset=G_find_cell(opt1->answers[i], "")) == NULL) {
        sprintf(tempbuf, "Unable to find cell map <%s>", opt1->answers[i]);
        G_fatal_error(tempbuf);
      }
      if ((infd=G_open_cell_old(opt1->answers[i], mapset)) < 0)
        {
          sprintf(tempbuf,"Error opening %s\n",opt1->answers[i]);
          G_fatal_error(tempbuf);
        }
      for (j=0 ; j<rows ; j++)
        {
          G_get_map_row(infd, rowbuf, j);
          for (k=0 ; k<cols ; k++)
            *(DATA[i]+((j*cols)+k))=(double)rowbuf[k];
        }
      G_close_cell(infd);
    }
  printf("Calculating band means...\n");
  means(DATA,NN,bands,mu);
  printf("Calculating covariance matrix...\n");
  covariance(DATA,NN,bands,mu,covar);
  printf("Calculating eigenvalues and eigenvectors...\n");
  jacobi(covar,bands,eigval,eigmat);
  printf("Ordering eigenvalues in descending order...\n");
  egvorder(eigval,eigmat,bands);
  printf("Transposing eigen matrix...\n");
  transpose(eigmat,bands);
  printf("Transforming data...\n");
  transform(DATA,NN,eigmat,bands,outbandmax);

  /* output the results */
  for (i=0 ; i<bands ; i++) {
    sprintf(tempbuf, "%s.%d", opt2->answer, i+1);
    if ((outfd=G_open_cell_new(tempbuf)) < 0)
      G_fatal_error("Error while opening an output file.");
    printf("Writing data to output file %s\n", tempbuf);
    for (j=0;j<rows;j++) {
      for (k=0; k<cols; k++)
        rowbuf[k] = (CELL) (*(DATA[i]+(j*cols)+k)+0.5);
      G_put_map_row(outfd, rowbuf);
    }
    G_close_cell(outfd);

    /* make grey scale color table */
    G_init_colors(&color_tbl);
    G_make_grey_scale_colors(&color_tbl, 0, outbandmax[i]);

    /* write a color table */
    G_write_colors(tempbuf, G_mapset(), &color_tbl);
    G_free_colors(&color_tbl);
  }
  printf("Completed write...\n");

  printf("Releasing dynamic memory...\n");
  free(rowbuf);
  freemem(DATA,bands);          /* Free dynamic memory */
}

