/*!
  \file args.c
  
  \brief Parse command
  
  (C) 2008, 2010 by the GRASS Development Team
  
  This program is free software under the GNU General Public
  License (>=v2). Read the file COPYING that comes with GRASS
  for details.
  
  \author Martin Landa <landa.martin gmail.com> (Google SoC 2008/2010)
*/

#include <stdlib.h>
#include <string.h>

#include <grass/gis.h>
#include <grass/glocale.h>

#include "local_proto.h"

static void print_error(int, int, int,
			const char *, const char *,
			const char *, const char *);

static void args_surface(struct GParams *);
static void args_vline(struct GParams *);
static void args_vpoint(struct GParams *);
static void args_viewpoint(struct GParams *);
static void args_volume(struct GParams *);
static void args_lighting(struct GParams *);

/*!
  \brief Parse command
  
  \param argc number of arguments
  \param argv arguments array
  \param params GRASS parameters
  
  \return 1
*/
void parse_command(int argc, char *argv[], struct GParams *params)
{
    params->mode_all = G_define_flag();
    params->mode_all->key = 'a';
    params->mode_all->description =
	_("Use draw mode for all loaded surfaces");
    params->mode_all->guisection = _("Surfaces");
    
    /*** surface attributes ***/
    args_surface(params);

    /*** vector lines ***/
    args_vline(params);

    /*** vector points ***/
    args_vpoint(params);

    /*** volumes ***/
    args_volume(params);

    /*** misc ***/
    /* background color */
    params->bgcolor = G_define_standard_option(G_OPT_C_BG);

    /*** viewpoint ***/
    args_viewpoint(params);

    /*** lighting ***/
    args_lighting(params);
    
    /*** output image ***/
    /* output */
    params->output = G_define_standard_option(G_OPT_F_OUTPUT);
    params->output->description =
	_("Name for output image file (without extension)");
    params->output->guisection = _("Image");

    /* format */
    params->format = G_define_option();
    params->format->key = "format";
    params->format->type = TYPE_STRING;
#ifdef HAVE_TIFFIO_H
    params->format->options = "ppm,tif";	/* TODO: png */
#else
    params->format->options = "ppm";
#endif
    params->format->answer = "ppm";
    params->format->description = _("Graphics file format");
    params->format->required = YES;
    params->format->guisection = _("Image");

    /* size */
    params->size = G_define_option();
    params->size->key = "size";
    params->size->type = TYPE_INTEGER;
    params->size->key_desc = "width,height";
    params->size->answer = "640,480";
    params->size->description = _("Size (width, height) of output image");
    params->size->required = YES;
    params->size->guisection = _("Image");

    if (G_parser(argc, argv))
	exit(EXIT_FAILURE);

    return;
}

