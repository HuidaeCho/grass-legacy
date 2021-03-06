/*!
  \file include/vect/dig_structs.h

  \brief Data structures for \ref vectorlib

  \author Written by Dave Gerdes (CERL)  5/1988
  \author Updated to GRASS 5.7 by Radim Blazek (2001)
  \author Doxygenized by Martin Landa <landa.martin gmail.com> (2011)
*/
#include <grass/config.h>

#ifndef  DIG___STRUCTS___
#define DIG___STRUCTS___

/*  this file depends on <stdio.h> */
#ifndef _STDIO_H
#include <stdio.h>
#endif

#include <grass/dgl.h>
#include <grass/shapefil.h>
#include <grass/btree.h>
#include <grass/rtree.h>
	
#ifdef HAVE_OGR
#include "ogr_api.h"
#endif

#define HEADSTR	50

/*!
  \brief plus_t size

  3.10 changes plus_t to ints. This assumes that any reasonable
  machine will use 4 bytes to store an int. The diglib code is not
  guaranteed to work if plus_t is changed to a type that is larger
  than an int.
*/
typedef int plus_t;

typedef struct bound_box BOUND_BOX;

typedef struct P_node P_NODE;
typedef struct P_area P_AREA;
typedef struct P_line P_LINE;
typedef struct P_isle P_ISLE;

/*!
  \brief Used by sites lib
*/
struct site_att
{
    /*!
      \brief Category number
    */
    int cat;
    /*!
      \brief Array of double attributes
    */
    double *dbl;
    /*!
      \brief Array of string attributes
    */
    char **str;
};

typedef struct site_att SITE_ATT;

/*!
  \brief Bounding box
*/
struct bound_box
{
    /*!
      \brief North
    */
    double N;
    /*!
      \brief South
    */
    double S;
    /*!
      \brief East
    */
    double E;
    /*!
      \brief West
    */
    double W;
    /*!
      \brief Top
    */
    double T;
    /*!
      \brief Bottom
    */
    double B;
};

/*!
  \brief File definition
*/
struct gvfile
{
    /*!
      \brief File descriptor
    */
    FILE *file;
    /*!
      \brief Pointer to beginnig of the file in the memory
    */
    char *start;
    /*!
      \brief Current position set by dig_seek()
    */
    char *current;
    /*!
      \brief End of file in the memory (pointer to first byte after)
    */
    char *end;
    /*!
      \brief Size of the file loaded to memory
    */
    long size;
    /*!
      \brief Allocated space
    */
    long alloc;
    /*!
      \brief Is file loaded?

      - 0 - not loaded
      - 1 - loaded
    */
    int loaded;
};

typedef struct gvfile GVFILE;

/* category field information */
/*!
  \brief Field (layer) information
*/
struct field_info
{
    /*!
      \brief Field number
    */
    int number;
    /*!
      \brief Field name (optional)
    */
    char *name;
    /*!
      \brief Name of DB driver ('sqlite', 'dbf', ...)
    */
    char *driver;
    /*!
      brief Name of database
    */
    char *database;
    /*!
      \brief Name of DB table
    */
    char *table;
    /*!
      \brief Name of key column (usually 'cat')
    */
    char *key;
};

/*!
  \brief Database links
*/
struct dblinks
{
    /*!
      \brief Pointer to the first field_info structure
    */
    struct field_info *field;
    /*!
      \brief Number of allocated slots
    */
    int alloc_fields;
    /*!
      \brief Number of available fields (layers)
    */
    int n_fields;
};

/*!
  \brief Portability info
  
  Set by V1_open_new() or V1_open_old()
*/
struct Port_info
{
    /*!
      \brief File byte order
    */
    int byte_order;

    /*!
      \brief Conversion matrices between file and native byte order (double)
    */
    unsigned char dbl_cnvrt[PORT_DOUBLE];
    /*!
      \brief Conversion matrices between file and native byte order (float)
    */
    unsigned char flt_cnvrt[PORT_FLOAT];
    /*!
      \brief Conversion matrices between file and native byte order (long)
    */
    unsigned char lng_cnvrt[PORT_LONG];
    /*!
      \brief Conversion matrices between file and native byte order (int)
    */
    unsigned char int_cnvrt[PORT_INT];
    /*!
      \brief Conversion matrices between file and native byte order (short)
    */
    unsigned char shrt_cnvrt[PORT_SHORT];

