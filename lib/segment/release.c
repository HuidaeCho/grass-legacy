
/**
 * \file release.c
 *
 * \brief Segment release routines.
 *
 * This program is free software under the GNU General Public License
 * (>=v2). Read the file COPYING that comes with GRASS for details.
 *
 * \author GRASS GIS Development Team
 *
 * \date 2005-2006
 */

#include <stdlib.h>
#include <grass/segment.h>


/**
 * \fn int segment_release (SEGMENT *SEG)
 *
 * \brief Free memory allocated to segment.
 *
 * Releases the allocated memory associated with the segment file 
 * <b>seg</b>.
 *
 * <b>Note:</b> Does not close the file. Does not flush the data which 
 * may be pending from previous <i>segment_put()</i> calls.
 *
 * \param[in,out] seg
 * \return 1 if successful
 * \return -1 if segment is not available (not open)
 */

int segment_release(SEGMENT * SEG)
{
    int i;

    if (SEG->open != 1)
	return -1;

    for (i = 0; i < SEG->nseg; i++)
	free(SEG->scb[i].buf);
    free(SEG->scb);

    SEG->open = 0;

    return 1;
}
