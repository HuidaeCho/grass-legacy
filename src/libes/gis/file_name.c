/**********************************************************************
 *  char *
 *  G__file_name (path, element, name, maps)
 *      char path[]       buffer to hold resultant full path to file.
 *      char *element     database element (eg, "cell", "cellhd", etc)
 *      char *name        name of file to build path to
 *      char *maps        mapset name
 *   
 *      builds full path names to GIS data files
 *
 *  returns:
 *         pointer to 'path'
 *
 *   note:
 *      if name is of the form nnn in ppp then path is set
 *      as if name had been nnn and mapset had been ppp
 *      (actual value of mapset is ignored)
 *********************************************************************/

#include "gis.h"

char *
G__file_name (path, element, name, mapset)

	char *path;
	char *element;
	char *name;
	char *mapset;
{
	char cpx_name[256];
	char cpx_maps[256];

/*
 * if a name is given, build a file name
 * must split the name into name, mapset if it is
 * in the name-in-mapset format
 */
	if (name && *name)
		if (G__name_in_mapset(name, cpx_name, cpx_maps))
		{
			name = cpx_name;
			mapset = cpx_maps;
		}

	sprintf(path,"%s/%s", G__location_path(), mapset);

	if (element && *element)
	{
		strcat (path, "/");
		strcat (path, element);
	}

	if (name && *name)
	{
		strcat (path, "/");
		strcat (path, name);
	}

/*
 * return pointer to users 'path' buffer
 */
	return path;
}
