/*************************************************************************
* I_get_group_satexp (group_name, expose)
    Read the group file INIT_SAT and fill in the Satellite_expose structure.

* I_put_group_satexp (group_name, expose)
    Write the satellite expose structure into group file SAT_EXP. 

*************************************************************************/

#include "ortho_image.h"
#define INITIAL_FILE "SAT_EXP"


/* internal function prototypes */
#ifdef _NO_PROTO
static int   read_init_info  ();
static int   write_init_info ();
#else
static int   read_init_info  (FILE *fd, Satellite_Expose sat_info);
static int   write_init_info (FILE *fd, Satellite_Expose sat_info);
#endif


/*-----------------------------------------------------------------------
  Read the group file SAT_EXP and fill in the sat_expose structure.

  Returns:
     1  : everything o.k.
    -1  : Can't open file.
    -2  : Error read of camera_expose data.
-----------------------------------------------------------------------*/
I_get_group_satexp (group_name, sat_expose)
char              *group_name;
Satellite_Expose  *sat_expose;
{
  FILE *fd;
  char msg[100];
  int stat;


  /* open the group INIT_EXP  file */
  fd = (FILE *) I_fopen_group_file_old (group_name, INITIAL_FILE) ; 
  
  /* error opening file */
  if (!fd) {
    sprintf (msg, 
	   "unable to open initial satellite exposure file for group [%s in %s]", 
	    group_name, G_mapset());
    G_warning (msg);
    return (-1);
  }

  /* read the data */
  stat = read_init_info (fd, sat_expose);

  fclose (fd);

  /* check read status and return */
  if (stat < 0) {
    sprintf (msg, "Bad format in camera initial file <%s> in <%s>",
	     group_name, G_mapset());
    G_warning (msg);
    return (-2);
  }
  else {
    return (1);
  }
}

/*-----------------------------------------------------------------------
  Write the satellite expose structure into group file SAT_EXP. 

  Returns:
    -1  : Can't open file.
    -2  : Error writing to file.
     1  : o.k.
-----------------------------------------------------------------------*/
I_put_group_satexp (group_name, sat_expose)
char            *group_name;
Satellite_Expose  sat_expose;
{
  FILE *fd;
  char msg[100];
  int  stat;


  /* open the group INIT_EXP  file */
  fd = (FILE *) I_fopen_group_file_new (group_name, INITIAL_FILE) ; 
  
  /* error opening file */
  if (!fd) {
    sprintf (msg, 
	   "unable to open initial camera exposure file for group [%s in %s]", 
	    group_name, G_mapset());
    G_warning (msg);
    return (-1);
  }


  /* write the data */
  stat = write_init_info (fd, sat_expose);

  fclose (fd);

  /* check read status and return */
  if (stat < 0) {
    sprintf (msg, "Problem writing camera initial file <%s> in <%s>",
	     group_name, G_mapset());
    G_warning (msg);
    return (-2);
  }
  else {
    return (1);
  }
}