    /*!
      \brief Byte order for dbl
      
      Specify if native byte order of that type is the same
      as byte order of vector file (TRUE) or not (FALSE)
    */
    int dbl_quick;
    /*!
      \brief Byte order for dbl
      
      Specify if native byte order of that type is the same
      as byte order of vector file (TRUE) or not (FALSE)
    */
    int flt_quick;
    /*!
      \brief Byte order for lng
      
      Specify if native byte order of that type is the same
      as byte order of vector file (TRUE) or not (FALSE)
    */
    int lng_quick;
    /*!
      \brief Byte order for int
      
      Specify if native byte order of that type is the same
      as byte order of vector file (TRUE) or not (FALSE)
    */
    int int_quick;
    /*!
      \brief Byte order for shrt
      
      Specify if native byte order of that type is the same
      as byte order of vector file (TRUE) or not (FALSE)
    */
    int shrt_quick;
};

/*!
  \brief List of dead lines in the file

  \todo Implement it

  The space can be reused, not yet used
*/
struct recycle
{
    char dummy;
};

/*!
  \brief Vector map header data

  Holds header data of vector map (see \ref vlibMap_info)
*/
struct dig_head
{
    /*!
      \brief Organization name
    */
    char *organization;
    /*!
      \brief Map date
    */
    char *date;
    /*!
      \brief User name
    */
    char *your_name;
    /*!
      \brief Map name
    */
    char *map_name;
    /*!
      \brief Source date
    */
    char *source_date;
    /*!
      \brief Original scale
    */
    long orig_scale;
    /*!
      \brief Comments
    */
    char *line_3;
    /*!
      \brief Zone (UTM only)
    */
    int plani_zone;
    /*!
      \brief Threshold for digitization
    */
    double digit_thresh;
    /* double map_thresh; *//* not used in g57 */

    /* Programmers should NOT touch any thing below here */
    /* Library takes care of everything for you          */

    /* coor elements */
    /*!
      \brief Backward compatibility info - major version
    */
    int Version_Major;
    /*!
      \brief Backward compatibility info - minor version
    */
    int Version_Minor;
    /*!
      \brief Backward compatibility info - back major version
    */
    int Back_Major;
    /*!
      \brief Backward compatibility info - back minor version
    */
    int Back_Minor;
    /*!
      \brief 2D/3D vector data

      - zero for 2D data
      - non-zero for 3D data
    */
    int with_z;

    /*!
      \brief Coor file size
    */
    long size;
    /*!
      \brief Coor header size
    */
    long head_size;

    /*!
      \brief Portability information
    */
    struct Port_info port;

    /*!
      \brief Offset of last read line
    */
    long last_offset;

    /*!
      \brief Recycle dead line

      \todo Not implemented yet
    */
    struct recycle *recycle;

    struct Map_info *Map;	/* X-ref to Map_info struct ?? */
};

/*!
  \brief Coor file info
*/
struct Coor_info
{
    /*!
      \brief Total size (in bytes)
    */
    long size;
    /*!
      \brief Time of last modification
    */
    long mtime;			
};

/*!
  \brief Non-native format info (OGR)

  \todo Structure size should not change depending on compilation I
        think, do it better
*/
struct Format_info_ogr
{
    /*!
      \brief OGR datasource name
    */
    char *dsn;
    /*!
      \brief OGR layer name
    */
    char *layer_name;
#ifdef HAVE_OGR
    /*!
      \brief Pointer to OGRDataSource
    */
    OGRDataSourceH ds;
    /*!
      \brief Pointer to OGRLayer
     */
    OGRLayerH layer;
#else
    void *ds;
    void *layer;
#endif

    /* Level 1 (used by V*_read_next_line_ogr) */
    
    /*!
      \brief Points cache (level 1)
    */
    struct line_pnts **lines;	
    /*!
      \brief List of line types (level 1)
    */
    int *lines_types;
    /*!
      \brief Number of allocated lines (level 1)
    */
    int lines_alloc;
    /*!
      \brief Number of lines in cache (level 1)
    */
    int lines_num;
    /*!
      \brief Next line to be read from cache (level 1)
    */
    int lines_next;