void args_surface(struct GParams *params)
{
    /* topography */
    params->elev_map = G_define_standard_option(G_OPT_R_ELEV);
    params->elev_map->key = "elevation_map";
    params->elev_map->required = NO;
    params->elev_map->multiple = YES;
    params->elev_map->description = _("Name of raster map(s) for elevation");
    params->elev_map->guisection = _("Surfaces");

    params->elev_const = G_define_option();
    params->elev_const->key = "elevation_value";
    params->elev_const->key_desc = "value";
    params->elev_const->type = TYPE_INTEGER;
    params->elev_const->required = NO;
    params->elev_const->multiple = YES;
    params->elev_const->description = _("Elevation value(s)");
    params->elev_const->guisection = _("Surfaces");

    /* color */
    params->color_map = G_define_standard_option(G_OPT_R_MAP);
    params->color_map->multiple = YES;
    params->color_map->required = NO;
    params->color_map->description = _("Name of raster map(s) for color");
    params->color_map->guisection = _("Surfaces");
    params->color_map->key = "color_map";

    params->color_const = G_define_standard_option(G_OPT_C_FG);
    params->color_const->multiple = YES;
    params->color_const->label = _("Color value(s)");
    params->color_const->guisection = _("Surfaces");

    /* mask */
    params->mask_map = G_define_standard_option(G_OPT_R_MAP);
    params->mask_map->multiple = YES;
    params->mask_map->required = NO;
    params->mask_map->description = _("Name of raster map(s) for mask");
    params->mask_map->guisection = _("Surfaces");
    params->mask_map->key = "mask_map";

    /* transparency */
    params->transp_map = G_define_standard_option(G_OPT_R_MAP);
    params->transp_map->multiple = YES;
    params->transp_map->required = NO;
    params->transp_map->description =
	_("Name of raster map(s) for transparency");
    params->transp_map->guisection = _("Surfaces");
    params->transp_map->key = "transparency_map";

    params->transp_const = G_define_option();
    params->transp_const->key = "transparency_value";
    params->transp_const->key_desc = "value";
    params->transp_const->type = TYPE_INTEGER;
    params->transp_const->required = NO;
    params->transp_const->multiple = YES;
    params->transp_const->description = _("Transparency value(s)");
    params->transp_const->guisection = _("Surfaces");
    params->transp_const->options = "0-255";

    /* shininess */
    params->shine_map = G_define_standard_option(G_OPT_R_MAP);
    params->shine_map->multiple = YES;
    params->shine_map->required = NO;
    params->shine_map->description = _("Name of raster map(s) for shininess");
    params->shine_map->guisection = _("Surfaces");
    params->shine_map->key = "shininess_map";

    params->shine_const = G_define_option();
    params->shine_const->key = "shininess_value";
    params->shine_const->key_desc = "value";
    params->shine_const->type = TYPE_INTEGER;
    params->shine_const->required = NO;
    params->shine_const->multiple = YES;
    params->shine_const->description = _("Shininess value(s)");
    params->shine_const->guisection = _("Surfaces");
    params->shine_const->options = "0-255";

    /* emission */
    params->emit_map = G_define_standard_option(G_OPT_R_MAP);
    params->emit_map->multiple = YES;
    params->emit_map->required = NO;
    params->emit_map->description = _("Name of raster map(s) for emission");
    params->emit_map->guisection = _("Surfaces");
    params->emit_map->key = "emission_map";

    params->emit_const = G_define_option();
    params->emit_const->key = "emission_value";
    params->emit_const->key_desc = "value";
    params->emit_const->type = TYPE_INTEGER;
    params->emit_const->required = NO;
    params->emit_const->multiple = YES;
    params->emit_const->description = _("Emission value(s)");
    params->emit_const->guisection = _("Surfaces");
    params->emit_const->options = "0-255";

    /*
       draw
     */
    /* mode */
    params->mode = G_define_option();
    params->mode->key = "mode";
    params->mode->key_desc = "string";
    params->mode->type = TYPE_STRING;
    params->mode->required = NO;
    params->mode->multiple = YES;
    params->mode->description = _("Draw mode");
    params->mode->options = "coarse,fine,both";
    params->mode->answer = "fine";
    params->mode->guisection = _("Draw");

    /* resolution fine */
    params->res_fine = G_define_option();
    params->res_fine->key = "resolution_fine";
    params->res_fine->key_desc = "value";
    params->res_fine->type = TYPE_INTEGER;
    params->res_fine->required = NO;
    params->res_fine->multiple = YES;
    params->res_fine->description = _("Fine resolution");
    params->res_fine->answer = "6";
    params->res_fine->guisection = _("Draw");

    /* resolution coarse */
    params->res_coarse = G_define_option();
    params->res_coarse->key = "resolution_coarse";
    params->res_coarse->key_desc = "value";
    params->res_coarse->type = TYPE_INTEGER;
    params->res_coarse->required = NO;
    params->res_coarse->multiple = YES;
    params->res_coarse->description = _("Coarse resolution");
    params->res_coarse->answer = "9";
    params->res_coarse->guisection = _("Draw");

    /* style */
    params->style = G_define_option();
    params->style->key = "style";
    params->style->key_desc = "string";
    params->style->type = TYPE_STRING;
    params->style->required = NO;
    params->style->multiple = YES;
    params->style->description = _("Draw style");
    params->style->options = "wire,surface";
    params->style->answer = "surface";
    params->style->guisection = _("Draw");

    /* shading */
    params->shade = G_define_option();
    params->shade->key = "shading";
    params->shade->key_desc = "string";
    params->shade->type = TYPE_STRING;
    params->shade->required = NO;
    params->shade->multiple = YES;
    params->shade->description = _("Shading");
    params->shade->options = "flat,gouraud";
    params->shade->answer = "gouraud";
    params->shade->guisection = _("Draw");

    /* wire color */
    params->wire_color = G_define_standard_option(G_OPT_C_FG);
    params->wire_color->multiple = YES;
    params->wire_color->required = NO;
    params->wire_color->label = _("Wire color");
    params->wire_color->key = "wire_color";
    params->wire_color->answer = "136:136:136";
    params->wire_color->guisection = _("Draw");

    /* shading */
    params->shade = G_define_option();
    params->shade->key = "shading";
    params->shade->key_desc = "string";
    params->shade->type = TYPE_STRING;
    params->shade->required = NO;
    params->shade->multiple = YES;
    params->shade->description = _("Shading");
    params->shade->options = "flat,gouraud";
    params->shade->answer = "gouraud";
    params->shade->guisection = _("Draw");

    /* position */
    params->surface_pos = G_define_option();
    params->surface_pos->key = "surface_position";
    params->surface_pos->key_desc = "x,y,z";
    params->surface_pos->type = TYPE_INTEGER;
    params->surface_pos->required = NO;
    params->surface_pos->multiple = YES;
    params->surface_pos->description = _("Position");
    params->surface_pos->guisection = _("Draw");
    params->surface_pos->answer = "0,0,0";
    
    return;
}

