#include "gis.h"
/*
 * patch in non-zero data over zero data
 * keep track of the categories which are patched in
 * for later use in constructing the new category and color files
 *
 * returns: 1 the result still contains nulls
 *          0 the result contains no zero nulls
 */

int do_patch (
    void *result,void *patch,
    struct Cell_stats *statf,int ncols,
    RASTER_MAP_TYPE out_type)
{
    int more;

    more = 0;
    while (ncols-- > 0)
    {
	if (G_is_null_value(result, out_type))
	{
	    if(G_is_null_value(patch, out_type))
		more = 1;
            else
	    {
	        G_raster_cpy(result, patch, 1, out_type);
		if(out_type==CELL_TYPE)
	            G_update_cell_stats ((CELL *) result, 1, statf);
	    }
            
	}
	result = G_incr_void_ptr(result, G_raster_size(out_type));
	patch = G_incr_void_ptr(patch, G_raster_size(out_type));
    }
    return more;
}