/*-----------------------------------------------------------------------
  Actual read of SAT_EXP file.

  TODO -- proper returns

  Returns:
     1:  read o.k
-----------------------------------------------------------------------*/
static int 
read_init_info (fd, init_info)
    FILE *fd;
    Satellite_Expose *init_info;
{   
    int n;
    char buf[100];
    double XC,YC,ZC,omega,phi,kappa;
    double XCv,YCv,ZCv,omegav,phiv,kappav;
    int status;

    G_getl (buf, sizeof buf, fd); 
    G_strip(buf);
    if (sscanf(buf,"INITIAL XC %lf \n",&XC) == 1)
       init_info->XC_init = XC;
    G_getl (buf, sizeof buf, fd); 
    G_strip(buf);
    if (sscanf(buf,"INITIAL YC %lf \n",&YC) == 1)
       init_info->YC_init = YC;    
    G_getl (buf, sizeof buf, fd); 
    G_strip(buf);
    if (sscanf(buf,"INITIAL ZC %lf \n",&ZC) == 1)
       init_info->ZC_init = ZC;    
    G_getl (buf, sizeof buf, fd); 
    G_strip(buf);
    if (sscanf(buf,"INITIAL OMEGA %lf \n",&omega) == 1)
       init_info->omega_init = omega;    
    G_getl (buf, sizeof buf, fd); 
    G_strip(buf);
    if (sscanf(buf,"INITIAL PHI %lf \n",&phi) == 1)
       init_info->phi_init = phi;    
    G_getl (buf, sizeof buf, fd); 
    G_strip(buf);
    if (sscanf(buf,"INITIAL KAPPA %lf \n",&kappa) == 1)
       init_info->kappa_init = kappa;

    G_getl (buf, sizeof buf, fd); 
    G_strip(buf);
    if (sscanf(buf,"VARIANCE XC %lf \n",&XCv) == 1)
       init_info->XC_dot = XCv;
    G_getl (buf, sizeof buf, fd); 
    G_strip(buf);
    if (sscanf(buf,"VARIANCE YC %lf \n",&YCv) == 1)
       init_info->YC_dot = YCv;    
    G_getl (buf, sizeof buf, fd); 
    G_strip(buf);
    if (sscanf(buf,"VARIANCE ZC %lf \n",&ZCv) == 1)
       init_info->ZC_dot = ZCv;    
    G_getl (buf, sizeof buf, fd); 
    G_strip(buf);
    if (sscanf(buf,"VARIANCE OMEGA %lf \n",&omegav) == 1)
       init_info->omega_dot = omegav;    
    G_getl (buf, sizeof buf, fd); 
    G_strip(buf);
    if (sscanf(buf,"VARIANCE PHI %lf \n",&phiv) == 1)
       init_info->phi_dot = phiv;    
    G_getl (buf, sizeof buf, fd); 
    G_strip(buf);
    if (sscanf(buf,"VARIANCE KAPPA %lf \n",&kappav) == 1)
       init_info->kappa_dot = kappav;
    G_getl (buf, sizeof buf, fd); 
    G_strip(buf);
    if (sscanf(buf,"STATUS (1=OK, 0=NOT OK) %d \n",&status) == 1)
       init_info->status = status;
    return 1;
}


/*-----------------------------------------------------------------------
  Actual write of INIT_EXP data.

  TODO -- proper returns

  Returns:
     1:  write  o.k
-----------------------------------------------------------------------*/
static int
write_init_info (fd, expose_info)
    FILE *fd;
    Satellite_Expose expose_info;
{
    int i;

    fprintf (fd,"INITIAL XC    %lf \n",expose_info.XC_init);
    fprintf (fd,"INITIAL YC    %lf \n",expose_info.YC_init);
    fprintf (fd,"INITIAL ZC    %lf \n",expose_info.ZC_init);
    fprintf (fd,"INITIAL OMEGA %lf \n",expose_info.omega_init);
    fprintf (fd,"INITIAL PHI   %lf \n",expose_info.phi_init);
    fprintf (fd,"INITIAL KAPPA %lf \n",expose_info.kappa_init);

    fprintf (fd,"VARIANCE XC    %lf \n",expose_info.XC_dot);
    fprintf (fd,"VARIANCE YC    %lf \n",expose_info.YC_dot);
    fprintf (fd,"VARIANCE ZC    %lf \n",expose_info.ZC_dot);
    fprintf (fd,"VARIANCE OMEGA %lf \n",expose_info.omega_dot);
    fprintf (fd,"VARIANCE PHI   %lf \n",expose_info.phi_dot);
    fprintf (fd,"VARIANCE KAPPA %lf \n",expose_info.kappa_dot);
    fprintf (fd,"STATUS (1=OK, 0=NOT OK) %d \n",expose_info.status);
}