void args_vline(struct GParams *params)
{
    params->vlines = G_define_standard_option(G_OPT_V_MAP);
    params->vlines->multiple = YES;
    params->vlines->required = NO;
    params->vlines->description = _("Name of line vector overlay map(s)");
    params->vlines->guisection = _("Vector lines");
    params->vlines->key = "vline";

    /* line width */
    params->vline_width = G_define_option();
    params->vline_width->key = "vline_width";
    params->vline_width->key_desc = "value";
    params->vline_width->type = TYPE_INTEGER;
    params->vline_width->required = NO;
    params->vline_width->multiple = YES;
    params->vline_width->description = _("Vector line width");
    params->vline_width->guisection = _("Vector lines");
    params->vline_width->options = "1-100";
    params->vline_width->answer = "2";

    /* line color */
    params->vline_color = G_define_standard_option(G_OPT_C_FG);
    params->vline_color->multiple = YES;
    params->vline_color->required = NO;
    params->vline_color->label = _("Vector line color");
    params->vline_color->key = "vline_color";
    params->vline_color->answer = "blue";
    params->vline_color->guisection = _("Vector lines");

    /* line mode */
    params->vline_mode = G_define_option();
    params->vline_mode->key = "vline_mode";
    params->vline_mode->key_desc = "string";
    params->vline_mode->type = TYPE_STRING;
    params->vline_mode->required = NO;
    params->vline_mode->multiple = YES;
    params->vline_mode->description = _("Vector line display mode");
    params->vline_mode->options = "surface,flat";
    params->vline_mode->answer = "surface";
    params->vline_mode->guisection = _("Vector lines");

    /* line height */
    params->vline_height = G_define_option();
    params->vline_height->key = "vline_height";
    params->vline_height->key_desc = "value";
    params->vline_height->type = TYPE_INTEGER;
    params->vline_height->required = NO;
    params->vline_height->multiple = YES;
    params->vline_height->description = _("Vector line height");
    params->vline_height->guisection = _("Vector lines");
    params->vline_height->options = "0-1000";
    params->vline_height->answer = "0";

    /* position */
    params->vline_pos = G_define_option();
    params->vline_pos->key = "vline_position";
    params->vline_pos->key_desc = "x,y,z";
    params->vline_pos->type = TYPE_INTEGER;
    params->vline_pos->required = NO;
    params->vline_pos->multiple = YES;
    params->vline_pos->description = _("Position");
    params->vline_pos->guisection = _("Vector lines");
    params->vline_pos->answer = "0,0,0";

    return;
}

