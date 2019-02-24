/*======================================================================
                             ltm_coeffs.c

======================================================================*/

#include "ortho_image.h"

I_get_ltm_coefs_data (group) 
Rectify_Group *group;
{

   group->coefs = (Coeffs_Ltm *) G_malloc (sizeof (Coeffs_Ltm));
   get_ltm_coeffs_data(group);
}


get_ltm_coeffs_data(group)
Rectify_Group    *group;
{
Coeffs_Ltm  *coeffs;
char           msg[100];


    /* make auxilary visiable */
    coeffs = (Coeffs_Ltm *) group->coefs;

}

