
MODULE_TOPDIR = ../..

PGM=v.out.ogr
EXTRA_CFLAGS = $(GDALCFLAGS)

OBJS = main.o

LIBES     = $(VECTLIB) $(GPROJLIB) $(GISLIB) $(GDALLIBS)
DEPENDENCIES = $(VECTDEP) $(GPROJDEP) $(GISDEP)
EXTRA_INC = $(VECT_INC) $(PROJINC)
EXTRA_CFLAGS = $(VECT_CFLAGS)
 
include $(MODULE_TOPDIR)/include/Make/Module.make

default: cmd

