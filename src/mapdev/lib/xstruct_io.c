/* %W% %G% */
/*
** Written by: Dave Gerdes 5 1988
** US Army Construction Engineering Research Lab
** Modified by: Dave Gerdes 9 1988   portable
*/

#include "digit.h"

/* routines for reading and writing Dig+ structures. */
/* return 0 on success, -1 on failure of whatever kind */
/* if you dont want it written out, then dont call these routines */
/* ie  check for deleted status before calling a write routine */
/*  in as much as it would be nice to hide that code in here,  */
/*  this is a library routine and we chose to make it dependant on */
/*  as few external files as possible */

/*  these routines assume ptr->alloc_lines  is valid */
/*  Make sure it is initialized before calling */

/*  Internally, my default variables for lines/areas/nodes/isles  are type
**  plus_t  which is typedefed as short.  This limits the current version
**  to no more than 32K lines, nodes etc. (excluding points)
**  All in the name of future expansion, I have converted these values to 
**  longs in the dig_plus data file.

**  NOTE: 3.10 changes plus_t to  ints.
**    This assumes that any reasonable machine will use 4 bytes to
**    store an int.  The mapdev code is not guaranteed to work if
**    plus_t is changed to a type that is larger than an int.
*/

dig_Rd_P_node (ptr, fp)
    FILE *fp;
    struct P_node *ptr;
{
    long conv;
    register int i;

    if (0 >= fread (&(ptr->x      ), sizeof (ptr->x      ), 1, fp)) return (-1);
	dig__double_convert (&ptr->x, &ptr->x, 1);
    if (0 >= fread (&(ptr->y      ), sizeof (ptr->y      ), 1, fp)) return (-1);
	dig__double_convert (&ptr->y, &ptr->y, 1);
    if (0 >= fread (&(conv        ), sizeof (long        ), 1, fp)) return (-1);
	dig__long_convert (&conv, &conv, 1); ptr->n_lines = conv;

    dig_node_alloc_line (ptr, (int) ptr->n_lines);
    if (ptr->n_lines)	/* Not guaranteed what fread does w/ 0 */
    {
     for (i = 0 ; i < ptr->n_lines ; i++)
     {
	 if (0>=fread(&conv, sizeof(long), 1,fp)) return(-1);
	 dig__long_convert (&conv, &conv, 1);
	 ptr->lines[i] = conv;
     }
     if(0>=fread(ptr->angles,sizeof(float),(int) ptr->n_lines,fp))return(-1);
     dig__float_convert (ptr->angles, ptr->angles, (int) ptr->n_lines);

    }
    ptr->alive = 1;
    return (0);
}
dig_Wr_P_node (ptr, fp)
    FILE *fp;
    struct P_node *ptr;
{
    long conv;
    double dconv;
    register int i;
    float *Ptmp;

    dconv = ptr->x; dig__double_convert (&dconv, &dconv, 1);
    if (0 >=
    fwrite (&(dconv        ), sizeof (ptr->x      ), 1, fp)) return (-1);
    dconv = ptr->y; dig__double_convert (&dconv, &dconv, 1);
    fwrite (&(dconv        ), sizeof (ptr->y      ), 1, fp);
    conv = ptr->n_lines; dig__long_convert (&conv, &conv, 1);
    fwrite (&(conv), sizeof (long), 1, fp);

    if (ptr->n_lines)
    {
	for (i = 0 ; i < ptr->n_lines ; i++)
	{
	    conv = ptr->lines[i]; dig__long_convert (&conv, &conv, 1);
	    fwrite (&conv,  sizeof (long),  1, fp);
	}
	if (NULL == (Ptmp = dig__float_convert (ptr->angles, NULL, (int) ptr->n_lines)))
	    return (dig_out_of_memory());
	fwrite (Ptmp, sizeof (float), (int) ptr->n_lines, fp);
    }
    return (0);
}

dig_Rd_P_line (ptr, fp)
    FILE *fp;
    struct P_line *ptr;
{
    long conv;