void args_vpoint(struct GParams *params)
{
    params->vpoints = G_define_standard_option(G_OPT_V_MAP);
    params->vpoints->multiple = YES;
    params->vpoints->required = NO;
    params->vpoints->description = _("Name of point vector overlay map(s)");
    params->vpoints->guisection = _("Vector points");
    params->vpoints->key = "vpoint";

    /* point width */
    params->vpoint_size = G_define_option();
    params->vpoint_size->key = "vpoint_size";
    params->vpoint_size->key_desc = "value";
    params->vpoint_size->type = TYPE_INTEGER;
    params->vpoint_size->required = NO;
    params->vpoint_size->multiple = YES;
    params->vpoint_size->description = _("Icon size");
    params->vpoint_size->guisection = _("Vector points");
    params->vpoint_size->options = "1-1000";
    params->vpoint_size->answer = "100";

    /* point width */
    params->vpoint_width = G_define_option();
    params->vpoint_width->key = "vpoint_width";
    params->vpoint_width->key_desc = "value";
    params->vpoint_width->type = TYPE_INTEGER;
    params->vpoint_width->required = NO;
    params->vpoint_width->multiple = YES;
    params->vpoint_width->description = _("Icon width");
    params->vpoint_width->guisection = _("Vector points");
    params->vpoint_width->options = "1-1000";
    params->vpoint_width->answer = "2";

    /* point color */
    params->vpoint_color = G_define_standard_option(G_OPT_C_FG);
    params->vpoint_color->multiple = YES;
    params->vpoint_color->required = NO;
    params->vpoint_color->label = _("Icon color");
    params->vpoint_color->key = "vpoint_color";
    params->vpoint_color->answer = "blue";
    params->vpoint_color->guisection = _("Vector points");

    /* point mode */
    params->vpoint_marker = G_define_option();
    params->vpoint_marker->key = "vpoint_marker";
    params->vpoint_marker->key_desc = "string";
    params->vpoint_marker->type = TYPE_STRING;
    params->vpoint_marker->required = NO;
    params->vpoint_marker->multiple = YES;
    params->vpoint_marker->description = _("Icon marker");
    params->vpoint_marker->options =
	"x,sphere,diamond,cube,box,gyro,aster,histogram";
    params->vpoint_marker->answer = "sphere";
    params->vpoint_marker->guisection = _("Vector points");

    /* position */
    params->vpoint_pos = G_define_option();
    params->vpoint_pos->key = "vpoint_position";
    params->vpoint_pos->key_desc = "x,y,z";
    params->vpoint_pos->type = TYPE_INTEGER;
    params->vpoint_pos->required = NO;
    params->vpoint_pos->multiple = YES;
    params->vpoint_pos->description = _("Position");
    params->vpoint_pos->guisection = _("Vector points");
    params->vpoint_pos->answer = "0,0,0";
    
    return;
}

