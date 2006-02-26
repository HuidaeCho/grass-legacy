#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "gis.h"
#include <limits.h>
#include <float.h> 
#include "tinf.h"

/* get the slope between two cells and return a slope direction */
void check(CELL newdir, CELL *dir, void *center, void *edge, double cnst, double *oldslope)
{
   double newslope;

/* always discharge to a null boundary */
   if(is_null(edge))
   {
      *oldslope=DBL_MAX;
      *dir=newdir;
   }
   else
   {
      newslope=slope(center, edge, cnst);
      if(newslope==*oldslope)
      {
         *dir+=newdir;
      }
      else if(newslope>*oldslope)
      {
         *oldslope=newslope;
         *dir=newdir;
      }
   }

   return;

}
 
/* process one row, filling single-cell pits */
int fill_row(int nl, int ns, struct band3 *bnd)
{
   int j,offset,inc,rc;
   void *min;
   char *center;
   char *edge;

   inc=bpe();

   min=malloc(bpe());

   rc=0;
   for(j=1;j<ns-1;j+=1)
   {
      offset=j*bpe();
      center=bnd->b[1]+offset;   
      if( is_null(center) )return rc;

      edge=bnd->b[0]+offset;
      min=edge-inc;
      min=get_min(min,edge);
      min=get_min(min,edge+inc);

      min=get_min(min,center-inc);
      min=get_min(min,center+inc);

      edge=bnd->b[2]+offset;
      min=get_min(min,edge-inc);
      min=get_min(min,edge);
      min=get_min(min,edge+inc);

      if(get_min(center,min)==center)
      {
         rc=1;
         memcpy(center,min,bpe());
      }

   }
   return rc;
}

/* determine the flow direction at each cell on one row */
void build_one_row(int i, int nl, int ns, struct band3 *bnd, CELL *dir)
{
   int j,offset,inc;
   CELL sdir;
   double slope;
   char *center;
   char *edge;

   inc=bpe();

   for(j=0;j<ns;j+=1)
   {
      offset=j*bpe();
      center=bnd->b[1]+offset;
      if( is_null(center) )
      {
         G_set_c_null_value(dir+j,1);
         continue;
      }
     
      sdir=0;
      slope=HUGE_VAL;
      if(i==0)
      {
         sdir=128;
      }
      else if(i==nl-1)
      {
         sdir=8;
      }
      else if(j==0)
      {
         sdir=32;
      }
      else if(j==ns-1)
      {
         sdir=2;
      }
      else
      {
         slope=-HUGE_VAL;

/* check one row back */
         edge=bnd->b[0]+offset;
         check(64,&sdir,center,edge-inc,1.4142136,&slope);
         check(128,&sdir,center,edge,1.,&slope);
         check(1,&sdir,center,edge+inc,1.4142136,&slope);

/* check this row */
         check(32,&sdir,center,center-inc,1.,&slope);
         check(2,&sdir,center,center+inc,1.,&slope);

/* check one row forward */
         edge=bnd->b[2]+offset;
         check(16,&sdir,center,edge-inc,1.4142136,&slope);
         check(8,&sdir,center,edge,1.,&slope);
         check(4,&sdir,center,edge+inc,1.4142136,&slope);
      }

      if(slope==0.)sdir=-sdir;
      else if(slope < 0.)sdir=-256;
      dir[j]=sdir;
   }
   return;
} 

void filldir(int fe, int fd, int nl, struct band3 *bnd)
{
   int i,bufsz;
   CELL *dir;

/* fill single-cell depressions, except on outer rows and columns */
   lseek(fe,0,SEEK_SET);
   advance_band3(fe, bnd);
   advance_band3(fe, bnd);
   for(i=1;i<nl-1;i+=1)
   {
      lseek(fe,(i+1)*bnd->sz,SEEK_SET);
      advance_band3(fe, bnd);
      if(fill_row(nl,bnd->ns,bnd))
      {
         lseek(fe,i*bnd->sz,SEEK_SET);
         write(fe,bnd->b[1],bnd->sz);
      }
   }
   advance_band3(0, bnd);
   if(fill_row(nl,bnd->ns,bnd))
   {
      lseek(fe,i*bnd->sz,SEEK_SET);
      write(fe,bnd->b[1],bnd->sz);
   }
  
/* determine the flow direction in each cell.  On outer rows and columns
 * the flow direction is always directly out of the map */

   dir=calloc(bnd->ns,sizeof(CELL));
   bufsz=bnd->ns*sizeof(CELL);

   lseek(fe,0,SEEK_SET);
   lseek(fd,0,SEEK_SET);
   advance_band3(fe,bnd);
   for(i=0;i<nl;i+=1)
   {
      advance_band3(fe,bnd);
      build_one_row(i,nl,bnd->ns,bnd,dir);
      write(fd,dir,bufsz);
   }
   advance_band3(fe,bnd);
   build_one_row(i,nl,bnd->ns,bnd,dir);
   write(fd,dir,bufsz);

   free(dir);

   return;
}
