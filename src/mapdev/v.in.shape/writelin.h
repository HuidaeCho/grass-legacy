/******************************************************************************
 * writelin.h
 * header file containing prototypes for routines to
 * write vector data into GRASS database

 * @Copyright David D.Gray <ddgray@armadce.demon.co.uk>
 * 8th. Apr. 2000
 * Last updated 8th. Apr. 2000
 *

* This file is part of GRASS GIS. It is free software. You can 
* redistribute it and/or modify it under the terms of 
* the GNU General Public License as published by the Free Software
* Foundation; either version 2 of the License, or (at your option)
* any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

 ******************************************************************************/

#include "shp2dig.h"
#include "btree.h"

/* Structure prototypes */


/* Function prototypes */

int vbase2segd( segmentList *seg0, BTREE *btr0 );