    /* Level 2 */
    /*!
      \brief Cache to avoid repeated reading (level 2)

      NULL if no feature is in cache
    */
#ifdef HAVE_OGR
    OGRFeatureH feature_cache;
#else
    void *feature_cache;
#endif
    /*!
      \brief Feature id read in feature_cache (level 2)
    */
    int feature_cache_id;

    /*!
      \brief Offset list

      Array where OGR feature/part info is stored for each line in
      GRASS. This is not used for GV_CENTROID. Because one feature
      may contain more elements (geometry collection also
      recursively), offset for one line may be stored in more
      records. First record is FID, next records are part indexes if
      necessary. Example: 5. ring in 3. polygon in 7. feature
      (multipolygon) of geometry collection which has FID = 123 123
      (feature 123: geometry colletion) 6 (7. feature in geometry
      collection: multiPolygon) 2 (3. polygon) 4 (5. ring in the
      polygon)
    */
    int *offset;
    /*!
      \brief Number of items in offset
    */
    int offset_num;
    /*!
      \brief Space allocated for offset
    */
    int offset_alloc;
    /*!					      
      \brief Next line to be read

      Used by V2_read_next_line_ogr()
    */
    int next_line;
};

/*!
  \brief Non-native format info (currently only OGR is implemented)
*/
struct Format_info
{
    /*!
      \brief id?
    */
    int i;
    /*!
      \brief OGR info
    */
    struct Format_info_ogr ogr;
};

/*!
  \brief Category index
*/
struct Cat_index
{
    /*!
      \brief Field (layer) number
    */
    int field;			
    /*!
      \brief Number of items in cat array
    */
    int n_cats;			
    /*!
      \brief Allocated space in cat array
    */
    int a_cats;
    /*!
      \brief Array of cats (cat, type, lines/area)
    */
    int (*cat)[3];		
    /*!
      \brief Number of unique cats (not updated)
    */
    int n_ucats;
    /*!
      \brief Number of types in type
    */
    int n_types;		
    /*!
      \brief Number of elements for each type

      - GV_POINT
      - GV_LINE
      - GV_BOUNDARY
      - GV_CENTROID
      - GV_FACE
      - GV_KERNEL
      - GV_AREA
    */
    int type[7][2];		
    /*!
      \brief Offset of the beginning of this index in cidx file
    */
    long offset;
};

/*!
  \brief Basic topology-related info

  Holds basic topology-related information about vector map
*/
struct Plus_head
{
    /*!
      \brief Version info (major)
    */
    int Version_Major;
    /*!
      \brief Version info (minor)
    */
    int Version_Minor;
    /*!
      \brief Earliest version that can use this data format (major)
    */
    int Back_Major;		 
    /*!
      \brief Earliest version that can use this data format (minor)
    */
    int Back_Minor;
    /*!
      \brief Version codes for spatial index (major)
    */
    int spidx_Version_Major;
    /*!
      \brief Version codes for spatial index (minor)
    */
    int spidx_Version_Minor;
    /*!
      \brief Earliest version that can use this data format (major)
    */
    int spidx_Back_Major;
     /*!
       \brief Earliest version that can use this data format (minor)
    */
    int spidx_Back_Minor;

    /*!
      \brief Version codes for category index (major)
    */
    int cidx_Version_Major;
    /*!
      \brief Version codes for category index (minor)
    */
    int cidx_Version_Minor;
    /*!
      \brief Earliest version that can use this data format (major)
    */
    int cidx_Back_Major;
    /*!
      \brief Earliest version that can use this data format (minor)
    */
    int cidx_Back_Minor;

    /*!
      \brief 2D/3D vector data

      - WITH_Z
      - WITHOUT_Z
    */
    int with_z;
    /*!
      \brief 2D/3D spatial index

      - WITH_Z
      - WITHOUT_Z
    */
    int spidx_with_z;

    /*** file header size ***/
    
    /*!
      \brief Topo header size 
    */
    long head_size;
    /*!
      \brief Spatial index header size
    */
    long spidx_head_size;
    /*!
      \brief Category index header size
    */
    long cidx_head_size;

    /*!
      \brief Release memory occupied by support
      (topo, spatial, category)
    */
    int release_support;

