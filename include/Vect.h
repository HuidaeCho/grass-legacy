#ifndef GRASS_VECT_H
#define GRASS_VECT_H
#include "gis.h"
#include "vect/digit.h"

/* --- ANSI prototypes for the lib/vector/Vlib functions --- */
/* "Public" functions, for use in modules */
    /* Basic structures */
      /* Points (line) */  
struct line_pnts *Vect_new_line_struct (void);
int Vect_append_point (struct line_pnts *, double, double, double);
int Vect_append_points (struct line_pnts *, struct line_pnts *, int direction);
int Vect_copy_xyz_to_pnts (struct line_pnts *, double *, double *, double *, int);
int Vect_copy_pnts_to_xyz (struct line_pnts *, double *, double *, double *, int *);
int Vect_reset_line (struct line_pnts *);
int Vect_destroy_line_struct (struct line_pnts *);
int Vect_point_on_line ( struct line_pnts *, double, double *, double *, double *, double *, double *);
int Vect_line_segment ( struct line_pnts *InPoints, double start, double end, struct line_pnts *OutPoints );
double Vect_line_length ( struct line_pnts *);
double Vect_line_geodesic_length ( struct line_pnts *);
int Vect_line_distance ( struct line_pnts *, double, double, double, int, 
	                 double *, double *, double *, double *, double *, double *);
int Vect_line_box ( struct line_pnts *, BOUND_BOX * );

      /* Categories */
struct line_cats *Vect_new_cats_struct (void);
int Vect_cat_set (struct line_cats *, int, int);
int Vect_cat_get (struct line_cats *, int, int *);
int Vect_cat_del (struct line_cats *, int);
int Vect_cat_in_array (int, int *, int);
int Vect_reset_cats (struct line_cats *);
int Vect_destroy_cats_struct (struct line_cats *);

      /* List of categories */
struct cat_list *Vect_new_cat_list (void);
int Vect_str_to_cat_list (char *, struct cat_list *);
int Vect_array_to_cat_list (int *, int, struct cat_list *);
int Vect_cat_in_cat_list (int, struct cat_list *);
int Vect_destroy_cat_list (struct cat_list *);

     /* DB connection - field info */
struct dblinks *Vect_new_dblinks_struct ( void );
void Vect_reset_dblinks ( struct dblinks *p );
int Vect_add_dblink ( struct dblinks *p, int number, char *name, char *table, char *key, 
	              char *db, char *driver );
int Vect_map_add_dblink ( struct Map_info *, int number, char *name, char *table, char *key, 
	              char *db, char *driver );
struct field_info *Vect_get_field_info (char *, char *, int);
int Vect_read_dblinks ( char *, char *,  struct dblinks *p );
int Vect_write_dblinks ( char *, char *,  struct dblinks *p );
struct field_info *Vect_default_field_info ( char *map, int  field, char *field_name, int  type );
struct field_info *Vect_get_dblink (  struct Map_info *Map, int link );

      /* List of FID (feature ID) (integers) */
struct ilist *Vect_new_list (void);
int Vect_list_append ( struct ilist *, int);
int Vect_list_append_list ( struct ilist *, struct ilist *);
int Vect_list_delete ( struct ilist *, int);
int Vect_list_delete_list ( struct ilist *, struct ilist *);
int Vect_val_in_list ( struct ilist *, int);
int Vect_reset_list (struct ilist *);
int Vect_destroy_list (struct ilist *);

      /* Bounding box (MBR) */
int Vect_point_in_box (double, double, double, BOUND_BOX *);
int Vect_box_overlap (BOUND_BOX *, BOUND_BOX *);
int Vect_box_copy (BOUND_BOX *, BOUND_BOX *);
int Vect_box_extend (BOUND_BOX *, BOUND_BOX *);
int Vect_region_box ( struct Cell_head *, BOUND_BOX * );

    /* Set/get Map header info */
