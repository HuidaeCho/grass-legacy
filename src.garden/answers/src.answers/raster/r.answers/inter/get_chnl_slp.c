/* %W% %G% */
/* 
    +---------------------------------------------------------+
    |            ANSWERS on GRASS Integration Project         |
    |  Developed in the Agriculture Engineering Department    |
    |                at Purdue University                     |
    |                        by                               |
    |           Chris Rewerts and Bernard Engel               |
    |                                                         |
    |   (c)Copyright, 1992 Purdue Research Foundation, West   |
    |   Lafayette, Indiana 47907. Permission to use, copy,    |
    |   modify, and distribute this software and its          |
    |   documentation for any purpose and without fee is      |
    |   hereby granted, provided that the above copyright     |
    |   notice appear in all copies.  This software is        |
    |   provided "as is" without express or implied warranty. |
    +---------------------------------------------------------+

   function: get_chnl_slp
   called by menu
   
   confirms a cell layer as a source for channel slope data 
   in watershed.

   since this is option input to answers, there is a chance that
   no chnl slp will be used. (answers will just use the element's 
   slope for in this case). 
*/

#include "answers.h"

get_chnl_slp()
{
    int 	i, j, check;
    int		option;
    char        line[200];

    G_clear_screen();
    fprintf (stdout,"\n\n          ANSWERS on GRASS Channel Slope Utility \n\n");
    fprintf (stdout,"An optional input to ANSWERS is the slope of channels. If a channel\n");
    fprintf (stdout,"slope input is not given, ANSWERS assumes the slope for the channel\n");
    fprintf (stdout,"is the same as the overland slope for the element.\n");
    fprintf (stdout,"\n");
    fprintf (stdout,"If desired, a raster map may be used to define channel slope values.\n");
    fprintf (stdout,"To do so, a raster map should be prepared with catgory values for\n");
    fprintf (stdout,"channel slopes in tenths of a percent (i.e. a category value of 31 \n");
    fprintf (stdout,"would indicate a channel slope of 3.1 percent).\n");
    fprintf (stdout,"\n");

    if(complete[8] == 0)
    {
        fprintf (stdout,"Before this step can be completed, the channel element\n");
        fprintf (stdout,"locations must be identified. (Step 8)\n");
        fprintf (stdout,"\n\n");
        hit_return();
        return(0);
    }

    option = 0;
  /* check to see if read_project found any previously recorded info
     in the project file about the channel layer. If so, make sure the
     user wants it. otherwise, prompt for a new one  */
    
    check = 0;
    if ((complete[9] > 0) && (chnl_slp_layer)) {
        check = 1;
        fprintf (stdout,"This utility was previously completed. You can:\n\n");
        fprintf (stdout,"- Use a different map layer \n");
        fprintf (stdout,"- Use no layer (hence no channel slope inputs)\n");
        fprintf (stdout,"- Leave things as they are currently set.\n\n");
        
        if(strcmp(chnl_slp_layer, "none") == 0)
        {
            if(!G_yes("Use channel slope data?", 0))
            {
                fprintf (stdout,"\n<No channel slope input>\n\n");
                option = 1;
            }
            else
                check = 0;
        }
        else
        {
            sprintf(line, "Use the raster map %s in %s for CHANNEL SLOPE?",
            chnl_slp_layer, chnl_slp_mapset);
            if(!G_yes(line, 1)) 
                check = 0;
         }
     }

    if (check == 0)
    {
        if(!G_yes("Do you wish to input CHANNEL SLOPES?", 1))
        {
            strcpy(chnl_slp_layer, "none");
            option = 1;
            fprintf (stdout,"\n<No channel slope input>\n\n");
            complete[9] = 1;
        }
        else
        {
            fprintf (stdout,"\n\nEnter the name of the CHANNEL SLOPE raster map.\n");
            chnl_slp_mapset = G_ask_cell_old ("",chnl_slp_layer);
            if(!chnl_slp_mapset) 
            {
                complete[9] = 0;
                return(0);
            }
            fprintf (stdout,"\n");
            if(mk_cat_tbl(0, chnl_slp_layer, chnl_slp_mapset) == -1)
            {
                complete[9] = 0;
                return(0);
            }
            fprintf (stdout,"\nNumber of channel categories found: %d \n", cat_tbl[0].cat);
            for (i = 1, j = 0; i <= cat_tbl[0].cat; i++, j++)
            {
                if (j > 9) 
                {
                    j = 0;
                    hit_return();
                }
            fprintf (stdout," %3ld %s\n", cat_tbl[i].cat, cat_tbl[i].label);
            }
        }
    }
    
    fprintf (stdout,"\n");

/* if this step was previously completed and the user currently tells
   us that no chnl slope input is to be used, we will make sure that
   there is no input file hanging around in the project database from
   a previous run */

    if ((option == 1) && (complete[9] > 1))
    {
        sprintf(line, "answers/data/%s", proj_name);
        G_remove( line, "in_chnl_slp");
        complete[9] = 1;
    }
    if(option == 0)
    {
        if(extract_chnl_slp())
        {
            complete[9] = 0;
            croak(1, "Failure to create ANSWERS input data");
        }
        else
        {
            fprintf (stdout,"\n\nANSWERS input data creation complete.\n\n");
            complete[9] = 1;
        }
    }
    hit_return();
    return (0);
}