    /*** portability info */

    /*!
      \brief Portability information
    */
    struct Port_info port;
    /*!
      \brief Portability information for spatial index
    */
    struct Port_info spidx_port;
    /*!
      \brief Portability information for category index
    */
    struct Port_info cidx_port;
    /*!
      \brief Access mode
      
      - GV_MODE_READ
      - GV_MODE_WRITE
      - GV_MODE_RW
    */
    int mode;

    /*!
      \brief Highest level of topology currently available

      - GV_BUILD_NONE
      - GV_BUILD_BASE
      - GV_BUILD_AREAS
      - GV_BUILD_ATTACH_ISLES
      - GV_BUILD_CENTROIDS
      - GV_BUILD_ALL
    */
    int built;
    /*!
      \brief Bounding box of features
    */
    struct bound_box box;

    /*** topology ***/
   /*!
     \brief Array topo nodes
   */
    P_NODE **Node;
   /*!
     \brief Array topo lines/edges
   */
    P_LINE **Line;
   /*!
     \brief Array topo areas
   */
    P_AREA **Area;
    /*!
      \brief Array topo isles
    */
    P_ISLE **Isle;

    
    /*!
      \brief Current number of nodes
    */
    plus_t n_nodes;
    /*!
      \brief Current number of edges
    */
    plus_t n_edges;
    /*!
      \brief Current number of lines
    */
    plus_t n_lines;
    /*!
      \brief Current number of areas
    */
    plus_t n_areas;
    /*!
      \brief Current number of isles
    */
    plus_t n_isles;
    /*!
      \brief Current number of faces
    */
    plus_t n_faces;
    /*!
      \brief Current number of volumes
    */
    plus_t n_volumes;
    /*!
      \brief Current number of holes
    */
    plus_t n_holes;

    /*!
      \brief Current number of points
    */
    plus_t n_plines;
    /*!
      \brief Current number of lines
    */
    plus_t n_llines;
    /*!
      \brief Current number of boundaries
    */
    plus_t n_blines;
    /*!
      \brief Current number of centroids
    */
    plus_t n_clines;
    /*!
      \brief Current number of faces
    */
    plus_t n_flines;
    /*!
      \brief Current number of kernels
    */
    plus_t n_klines;

   /*!
     \brief Number of allocated nodes

     i.e. array size - 1
   */
    plus_t alloc_nodes;
   /*!
     \brief Number of allocated edges

     i.e. array size - 1
   */
    plus_t alloc_edges;
   /*!
     \brief Number of allocated lines

     i.e. array size - 1
   */
    plus_t alloc_lines;
   /*!
     \brief Number of allocated areas

     i.e. array size - 1
   */
    plus_t alloc_areas;
   /*!
     \brief Number of allocated isles

     i.e. array size - 1
   */
    plus_t alloc_isles;
    plus_t alloc_volumes;
   /*!
     \brief Number of allocated holes

     i.e. array size - 1
   */
    plus_t alloc_holes;

    /*!
      \brief Offset of array of nodes in topo file
    */
    long Node_offset;
    /*!
      \brief Offset of array of edges in topo file
    */
    long Edge_offset;
    /*!
      \brief Offset of array of lines in topo file
    */
    long Line_offset;
    /*!
      \brief Offset of array of areas in topo file
    */
    long Area_offset;
    /*!
      \brief Offset of array of isles in topo file
    */
    long Isle_offset;
    /*!
      \brief Offset of array of volumes in topo file
    */
    long Volume_offset;
    /*!
      \brief Offset of array of holes in topo file
    */
    long Hole_offset;

    /*** spatial index ***/
    /* Spatial index is never saved, it is built automatically for new and updated vectors.
     * It is not built for old vectors until it is needed, i.e. until Vect_select() is called. 
     * or until Vect_build() is called */

    /*!
      \brief Spatial index built?

      Set to 1 if spatial index is available
    */
    int Spidx_built;
    /*!
      \brief Offset of nodes in sidx file
    */
    long Node_spidx_offset;
    /*!
      \brief Offset of edges in sidx file
    */
    long Edge_spidx_offset;
    /*!
      \brief Offset of lines in sidx file
    */
    long Line_spidx_offset;
    /*!
      \brief Offset of areas in sidx file
    */
    long Area_spidx_offset;
    /*!
      \brief Offset of isles in sidx file
    */
    long Isle_spidx_offset;
    /*!
      \brief Offset of volumes in sidx file
    */
    long Volume_spidx_offset;
    /*!
      \brief Offset of holes in sidx file
    */
    long Hole_spidx_offset;

