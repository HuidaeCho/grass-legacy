/*

-i to force interactive running
-s to read commands from stdin
*/

#include <unistd.h>
#define MAIN
#include "gis.h"
#include "globals.h"

int main (int argc, char *argv[])
{
int n;
FILE *fpin;

G_gisinit (argv[0]);

proj = G_projection();

reset_m_strings();
reset_cats();
both_conds_req = 0;
parse_input(argc,argv);
if (proj != PROJECTION_LL && proj != PROJECTION_UTM) 
  G_fatal_error("Must be in UTM or Lat-Long location to use this program.");
tig_names();
if (flag1->answer == 0 && flag2->answer == 0 && optin->answer == NULL)
  interactive = 1;
else
  interactive = flag1->answer;
if (proj == PROJECTION_UTM) {
  sscanf(optz->answer,"%d",&zone);
  sphere = optsph->answer;
}

fpin = NULL;
vectorfile = 0; /* initialize output flags so we can tell if opened */
sitefile   = NULL;

if (interactive) { /* do interactive stuff */
  while(n=ask_info_type()) { /* points | lines | areas ? */
    if (n==1) {
      ask_cfcc_list(); /* get CFCC's to match */
      vask_matches();  /* get strings to match */
      report(do_points(), do_areas(), do_lines() );
    }
  }
}
else {             /* do command driven stuff */
  if (optin->answer != NULL) {
    if (fpin==NULL)
      if ((fpin=fopen(optin->answer,"r"))==NULL)
        G_fatal_error("Can't open command input file.");
    while (process(fpin))
      report(do_points(), do_areas(), do_lines() );
  }
  if (flag2->answer)
    while (process(stdin))
      report(do_points(), do_areas(), do_lines() );
}

  return 0;
} /* end of main */


/* give summary of pts, areas, lines written */
int report (int n_points, int n_areas, int n_lines)
{
if (n_points){
   fprintf (stdout,"\n%7d Landmark point locations written to site list <%s>\n",
      n_points, site_name);
}

if (n_areas)
    fprintf (stdout,"\n%7d Landmark areas (polygons) written to vector file <%s>\n",
             n_areas ,vect_name);
if (n_lines) fprintf (stdout,"\n%7d Type 1 lines written to vector file <%s>\n",
             n_lines, vect_name);
sleep(3);

  return 0;
}