void args_viewpoint(struct GParams *params)
{
    /* position */
    params->pos = G_define_option();
    params->pos->key = "position";
    params->pos->key_desc = "x,y";
    params->pos->type = TYPE_DOUBLE;
    params->pos->required = NO;
    params->pos->multiple = NO;
    params->pos->description =
	_("Viewpoint position (x,y model coordinates)");
    params->pos->guisection = _("Viewpoint");
    params->pos->answer = "0.84,0.16";

    /* height */
    params->height = G_define_option();
    params->height->key = "height";
    params->height->key_desc = "value";
    params->height->type = TYPE_INTEGER;
    params->height->required = NO;
    params->height->multiple = NO;
    params->height->description = _("Viewpoint height (in map units)");
    params->height->guisection = _("Viewpoint");

    /* perspective */
    params->persp = G_define_option();
    params->persp->key = "perspective";
    params->persp->key_desc = "value";
    params->persp->type = TYPE_INTEGER;
    params->persp->required = NO;
    params->persp->multiple = NO;
    params->persp->description = _("Viewpoint field of view (in degrees)");
    params->persp->guisection = _("Viewpoint");
    params->persp->answer = "40";
    params->persp->options = "1-100";

    /* twist */
    params->twist = G_define_option();
    params->twist->key = "twist";
    params->twist->key_desc = "value";
    params->twist->type = TYPE_INTEGER;
    params->twist->required = NO;
    params->twist->multiple = NO;
    params->twist->description = _("Viewpoint twist angle (in degrees)");
    params->twist->guisection = _("Viewpoint");
    params->twist->answer = "0";
    params->twist->options = "-180-180";

    /* z-exag */
    params->exag = G_define_option();
    params->exag->key = "zexag";
    params->exag->key_desc = "value";
    params->exag->type = TYPE_DOUBLE;
    params->exag->required = NO;
    params->exag->multiple = NO;
    params->exag->description = _("Vertical exaggeration");

    return;
}

void args_volume(struct GParams *params)
{
    params->volume = G_define_standard_option(G_OPT_R3_MAPS);
    params->volume->required = NO;
    params->volume->guisection = _("Volumes");
    params->volume->key = "volume";

    /* mode */
    params->volume_mode = G_define_option();
    params->volume_mode->key = "volume_mode";
    params->volume_mode->key_desc = "string";
    params->volume_mode->type = TYPE_STRING;
    params->volume_mode->required = NO;
    params->volume_mode->multiple = YES;
    params->volume_mode->description = _("Volume draw mode");
    params->volume_mode->options = "isosurface,slice";
    params->volume_mode->answer = "isosurface";
    params->volume_mode->guisection = _("Draw");

    /* shading */
    params->volume_shade = G_define_option();
    params->volume_shade->key = "volume_shading";
    params->volume_shade->key_desc = "string";
    params->volume_shade->type = TYPE_STRING;
    params->volume_shade->required = NO;
    params->volume_shade->multiple = YES;
    params->volume_shade->description = _("Volume shading");
    params->volume_shade->options = "flat,gouraud";
    params->volume_shade->answer = "gouraud";
    params->volume_shade->guisection = _("Draw");

    /* position */
    params->volume_pos = G_define_option();
    params->volume_pos->key = "volume_position";
    params->volume_pos->key_desc = "x,y,z";
    params->volume_pos->type = TYPE_INTEGER;
    params->volume_pos->required = NO;
    params->volume_pos->multiple = YES;
    params->volume_pos->description = _("Volume position");
    params->volume_pos->guisection = _("Volumes");
    params->volume_pos->answer = "0,0,0";
    
    /* resolution  */
    params->volume_res = G_define_option();
    params->volume_res->key = "volume_resolution";
    params->volume_res->key_desc = "value";
    params->volume_res->type = TYPE_INTEGER;
    params->volume_res->required = NO;
    params->volume_res->multiple = YES;
    params->volume_res->description = _("Volume resolution");
    params->volume_res->answer = "3";
    params->volume_res->guisection = _("Volumes");

    /* isosurface level */
    params->isosurf_level = G_define_option();
    params->isosurf_level->key = "isosurf_level";
    params->isosurf_level->key_desc = "volume:value";
    params->isosurf_level->type = TYPE_STRING;
    params->isosurf_level->required = NO;
    params->isosurf_level->multiple = YES;
    params->isosurf_level->description = _("Isosurface level");
    params->isosurf_level->guisection = _("Volumes");

    return;
}