    /*!
      \brief Node spatial index
    */
    struct Node *Node_spidx;
    /*!
      \brief Line spatial index
    */
    struct Node *Line_spidx;
    /*!
      \brief Area spatial index
    */
    struct Node *Area_spidx;
    /*!
      \brief Isles spatial index
    */
    struct Node *Isle_spidx;

    /*** category index ***/
    /*!
      \brief Update category index if vector is modified 

      By default, category index is not updated 
    */
    int update_cidx;

    /*!
      \brief Number of category indexes (one for each field/layer)
    */
    int n_cidx;
    /*!
      \brief Allocated space for category indexes
    */
    int a_cidx;
    /*!
      \brief Array of category indexes
    */
    struct Cat_index *cidx;
    /*!
      \brief Category index to be updated

      Set to 1 when cidx is created
      and reset to 0 whenever any line is changed
    */
    int cidx_up_to_date;
    /*!
      \brief Size of coor file
    */
    long coor_size;
    /*!
      \brief Time of last coor modification
    */
    long coor_mtime;

    /* Level2 update: list of lines and nodes updated (topo info for the line was changed) 
     *                 by last write/rewrite/delete operation.
     *                 Lines/nodes in the list may be deleted (e.g. delete boundary: first added for
     *                 delete area and then delete */
    int do_uplist;

    /*!
      \brief Array of updated lines

      List of lines and nodes updated (topo info for the line was
      changed) by last write/rewrite/delete operation.  Lines/nodes in
      the list may be deleted (e.g. delete boundary: first added for
      delete area and then delete
    */
    int *uplines;
    /*!
      \brief Allocated array (level 2)
    */
    int alloc_uplines;
    /*!
      \brief Number of updated lines (level 2)
    */
    int n_uplines;
    /*!
      \brief Array of updated nodes (level 2)
    */
    int *upnodes;
    /*!
      \brief Allocated array (level 2)
    */
    int alloc_upnodes;
    /*!
      \brief number of updated nodes (level 2)
    */
    int n_upnodes;
};

/*! \brief
  Vector map info

  Maintains all information about an individual open vector map. The
structure must be passed to the mosy vector library routines.
*/
struct Map_info
{
    /* common info for all formats */
    /*!
      \brief Format (native, ogr)
      
      - GV_FORMAT_NATIVE
      - GV_FORMAT_OGR
      - GV_FORMAT_DIRECT
    */
    int format;
    /*!
      \brief Temporary file flag, not yet used
    */
    int temporary;

    /*!
      \brief DB links
    */
    struct dblinks *dblnk;

    /*!
      \brief Header info
    */
    struct Plus_head plus;

    /* graph-related section */
    /*!
      \brief Graph-related section - line type used to build the graph
     */
    int graph_line_type;
    /*!
      \brief Graph-related section - graph structure
    */
    dglGraph_s graph;
    /*!
      \brief Graph-related section - shortest path cache
    */
    dglSPCache_s spCache;
    /*!
      \brief Graph-related section - forward costs used for graph

      dglGetEdge() is not supported for _DGL_V1)
    */
    double *edge_fcosts;
    /*!
      \brief Graph-related section - backward costs used for graph
    */
    double *edge_bcosts;
    /*!
      \brief Graph-related section - node costs used for graph
    */
    double *node_costs;
    /*!
      \brief Graph-related section - edge and node costs multiplicator
    */
    int cost_multip;

    /*!
      \brief Open indicator

      Should be 0x5522AA22 (VECT_OPEN_CODE) if opened correctly
      or        0x22AA2255 (VECT_CLOSED_CODE) if closed
      anything else implies that structure has
      never been initialized
    */
    int open;
    /* Open mode
       - read (GV_MODE_READ),
       - write (GV_MODE_WRITE),
       - rw (GV_MODE_RW)
    */
    int mode;
    /*!
      \brief Topology level
      - 1 (without topo)
      - 2 (with topo)
      - 3 (with 3D topo) - not yet implemented
    */
    int level;
    /*!
      \brief Open only header

      Non-zero code to open only header of vector map
    */
    int head_only;
    /*!
      \brief Support files were updated

      Non-zero code to indicate that support file were updated
    */
    int support_updated;
    /*!
      \brief Sequential read (level 1)
    */
    plus_t next_line;

