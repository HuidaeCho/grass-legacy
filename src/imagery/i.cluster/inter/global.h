#include "imagery.h"

#ifndef GLOBAL
# define GLOBAL extern
#endif

GLOBAL char groupname[50];
GLOBAL char subgroupname[50];
GLOBAL char outsigname[50];
GLOBAL char seedname[50];
GLOBAL char reportname[50];
GLOBAL int verbose;
GLOBAL int sample_rows, sample_cols;
GLOBAL int class;
GLOBAL int iters;
GLOBAL double conv;
GLOBAL double sep;
GLOBAL int min_size; 

/* ask_files.c */
int ask_files(void);
/* ask_parms.c */
int ask_parms(void);
/* run_cluster.c */
int run_cluster(void);