void args_lighting(struct GParams *params)
{
    params->light_pos = G_define_option();
    params->light_pos->key = "light_position";
    params->light_pos->key_desc = "x,y,z";
    params->light_pos->type = TYPE_DOUBLE;
    params->light_pos->required = NO;
    params->light_pos->multiple = NO;
    params->light_pos->description =
	_("Light position (x,y,z model coordinates)");
    params->light_pos->guisection = _("Lighting");
    params->light_pos->answer = "0.68,-0.68,0.80";

    params->light_color = G_define_standard_option(G_OPT_C_FG);
    params->light_color->key = "light_color";
    params->light_color->label = _("Light color");
    params->light_color->guisection = _("Lighting");
    params->light_color->answer = "white";
    
    params->light_bright = G_define_option();
    params->light_bright->key = "light_brightness";
    params->light_bright->type = TYPE_INTEGER;
    params->light_bright->required = NO;
    params->light_bright->multiple = NO;
    params->light_bright->description =	_("Light brightness");
    params->light_bright->guisection = _("Lighting");
    params->light_bright->answer = "80";
    params->light_bright->options="0-100";

    params->light_ambient = G_define_option();
    params->light_ambient->key = "light_ambient";
    params->light_ambient->type = TYPE_INTEGER;
    params->light_ambient->required = NO;
    params->light_ambient->multiple = NO;
    params->light_ambient->description = _("Light ambient");
    params->light_ambient->guisection = _("Lighting");
    params->light_ambient->answer = "20";
    params->light_ambient->options="0-100";
}

/*!
   \brief Get number of answers of given option

   \param pointer to option

   \return number of arguments
 */
int opt_get_num_answers(const struct Option *opt)
{
    int i;

    i = 0;

    if (opt->answer) {
	while (opt->answers[i]) {
	    i++;
	}
    }

    return i;
}

/*!
  \brief Check parameters consistency
  
  \param params module parameters
 */
