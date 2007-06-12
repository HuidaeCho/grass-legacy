"""
MODULE:     menudata.py

CLASSES:
    * Data

PURPOSE:    Complex list for main menu entries for GRASS wxPython GUI.

AUTHORS:    The GRASS Development Team
            Michael Barton (Arizona State University)
            Yann Chemin

COPYRIGHT:  (C) 2007 by the GRASS Development Team
            This program is free software under the GNU General Public
            License (>=v2). Read the file COPYING that comes with GRASS
            for details.

"""

import os

class Data:
    '''Data object that returns menu descriptions to be used in wxgui.py.
    Probably could be changed to XML or *.dtd file.'''
    def GetMenu(self):
        return [(
          ("Files", (
             ("Import", (
                 ("Raster map", (
                     ("Multiple formats using GDAL", "Import multiple formats using GDAL", "self.OnMenuCmd", "r.in.gdal"),
                     ("","","", ""),
                     ("Aggregate ASCII xyz", "Import aggregate ASCII xyz data", "self.OnMenuCmd", "r.in.xyz"),
                     ("ASCII grid", "Import ASCII GRID (includes GRASS ASCII)", "self.OnMenuCmd", "r.in.ascii"),
                     ("ASCII polygons and lines", "Import Polygons and lines from ASCII file", "self.OnMenuCmd", "r.in.poly"),
                     ("","","", ""),
                     ("Binary", "Import binary file (includes GTOPO30 format)", "self.OnMenuCmd", "r.in.bin"),
                     ("ESRI Arc/Info ASCII grid", "Import ESRI Arc/Info ASCII grid", "self.OnMenuCmd", "r.in.arc"),
                     ("GRIDATB.FOR", "Import GRIDATB.FOR map file (TOPMODEL)", "self.OnMenuCmd", "r.in.gridatb"),
                     ("MAT-File (v.4)", "Import MAT-File (v.4) array (Matlab or Octave)", "self.OnMenuCmd", "r.in.mat"),
                     ("SPOT NDVI", "Import SPOT vegetation NDVI data sets", "self.OnMenuCmd", "r.in.spotvgt"),
                     ("SRTM HGT", "Import SRTM hgt files", "self.OnMenuCmd", "r.in.srtm"),
                     ("Terra ASTER HDF", "Import Terra ASTER HDF files", "self.OnMenuCmd", "r.in.aster"),
                     ("","","", ""),
                     ("WMS", "Import Web Mapping Server", "self.OnMenuCmd", "r.in.wms"),
                     )),
                 ("Vector map", (
                     ("Multiple formats using OGR", "Import multiple formats using OGR", "self.OnMenuCmd", "v.in.ogr"),
                     ("","","", ""),
                     ("ASCII points or GRASS ASCII vector", "Import ASCII points file or GRASS ASCII vector file", "self.OnMenuCmd", "v.in.ascii"),
                     ("Old GRASS vector", "Import old GRASS vector format", "self.OnMenuCmd", "v.convert"),
                     ("","","", ""),
                     ("DXF", "Import DXF file", "self.OnMenuCmd", "v.in.dxf"),
                     ("ESRI e00", "Import ESRI e00 file", "self.OnMenuCmd", "v.in.e00"),
                     ("Garmin GPS", "Import Garmin GPS Waypoints/Routes/Tracks", "self.OnMenuCmd", "v.in.garmin"),
                     ("GPSBabel GPS", "Import GPS Waypoints/Routes/Tracks using GPSBabel", "self.OnMenuCmd", "v.in.gpsbabel"),
                     ("GEOnet", "Import GEOnet Name server country files (US-NGA GNS)", "self.OnMenuCmd", "v.in.gns"),
                     ("Matlab and MapGen", "Import Matlab and MapGen files", "self.OnMenuCmd", "v.in.mapgen"),
                     )),

                 ("Grid 3D", (
                     ("ASCII 3D", "Import ASCII 3D file", "self.OnMenuCmd", "r3.in.ascii"),
                     ("Vis5D", "Import Vis5D file", "self.OnMenuCmd", "r3.in.v5d"),
                     )),
                 )),
              ("Export", (
                 ("Raster map", (
                     ("Multiple formats using GDAL", "Export multiple formats using GDAL", "self.OnMenuCmd", "r.out.gdal"),
                     ("","","", ""),
                     ("ASCII grid", "Export ASCII grid (for GRASS, Surfer, Modflow, etc)", "self.OnMenuCmd", "r.out.ascii"),
                     ("ASCII x,y,z", "Export ASCII x,y,z values of cell centers", "self.OnMenuCmd", "r.out.xyz"),
                     ("","","", ""),
                     ("ESRI ASCII grid", "Export ESRI Arc/Info ASCII grid", "self.OnMenuCmd", "r.out.arc"),
                     ("GRIDATB.FOR", "Export GRIDATB.FOR map file (TOPMODEL)", "self.OnMenuCmd", "r.out.gridatb"),
                     ("MAT-File (v.4)", "Export MAT-File (v.4) array (Matlab or Octave)", "self.OnMenuCmd", "r.out.mat"),
                     ("","","", ""),
                     ("Binary", "Export binary file", "self.OnMenuCmd", "r.out.bin"),
                     ("","","", ""),
                     ("MPEG-1", "Export MPEG-1 animations", "self.OnMenuCmd", "r.out.mpeg"),
                     ("PNG", "Export PNG image (not georeferenced)", "self.OnMenuCmd", "r.out.png"),
                     ("PPM", "Export PPM image (24bit)", "self.OnMenuCmd", "r.out.ppm"),
                     ("POV-Ray", "Export POVray height-field", "self.OnMenuCmd", "r.out.pov"),
                     ("TIFF", "Export TIFF image (8/24bit)", "self.OnMenuCmd", "r.out.tiff"),
                     ("VRML", "Export VRML file", "self.OnMenuCmd", "r.out.vrml"),
                     ("VTK", "Export VTK ASCII file", "self.OnMenuCmd", "r.out.vtk"),
                     )),
                 ("Vector map", (
                     ("Multiple formats using OGR", "Export multiple formats using OGR", "self.OnMenuCmd", "v.out.ogr"),
                     ("","","", ""),
                     ("ASCII points or GRASS ASCII vector", "Import ASCII points file or GRASS ASCII vector file", "self.OnMenuCmd", "v.out.ascii"),

                     ("DXF", "Export DXF file", "self.OnMenuCmd", "v.out.dxf"),
                     ("POV-Ray", "Export POV-Ray format file", "self.OnMenuCmd", "v.out.pov"),
                     ("SVG", "Export SVG file", "self.OnMenuCmd", "v.out.svg"),
                     ("VTK", "Export VTK ASCII file", "self.OnMenuCmd", "v.out.vtk"),
                     )),

                 ("Grid 3D", (
                     ("ASCII 3D", "Export ASCII 3D file", "self.OnMenuCmd", "r3.out.ascii"),
                     ("Vis5D", "Export Vis5D file", "self.OnMenuCmd", "r3.out.v5d"),
                     ("VTK", "Export VTK ASCII file", "self.OnMenuCmd", "r3.out.vtk"),
                     )),
                 )),
              ("","","", ""),
              ("Manage maps and volumes", (
                ("Copy", "Copy maps within mapsets or between mapsets", "self.OnMenuCmd", "g.copy"),
                ("","","", ""),
                ("List", "List maps by type", "self.OnMenuCmd", "list"),
                ("List filtered", "List maps filtered by expressions and wildcards", "self.OnMenuCmd", "g.mlist"),
                ("","","", ""),
                ("Rename", "Rename maps", "self.OnMenuCmd", "g.rename"),
                ("","","", ""),
                ("Delete", "Delete maps", "self.OnMenuCmd", "g.remove"),
                ("Delete filtered", "Delete maps using expressions and wildcards", "self.OnMenuCmd", "g.mremove"),
                )),
              ("Map type conversions", (
                ("Raster to vector", "Convert raster to vector map", "self.OnMenuCmd", "r.to.vect"),
                ("Raster series to volume", "Convert raster map series to volume", "self.OnMenuCmd", "r.to.rast3"),
                ("Raster 2.5D to volume", "Convert raster 2.5D map to volume", "self.OnMenuCmd", "r.to.rast3elev"),
                ("","","", ""),
                ("Vector to raster", "Convert vector to raster map", "self.OnMenuCmd", "v.to.rast"),
                ("Vector to volume", "Convert vector 3D points to volume voxels", "self.OnMenuCmd", "v.to.rast3"),
                ("Sites to vector", "Convert sites (GRASS 5) to vector map", "self.OnMenuCmd", "v.in.sites"),
                ("","","", ""),
                ("Volumes to raster series", "Convert raster to vector map", "self.OnMenuCmd", "r3.to.rast"),
                )),
              ("","","", ""),
              ("Georectify (not functional)", "Georectify raster and vector maps", "self.OnGeorectify", ""),
              ("","","", ""),
              ("Bearing/distance to coordinates", "Convert between bearing/distance and coordinates", "self.OnMenuCmd", "m.cogo"),
              ("","","", ""),
              ("Postscript plot", "Create cartographic PostScript plot", "self.OnMenuCmd", "ps.map"),
              ("","","", ""),
              ("E&xit", "Exit from wxgui.py", "self.OnCloseWindow", ""),
              )),
          ("Config", (
              ("Region", (
                ("Display region", "Display region settings", "self.RunMenuCmd", ['g.region', '-p']),
                ("Set region", "Change region settings", "self.OnMenuCmd", "g.region"),
              )),
              ("GRASS working environment", (
                ("Mapset access (not functional)", "Set access to other mapsets in current location", "self.OnMapsets", ""),
                ("Change working environment", "Change current working session to new mapset, location, or data directory", "self.OnMenuCmd", "g.mapset"),
                ("User access", "Change access by other users to current mapset", "self.OnMenuCmd", "g.access"),
                ("Show settings", "Show current GRASS environment settings", "self.RunMenuCmd", ['g.gisenv',' ']),
                ("Change settings", "Change GRASS environment settings", "self.OnMenuCmd", "g.gisenv"),
                ("Version", "Show current GRASS version", "self.RunMenuCmd", ['g.version','-c']),
              )),
              ("Manage projections", (
                ("Projections", "Show projection information and create projection files", "self.OnMenuCmd", "g.proj"),
                ("Convert coordinates", "Convert coordinates from one projection to another", "self.OnMenuCmd", "m.proj"),
              )),
              ("Display font", "Set default font for GRASS displays", "self.DefaultFont", ""),
              )),
          ("Raster", (
              ("Develop map", (
                ("Digitize raster (requires XTerm)", "Digitize raster", "self.OnXTerm", "r.digit"),
                ("","","", ""),
                ("Compress/decompress", "Compress/decompress raster file", "self.OnMenuCmd", "r.compress"),
                ("Boundaries", "Manage boundary definition", "self.OnMenuCmd", "r.region"),
                ("Null values", "Manage null values", "self.OnMenuCmd", "r.null"),
                ("Timestamp", "Manage timestamp for files", "self.OnMenuCmd", "r.timestamp"),
                ("Quantization", "Quantization for floating-point maps", "self.OnMenuCmd", "r.quant"),
                ("Resample (change resolution)", (
                    ("Aggregate statistics", "Resample using aggregate statistics", "self.OnMenuCmd", "r.resamp.stats"),
                    ("Multiple methods", "Resample select nearest neighbor, bilinear, or bicubic interpolation", "self.OnMenuCmd", "r.resamp.interp"),
                    ("Nearest neighbor", "Resample using nearest neighbor interpolation", "self.OnMenuCmd", "r.resample"),
                    ("Regularized spline tension", "Resample using regularized spline tension", "self.OnMenuCmd", "r.resamp.rst"),
                    )),
                ("Support file maintenance", "Support file creation and maintenance", "self.OnMenuCmd", "r.support"),
                ("Update map statistics", "Update map statistics", "self.OnMenuCmd", "r.support.stats"),
                ("","","", ""),
                ("Reproject", "Reproject raster from other location", "self.OnMenuCmd", "r.proj"),
                ("Tiling", "Generate tiling for other projection", "self.OnMenuCmd", "r.tileset"),
                )),
              ("Manage map colors", (
                ("Color tables", "Set colors to predefined color tables", "self.OnMenuCmd", "r.colors"),
                ("Color rules", "Set colors interactively with color rules", "self.RulesCmd", "r.colors"),
                ("","","", ""),
                ("Blend 2 color maps", "Blend 2 color maps to produce 3 RGB files", "self.OnMenuCmd", "r.blend"),
                ("Create RGB", "Create color image from RGB files", "self.OnMenuCmd", "r.composite"),
                ("RGB to HIS", "Create 3 RGB (red, green, blue) maps from 3 HIS (hue, intensity, saturation) maps", "self.OnMenuCmd", "r.his"),
                )),
              ("Query by coordinates", "Query by coordinates", "self.OnMenuCmd", "r.what"),
              ("","","", ""),
              ("Buffers", "Develop raster buffer", "self.OnMenuCmd", "r.buffer"),
              ("MASK", "Develop raster mask", "self.OnMenuCmd", "r.mask"),
              ("Closest points", "r.distance", "self.OnMenuCmd", "r.distance"),
              ("Map calculator", "Map calculator", "self.OnMenuCmd", "scripts/mapcalc_gparser.sh"),
              ("Neighborhood analysis", (
                ("Moving window", "Moving window analysis of raster cells", "self.OnMenuCmd", "r.neighbors"),
                ("Neighborhood points", "Analyze vector points in neighborhood of raster cells", "self.OnMenuCmd", "v.neighbors"),
                )),
              ("Overlay maps", (
                ("Cross product", "Cross product", "self.OnMenuCmd", "r.cross"),
                ("Map series", "Function of map series (time series)", "self.OnMenuCmd", "r.series"),
                ("Patch maps", "Patch maps", "self.OnMenuCmd", "r.patch"),
                ("","","", ""),
                ("Statistical overlay", "Statistical calculations for cover map over base map", "self.OnMenuCmd", "r.statistics"),
                )),
              ("Solar radiance and shadows", (
                ("Solar irradiance and irradiation", "Solar irradiance and daily irradiation", "self.OnMenuCmd", "r.sun"),
                ("Shadow map", "Shadow map for sun position or date/time", "self.OnMenuCmd", "r.sunmask"),
                )),
              ("Terrain analysis", (
                ("Cumulative movement costs", "Calculate cumulative movement costs between locales", "self.OnMenuCmd", "r.walk"),
                ("cost surface", "cost surface", "self.OnMenuCmd", "r.cost"),
                ("Least cost route or flow", "Least cost route or flow", "self.OnMenuCmd", "r.drain"),
                ("Shaded relief", "Shaded relief map", "self.OnMenuCmd", "r.shaded.relief"),
                ("Slope and aspect", "Slope and aspect", "self.OnMenuCmd", "r.slope.aspect"),
                ("Terrain parameters", "Terrain parameters", "self.OnMenuCmd", "r.param.scale"),
                ("Textural features", "Textural features", "self.OnMenuCmd", "r.texture"),
                ("Visibility", "Visibility and line of sight", "self.OnMenuCmd", "r.los"),
                )),
              ("Transform features", (
                ("Clump", "Clump small areas (statistics calculated by r.volume)", "self.OnMenuCmd", "r.clump"),
                ("Grow", "Grow areas", "self.OnMenuCmd", "r.grow"),
                ("Thin", "Thin linear features", "self.OnMenuCmd", "r.thin"),
                )),
              ("","","", ""),
              ("Hydrologic modeling", (
                ("Carve stream channels", "Carve stream channels into elevation map using vector streams map", "self.OnMenuCmd", "r.carve"),
                ("Depressionless elevation and flowline map", "Depressionless elevation map and flowline map", "self.OnMenuCmd", "r.fill.dir"),
                ("Fill lake", "Fill lake from seed point to specified level", "self.OnMenuCmd", "r.lake"),
                ("Flow accumulation", "Flow accumulation for massive grids", "self.OnMenuCmd", "r.terraflow"),
                ("Flow lines", "Generate flow lines for raster map", "self.OnMenuCmd", "r.flow"),
                ("Overland flow modeling", "SIMWE overland flow modeling", "self.OnMenuCmd", "r.sim.water"),
                ("Sediment modeling", "SIMWE sediment erosion, transport, deposition modeling", "self.OnMenuCmd", "r.sim.sediment"),
                ("Topographic index map", "Topographic index map", "self.OnMenuCmd", "r.topidx"),
                ("TOPMODEL simulation", "TOPMODEL simulation", "self.OnMenuCmd", "r.topmodel"),
                ("Watershed subbasins", "Watershed subbasins", "self.OnMenuCmd", "r.basins.fill"),
                ("Watershed analysis", "Watershed analysis", "self.OnMenuCmd", "r.watershed"),
                ("Watershed basin creation", "Watershed basin creation", "self.OnMenuCmd", "r.water.outlet"),
                )),
              ("Landscape structure modeling", (
                ("Set up (requires XTerm)", "Set up sampling and analysis framework", "self.OnXTerm", "r.le.setup"),
                ("","","", ""),
                ("Analyze landscape", "Analyze landscape characteristics", "self.OnMenuCmd", "r.le.pixel"),
                ("Analyze patches", "Analyze landscape patch characteristics", "self.OnMenuCmd", "r.le.patch"),
                ("Output", "Output landscape patch information", "self.OnMenuCmd", "r.le.trace"),
                )),
              ("Landscape patch analysis", (
                ("Set up sampling and analysis framework", "Configure and create patch map for analysis", "self.OnMenuCmd", "r.li.setup"),
                ("","","", ""),
                ("Edge density", "Calculate edge density index using a 4 neighbour algorithm", "self.OnMenuCmd", "r.li.edgedensity"),
                ("Contrast weighted edge density", "Calculate contrast weighted edge density index", "self.OnMenuCmd", "r.li.cwed"),
                ("","","", ""),
                ("Patch area mean", "Calculate mean patch size index using a 4 neighbour algorithm", "self.OnMenuCmd", "r.li.mps"),
                ("Patch area range", "Calculate range of patch area size", "self.OnMenuCmd", "r.li.padrange"),
                ("Patch area SD", "Calculate standard deviation of patch area", "self.OnMenuCmd", "r.li.padsd"),
                ("Patch area CV", "Calculate coefficient of variation of patch area", "self.OnMenuCmd", "r.li.padcv"),
                ("Patch density", "Calculate patch density index using a 4 neighbour algorithm", "self.OnMenuCmd", "r.li.patchdensity"),
                ("Patch number", "Calculate patch number index using a 4 neighbour algorithm", "self.OnMenuCmd", "r.li.patchnum"),
                ("","","", ""),
                ("Dominance's diversity", "Calculate Dominance's diversity index", "self.OnMenuCmd", "r.li.dominance"),
                ("Shannon's diversity", "Calculate Shannon's diversity index", "self.OnMenuCmd", "r.li.shannon"),
                ("Simpson's diversity", "Calculate Simpson's diversity index", "self.OnMenuCmd", "r.li.simpson"),
                ("","","", ""),
                ("Richness", "Calculate richness index", "self.OnMenuCmd", "r.li.richness"),
                ("Shape index", "Calculate shape index", "self.OnMenuCmd", "r.li.shape"),
                )),
              ("Wildfire modeling", (
                ("Generate rate of spread (ROS) maps", "Generate rate of spread (ROS) maps", "self.OnMenuCmd", "r.ros"),
                ("Generate least-cost spread paths", "Generate least-cost spread paths", "self.OnMenuCmd", "r.spreadpath"),
                ("Simulate anisotropic spread phenomena", "Simulate anisotropic spread phenomena", "self.OnMenuCmd", "r.spread"),
                )),
              ("","","", ""),
              ("Change category values and labels", (
                ("Interactively edit category values (not functional)", "Edit category values of individual cells for displayed raster map", "self.OnMenuCmd", "d.rast.edit"),
                ("","","", ""),
                ("Reclassify by size", "Reclassify categories for areas of specified sizes", "self.OnMenuCmd", "r.reclass.area"),
                ("Reclassify using rules", "Reclassify categories interactively with rules", "self.RulesCmd", "r.reclass"),
                ("Reclassify using rules file", "Reclassify categories using rules file", "self.OnMenuCmd", "r.reclass.file"),
                ("","","", ""),
                ("Recode using rules", "Recode categories interactively with rules (create new map)", "self.RulesCmd", "r.recode"),
                ("Recode using rules file", "Recode categories using rules file (create new map)", "self.OnMenuCmd", "r.recode.file"),
                ("","","", ""),
                ("Rescale", "Rescale categories (create new map)", "self.OnMenuCmd", "r.rescale"),
                ("Rescale with histogram", "Rescale categories with equalized histogram (create new map)", "self.OnMenuCmd", "r.rescale.eq"),
                )),
              ("","","", ""),
              ("Generate concentric circles around points", "Generate concentric circles around points", "self.OnMenuCmd", "r.circle"),
              ("Generate random raster cells", (
                ("Generate random cells", "Generate random cells", "self.OnMenuCmd", "r.random.cells"),
                ("Generate random cells and vector points from raster map", "Generate random cells and vector points from raster map", "self.OnMenuCmd", "r.random"),
                )),
              ("Generate surfaces", (
                ("Fractal surface", "Generate fractal surface", "self.OnMenuCmd", "r.surf.fractal"),
                ("","","", ""),
                ("Gaussian kernel surface", "Generate density surface using moving Gaussian kernel", "self.OnMenuCmd", "v.kernel"),
                ("Gaussian deviates surface", "Generate gaussian deviates surface", "self.OnMenuCmd", "r.surf.gauss"),
                ("","","", ""),
                ("Plane", "Generate plane", "self.OnMenuCmd", "r.plane"),
                ("","","", ""),
                ("Random deviates surface", "Generate random deviates surface", "self.OnMenuCmd", "r.surf.random"),
                ("Random surface with spatial dependence", "Generate random surface with spatial dependence", "self.OnMenuCmd", "r.random.surface"),
                )),
              ("Generate contour lines", "Generate vector contour lines", "self.OnMenuCmd", "r.contour"),
              ("Interpolate surfaces", (
                ("Bilinear from raster points", "Bilinear interpolation from raster points", "self.OnMenuCmd", "r.bilinear"),
                ("Inverse distance weighted from raster points", "Inverse distance weighted interpolation from raster points", "self.OnMenuCmd", "r.surf.idw"),
                ("Raster contours", "Interpolation from raster contour", "self.OnMenuCmd", "r.surf.contour"),
                ("","","", ""),
                ("Bilinear/bicubic from vector points", "Bicubic and bilinear interpolation with Tykhonov regularization from vector points", "self.OnMenuCmd", "v.surf.bspline"),
                ("Inverse distance weighted from vector points", "Inverse distance weighted interpolation from vector points", "self.OnMenuCmd", "v.surf.idw"),
                ("Regularized spline tension from vector points or contours (WHICH COMMAND ?)", "Regularized spline tension interpolation from vector points or contours", "self.OnMenuCmd", "v.surf.rst"),
                ("","","", ""),
                ("Fill NULL cells", "Fill NULL cells by interpolation using regularized spline tension", "self.OnMenuCmd", "r.fillnulls"),
                )),
              ("","","", ""),
              ("Report and statistics", (
                ("Report basic information", "Report basic file information", "self.OnMenuCmd", "r.info"),
                ("Report category information", "Report category labels and values", "self.OnMenuCmd", "r.cats"),
                ("","","", ""),
                ("General statistics", "General statistics", "self.OnMenuCmd", "r.stats"),
                ("Range of category values", "Range of all category values", "self.OnMenuCmd", "r.describe"),
                ("Sum cell category values", "Sum all cell category values", "self.OnMenuCmd", "r.sum"),
                ("Sum area by map and category", "Sum area by map and category", "self.OnMenuCmd", "r.report"),
                ("Statistics for clumped cells", "Summary statistics for clumped cells (work with r.clump)", "self.OnMenuCmd", "r.volume"),
                ("Total corrected area", "Total surface area corrected for topography", "self.OnMenuCmd", "r.surf.area"),
                ("Univariate statistics", "Univariate statistics", "self.OnMenuCmd", "r.univar"),
                ("","","", ""),
                ("Sample transects", "Sample values along transects", "self.OnMenuCmd", "r.profile"),
                ("Sample transects (bearing/distance)", "Sample values along transects (use azimuth, distance)", "self.OnMenuCmd", "r.transect"),
                ("","","", ""),
                ("Covariance/correlation", "Covariance/correlation", "self.OnMenuCmd", "r.covar"),
                ("Linear regression", "Linear regression between 2 maps", "self.OnMenuCmd", "r.regression.line"),
                ("Mutual category occurrences", "Mutual category occurrences (coincidence)", "self.OnMenuCmd", "r.coin"),
                )),
              ("","","", "")
              )),
          ("Vector", (
              ("Develop map", (
                ("Digitize", "Digitize vector", "self.OnMenuCmd", "v.digit"),
                ("","","", ""),
                ("Create/Rebuild topology", "Create/Rebuild topology", "self.OnMenuCmd", "v.build"),
                ("Clean vector", "clean vector files", "self.OnMenuCmd", "v.clean"),
                ("","","", ""),
                ("Break lines", "Break lines at intersections", "self.OnMenuCmd", "v.topo.check"),
                ("Build polylines", "Build polylines from adjacent segments", "self.OnMenuCmd", "v.build.polylines"),
                ("Split polylines", "Split polylines into segments", "self.OnMenuCmd", "v.segment"),
                ("Parallel lines", "Create lines parallel to existing lines", "self.OnMenuCmd", "v.parallel"),
                ("","","", ""),
                ("Convert feature types", "Convert vector feature types", "self.OnMenuCmd", "v.type"),
                ("Convert 2D to 3D", "Convert 2D vector to 3D by sampling raster", "self.OnMenuCmd", "v.drape"),
                ("Extrude 3D", "Extrude 2D vector into 3D vector", "self.OnMenuCmd", "v.extrude"),
                ("","","", ""),
                ("Create labels", "Create text label file for vector features", "self.OnMenuCmd", "v.label"),
                ("","","", ""),
                ("Reproject vector", "Reproject vector from other location", "self.OnMenuCmd", "v.proj"),
                ("","","", "")
                )),
              ("","","", ""),
              ("vector<->database connections", (
                ("Create new vector as link to external OGR layer", "Create new vector as link to external OGR layer", "self.OnMenuCmd", "v.external"),
                ("Set database connection for vector attributes", "Set database connection for vector attributes", "self.OnMenuCmd", "v.db.connect"),
                )),
              ("Query attributes", "Query by attributes", "self.OnMenuCmd", "v.extract"),
              ("Query coordinate(s)", "Query by coordinate(s)", "self.OnMenuCmd", "v.what"),
              ("Query map features", "Query by map features", "self.OnMenuCmd", "v.select"),
              ("","","", ""),
              ("Buffers", "Create vector buffers", "self.OnMenuCmd", "v.buffer"),
              ("Linear referencing", (
                ("Create LRS", "Create linear reference system", "self.OnMenuCmd", "v.lrs.create"),
                ("Create stationing", "Create stationing from input lines, and linear reference system", "self.OnMenuCmd", "v.lrs.label"),
                ("Create points/segments", "Create points/segments from input lines, linear reference system and positions read from stdin", "self.OnMenuCmd", "v.lrs.segment"),
                ("Find line id and offset", "Find line id and real km+offset for given points in vector map using linear reference system", "self.OnMenuCmd", "v.lrs.where"),
                )),
              ("Nearest features", "Locate nearest feature to points or centroids", "self.OnMenuCmd", "v.distance"),
              ("Network analysis", (
                ("Allocate subnets", "Allocate subnets", "self.OnMenuCmd", "v.net.alloc"),
                ("Network maintenance", "Network maintenance", "self.OnMenuCmd", "v.net"),
                ("Shortest route", "Shortest route", "self.OnMenuCmd", "v.net.path"),
                ("Shortest route visualization (requires XTErm)", "Shortest route (visualization only)", "self.OnXTerm", "d.path"),
                ("Split net", "Split net into bands between cost isolines", "self.OnMenuCmd", "v.net.iso"),
                ("Steiner tree", "Steiner tree", "self.OnMenuCmd", "v.net.steiner"),
                ("Traveling salesman analysis", "Traveling salesman analysis", "self.OnMenuCmd", "v.net.salesman"),
                )),
              ("Overlay maps", (
                ("Overlay maps", "Overlay/combine 2 vector maps", "self.OnMenuCmd", "v.overlay"),
                ("Patch maps", "Patch multiple maps (combine)", "self.OnMenuCmd", "v.patch"),
                )),
              ("Generate area for current region", "Generate area feature for extent of current region", "self.OnMenuCmd", "v.in.region"),
              ("Generate grid", "Generate rectangular vector grid", "self.OnMenuCmd", "v.mkgrid"),
              ("","","", ""),
              ("Change attributes", (
                ("Manage or report categories", "Attach/delete, or report categories", "self.OnMenuCmd", "v.category"),
                ("Reclassify vector using rules", "Reclassify features interactively using rules", "self.RulesCmd", "v.reclass"),
                ("Reclassify vector using rules file", "Reclassify features using rules file", "self.OnMenuCmd", "v.reclass"),
                )),
              ("","","", ""),
              ("Work with vector points", (
                ("Generate points", (
                    ("Generate from database", "Generate points from database", "self.OnMenuCmd", "v.in.db"),
                    ("Generate random", "Generate random points", "self.OnMenuCmd", "v.random"),
                    ("Perturb points", "Random location perturbations of points", "self.OnMenuCmd", "v.perturb"),
                    )),
                ("Generate areas from points", (
                    ("Convex hull", "Generate convex hull for point set", "self.OnMenuCmd", "v.hull"),
                    ("Delaunay triangles", "Generate Delaunay triangles for point set", "self.OnMenuCmd", "v.delaunay"),
                    ("Voronoi diagram/Thiessen polygons", "Generate Voronoi diagram/Thiessen polygons for point set", "self.OnMenuCmd", "v.voronoi"),
                    )),
                ("Sample raster maps", (
                    ("Calculate statistics for raster map overlain by vector map", "Calculate statistics for raster map overlain by vector map", "self.OnMenuCmd", "v.rast.stats"),
                    ("Sample raster maps at point locations", "Sample raster maps at point locations", "self.OnMenuCmd", "v.what.rast"),
                    ("Sample raster neighborhood around points", "Sample raster neighborhood around points", "self.OnMenuCmd", "v.sample"),
                    )),
                ("Test/training sets", "Partition points into test/training sets for k-fold cross validation", "self.OnMenuCmd", "v.kcv"),
                ("Update point attributes from areas", "Transfer attribute data from queried area map to points", "self.OnMenuCmd", "v.what.vect"),
                )),
              ("","","", ""),
              ("Reports and statistics", (
                ("Basic information", "Basic information", "self.OnMenuCmd", "v.info"),
                ("Report topology or load to attributes", "Load vector attributes to database or create reports", "self.OnMenuCmd", "v.to.db"),
                ("Report topology by category", "Report areas for vector attribute categories", "self.OnMenuCmd", "v.report"),
                ("Univariate statistics", "Univariate statistics", "self.OnMenuCmd", "v.univar"),
                ("","","", ""),
                ("Test normality", "Test normality of point distribution", "self.OnMenuCmd", "v.normal"),
                ("Calculate stats for raster map underlying vector objects", "Calculate stats for raster map underlying vector objects", "self.OnMenuCmd", "v.rast.stats"),
                ("Quadrat indices", "Indices of point counts in quadrats", "self.OnMenuCmd", "v.qcount"),
                )),
              ("","","", "")
              )),
          ("Image", (
              ("Develop images and groups", (
                ("Create/edit imagery group", "Create/edit imagery group", "self.OnMenuCmd", "i.group"),
                ("Target imagery group", "Target imagery group", "self.OnMenuCmd", "i.target"),
                ("","","", ""),
                ("Mosaic up to 4 adjacent images", "Mosaic up to 4 adjacent images", "self.OnMenuCmd", "i.image.mosaic"),
                )),
              ("Manage image colors", (
                ("Color balance and enhance color tables of multiband imagery for rgb display", "Color balance and enhance color tables of multiband imagery for rgb display", "self.OnMenuCmd", "i.landsat.rgb"),
                ("Transform HIS (Hue/Intensity/Saturation) color image to RGB (Red/Green/Blue)", "Transform HIS (Hue/Intensity/Saturation) color image to RGB (Red/Green/Blue)", "self.OnMenuCmd", "i.his.rgb"),
                ("Transform RGB (Red/Green/Blue) color image to HIS (Hue/Intensity/Saturation)", "Transform RGB (Red/Green/Blue) color image to HIS (Hue/Intensity/Saturation)", "self.OnMenuCmd", "i.rgb.his"),
                )),
              ("Rectify and georeference image group", (
                ("Set ground control points (GCP's) from raster map or keyboard entry", "Set ground control points (GCP's) from raster map or keyboard entry", "self.OnMenuCmd", "i.points"),
                ("Set ground control points (GCP's) from vector map or keyboard entry", "Set ground control points (GCP's) from vector map or keyboard entry", "self.OnMenuCmd", "i.vpoints"),
                ("Affine and Polynomial rectification (rubber sheet)", "Affine and Polynomial rectification (rubber sheet)", "self.OnMenuCmd", "i.rectify"),
                ("Ortho Photo rectification", "Ortho Photo rectification", "self.OnMenuCmd", "i.ortho.photo"),
                )),
              ("","","", ""),
              ("Brovey transformation and pan sharpening", "Brovey transformation and pan sharpening", "self.OnMenuCmd", "i.fusion.brovey"),
              ("Classify image", (
                ("Clustering input for unsupervised classification", "Clustering input for unsupervised classification", "self.OnMenuCmd", "i.cluster"),
                ("","","", ""),
                ("Maximum likelihood Classification (MLC)", "Maximum likelihood Classification (MLC)", "self.OnMenuCmd", "i.maxlik"),
                ("Sequential maximum a posteriori classification (SMAP)", "Sequential maximum a posteriori classification (SMAP)", "self.OnMenuCmd", "i.smap"),
                ("","","", ""),
                ("Interactive input for supervised classification", "Interactive input for supervised classification", "self.OnMenuCmd", "i.class"),
                ("Non-interactive input for supervised classification (MLC)", "Non-interactive input for supervised classification (MLC)", "self.OnMenuCmd", "i.gensig"),
                ("Non-interactive input for supervised classification (SMAP)", "Non-interactive input for supervised classification (SMAP)", "self.OnMenuCmd", "i.gensigset"),
                )),
              ("Filter image", (
                ("Zero edge crossing detection", "Zero edge crossing detection", "self.OnMenuCmd", "i.zc"),
                ("User defined matrix/convolving filter", "User defined matrix/convolving filter", "self.OnMenuCmd", "r.mfilter"),
                )),
              ("Histogram image", "Histogram image", "self.DispHistogram", ""),
              ("Spectral response", "Spectral response", "self.OnMenuCmd", "i.spectral"),
              ("Tasseled cap vegetation index", "Tasseled cap vegetation index", "self.OnMenuCmd", "i.tasscap"),
              ("Transform image", (
                ("Canonical component", "Canonical component", "self.OnMenuCmd", "i.cca"),
                ("Principal component", "Principal component", "self.OnMenuCmd", "i.pca"),
                ("Fast Fourier Transform", "Fast Fourier Transform", "self.OnMenuCmd", "i.fft"),
                ("Inverse Fast Fourier Transform", "Inverse Fast Fourier Transform", "self.OnMenuCmd", "i.ifft"),
                )),
              ("","","", ""),
              ("Report and statistics", (
                ("Report basic file information", "Report basic file information", "self.OnMenuCmd", "r.info"),
                ("Range of image values", "Range of image values", "self.OnMenuCmd", "r.describe"),
                ("","","", ""),
                ("Bit pattern comparison for ID of low quality pixels", "Bit pattern comparison for ID of low quality pixels", "self.OnMenuCmd", "r.bitpattern"),
                ("Kappa classification accuracy assessment", "Kappa classification accuracy assessment", "self.OnMenuCmd", "r.kappa"),
                ("Optimum index factor for LandSat TM", "Optimum index factor for LandSat TM", "self.OnMenuCmd", "i.oif"),
                )),
              ("","","", "")
              )),
          ("Database", (
              ("Query", "Query database", "self.Nomethod", ""),
              ("","","", "")
              )))]