    /*!
      \brief Map name (for 4.0)
    */
    char *name;
    /*!
      \brief Mapset name
    */
    char *mapset;
    /* location and gisdbase is usefull if changed (v.proj or external apps) */
    /*!
      \brief Location name
    */
    char *location;
    /*!
      \brief GISDBASE path
    */
    char *gisdbase;

    /* constraints for reading in lines  (not polys yet) */

    /*!
      \brief Constraints for reading features (region)

      Non-zero code to enable this constraint
    */
    int Constraint_region_flag;
    /*!
      \brief Constraints for reading features (type)

      Non-zero code to enable this constraint
    */
    int Constraint_type_flag;
    /*!
      \brief Constraints for reading features (north)
    */
    double Constraint_N;
    /*!
      \brief Constraints for reading features (south)
    */
    double Constraint_S;
    /*!
      \brief Constraints for reading features (east)
    */
    double Constraint_E;
    /*!
      \brief Constraints for reading features (west)
    */
    double Constraint_W;
    /*!
      \brief Constraints for reading features (top)
    */
    double Constraint_T;
    /*!
      \brief Constraints for reading features (bottom)
    */
    double Constraint_B;
    /*!
      \brief Constraints for reading features (type)
    */
    int Constraint_type;
    /*!
      \brief ???
    */
    int proj;

    /* format specific */

    /*!
      \brief GV file pointer (native format only)
    */
    GVFILE dig_fp;
    /*!
      \brief Coor file header info (native format only)
    */
    struct dig_head head;

    /* non native */
    /*!
      \brief Format info for non-native formats
    */
    struct Format_info fInfo;

    /*!
      \brief History file
    */
    FILE *hist_fp;

    /* Temporary solution for sites */
    SITE_ATT *site_att;		/* array of attributes loaded from db */
    int n_site_att;		/* number of attributes in site_att array */
    int n_site_dbl;		/* number of double attributes for one site */
    int n_site_str;		/* number of string attributes for one site */
};

/*!
  \brief Topological primitive - node
*/
struct P_node
{
    /*!
      \brief X coordinate
    */
    double x;			/* X coordinate */
    /*!
      \brief Y coordinate
    */
    double y;
    /*!
      \brief Z coordinate (used only for 3D data)
    */
    double z;
    /*!
      \brief Allocated space for lines
    */
    plus_t alloc_lines;
    /*!
      \brief Number of attached lines (size of
      lines, angle)

      If 0, then is degenerate node, for snappingi ???
    */
    plus_t n_lines;
    /*!
      \brief List of connected lines
    */
    plus_t *lines;
    /*!
      \brief List of respected angles

      Angles for lines/boundaries are in radians between -PI and
      PI. Value for points or lines with identical points
      (degenerated) is set to -9.
    */
    float *angles;
};

/*!
  \brief Topological primitive - line (edge)
*/
struct P_line
{
    /*!
      \brief Start node
    */
    plus_t N1;
    /*!
      \brief End node
    */
    plus_t N2;
    /*!
      \brief Area/isle number to left

      - negative for isle
      - area number for centroid (negative for duplicate centroid)
    */
    plus_t left;
    /*!
      \brief Area/isle number to right

      - negative for isle
    */
    plus_t right;
    /*!
      \brief Line bounding box - north
    */
    double N;
    /*!
      \brief Line bounding box - south
    */
    double S;
    /*!
      \brief Line bounding box - east
    */
    double E;
    /*!
      \brief Line bounding box - west
    */
    double W;
    /*!
      \brief Line bounding box - top (3D data only)
    */
    double T;
    /*!
      \brief Line bounding box - bottom (3D data only)
    */
    double B;

    /*!
      \brief Offset in coor file for line
    */
    long offset;
    /*!
      \brief Line type

      - GV_POINT
      - GV_LINE
      - GV_BOUNDARY
      - GV_CENTROID
      - GV_FACE
      - GV_KERNEL
      - GV_AREA
      - GV_VOLUME
    */
    int type;
};

