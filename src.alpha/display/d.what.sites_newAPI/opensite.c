#include "what.h"

double G_adjust_easting();
double atof();

static FILE *Sitefd;
static Site **CurSites; 
static struct Cell_head *Wind;

#define SITE_BLOCK 512

open_sites(sname)
char *sname;
{
char *site_map;
    
        if (NULL == (site_map = G_find_file2 ("site_lists", sname, ""))){
	    fprintf (stderr,  "Could not find file '%s'", sname);
	    return(0);
	}

	Sitefd = G_fopen_sites_old (sname, site_map);
	if (Sitefd == NULL){
	    fprintf (stderr, "can't open sites file [%s]", sname);
	    return(0);
	}
	fprintf(stderr,"loading %s...", sname);

	return(1);
    
}


load_sites(wind, verbose)
struct Cell_head *wind;
int verbose;
{
int ndim, nstr, ndec;
RASTER_MAP_TYPE rtype;
int s_alloc=0, snum=0, outside=0, tot_mem=0;

    Wind = wind;
    rtype = -1;
    G_site_describe (Sitefd, &ndim, &rtype, &nstr, &ndec);
    /* use these for allocation */

    if((CurSites = (Site **)malloc(SITE_BLOCK*sizeof(Site *)))
	     == NULL){
	fprintf(stderr,"site malloc failed-not enough memory");
	return(NULL);
    }
    s_alloc = SITE_BLOCK;

    CurSites[snum] = G_site_new_struct (rtype, ndim, nstr, ndec);

    while(G_site_get (Sitefd, CurSites[snum]) >= 0){
	
	if( G_site_in_region (CurSites[snum], wind) ){

	    if(nstr) compress_cached_site(CurSites[snum]);
	    tot_mem += site_mem(CurSites[snum]);
	    snum++;
	    if (snum == s_alloc){   /* need more memory */
		
		if((CurSites = (Site **)realloc(CurSites,
			(s_alloc + SITE_BLOCK)*sizeof(Site *))) ==NULL){
		    fprintf(stderr,"site malloc failed-not enough memory");
		    return(NULL);
		}    
		s_alloc += SITE_BLOCK;
	    }
	    CurSites[snum] = G_site_new_struct (rtype, ndim, nstr, ndec);
	    if(!(snum%100) && verbose) fprintf(stderr,"%6d\b\b\b\b\b\b", snum);
	}
	else{
	    outside++;
	}
    }
    G_site_free_struct (CurSites[snum]);
    Snum = snum;
    
    if(verbose){
	fprintf(stderr,"Total sites cached: %d\n", Snum);
	fprintf(stderr,"Minimum sites memory used: %.3f Kb\n", tot_mem/1000.);
	fprintf(stderr,"Total sites outside region: %d\n", outside);
    }

}

site_mem(s)
Site *s;
{
int i, tot;

    tot = sizeof(Site);
    tot += s->dim_alloc * sizeof(double);
    tot += s->dbl_alloc * sizeof(double);
    tot += s->str_alloc * sizeof(char *);

    for (i=0; i<s->str_alloc; i++){
	tot += strlen(s->str_att[i]);
	tot++; 
    }

    return(tot);

}

compress_cached_site(s)
Site *s;
{
char *tofree;
int i;

    for (i=0; i<s->str_alloc; i++){
	tofree=s->str_att[i];
	s->str_att[i]=G_store(tofree);
	free(tofree);
    }

}

free_cached_sites()
{

    while (Snum){
        G_site_free_struct (CurSites[--Snum]);
    }
    free(CurSites);
    CurSites = NULL;

}


Site *
closest_site(pick_e, pick_n)
double pick_e, pick_n;
{
int i;
double dsq, mdsq, de, dn;
int idx = -1;

/* pick_e already "adjusted? */
    for (i = 0 ; i< Snum; i++){
	de = pick_e - G_adjust_easting (CurSites[i]->east, Wind);
	dn = pick_n - CurSites[i]->north;
	dsq = de * de + dn * dn; 
	if( idx < 0 || dsq < mdsq){
	    mdsq = dsq;
	    idx = i;
	}
    }

    return(CurSites[idx]);

}