void check_parameters(const struct GParams *params)
{
    int nelev_map, nelev_const, nelevs;
    int nmaps, nconsts;

    int nvects;

    /* topography */
    nelev_map = opt_get_num_answers(params->elev_map);
    nelev_const = opt_get_num_answers(params->elev_const);
    nelevs = nelev_map + nelev_const;

    if (nelevs < 1)
	G_fatal_error(_("At least one <%s> or <%s> required"),
		      params->elev_map->key, params->elev_const->key);

    /* color */
    nmaps = opt_get_num_answers(params->color_map);
    nconsts = opt_get_num_answers(params->color_const);

    print_error(nmaps, nconsts, nelevs,
		params->elev_map->key, params->elev_const->key,
		params->color_map->key, params->color_const->key);

    /* mask */
    nmaps = opt_get_num_answers(params->mask_map);
    if (nmaps > 0 && nelevs != nmaps)
	G_fatal_error(_("Inconsistent number of attributes (<%s/%s> %d: <%s> %d)"),
		      params->elev_map->key, params->elev_const->key, nelevs,
		      params->mask_map->key, nmaps);


    /* transparency */
    nmaps = opt_get_num_answers(params->transp_map);
    nconsts = opt_get_num_answers(params->transp_const);
    print_error(nmaps, nconsts, nelevs,
		params->elev_map->key, params->elev_const->key,
		params->transp_map->key, params->transp_const->key);

    /* shininess */
    nmaps = opt_get_num_answers(params->shine_map);
    nconsts = opt_get_num_answers(params->shine_const);
    print_error(nmaps, nconsts, nelevs,
		params->elev_map->key, params->elev_const->key,
		params->shine_map->key, params->shine_const->key);

    /* emit */
    nmaps = opt_get_num_answers(params->emit_map);
    nconsts = opt_get_num_answers(params->emit_const);
    print_error(nmaps, nconsts, nelevs,
		params->elev_map->key, params->elev_const->key,
		params->emit_map->key, params->emit_const->key);

    /* draw mode */
    if (!params->mode_all->answer) {	/* use one mode for all surfaces */
	nconsts = opt_get_num_answers(params->mode);
	if (nconsts > 0 && nconsts != nelevs)
	    G_fatal_error(_("Inconsistent number of attributes (<%s/%s> %d: <%s> %d)"),
			  params->elev_map->key, params->elev_const->key,
			  nelevs, params->mode->key, nconsts);

	nconsts = opt_get_num_answers(params->res_fine);
	if (nconsts > 0 && nconsts != nelevs)
	    G_fatal_error(_("Inconsistent number of attributes (<%s/%s> %d: <%s> %d"),
			  params->elev_map->key, params->elev_const->key,
			  nelevs, params->res_fine->key, nconsts);

	nconsts = opt_get_num_answers(params->res_coarse);
	if (nconsts > 0 && nconsts != nelevs)
	    G_fatal_error(_("Inconsistent number of attributes (<%s/%s> %d: <%s> %d)"),
			  params->elev_map->key, params->elev_const->key,
			  nelevs, params->res_coarse->key, nconsts);

	nconsts = opt_get_num_answers(params->style);
	if (nconsts > 0 && nconsts != nelevs)
	    G_fatal_error(_("Inconsistent number of attributes (<%s/%s> %d: <%s> %d)"),
			  params->elev_map->key, params->elev_const->key,
			  nelevs, params->style->key, nconsts);

	nconsts = opt_get_num_answers(params->shade);
	if (nconsts > 0 && nconsts != nelevs)
	    G_fatal_error(_("Inconsistent number of attributes (<%s/%s> %d: <%s> %d)"),
			  params->elev_map->key, params->elev_const->key,
			  nelevs, params->shade->key, nconsts);

	nconsts = opt_get_num_answers(params->wire_color);
	if (nconsts > 0 && nconsts != nelevs)
	    G_fatal_error(_("Inconsistent number of attributes (<%s/%s> %d: <%s> %d)"),
			  params->elev_map->key, params->elev_const->key,
			  nelevs, params->wire_color->key, nconsts);
    }

    /*
     * vector
     */
    nvects = opt_get_num_answers(params->vlines);

    /* width */
    nconsts = opt_get_num_answers(params->vline_width);
    if (nvects > 0 && nconsts != nvects)
	G_fatal_error(_("Inconsistent number of attributes (<%s> %d: <%s> %d)"),
		      params->vlines->key, nvects, params->vline_width->key,
		      nconsts);

    /* color */
    nconsts = opt_get_num_answers(params->vline_color);
    if (nvects > 0 && nconsts != nvects)
	G_fatal_error(_("Inconsistent number of attributes (<%s> %d: <%s> %d"),
		      params->vlines->key, nvects, params->vline_color->key,
		      nconsts);

    /* mode */
    nconsts = opt_get_num_answers(params->vline_mode);
    if (nvects > 0 && nconsts != nvects)
	G_fatal_error(_("Inconsistent number of attributes (<%s> %d: <%s> %d)"),
		      params->vlines->key, nvects, params->vline_mode->key,
		      nconsts);

    /* height */
    nconsts = opt_get_num_answers(params->vline_height);
    if (nvects > 0 && nconsts != nvects)
	G_fatal_error(_("Inconsistent number of attributes (<%s> %d: <%s> %d)"),
		      params->vlines->key, nvects, params->vline_height->key,
		      nconsts);

    return;
}

void print_error(int nmaps, int nconsts, int nelevs,
		 const char *elev_map, const char *elev_const,
		 const char *map_name, const char *const_name)
{
    if ((nmaps > 0 && nelevs != nmaps) || (nconsts > 0 && nelevs != nconsts))
	G_fatal_error(_("Inconsistent number of attributes (<%s/%s> %d: <%s> %d, <%s> %d"),
		      elev_map, elev_const, nelevs, map_name, nmaps,
		      const_name, nconsts);

    return;
}