/*!
  \brief Topological primitive - area
*/
struct P_area
{
    /*!
      \brief Area bounding box - north
    */
    double N;
    /*!
      \brief Area bounding box - south
    */
    double S;
    /*!
      \brief Area bounding box - east
    */
    double E;
    /*!
      \brief Area bounding box - west
    */
    double W;
    /*!
      \brief Area bounding box - top
    */
    double T;
    /*!
      \brief Area bounding box - bottom
    */
    double B;
    /*!
      \brief Number of boundary lines
    */
    plus_t n_lines;
    /*!
      \brief Allocated space for lines
    */
    plus_t alloc_lines;
    /*!
      \brief List of boundary lines

      - negative means direction N2 to N1
      - lines are in clockwise order
    */
    plus_t *lines;

    /*********  Above this line is compatible with P_isle **********/

    /*!
      \brief Number of first centroid within area
    */
    plus_t centroid;
    /*!
      \brief Number of islands inside
    */
    plus_t n_isles;
    /*!
      \brief Allocated space for isles
    */
    plus_t alloc_isles;
    /*!
      \brief 1st generation interior islands
    */
    plus_t *isles;
};

/*!
  \brief Topological primitive - isle
*/
struct P_isle
{
    /*!
      \brief Isle bounding box - north
    */
    double N;			
    /*!
      \brief Isle bounding box - south
    */
    double S;
    /*!
      \brief Isle bounding box - east
    */
    double E;
    /*!
      \brief Isle bounding box - west
    */
    double W;
    /*!
      \brief Isle bounding box - top
    */
    double T;			
    /*!
      \brief Isle bounding box - bottom
    */
    double B;
    /*!
      \brief Number of boundary lines
    */
    plus_t n_lines;
    /*!
      \brief Allocated space for lines
    */
    plus_t alloc_lines;
    /*!
      \brief List of boundary lines

      - negative means direction N2 to N1
      - lines are in counter clockwise order
    */
    plus_t *lines;
 
    /*********  Above this line is compatible with P_area **********/
    
    /*!
      \brief Area it exists w/in, if any
    */
    plus_t area;
};

/*!
  \brief Feature geometry info
*/
struct line_pnts
{
    /*!
      \brief Array of X coordinates
    */
    double *x;
    /*!
      \brief Array of Y coordinates
    */
    double *y;
    /*!
      \brief Array of Z coordinates
    */
    double *z;
    /*!
      \brief Number of points
    */
    int n_points;
    /*!
      \brief Allocated space for points
    */
    int alloc_points;
};

/*!
  \brief Feature category info
*/
struct line_cats
{
    /*!
      \brief Array of fields (layers)
    */
    int *field;
    /*!
      \brief Array of categories
    */
    int *cat;
    /*!
      \brief Number of categories attached to element
    */
    int n_cats;
    /*!
      \brief Allocated space for categories
    */
    int alloc_cats;
};

/*! \brief Category list */
struct cat_list
{
    /*!
      \brief Category field (layer)
    */
    int field;
    /*!
      \brief Array of minimun values
    */
    int *min;
    /*!
      \brief Array of maximum values
    */
    int *max;
    /*!
      \brie Number of ranges
    */
    int n_ranges;
    /*!
      \brief Allocated space for ranges
    */
    int alloc_ranges;
};

/* list of integers */
struct ilist
{
    /*!
      \brief Array of values
    */
    int *value;
    /*!
      \brief Number of values in the array
    */
    int n_values;
    /*!
      \brief Allocated space for values
    */
    int alloc_values;
};

/*!
  \brief Vector array

  Space allocated is size + 1
*/
struct varray
{
    /*!
      \brief Array size
    */
    int size;	
    /*!
      \brief Array

      Where 'class' or new category
      or something like that is stored
    */
    int *c;	
};

typedef struct varray VARRAY;

/*!
  \brief Spatial index info

  For use in modules
*/
struct spatial_index
{
    struct Node *root;
};

typedef struct spatial_index SPATIAL_INDEX;

typedef dglGraph_s GRAPH;	/* graph structure */

#endif /* DIG___STRUCTS___ */
