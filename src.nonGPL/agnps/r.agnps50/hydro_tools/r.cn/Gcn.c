/* %W% %G% */

/* January, 1991 Agricultural Engineering, Purdue University
   Raghavan Srinivasan (srin@ecn.purdue.edu)

   r.cn()

   This program generates a Curve Number (CN) map based on the 
   four input map layers include, hydrological soil group, land-
   use, practice or management, hydrological condition maps.
   This program follows the SCS CN table.
*/

#include "CN.h"


main(argc,argv)
int argc;
char	*argv[];
{
	char    buf[100], *title[100];

	int     i, j; 
	int	row_id, col_id;
	int 	G_get_map_row_nomask();
	int	cell_open(), cell_open_new();
	int	rules(), hy_soil_group(),amc_conversion();
	CELL    *hy_soil_group_rbuf, *veg_cover_rbuf, *hy_cond_rbuf;
	CELL    *land_use_rbuf, *cn_rbuf;
	int     hy_soil_group_id, veg_cover_id,cn_id;
	int     hy_cond_id,land_use_id;
	int amc;

/*  Initialize the GRASS environment variables */
	G_gisinit (argv[0]);

	hy_soil_group_flag = land_use_flag = veg_cover_flag = cn_flag = 0;

/* Check for any error at the command line key words */
	for (i = 1; i < argc; i++)
	{
	    if  (sscanf(argv[i], "sg=%[^\n]", hy_soil_group_name) == 1) hy_soil_group_flag = 1;
	    else if (sscanf(argv[i], "lu=%[^\n]", land_use_name) == 1) land_use_flag = 1;
	    else if (sscanf(argv[i], "pr=%[^\n]", veg_cover_name) == 1) veg_cover_flag = 1;
	    else if (sscanf(argv[i], "hc=%[^\n]", hy_cond_name) == 1) hy_cond_flag = 1;
	    else if (sscanf(argv[i], "cn=%[^\n]", cn_name) == 1) cn_flag = 1;
	    else if (sscanf(argv[i], "amc=%[^\n]", amc_name) == 1) amc_flag = 1;
	    else usage(argv[0]);
	}


	if((hy_soil_group_flag != 1) && (land_use_flag != 1) && (veg_cover_flag != 1) && (hy_cond_flag != 1) && (cn_flag != 1))
	 {  
	    usage(argv[0]);
	 }

	 if(strcmp(amc_name,"1") == 0) amc = 1;
	 else if(strcmp(amc_name,"3") == 0) amc = 3;
	 else amc = 2;

/*	get the current mapset */
	 this_mapset = G_mapset();

/*	check for the legal file names for all the map layer required */
	 if(G_legal_filename(hy_soil_group_name) == -1)
	 {
	      sprintf(buf, "hydrological soil group map layer name [%s] not legal for GRASS\n",hy_soil_group_name);
	      G_fatal_error(buf);
	      exit(1);
	 }

	 if(G_legal_filename(land_use_name) == -1)
	 {
	      sprintf(buf, "landuse map layer name [%s] not legal for GRASS\n",land_use_name);
	      G_fatal_error(buf);
	      exit(1);
	 }

	 if(G_legal_filename(veg_cover_name) == -1)
	 {
	      sprintf(buf, "vegetation cover map layer name [%s] not legal for GRASS\n",veg_cover_name);
	      G_fatal_error(buf);
	      exit(1);
	 }

	 if(G_legal_filename(hy_cond_name) == -1)
	 {
	      sprintf(buf, "hydrological condition map layer name [%s] not legal for GRASS\n",hy_cond_name);
	      G_fatal_error(buf);
	      exit(1);
	 }

	 if(G_legal_filename(cn_name) == -1)
	 {
	      sprintf(buf, "curver number map layer name [%s] not legal for GRASS\n",cn_name);
	      G_fatal_error(buf);
	      exit(1);
	 }

/*	if curver number output map exists in the mapset then
	print error message and quit */

	 cn_mapset = G_find_cell2(cn_name, this_mapset);
	 if (cn_mapset)
	 {
	     sprintf(buf, "curve number file [%s] is existing\n", cn_name);
	     G_fatal_error (buf);
	     exit(1);
	 }

/*	find all the map in the mapset and get their mapset location */

	 hy_soil_group_mapset = G_find_cell2(hy_soil_group_name, "");
	 if (!hy_soil_group_mapset)
	 {
	     sprintf(buf, "hydrological soil group file [%s] not found\n", hy_soil_group_name);
	     G_fatal_error (buf);
	     exit(1);
	 }

	 land_use_mapset = G_find_cell2(land_use_name, "");
	 if (!land_use_mapset)
	 {
	     sprintf(buf, "landuse file [%s] not found\n", land_use_name);
	     G_fatal_error (buf);
	     exit(1);
	 }

	 veg_cover_mapset = G_find_cell2(veg_cover_name, "");
	 if (!veg_cover_mapset)
	 {
	     sprintf(buf, "vegetation cover file [%s] not found\n", veg_cover_name);
	     G_fatal_error (buf);
	     exit(1);
	 }

	 hy_cond_mapset = G_find_cell2(hy_cond_name, "");
	 if (!hy_cond_mapset)
	 {
	     sprintf(buf, "hydrological condition file [%s] not found\n", hy_cond_name);
	     G_fatal_error (buf);
	     exit(1);
	 }

/*	get the window information  */
	 G_get_set_window (&window);
	 nrows = G_window_rows();
	 ncols = G_window_cols();

/*	open the map and get their file id  */

	hy_soil_group_id = cell_open(hy_soil_group_name,hy_soil_group_mapset);
	veg_cover_id = cell_open(veg_cover_name,veg_cover_mapset);
	hy_cond_id = cell_open(hy_cond_name,hy_cond_mapset);
	cn_id = cell_open_new(cn_name);
	land_use_id = cell_open(land_use_name,land_use_mapset);


/* get the category names and cell title */

	if (G_read_cats (hy_soil_group_name, hy_soil_group_mapset, &hy_soil_group_cats) < 0)
	    exit(-1);
	if (G_read_cats (land_use_name, land_use_mapset, &land_use_cats) < 0)
	    exit(-1);
	if (G_read_cats (veg_cover_name, veg_cover_mapset, &veg_cover_cats) < 0)
	    exit(-1);
	if (G_read_cats (hy_cond_name, hy_cond_mapset, &hy_cond_cats) < 0)
	    exit(-1);


/*	assign curve number values for the array hy_soil_cover  */
	data();

	/*
	amc = 0;

	fprintf (stderr,"\n	The Curver Number map created will contain curver number\n 	equivalent to AMC II condition.\n\n");
	fprintf (stderr,"	Would you like to revise to AMC I or AMC III (y/n)? ");
	scanf("%s",buf);

	if ((strcmp(buf,"y") == 0) || (strcmp(buf,"Y") == 0) || (strcmp(buf,"yes") == 0) || (strcmp(buf,"YES") == 0))
	{
	    for (;;)
	    {
	    fprintf (stderr,"\n		Enter 1 for AMC I or 3 for AMC III --> ");
	    scanf("%d",&amc);
	    if (amc != 2 && amc > 0 && amc < 4) break;
	    else continue;
	    }
	}

	else amc = 2;
*/



/*	allocate cell buf for all the map layers */

	hy_soil_group_rbuf = G_allocate_cell_buf();
	veg_cover_rbuf = G_allocate_cell_buf();
	hy_cond_rbuf = G_allocate_cell_buf();
	land_use_rbuf = G_allocate_cell_buf();
	cn_rbuf = G_allocate_cell_buf();

	for(i = 0; i < nrows; i++) 
	{
	/*
	*/
	    G_get_c_raster_row(veg_cover_id,veg_cover_rbuf,i);
	    G_get_c_raster_row(hy_soil_group_id,hy_soil_group_rbuf,i);
	    G_get_c_raster_row(hy_cond_id,hy_cond_rbuf,i);
	    G_get_c_raster_row(land_use_id,land_use_rbuf,i);

	    G_zero_cell_buf(cn_rbuf);

	    for(j=0;j < ncols;j++) 
	    {
	        if(hy_soil_group_rbuf[j] > 0)
		{
/*	check the rules for the practice, landuse and hydrological
	condition and get the row number from the CN table */

		   row_id = rules(veg_cover_rbuf[j],land_use_rbuf[j],hy_cond_rbuf[j]);
	/*
		   row_id = rules(1,land_use_rbuf[j],hy_cond_rbuf[j]);
	*/

/*	check the hydrological soil group and get the column
	from the CN table */

		   col_id = hy_soil_group(hy_soil_group_rbuf[j]);

/*	if AMC is 2 then assign the corresponding CN from the table */
		   if (amc == 2)
		       cn_rbuf[j] = hy_soil_cover[row_id][col_id];
/*	else convert the CN to the selected AMC condition */
		   else
		       cn_rbuf[j] = amc_conversion(hy_soil_cover[row_id][col_id],amc);
		 }
	    }
	    G_put_c_raster_row(cn_id,cn_rbuf);
	 }

	 G_close_cell(hy_soil_group_id);
	 G_close_cell(veg_cover_id);
	 G_close_cell(land_use_id);
	 G_close_cell(hy_cond_id);
	 G_close_cell(cn_id);

/*	write the appropriate CN title in the curver number map generated */
	 if (amc == 1) strcpy(title,"Curve Number Map for AMC I");
	 if (amc == 2) strcpy(title,"Curve Number Map for AMC II");
	 if (amc == 3) strcpy(title,"Curve Number Map for AMC III");

	 G_put_cell_title(cn_name,title);

}