    if (0 >= fread (&(conv       ), sizeof (long       ), 1, fp)) return (-1);
    dig__long_convert (&conv, &conv, 1); ptr->N1 = conv;
    if (0 >= fread (&(conv       ), sizeof (long       ), 1, fp)) return (-1);
    dig__long_convert (&conv, &conv, 1); ptr->N2 = conv;
    if (0 >= fread (&(conv       ), sizeof (long       ), 1, fp)) return (-1);
    dig__long_convert (&conv, &conv, 1); ptr->left = conv;
    if (0 >= fread (&(conv       ), sizeof (long       ), 1, fp)) return (-1);
    dig__long_convert (&conv, &conv, 1); ptr->right = conv;

    if (0 >= fread (&(ptr->N     ), sizeof (ptr->N     ), 1, fp)) return (-1);
    dig__double_convert (&ptr->N, &ptr->N, 1);
    if (0 >= fread (&(ptr->S     ), sizeof (ptr->S     ), 1, fp)) return (-1);
    dig__double_convert (&ptr->S, &ptr->S, 1);
    if (0 >= fread (&(ptr->E     ), sizeof (ptr->E     ), 1, fp)) return (-1);
    dig__double_convert (&ptr->E, &ptr->E, 1);
    if (0 >= fread (&(ptr->W     ), sizeof (ptr->W     ), 1, fp)) return (-1);
    dig__double_convert (&ptr->W, &ptr->W, 1);
    if (0 >= fread (&(ptr->offset), sizeof (ptr->offset), 1, fp)) return (-1);
    dig__long_convert (&ptr->offset, &ptr->offset, 1);

    if (0 >= fread (&(conv       ), sizeof (long       ), 1, fp)) return (-1);
    dig__long_convert (&conv, &conv, 1); ptr->att = conv;

    if (0 >= fread (&(ptr->type  ), sizeof (ptr->type  ), 1, fp)) return (-1);
    return (0);
}
dig_Wr_P_line (ptr, fp)
    FILE *fp;
    struct P_line *ptr;
{
    long conv;
    double dconv;

    conv = ptr->N1; dig__long_convert (&conv, &conv, 1);
    if (0 >= 
    fwrite (&(conv       ), sizeof (long       ), 1, fp)) return (-1);
    conv = ptr->N2; dig__long_convert (&conv, &conv, 1);
    fwrite (&(conv       ), sizeof (long       ), 1, fp);
    conv = ptr->left; dig__long_convert (&conv, &conv, 1);
    fwrite (&(conv       ), sizeof (long       ), 1, fp);
    conv = ptr->right; dig__long_convert (&conv, &conv, 1);
    fwrite (&(conv       ), sizeof (long       ), 1, fp);

    dconv = ptr->N; dig__double_convert (&dconv, &dconv, 1);
    fwrite (&(dconv      ), sizeof (ptr->N     ), 1, fp);
    dconv = ptr->S; dig__double_convert (&dconv, &dconv, 1);
    fwrite (&(dconv      ), sizeof (ptr->S     ), 1, fp);
    dconv = ptr->E; dig__double_convert (&dconv, &dconv, 1);
    fwrite (&(dconv      ), sizeof (ptr->E     ), 1, fp);
    dconv = ptr->W; dig__double_convert (&dconv, &dconv, 1);
    fwrite (&(dconv      ), sizeof (ptr->W     ), 1, fp);
    conv = ptr->offset; dig__long_convert (&conv, &conv, 1);
    fwrite (&(conv       ), sizeof (ptr->offset), 1, fp);

    conv = ptr->att; dig__long_convert (&conv, &conv, 1);
    fwrite (&(conv       ), sizeof (long       ), 1, fp);
    fwrite (&(ptr->type  ), sizeof (ptr->type  ), 1, fp);
    return (0);
}

dig_Rd_P_area (ptr, fp)
    FILE *fp;
    struct P_area *ptr;
{
    register int i;
    long conv;

    if (0 >= fread (&(ptr->N), sizeof (ptr->N), 1, fp)) return (-1);
    dig__double_convert (&ptr->N, &ptr->N, 1);
    if (0 >= fread (&(ptr->S), sizeof (ptr->S), 1, fp)) return (-1);
    dig__double_convert (&ptr->S, &ptr->S, 1);
    if (0 >= fread (&(ptr->E), sizeof (ptr->E), 1, fp)) return (-1);
    dig__double_convert (&ptr->E, &ptr->E, 1);
    if (0 >= fread (&(ptr->W), sizeof (ptr->W), 1, fp)) return (-1);
    dig__double_convert (&ptr->W, &ptr->W, 1);

