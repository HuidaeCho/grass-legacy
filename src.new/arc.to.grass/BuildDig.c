
/* BuildDig.c 
 *
 * function defined:
 *
 * BuildDig(cov_type, neatline,
 *          lines_file, pts_file, txt_file,
 *          dig_file, atts_file, cats_filename);
 * 
 *    char *cov_type;      - either "line" or "polygon"
 *    int  neatline;       - true if neatline is desired 
 *    FILE *lines_file,    - ARC/INFO generate format lines file
 *         *pts_file,      - ARC/INFO generate format point-labels
 *         *txt_file,      - file assoc. text with attrib. numbers
 *         *dig_file,      - GRASS vector (dig) file to be created
 *         *atts_file;     - GRASS vector attribute (dig_atts) file 
 *                           to be created
 *    char *cats_filename; - GRASS vector category (dig_cats) file
 *                           to be created
 *
 * PURPOSE: builds a GRASS vector file from ARC/INFO Generate
 * files that represent either an line or polygon coverage.
 *
 * NOTES: none
 *
 * Dave Johnson
 * DBA Systems, Inc.
 * 10560 Arrowhead Drive
 * Fairfax, Virginia 22030
 *
 */

#include "stdio.h"
#include "dig_head.h"

BuildDig(cov_type,neatline,
        lines_file,pts_file,txt_file,
        dig_file,atts_file,cats_filename)
char *cov_type;
int  neatline;
FILE *lines_file,
     *pts_file,
     *txt_file,
     *dig_file,
     *atts_file;
char *cats_filename;
{
int flag=0;

#ifdef DEBUG 
printf("BuildDig\n");
#endif

if (strcmp(cov_type,"line")==0)
   {
   if (txt_file == NULL)
      {
      if (GenToDigLine(lines_file,dig_file,0) < 0)
         return(-1);
      }
   else 
      {
      if (GenToLabelledDigLines(lines_file,txt_file,dig_file,
         atts_file,cats_filename)<0)
         return(-2);
      }
   }
else if (strcmp(cov_type,"polygon")==0)
   {
   if (pts_file != NULL && txt_file!=NULL)
      {
      if (GenToDigAreaLabels(pts_file,txt_file,atts_file,cats_filename)<0)
         return(-3);
      } 
   if (GenToDigArea(lines_file,dig_file,neatline)<0)
      return(-4);
   }
return(-5);
}
