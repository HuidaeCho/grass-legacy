
/****************************************************************************
*
* MODULE:       r3.out.vtk  
*   	    	
* AUTHOR(S):    Original author 
*               Soeren Gebbert soerengebbert at gmx de
* 		27 Feb 2006 Berlin
* PURPOSE:      Converts 3D raster maps (G3D) into the VTK-Ascii format  
*
* COPYRIGHT:    (C) 2005 by the GRASS Development Team
*
*               This program is free software under the GNU General Public
*   	    	License (>=v2). Read the file COPYING that comes with GRASS
*   	    	for details.
*
*****************************************************************************/

#ifndef __R3_OUT_VTK_WRITE_DATA_H__
#define __R3_OUT_VTK_WRITE_DATA_H__

struct inputMaps;

/*Write the point coordinates of type point (1) or celldata (0) */
void writeVTKPoints(inputMaps * in, FILE * fp, G3D_Region region, int dp,
		    int type);
/*Write the uGrid Cells */
void writeVTKUnstructuredGridCells(FILE * fp, G3D_Region region);
/*Write the outputdata */
void writeVTKData(FILE * fp, void *map, G3D_Region region, char *varname,
		  int dp);
/*Write the rgb voxel data to the output */
void writeVTKRGBVoxelData(void *map_r, void *map_g, void *map_b, FILE * fp,
			  const char *string, G3D_Region region, int dp);
/*Write the vector data to the output */
void writeVTKVectorData(void *map_x, void *map_y, void *map_z, FILE * fp,
			const char *string, G3D_Region region, int dp);

#endif