    if (0 >= fread (&(conv  ), sizeof (long  ), 1, fp)) return (-1);
    dig__long_convert (&conv, &conv, 1);
    ptr->att = conv;
    if (0 >= fread (&(conv  ), sizeof (long  ), 1, fp)) return (-1);
    dig__long_convert (&conv, &conv, 1);
    ptr->n_lines = conv;
    if (0 >= fread (&(conv  ), sizeof (long  ), 1, fp)) return (-1);
    dig__long_convert (&conv, &conv, 1);
    ptr->n_isles = conv;


    ptr->lines = (plus_t *) dig_falloc (sizeof (plus_t), (int) ptr->n_lines);
    if (ptr->n_lines)
    {
	for (i = 0 ; i < ptr->n_lines ; i++)
	{
	    if (0 >= fread (&conv, sizeof (long), 1, fp)) return (-1);
	    dig__long_convert (&conv, &conv, 1);
	    ptr->lines[i] = conv;
	}
    }
    ptr->alloc_lines = ptr->n_lines;

    /* island stuff */
    ptr->isles = (plus_t *) dig_falloc (sizeof (plus_t), (int) ptr->n_isles);
    if (ptr->n_isles)
    {
	for (i = 0 ; i < ptr->n_isles ; i++)
	{
	    if (0 >= fread (&conv, sizeof (long), 1, fp)) return (-1);
	    dig__long_convert (&conv, &conv, 1);
	    ptr->isles[i] = conv;
	}
    }
    ptr->alloc_isles = ptr->n_isles;

    ptr->alive = 1;
    return (0);
}
dig_Wr_P_area (ptr, fp)
    FILE *fp;
    struct P_area *ptr;
{
    register int i;
    long conv;
    double dconv;

    dig__double_convert (&ptr->N, &dconv, 1);
    if (0 >= 
    fwrite (&(dconv ), sizeof (ptr->N), 1, fp)) return (-1);
    dig__double_convert (&ptr->S, &dconv, 1);
    fwrite (&(dconv ), sizeof (ptr->S), 1, fp);
    dig__double_convert (&ptr->E, &dconv, 1);
    fwrite (&(dconv ), sizeof (ptr->E), 1, fp);
    dig__double_convert (&ptr->W, &dconv, 1);
    fwrite (&(dconv ), sizeof (ptr->W), 1, fp);

    conv = ptr->att;
    dig__long_convert (&conv, &conv, 1);
    fwrite (&(conv  ), sizeof (long   ), 1, fp);
    conv = ptr->n_lines;
    dig__long_convert (&conv, &conv, 1);
    fwrite (&(conv  ), sizeof (long  ), 1, fp);
    conv = ptr->n_isles;
    dig__long_convert (&conv, &conv, 1);
    fwrite (&(conv  ), sizeof (long  ), 1, fp);

    if (ptr->n_lines)
    {
	for ( i = 0 ; i < ptr->n_lines ; i++)
	{
	    conv = ptr->lines[i];
	    dig__long_convert (&conv, &conv, 1);
	    fwrite (&conv, sizeof (long), 1, fp);
	}
    }
    /* island stuff */
    if (ptr->n_isles)
    {
	for ( i = 0 ; i < ptr->n_isles ; i++)
	{
	    conv = ptr->isles[i];
	    dig__long_convert (&conv, &conv, 1);
	    fwrite (&conv, sizeof (long), 1, fp);
	}
    }
    return (0);
}

/* island stuff */
dig_Rd_P_isle (ptr, fp)
    FILE *fp;
    struct P_isle *ptr;
{
    register int i;
    long conv;

    if (0 >= fread (&(ptr->N), sizeof (ptr->N), 1, fp)) return (-1);
    dig__double_convert (&ptr->N, &ptr->N, 1);
    if (0 >= fread (&(ptr->S), sizeof (ptr->S), 1, fp)) return (-1);
    dig__double_convert (&ptr->S, &ptr->S, 1);
    if (0 >= fread (&(ptr->E), sizeof (ptr->E), 1, fp)) return (-1);
    dig__double_convert (&ptr->E, &ptr->E, 1);
    if (0 >= fread (&(ptr->W), sizeof (ptr->W), 1, fp)) return (-1);
    dig__double_convert (&ptr->W, &ptr->W, 1);