char *Vect_get_name (struct Map_info *);
char *Vect_get_mapset (struct Map_info *);
char *Vect_get_full_name (struct Map_info *);
int  Vect_is_3d (struct Map_info *);
int  Vect_set_organization (struct Map_info *, char *);
char *Vect_get_organization (struct Map_info *);
int  Vect_set_date (struct Map_info *, char *);
char *Vect_get_date (struct Map_info *);
int  Vect_set_person (struct Map_info *, char *);
char *Vect_get_person (struct Map_info *);
int  Vect_set_map_name (struct Map_info *, char *);
char *Vect_get_map_name (struct Map_info *);
int  Vect_set_map_date (struct Map_info *, char *);
char *Vect_get_map_date (struct Map_info *);
int  Vect_set_comment (struct Map_info *, char *);
char *Vect_get_comment (struct Map_info *);
int  Vect_set_scale (struct Map_info *, int );
int  Vect_get_scale (struct Map_info *);
int  Vect_set_zone (struct Map_info *, int );
int  Vect_get_zone (struct Map_info *);
int  Vect_set_thresh (struct Map_info *, double );
double Vect_get_thresh (struct Map_info *);
int Vect_get_constraint_box ( struct Map_info *, BOUND_BOX *);

    /* Get map level 2 informations */
int Vect_level (struct Map_info *);
int Vect_get_num_nodes (struct Map_info *);
int Vect_get_num_centroids (struct Map_info *);
int Vect_get_num_lines (struct Map_info *);
int Vect_get_num_areas (struct Map_info *);
int Vect_get_num_islands (struct Map_info *);
int Vect_get_line_box (struct Map_info *, int, BOUND_BOX *);
int Vect_get_area_box (struct Map_info *, int, BOUND_BOX *);
int Vect_get_isle_box (struct Map_info *, int, BOUND_BOX *);
int Vect_get_map_box (struct Map_info *, BOUND_BOX *);
int V__map_overlap (struct Map_info *, double, double, double, double);

    /* Set/get fatal error behaviour */
int Vect_set_fatal_error (int);
int Vect_get_fatal_error ();

    /* Open/close/rewind/set_constraints for map */
int Vect_set_open_level (int);
int Vect_open_old (struct Map_info *, char *, char *);
int Vect_open_new (struct Map_info *, char *, int);
int Vect_open_update (struct Map_info *, char *, char *);
int Vect_copy_head_data (struct Map_info *, struct  Map_info *);
int Vect_build ( struct Map_info *, FILE *);
int Vect_set_constraint_region (struct Map_info *, double, double, double, double, double, double);
int Vect_set_constraint_type (struct Map_info *, int);
int Vect_remove_constraints (struct Map_info *);
int Vect_rewind (struct Map_info *);
int Vect_close (struct Map_info *);

    /* Read/write lines, nodes, areas */
      /* Level 1 and 2 */
int Vect_read_next_line (struct Map_info *, struct line_pnts *, struct line_cats *);
long Vect_write_line (struct Map_info *, int type, struct line_pnts *, struct line_cats *);

int Vect_get_num_dblinks (struct Map_info *map);
    
      /* Level 2 only */
int Vect_read_line (struct Map_info *, struct line_pnts *, struct line_cats *, int);
int Vect_rewrite_line (struct Map_info *, int, int, struct line_pnts *, struct line_cats *);
int Vect_delete_line (struct Map_info *, int);

int Vect_line_alive ( struct Map_info *, int);
int Vect_node_alive ( struct Map_info *, int);
int Vect_area_alive ( struct Map_info *, int);
int Vect_isle_alive ( struct Map_info *, int);
int Vect_get_line_nodes (struct Map_info *, int, int *, int *);
int Vect_get_line_areas (struct Map_info *, int, int *, int *);

int Vect_get_node_coor (struct Map_info *, int, double *, double *, double *);
int Vect_get_node_n_lines (struct Map_info *, int );
int Vect_get_node_line (struct Map_info *, int, int );

int Vect_get_area_points (struct Map_info *, int, struct line_pnts *);
int Vect_get_area_centroid ( struct Map_info *, int );
int Vect_get_area_num_isles ( struct Map_info *, int );
int Vect_get_area_isle ( struct Map_info *, int, int );
double Vect_get_area_area ( struct Map_info *, int );
int Vect_get_area_boundaries ( struct Map_info *, int, struct ilist * );

int Vect_get_isle_points (struct Map_info *, int, struct line_pnts *);

int Vect_get_centroid_area ( struct Map_info *, int );

      /* Level 2 update only */
