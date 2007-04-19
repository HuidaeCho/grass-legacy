/****************************************************************************
 *
 * MODULE:       i.target
 * AUTHOR(S):    Michael Shapiro (USACERL) and Bob Covill (original contributors)
 *               Markus Neteler <neteler itc.it>, 
 *               Bernhard Reiter <bernhard intevation.de>, 
 *               Brad Douglas <rez touchofmadness.com>, 
 *               Glynn Clements <glynn gclements.plus.com>
 * PURPOSE:      targets an imagery group to a GRASS data base location name 
 *               and mapset for reprojection
 * COPYRIGHT:    (C) 2001-2007 by the GRASS Development Team
 *
 *               This program is free software under the GNU General Public
 *               License (>=v2). Read the file COPYING that comes with GRASS
 *               for details.
 *
 *****************************************************************************/
/* 
 * Original INTER author: M. Shapiro
 * New cmd line version by Bob Covill 10/2001
 * Rewritten for GRASS6 by Brad Douglas 08/2005
 */
 
#include <stdlib.h>
#include <string.h>
#include <grass/gis.h>
#include <grass/glocale.h>
#include <grass/imagery.h>


int 
main (int argc, char *argv[])
{
    struct Option *group, *mapset, *loc;
    struct GModule *module;
    struct Flag *c;

    G_gisinit (argv[0]);

    module = G_define_module();
    module->keywords = _("imagery");
    module->description =
        	_("Targets an imagery group to a GRASS location and mapset.");
        	
    group = G_define_standard_option (G_OPT_I_GROUP);

    loc = G_define_option();
    loc->key             = "location";
    loc->type            =  TYPE_STRING;
    loc->required        =  NO;
    loc->description     = _("Name of imagery target location");

    mapset = G_define_option();
    mapset->key          = "mapset";
    mapset->type         =  TYPE_STRING;
    mapset->required     =  NO;
    mapset->description  = _("Name of target mapset");

    c = G_define_flag();
    c->key              = 'c';
    c->description      = _("Set current location and mapset as target for of imagery group");

    if (G_parser(argc, argv))
        exit(EXIT_FAILURE);

    /* error if -c is specified with other options */
    if ((!c->answer) && (!mapset->answer || !loc->answer))
	G_fatal_error(_("You must use either the Current Mapset and "
             "Location Flag (-c)\n OR\n manually enter the variables."));

    /* error if -c is not specified and not enough other options */
    if ((c->answer) && (mapset->answer || loc->answer))
	G_fatal_error(_("You must use either the Current Mapset and "
             "Location Flag (-c)\n OR\n manually enter the variables."));

    if (c->answer) {
        /* point group target to current mapset and location */
        I_put_target(group->answer, G_location(), G_mapset());
        G_message(_("group [%s] targeted for location [%s], mapset [%s]\n"),
                group->answer, G_location(), G_mapset());
    } else {
        /* point group target to specified mapset and location */
        I_put_target(group->answer, loc->answer, mapset->answer);
        G_message(_("group [%s] targeted for location [%s], mapset [%s]\n"),
                group->answer, loc->answer, mapset->answer);
    }

    return (EXIT_SUCCESS);
}