    if (0 >= fread (&(conv  ), sizeof (long  ), 1, fp)) return (-1);
    dig__long_convert (&conv, &conv, 1);
    ptr->area = conv;
    if (0 >= fread (&(conv  ), sizeof (long  ), 1, fp)) return (-1);
    dig__long_convert (&conv, &conv, 1);
    ptr->n_lines = conv;

    ptr->lines = (plus_t *) dig_falloc (sizeof (plus_t), (int) ptr->n_lines);
    if (ptr->n_lines)
    {
	for (i = 0 ; i < ptr->n_lines ; i++)
	{
	    if (0 >= fread (&conv, sizeof (long),  1, fp)) return (-1);
	    dig__long_convert (&conv, &conv, 1);
	    ptr->lines[i] = conv;
	}
    }
    ptr->alloc_lines = ptr->n_lines;

    ptr->alive = 1;
    return (0);
}
dig_Wr_P_isle (ptr, fp)
    FILE *fp;
    struct P_isle *ptr;
{
    register int i;
    long conv;
    double dconv;

    dig__double_convert (&ptr->N, &dconv, 1);
    if (0 >= 
    fwrite (&(dconv       ), sizeof (ptr->N      ), 1, fp)) return (-1);
    dig__double_convert (&ptr->S, &dconv, 1);
    fwrite (&(dconv       ), sizeof (ptr->S      ), 1, fp);
    dig__double_convert (&ptr->E, &dconv, 1);
    fwrite (&(dconv       ), sizeof (ptr->E      ), 1, fp);
    dig__double_convert (&ptr->W, &dconv, 1);
    fwrite (&(dconv       ), sizeof (ptr->W      ), 1, fp);

    conv = ptr->area;
    dig__long_convert (&conv, &conv, 1);
    fwrite (&(conv        ), sizeof (long        ), 1, fp);
    conv = ptr->n_lines;
    dig__long_convert (&conv, &conv, 1);
    fwrite (&(conv        ), sizeof (long        ), 1, fp);
    if (ptr->n_lines)
    {
	for (i = 0 ; i < ptr->n_lines ; i++)
	{
	    conv = ptr->lines[i];
	    dig__long_convert (&conv, &conv, 1);
	    fwrite (&conv, sizeof (long), 1, fp);
	}
    }
    return (0);
}

dig_Rd_P_att (ptr, fp)
    FILE *fp;
    struct P_att *ptr;
{
    long conv;

    if (0 >= fread (&(ptr->x     ), sizeof (ptr->x     ), 1, fp)) return (-1);
    dig__double_convert (&ptr->x, &ptr->x, 1);
    if (0 >= fread (&(ptr->y     ), sizeof (ptr->y     ), 1, fp)) return (-1);
    dig__double_convert (&ptr->y, &ptr->y, 1);
    if (0 >= fread (&(ptr->offset), sizeof (ptr->offset), 1, fp)) return (-1);
    dig__long_convert (&ptr->offset, &ptr->offset, 1);
    if (0 >= fread (&(conv       ), sizeof (long       ), 1, fp)) return (-1);
    dig__long_convert (&conv, &conv, 1);
    ptr->cat = conv;

    if (0 >= fread (&(conv       ), sizeof (long       ), 1, fp)) return (-1);
    dig__long_convert (&conv, &conv, 1);
    ptr->index = conv;

    if (0 >= fread (&(ptr->type  ), sizeof (ptr->type  ), 1, fp)) return (-1);
    return (0);
}
dig_Wr_P_att (ptr, fp)
    FILE *fp;
    struct P_att *ptr;
{
    long conv;
    double dconv;

    dig__double_convert (&ptr->x, &dconv, 1);
    if (0 >= 
    fwrite (&(dconv      ), sizeof (ptr->x     ), 1, fp)) return (-1);
    dig__double_convert (&ptr->y, &dconv, 1);
    fwrite (&(dconv      ), sizeof (ptr->y     ), 1, fp);
    dig__long_convert (&ptr->offset, &conv, 1);
    fwrite (&(conv       ), sizeof (ptr->offset), 1, fp);
    dig__int_convert (&ptr->cat, &conv, 1);
    fwrite (&(conv       ), sizeof (long       ), 1, fp);