int Vect_get_num_updated_lines (struct Map_info *map);
int Vect_get_updated_line (struct Map_info *map, int idx);
int Vect_get_num_updated_nodes (struct Map_info *map);
int Vect_get_updated_node (struct Map_info *map, int idx);

      /* Selecting features */
int Vect_select_lines_by_box (struct Map_info *, BOUND_BOX *, int, struct ilist *);
int Vect_select_areas_by_box (struct Map_info *, BOUND_BOX *, struct ilist *);
int Vect_select_isles_by_box (struct Map_info *, BOUND_BOX *, struct ilist *);
int Vect_select_nodes_by_box (struct Map_info *, BOUND_BOX *, struct ilist *);
int Vect_find_node (struct Map_info *, double, double, double, double, int);
int Vect_find_line (struct Map_info *, double, double, double, int, double, int, int);
int Vect_find_area (struct Map_info *, double, double);

      /* Analysis */
int Vect_point_in_area (struct Map_info *, int, double, double);
int Vect_tin_get_z (struct Map_info *, double, double, double *, double *, double *);
int Vect_get_point_in_area (struct Map_info *, int, double *, double *);
int Vect_point_in_islands (struct Map_info *, int, double, double);
int Vect_find_poly_centroid (struct line_pnts *, double *, double *);
int Vect_get_point_in_poly_isl (struct line_pnts *, struct line_pnts **, int, double *, double *);
int Vect__intersect_line_with_poly (struct line_pnts *, double, struct line_pnts *);
int Vect_get_point_in_poly (struct line_pnts *, double *, double *);


      /* Overlay */
int Vect_overlay_str_to_operator ( char * );
int Vect_overlay ( struct Map_info *, int, struct ilist *, struct ilist *, 
	           struct Map_info *, int, struct ilist *, struct ilist *,
                   int, struct Map_info *);

    /* Network (graph) */
int Vect_net_build_graph ( struct Map_info *, int, int, int, char *, char *, char *, int, int);
int Vect_net_shortest_path ( struct Map_info *, int, int, struct ilist *, double *);

    /* Miscellaneous */
int Vect_topo_dump ( struct Plus_head *, FILE *);
double Vect_points_distance ( double, double, double, double, double, double, int);
int Vect_option_to_types (struct Option *);
int Vect_copy_map_lines ( struct Map_info *, struct Map_info * );
int Vect_segment_intersection ( double, double, double, double, double, double, 
	                        double, double, double, double, double, double,    
				double *, double *, double *, double *, double *, double *,  
				int);
int Vect_line_intersection ( struct line_pnts *, struct line_pnts *,
                             struct line_pnts ***, struct line_pnts ***,
                             int *, int *, int );
char *Vect_subst_var ( char *str, char *map, char *mapset );

/* Internal functions, MUST NOT be used in modules */
int Vect_init (void);
int Vect_print_header (struct Map_info *);
int Vect__init_head (struct Map_info *);

    /* Open/close/rewind map */
int Vect_coor_info ( struct Map_info *, struct Coor_info *);
int Vect_open_topo (struct Map_info *);
int Vect_save_topo ( struct Map_info *);
int Vect_open_spatial_index (struct Map_info *);
int Vect_save_spatial_index ( struct Map_info *);
int Vect_spatial_index_dump ( struct Plus_head *, FILE * ); 

int Vect__write_head (struct Map_info *);
int Vect__read_head (struct Map_info *);
int V1_open_old_nat (struct Map_info *, int);
int V1_open_old_shp (struct Map_info *, int);
int V1_open_old_post (struct Map_info *, int);
int V1_open_new_nat (struct Map_info *, char *, int);
int V1_open_new_shp (struct Map_info *, char *, int);
int V1_open_new_post (struct Map_info *, char *, int);
int V2_open_old_nat (struct Map_info *, int);
int V2_open_old_shp (struct Map_info *, int);
int V2_open_old_post (struct Map_info *, int);
int V1_rewind_nat (struct Map_info *);
int V1_rewind_shp (struct Map_info *);
int V1_rewind_post (struct Map_info *);
int V2_rewind_nat (struct Map_info *);
int V2_rewind_shp (struct Map_info *);
int V2_rewind_post (struct Map_info *);
int V1_close_nat (struct Map_info *);
int V1_close_shp (struct Map_info *);
int V1_close_post (struct Map_info *);
int V2_close_nat (struct Map_info *);
int V2_close_shp (struct Map_info *);
int V2_close_post (struct Map_info *);

    /* Read/write lines */
