#include "dxf2vect.h"

dxf_entities (dxf_file)
FILE	*dxf_file;
{
	fgets (dxf_line, 80, dxf_file); 
	while (strcmp (dxf_line, entitie) != 0) 
	{ 
		fgets (dxf_line, 80, dxf_file); 
		if (feof (dxf_file)) 
		{ 
			fprintf (stderr, "end of file while looking"); 
			fprintf (stderr, " for ENTITIES\n"); 
			return (-1); 
		} 
	} 
	return (0);
}
