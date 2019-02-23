
#include <stdio.h>
#include "gis.h"
#include "digit.h"
#include "gtoa.h"

write_areas(name,mapset,map,lines_file,points_file,text_file)
char   *name,
       *mapset;
struct Map_info *map;
FILE   *lines_file,
       *points_file,
       *text_file;
{
P_AREA *Area;
P_ISLE *Isle;
double ax, ay;
double *x, *y;
int area; 
int type; 
int offset;
int count;
int attflag, NumPoints ;
char att[1000];
int alloc_points;
int n_points;
int n_atts;
int i,n,catflag=-1;
plus_t zero = 0;
struct line_pnts *Gpoints;
struct Categories cats;
int n_lines;
int NumAreas;

#ifdef DEBUG
printf("write_areas %s %s\n",name,mapset);
#endif

if ((catflag=G_read_vector_cats(name,mapset,&cats)) != -1)
   {
   n = cats.count;
   G_sort_cats(&cats);
   }
else 
   {
#  ifdef DEBUG
   printf("   error %d reading vector cats\n",catflag);
#  endif
   }

/* LOOP through all areas in the dig file */
count=0;
NumAreas = dig_P_num_areas(map);
for (area = 1 ; area <= NumAreas; area++)
   {
   Area = &(map->Area[area]);
   attflag=0;
   if (Area->att || area == 1)
       n_atts=1;
   else
       n_atts=0;

   /* write label-point coordinates to points_file */
   if (AREA_LABELED (Area))
      {
      count++;
      attflag=1;
      ax = map->Att[Area->att].x;
      ay = map->Att[Area->att].y;
      fprintf(points_file,"%d %lf %lf\n",count,ax,ay);
      lab_flg=1;
      }

   /* write category number and attribute text to text_file */
   if (attflag==1 && catflag!=-1)
      {
      /* get attribute text */
      i=0; att[0]=0;
      do {
         if (cats.list[i].num == map->Att[Area->att].cat)
            {
            sscanf(cats.list[i].label,"%s",att);
            i=n;
            }
         i++;
         }
      while (i<n);
      fprintf(text_file,"%d %d %d %s\n",count,map->Att[Area->att].cat,count,att);
      txt_flg=1;
      }
   }

fprintf(points_file,"END\n");
fprintf(text_file,"END\n");
}
