
#include "dxf2vect.h"
#include "gis.h"


dxf_close_layer (o_count)
    int o_count;
{
    int found_flag = 0; /* Reinitilized each time */
    int count;


    /* LOOKING FOR A LOCATION IN CLOSED_LAYERS[] THAT IS AVAILABLE */
    for (count = 0; count < num_closed_layers; count++)
    {
	if (closed_layers[count].status < 0)/*if available set to -2 in reopen*/
	{
	    found_flag = 1;
	    break;
	}
    }
    if (!num_closed_layers) /* this only happens the first time */
    {
	closed_layers = (DXF_DIG *) G_malloc (sizeof (DXF_DIG));
	num_closed_layers++;
	found_flag = 1;
    }

    if (!found_flag)
    {
	closed_layers = (DXF_DIG *) G_realloc (closed_layers,
		        (num_closed_layers + 1) * sizeof (DXF_DIG));
	count = num_closed_layers;
	num_closed_layers++;
    }

    /* PLACE INFO FROM LAYERS STRUCT INTO CLOSED_LAYERS STRUCT */

    closed_layers[count].name = G_store(layers[o_count].name);

    closed_layers[count].type = layers[o_count].type;

    closed_layers[count].status = layers[o_count].status;

    fclose(layers[o_count].fd);
}