int Vect_read_line_by_offset (struct Map_info *, struct line_pnts *, struct line_cats *, long);
long Vect_next_line_offset (struct Map_info *);
long Vect_next_line_offset_nat (struct Map_info *);
long Vect_next_line_offset_shp (struct Map_info *);
long Vect_next_line_offset_post (struct Map_info *);
long Vect_last_line_offset (struct Map_info *);
long Vect_last_line_offset_nat (struct Map_info *);
long Vect_last_line_offset_shp (struct Map_info *);
long Vect_last_line_offset_post (struct Map_info *);
int V1_read_line_nat (struct Map_info *, struct line_pnts *, struct line_cats *, long);
int V1_read_line_shp (struct Map_info *, struct line_pnts *, struct line_cats *, long);
int V1_read_line_post (struct Map_info *, struct line_pnts *, struct line_cats *, long);
int V1_read_next_line (struct Map_info *, struct line_pnts *, struct line_cats *);
int V1_read_next_line_nat (struct Map_info *, struct line_pnts *, struct line_cats *);
int V1_read_next_line_shp (struct Map_info *, struct line_pnts *, struct line_cats *);
int V1_read_next_line_post (struct Map_info *, struct line_pnts *, struct line_cats *);
int V2_read_line_nat (struct Map_info *, struct line_pnts *, struct line_cats *, int);
int V2_read_line_shp (struct Map_info *, struct line_pnts *, struct line_cats *, int);
int V2_read_line_post (struct Map_info *, struct line_pnts *, struct line_cats *, int);
int V2_read_next_line (struct Map_info *, struct line_pnts *, struct line_cats *);
int V2_read_next_line_nat (struct Map_info *, struct line_pnts *, struct line_cats *);
int V2_read_next_line_shp (struct Map_info *, struct line_pnts *, struct line_cats *);
int V2_read_next_line_post (struct Map_info *, struct line_pnts *, struct line_cats *);
int V1_delete_line (struct Map_info *, long);
int V2_delete_line (struct Map_info *, int);
int V1_delete_line_nat (struct Map_info *, long);
int V2_delete_line_nat (struct Map_info *, int);
int V1_delete_line_post (struct Map_info *, long);
int V2_delete_line_post (struct Map_info *, int);
long V1_write_line_nat (struct Map_info *, int type, struct line_pnts *, struct line_cats *);
long V2_write_line_nat (struct Map_info *, int type, struct line_pnts *, struct line_cats *);
long V1_write_line_shp (struct Map_info *, int type, struct line_pnts *, struct line_cats *);
long V1_write_line_post (struct Map_info *, int type, struct line_pnts *, struct line_cats *);
long V2_write_line_post (struct Map_info *, int type, struct line_pnts *, struct line_cats *);
long V1_rewrite_line (struct Map_info *, long offset, int type, struct line_pnts *, struct line_cats *);
long V1_rewrite_line_nat (struct Map_info *, long offset, int type, struct line_pnts *, struct line_cats *);
int V2_rewrite_line_nat (struct Map_info *, int line, int type, struct line_pnts *, struct line_cats *);
long V1_rewrite_line_shp (struct Map_info *, long offset, int type, struct line_pnts *, struct line_cats *);
long V1_rewrite_line_post (struct Map_info *, long offset, int type, struct line_pnts *, struct line_cats *);
long V2_rewrite_line_post (struct Map_info *, long offset, int type, struct line_pnts *, struct line_cats *);

    /* Miscellaneous */
int Vect_build_nat ( struct Map_info *, FILE *);
int Vect_build_shp ( struct Map_info *, FILE *);
int Vect_build_post ( struct Map_info *, FILE *);
int Vect_type_to_code (int);
int Vect_code_to_type (int);
int Vect_type_to_store (int);
int Vect_type_from_store (int);
int Vect_build_line_area ( struct Map_info *, int, int);
int Vect_isle_find_area ( struct Map_info *, int);
int Vect_attach_isle ( struct Map_info *, int);
int Vect_attach_isles ( struct Map_info *, BOUND_BOX *);
int Vect_attach_centroids ( struct Map_info *, BOUND_BOX *);

#endif