    conv = ptr->index;
    dig__long_convert (&conv, &conv, 1);
    fwrite (&(conv       ), sizeof (long       ), 1, fp);

    fwrite (&(ptr->type  ), sizeof (ptr->type  ), 1, fp);
    return (0);
}

dig_Rd_Plus_head (ptr, fp)
    struct Plus_head *ptr;
    FILE *fp;
{
    long conv;

rewind (fp);
if(0 >= fread (&(conv            ), sizeof(long            ), 1, fp))return(-1);
    dig__long_convert (&conv, &conv, 1);
    ptr->Major = (int) conv;
if(0 >= fread (&(conv            ), sizeof(long            ), 1, fp))return(-1);
    dig__long_convert (&conv, &conv, 1);
    ptr->Minor = (int) conv;
if(0>=fread (&(conv), sizeof(long), 1, fp))return(-1);
    dig__long_convert (&conv, &conv, 1); ptr->n_nodes = conv;
if(0>=fread (&(conv), sizeof(long), 1, fp))return(-1);
    dig__long_convert (&conv, &conv, 1); ptr->n_lines = conv;
if(0>=fread (&(conv), sizeof(long), 1, fp))return(-1);
    dig__long_convert (&conv, &conv, 1); ptr->n_areas = conv;
if(0>=fread (&(conv), sizeof(long), 1, fp))return(-1);
    dig__long_convert (&conv, &conv, 1); ptr->n_atts = conv;
if(0>=fread (&(conv), sizeof(long), 1, fp))return(-1);
    dig__long_convert (&conv, &conv, 1); ptr->n_isles = conv;
if(0>=fread (&(conv), sizeof(long), 1, fp))return(-1);
    dig__long_convert (&conv, &conv, 1); ptr->n_llines = conv;
if(0>=fread (&(conv), sizeof(long), 1, fp))return(-1);
    dig__long_convert (&conv, &conv, 1); ptr->n_alines = conv;
if(0>=fread (&(conv), sizeof(long), 1, fp))return(-1);
    dig__long_convert (&conv, &conv, 1); ptr->n_plines = conv;

if(0>=fread (&(conv             ), sizeof(long             ), 1, fp))return(-1);
    dig__long_convert (&conv, &conv, 1); ptr->n_points = conv;
if(0>=fread (&(ptr->Node_offset ), sizeof(ptr->Node_offset ), 1, fp))return(-1);
    dig__long_convert (&ptr->Node_offset, &ptr->Node_offset, 1);
if(0>=fread (&(ptr->Line_offset ), sizeof(ptr->Line_offset ), 1, fp))return(-1);
    dig__long_convert (&ptr->Line_offset, &ptr->Line_offset, 1);
if(0>=fread (&(ptr->Area_offset ), sizeof(ptr->Area_offset ), 1, fp))return(-1);
    dig__long_convert (&ptr->Area_offset, &ptr->Area_offset, 1);
if(0>=fread (&(ptr->Att_offset  ), sizeof(ptr->Att_offset  ), 1, fp))return(-1);
    dig__long_convert (&ptr->Att_offset, &ptr->Att_offset, 1);
if(0>=fread (&(ptr->Isle_offset ), sizeof(ptr->Isle_offset ), 1, fp))return(-1);
    dig__long_convert (&ptr->Isle_offset, &ptr->Isle_offset, 1);
if(0>=fread (&(ptr->Dig_size    ), sizeof(ptr->Dig_size    ), 1, fp))return(-1);
    dig__long_convert (&ptr->Dig_size, &ptr->Dig_size, 1);
if(0>=fread (&(ptr->Att_size    ), sizeof(ptr->Att_size    ), 1, fp))return(-1);
    dig__long_convert (&ptr->Att_size, &ptr->Att_size, 1);
if(0>=fread (&(ptr->Dig_code    ), sizeof(ptr->Dig_code    ), 1, fp))return(-1);
    dig__long_convert (&ptr->Dig_code, &ptr->Dig_code, 1);
if(0>=fread (&(ptr->Att_code    ), sizeof(ptr->Att_code    ), 1, fp))return(-1);
    dig__long_convert (&ptr->Att_code, &ptr->Att_code, 1);
if(0>=fread (&(conv             ), sizeof(long             ), 1, fp))return(-1);
    dig__long_convert (&conv, &conv, 1); ptr->all_areas = conv;
if(0>=fread (&(conv             ), sizeof(long             ), 1, fp))return(-1);
    dig__long_convert (&conv, &conv, 1); ptr->all_isles = conv;
if(0>=fread (&(ptr->snap_thresh ), sizeof(ptr->snap_thresh ), 1, fp))return(-1);
    dig__double_convert (&ptr->snap_thresh, &ptr->snap_thresh, 1);
if(0>=fread (&(ptr->prune_thresh), sizeof(ptr->prune_thresh), 1, fp))return(-1);
    dig__double_convert (&ptr->prune_thresh, &ptr->prune_thresh, 1);
if(0>=fread (&(conv             ), sizeof(long             ), 1, fp))return(-1);
    dig__long_convert (&conv, &conv, 1); ptr->Back_Major = conv;
if(0>=fread (&(conv             ), sizeof(long             ), 1, fp))return(-1);
    dig__long_convert (&conv, &conv, 1); ptr->Back_Minor = conv;
if(0>=fread (&(ptr->future3     ), sizeof(ptr->future3     ), 1, fp))return(-1);
    dig__long_convert (&ptr->future3, &ptr->future3, 1);
if(0>=fread (&(ptr->future4     ), sizeof(ptr->future4     ), 1, fp))return(-1);
    dig__long_convert (&ptr->future4, &ptr->future4, 1);
if(0>=fread (&(ptr->F1          ), sizeof(ptr->F1          ), 1, fp))return(-1);
    dig__double_convert (&ptr->F1, &ptr->F1, 1);
if(0>=fread (&(ptr->F2          ), sizeof(ptr->F2          ), 1, fp))return(-1);
    dig__double_convert (&ptr->F2, &ptr->F2, 1);
if(0>=fread (&(ptr->F3          ), sizeof(ptr->F3          ), 1, fp))return(-1);
    dig__double_convert (&ptr->F3, &ptr->F3, 1);
if(0>=fread (&(ptr->F4          ), sizeof(ptr->F4          ), 1, fp))return(-1);
    dig__double_convert (&ptr->F4, &ptr->F4, 1);

if(0 >= fread (ptr->Dig_name, HEADSTR, 1, fp)) return (-1);
if(0 >= fread (ptr->filler,   HEADSTR, 1, fp)) return (-1);

/* check version numbers */
if(ptr->Major != VERSION_MAJOR || (ptr->Major==VERSION_MAJOR && ptr->Minor > VERSION_MAJOR + 5))
{
    if (VERSION_MAJOR < ptr->Back_Major || (VERSION_MAJOR == ptr->Back_Major &&
	VERSION_MINOR < ptr->Back_Minor))
    {
	fprintf (stderr, "Vector format version (%d.%d) is not known by this release.  EXITING\n",
	    ptr->Major, ptr->Minor);
	fprintf(stderr, "Try running support.vect to reformat the dig_plus file\n");
	exit (-1);
    }
}

return (0);
}
dig_Wr_Plus_head (ptr, fp)
    struct Plus_head *ptr;
    FILE *fp;
{
    long    conv;
    double dconv;
    
    /* is there a better place for this? */
    ptr->Major = VERSION_MAJOR;
    ptr->Minor = VERSION_MINOR;

    rewind (fp);
    conv = ptr->Major; dig__long_convert (&conv, &conv, 1);
    if (0 >= 
    fwrite (&(conv             ), sizeof (long             ), 1, fp))return(-1);
    conv = ptr->Minor; dig__long_convert (&conv, &conv, 1);
    fwrite (&(conv             ), sizeof (long             ), 1, fp);


    /* force to longs for future */
    conv = ptr->n_nodes; dig__long_convert  (&conv, &conv, 1);
    fwrite (&(conv ), sizeof (long), 1, fp);
    conv = ptr->n_lines; dig__long_convert  (&conv, &conv, 1);
    fwrite (&(conv ), sizeof (long), 1, fp);
    conv = ptr->n_areas; dig__long_convert  (&conv, &conv, 1);
    fwrite (&(conv ), sizeof (long), 1, fp);
    conv = ptr->n_atts; dig__long_convert  (&conv, &conv, 1);
    fwrite (&(conv ), sizeof (long), 1, fp);
    conv = ptr->n_isles; dig__long_convert  (&conv, &conv, 1);
    fwrite (&(conv ), sizeof (long), 1, fp);
    conv = ptr->n_llines; dig__long_convert  (&conv, &conv, 1);
    fwrite (&(conv ), sizeof (long), 1, fp);
    conv = ptr->n_alines; dig__long_convert  (&conv, &conv, 1);
    fwrite (&(conv ), sizeof (long), 1, fp);
    conv = ptr->n_plines; dig__long_convert  (&conv, &conv, 1);
    fwrite (&(conv ), sizeof (long), 1, fp);

	dig__int_convert (&ptr->n_points, &conv, 1);
    fwrite (&(conv             ), sizeof (long             ), 1, fp);
	dig__long_convert (&ptr->Node_offset, &conv, 1);
    fwrite (&(conv             ), sizeof (ptr->Node_offset ), 1, fp);
	dig__long_convert (&ptr->Line_offset, &conv, 1);
    fwrite (&(conv             ), sizeof (ptr->Line_offset ), 1, fp);
	dig__long_convert (&ptr->Area_offset, &conv, 1);
    fwrite (&(conv             ), sizeof (ptr->Area_offset ), 1, fp);
	dig__long_convert (&ptr->Att_offset, &conv, 1);
    fwrite (&(conv             ), sizeof (ptr->Att_offset  ), 1, fp);
	dig__long_convert (&ptr->Isle_offset, &conv, 1);
    fwrite (&(conv             ), sizeof (ptr->Isle_offset ), 1, fp);
	dig__long_convert (&ptr->Dig_size, &conv, 1);
    fwrite (&(conv             ), sizeof (ptr->Dig_size    ), 1, fp);
	dig__long_convert (&ptr->Att_size, &conv, 1);
    fwrite (&(conv             ), sizeof (ptr->Att_size    ), 1, fp);
	dig__long_convert (&ptr->Dig_code, &conv, 1);
    fwrite (&(conv             ), sizeof (ptr->Dig_code    ), 1, fp);
	dig__long_convert (&ptr->Att_code, &conv, 1);
    fwrite (&(conv             ), sizeof (ptr->Att_code    ), 1, fp);
	dig__int_convert (&ptr->all_areas, &conv, 1);
    fwrite (&(conv             ), sizeof (long             ), 1, fp);
	dig__int_convert (&ptr->all_areas, &conv, 1);
    fwrite (&(conv             ), sizeof (long             ), 1, fp);
	dig__double_convert (&ptr->snap_thresh, &dconv, 1);
    fwrite (&(dconv            ), sizeof (ptr->snap_thresh ), 1, fp);
	dig__double_convert (&ptr->prune_thresh, &dconv, 1);
    fwrite (&(dconv            ), sizeof (ptr->prune_thresh), 1, fp);
	ptr->Back_Major = EARLIEST_MAJOR;
	dig__long_convert (&ptr->Back_Major, &conv, 1);
    fwrite (&(conv             ), sizeof (long             ), 1, fp);
	ptr->Back_Minor = EARLIEST_MINOR;
	dig__long_convert (&ptr->Back_Minor, &conv, 1);
    fwrite (&(conv             ), sizeof (long             ), 1, fp);
	dig__long_convert (&ptr->future3, &conv, 1);
    fwrite (&(conv             ), sizeof (ptr->future3     ), 1, fp);
	dig__long_convert (&ptr->future4, &conv, 1);
    fwrite (&(conv             ), sizeof (ptr->future4     ), 1, fp);
	dig__double_convert (&ptr->F1, &dconv, 1);
    fwrite (&(dconv            ), sizeof (ptr->F1          ), 1, fp);
	dig__double_convert (&ptr->F2, &dconv, 1);
    fwrite (&(dconv            ), sizeof (ptr->F2          ), 1, fp);
	dig__double_convert (&ptr->F3, &dconv, 1);
    fwrite (&(dconv            ), sizeof (ptr->F3          ), 1, fp);
	dig__double_convert (&ptr->F4, &dconv, 1);
    fwrite (&(dconv            ), sizeof (ptr->F4          ), 1, fp);

    fwrite (ptr->Dig_name, HEADSTR, 1, fp);
    fwrite (ptr->filler, HEADSTR, 1, fp);
    return (0);
}
