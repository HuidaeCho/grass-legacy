/*C
 * Original project: Lars Arge, Jeff Chase, Pat Halpin, Laura Toma, Dean
 *		     Urban, Jeff Vitter, Rajiv Wickremesinghe 1999
 * 
 * GRASS Implementation: Lars Arge, Helena Mitasova, Laura Toma 2002
 *
 * Copyright (c) 2002 Duke University -- Laura Toma 
 *
 * Copyright (c) 1999-2001 Duke University --
 * Laura Toma and Rajiv Wickremesinghe
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Duke University
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE TRUSTEES AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE TRUSTEES OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *C*/


#include <stdio.h>
#include <iostream.h>
#include <assert.h>
#include <math.h>

#include "weightWindow.h"
#include "direction.h"


/* #define CHECK_WEIGHTS   */
/* enables printing weights as they are computed */

/* 
   Distribute flow to neighbors as in "The prediction of HIllslope
   Flow Paths for Distributed Hydrollogical Modeling using Digital
   Terrain Models" by Quinn, Chevallier, Planchon, in Hydrollogical
   Processes vol. 5, 1991

 */



/***************************************************************/
weightWindow::weightWindow(const float dx, const float dy) :
  cell_dx(dx), cell_dy(dy) {
  
  celldiag = sqrt(dx*dx + dy*dy);
  sumweight = sumcontour = 0;
}



/***************************************************************/
/* initialize all weights to 0 */
void
weightWindow::init() {
  sumweight = sumcontour = (float)0;
  for (int l = 0;l < 9; l++) {
    weight.set(l,(float)0);
  }
}

/***************************************************************/
/* set weight of neighbor (di,dj) equal to e_diff x
   computeContour/computeDist. This basically reduces to e_diff/2 (if
   not on diagonal) or e_diff/4 (if on diagonal). 
*/


void
weightWindow::computeWeight(const short di, const short dj, 
			    const elevation_type elev_crt,
			    const elevation_type elev_neighb) {

  /* NOTE: it is possible that elev_neighb is EDGE_NODATA. In this
     case, we just consider the value of EDGE_NODATA as an elevation,
     and push flow to it. Currently the value of EDGE_NODATA is -9998,
     and thus these cells will get most of the flow. */
  
  elevation_type e_diff = elev_crt - elev_neighb;
  assert(e_diff >= 0);
  if (di == 0 && dj == 0) {
    return;  
  }

  double contour, flow;

  if (dj==0) {
    flow = 0.5;
    contour = cell_dy/2;
  } else  if (di ==0) {
    flow = 0.5;
    contour = cell_dx/2;
  } else { /* on diagonal */
    flow = 0.25;
    contour = celldiag/4;
  }
  assert(contour > 0);

  /* at this point, 'flow' corresponds to the relative distance to the
   neighbor: 0.5 if horizontal/vertical, or 0.25 if diagonal. Diagonal
   points are further away. These are somewhat arbitrary; see paper.
   'contour' is the length perpendicular to the flow toward the
   neighbor. */
  
  if (e_diff > 0) {
    flow *= e_diff;
  } else {
    /* NOTE: how much flow to distribute to neighbors which are
       at same height?? */
    flow *=  1.0/contour; /* NOTE: this may cause overflow if contour
                             is v small */
  }
  weight.set(di, dj, flow);
  sumcontour += contour;
  sumweight += flow;
}



/***************************************************************/
/* computes and returns the distance corresponding to this direction */
double
weightWindow::computeDist(const short di, const short dj) {
  double dist;
  
  if (di == 0 && dj == 0) {
    return 0;
  }
  if (dj==0) {
    dist = cell_dy;
  } else  if (di ==0) {
    dist = cell_dx;
  } else { /* on diagonal */
    dist = celldiag;
  }
  assert(dist > 0);
  return dist;
}  

/***************************************************************/
/* computes and returns the contour corresponding to this direction */
double
weightWindow::computeContour(const short di, const short dj) {
  double contour;

  if (di == 0 && dj == 0) {
    return 0;
  }
  if (dj==0) {
    contour = cell_dy/2;
  } else  if (di ==0) {
    contour = cell_dx/2;
  } else { /* on diagonal */
    contour = celldiag/4;
  }
  assert(contour > 0);
  return contour;
}  

/***************************************************************/
/* compute the tanB corresponding to the elevation window and neighbor
   di,dj. */
double 
weightWindow::computeTanB(const short di,const short dj, 
			  const genericWindow<elevation_type>& elevwin) {
  
  assert(di != 0 || dj != 0);
  double dist = computeDist(di, dj);
  assert(dist > 0);
  return (elevwin.get() - elevwin.get(di, dj)) / dist;
}




/***************************************************************/
void
weightWindow::normalize() {
  if (sumweight > 0) {
    weight.scalarMultiply(1.0/sumweight);
  }
}


/***************************************************************/
/* compute the weights of the neighbors of a cell given an elevation
   window and precomputed directions dir; if trustdir = 1 then trust
   directions; otherwise push to all downslope neighbors and use dir
   only for cells which do not have any downslope neighbors */
/***************************************************************/
void 
weightWindow::compute(const dimension_type i, const dimension_type j,
		      const genericWindow<elevation_type>& elevwin, 
		      const direction_type dir,
		      const int trustdir) {
  
  elevation_type elev_crt, elev_neighb, e_diff;
  dimension_type i_neighb, j_neighb;
  
  /* initialize all weights to 0 */
  init();

  elev_crt = elevwin.get(); 
  assert(!is_nodata(elev_crt));
  
  /* map direction to neighbors */
  directionWindow dirwin(dir);  

  /* compute weights of the 8 neighbours  */
  int skipit = 0;
  for (short di = -1; di <= 1; di++) {
    for (short dj = -1; dj <= 1; dj++) {
      
      /* grid coordinates and elevation of neighbour */
      i_neighb = i + di; 
      j_neighb = j + dj;
      elev_neighb = elevwin.get(di, dj);
      e_diff = (elevation_type)(elev_crt - elev_neighb);

      skipit = ((di ==0) && (dj==0));
      skipit |= (elev_crt < elev_neighb);
      /* skipit |= (elev_neighb == edge_nodata); ?? */

      if (!trustdir) {
	dirwin.correctDirection(di,dj,skipit, i,j, elev_crt,dir,elev_neighb);
      }
      
      /* if direction points to it then compute its weight */
      if (dirwin.get(di,dj) == true) {
	computeWeight(di,dj, elev_crt, elev_neighb);
      }
    } /* for dj */
  } /* for di */
  normalize(); /* normalize the weights */
  
#ifdef CHECK_WEIGHTS 
  cout <<"weights: [";
  for (int l=0;l<9;l++) cout << form("%3.2f ",weight.get(l));
  cout <<"]\n";
#endif
};



/* Find the dominant direction. Set corresponding weight to 1, and
   sets all other weights to 0. Set sumweight and sumcontour.*/
void 
weightWindow::makeD8(const dimension_type i, const dimension_type j,
		     const genericWindow<elevation_type>& elevwin, 
		     const direction_type dir,
		     const bool trustdir) {
  

  elevation_type elev_crt;
  short di,dj;
  elev_crt = elevwin.get(); 
  assert(!is_nodata(elev_crt));

  int maxi=0, maxj=0;
  double tanb, contour, maxtanb = -1, maxcontour = -1;
  /* map direction to neighbors */
  directionWindow dirwin(dir); 

  /* compute biggest angle to a neighbor */
  for (di=-1; di <=1; di++) {
    for (dj = -1; dj <= 1; dj++) {
      if (dirwin.get(di,dj)) {
	
	tanb = computeTanB(di,dj, elevwin);
	contour = computeContour(di, dj);
	
	if (tanb > maxtanb) {
	  maxtanb = tanb;
	  maxi = di;
	  maxj = dj;
	  maxcontour = contour;
	}
      }
    }
  }
  /* at this point maxi and maxj must be set */
  assert((maxi != 0 || maxj != 0) && maxtanb >= 0);
  
  /* set weights corresponding to this direction */
  init();   /* initialize all weights to 0 */
  int maxindex = 3* (maxi + 1) + maxj+1;
  weight.set(maxindex,1);   /* set maxweight to 1 */

  sumweight = 1;
  sumcontour = maxcontour;
}

