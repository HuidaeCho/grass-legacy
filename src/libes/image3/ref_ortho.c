/* orthoref.c */
/***********************************************************************
 * I_compute_ortho_equations()
 * I_orthoref()
 * I_inverse_orthoref()
 ***********************************************************************/

#include "ortho_image.h"
#include "matrix_ops.h"

#include <math.h>
#include <signal.h>
#include <stdio.h>

#define MAX_ITERS   10     /* Max iteration is normal equation solution */
#define CONV_LIMIT  1.0    /* meters */


#define DEBUG 1
#ifdef DEBUG
FILE *debug_file; 
char  msg[120];
#endif

static int floating_exception;
static void catch(int);



/* Compute the ortho rectification parameters */
/* XC, YC, ZC, Omega, Phi, Kappa */
/*-------------------------------------------------------------------
RETURNS:
      -1: floating point exception (not good)
       0: not enough points
       1: all ok.
-------------------------------------------------------------------*/
int 
I_compute_ortho_equations (Control_Points_3D *cpz, Auxillary_Photo *auxil, Coeffs_Photo *coefs)
{
    Camera            *cam_info;
    Camera_Expose     *init_info;
    double *XC, *YC, *ZC;
    double *Omega, *Phi, *Kappa; 

    MATRIX delta, epsilon, B, BT, C, E, N, CC, NN, UVW, XYZ, M, WT1;
    double meanx, meany;                         
    double X1,X2,x1,x2,Z1,Y1,Y2,y1,y2, dist_grnd, dist_photo;            
    double x,y,z,X,Y,Z,Xp,Yp,CFL;
    double lam,mu,nu,U,V,W;
    double xbar, ybar;
    double m11,m12,m13,m21,m22,m23,m31,m32,m33;
    double sw,cw,sp,cp,sk,ck;
    double dx,dy,dz,dd;
    double Q1;
    double kappa1, kappa2, XC_var, YC_var, ZC_var; 
    double omega_var, phi_var, kappa_var;
    int i, iter, n;
    int first, active;
    void (*sigfpe)();
    Q1 = (double) 1.0;

/*
 *  floating_exception = 0;
 *  sigfpe = signal (SIGFPE, catch);
*/

   cam_info = &auxil->camera;
   init_info = &auxil->camera_expose;
   XC = &coefs->XC;
   YC = &coefs->YC;
   ZC = &coefs->ZC;
   Omega = &coefs->omega;
   Phi   = &coefs->phi;
   Kappa = &coefs->kappa;

/* DEBUG */
#ifdef DEBUG
    debug_file = (FILE *) fopen("ortho_compute.rst", "w"); 
    if (debug_file == NULL) {
       sprintf (msg, "Cant open debug file ortho_analyze.rst\n");
       G_fatal_error (msg);
    } 
#endif

    /*  Need 4 active control points */
    active = 0;
    for (i = 0; i < cpz->count; i++)
    {   
         if (cpz->status[i] > 0)
	    active++;
    }
    if (active <  4)
    { 
#ifdef DEBUG 
       fclose(debug_file);
#endif
       return (0);   /* not enough points */
    }
    
/*  Initialize (and zero out) all matrices needed */
/*  Format is delta = [XC,YC,ZC,Omega,Phi,Kappa]-transpose  */

		/*  Normal Equation Matrix */
    N.nrows = 6;
    N.ncols = 6;
    m_zero ( &N);
		/*  Sum of Normal Equation Matrix */
    NN.nrows = 6;
    NN.ncols = 6;
    m_zero ( &NN);
		/*  Partial derivates of observation equations */
    B.nrows = 2;
    B.ncols = 6;
    m_zero ( &B);
		/*  Transpose of B */
    BT.nrows = 6;
    BT.ncols = 2;
    m_zero ( &BT);
		/*  Partial solution matrix */
    C.nrows = 6;
    C.ncols = 1;
    m_zero ( &C);
		/*  Sum of Partial solution matrix */
    CC.nrows = 6;
    CC.ncols = 1;
    m_zero ( &CC);
		/*  Residual matrix */
    E.nrows = 2;
    E.ncols = 1;
    m_zero ( &E);
		/*  delta Matrix  - [XC,YC,ZC,Omega,Phi,Kappa]-transpose */
    delta.nrows = 6;
    delta.ncols = 1;
    m_zero ( &delta);
		/*  corrections to delta matrix */
    epsilon.nrows = 6;
    epsilon.ncols = 1;
    m_zero ( &epsilon);
		/*  Object Space Coordinates (X,Y,Z) */
    XYZ.nrows = 3;
    XYZ.ncols = 1;
    m_zero ( &XYZ);
		/*  Image Space coordinates  (u,v,w) */
    UVW.nrows = 3;
    UVW.ncols = 1;
    m_zero ( &UVW);
		/* Oreintaiton Matrix  M=[3,3] functions of (omega,phi,kappa) */
    M.nrows = 3;
    M.ncols = 3;
    m_zero ( &M);
		/*  Weight Matrix for delta */
		/*  Weights set to identity matrix unless */ 
    WT1.nrows = 6;
    WT1.ncols = 6;
    m_zero ( &WT1);

/******************** Start the solution *****************************/

    /* set Xp, Yp, and CFL form cam_info */
    Xp = cam_info->Xp;
    Yp = cam_info->Yp;
    CFL= cam_info->CFL;

#ifdef DEBUG
    fprintf (debug_file,  "CAMERA INFO:\n");
    fprintf (debug_file,  "\txp = %f  \typ = %f \tCFL  = %f \n",Xp,Yp,CFL);
#endif

/* use initial estiamtes for XC,YC,ZC,omega,phi,kappa             */
/* and initial standard deviations if proveded by i.ortho.initial */
/*                                                                */
/* otherwise set from mean value of all active control points     */
   if ((init_info != NULL) && (init_info->status ==1))
   {
      /* Have initial values */
      *XC = init_info->XC_init;
      *YC = init_info->YC_init;
      *ZC = init_info->ZC_init;
      *Omega = init_info->omega_init;
      *Phi   = init_info->phi_init;
      *Kappa = init_info->kappa_init;

      /* weight matrix computed from initial standard variances */
      WT1.x[0][0] = (Q1/(init_info->XC_var * init_info->XC_var));
      WT1.x[1][1] = (Q1/(init_info->YC_var * init_info->YC_var));
      WT1.x[2][2] = (Q1/(init_info->ZC_var * init_info->ZC_var));
      WT1.x[3][3] = (Q1/(init_info->omega_var * init_info->omega_var));
      WT1.x[4][4] = (Q1/(init_info->phi_var * init_info->phi_var));
      WT1.x[5][5] = (Q1/(init_info->kappa_var * init_info->kappa_var));
   }
   else  /* set from mean values of active control points */
   {
      
      /* set intial XC and YC from mean values of control points */
      meanx = meany = 0;
      n = 0;
      first = 1;
      for (i = 0; i < cpz->count; i++)
      {   
         if (cpz->status[i] <= 0)
	    continue;

          /* set initial ZC */
         if (first)
         {   X1 = *(cpz->e2);
             x1 = *(cpz->e1);
             Y1 = *(cpz->n2);
             y1 = *(cpz->n1);
             Z1 = *(cpz->z2);
             first = 0;
         }
         if (!first)
         {   X2 = *(cpz->e2);
             x2 = *(cpz->e1);
             Y2 = *(cpz->n2);
             y2 = *(cpz->n1);
             dist_photo = sqrt (((x1-x2)*(x1-x2)) + ((y1-y2)*(y1-y2)));
             dist_grnd  = sqrt (((X1-X2)*(X1-X2)) + ((Y1-Y2)*(Y1-Y2)));
         }
 
         n++;
         meanx  += *((cpz->e2)++);
         meany  += *((cpz->n2)++);
         ((cpz->e1)++);
         ((cpz->n1)++);
       }
       *XC = meanx / n;
       *YC = meany / n;

       /* reset pointers */
       for (i = 0; i < cpz->count; i++)
       {     
         if (cpz->status[i] <= 0)
	    continue;
         (cpz->e1)--;
         (cpz->e2)--;
         (cpz->n1)--;
         (cpz->n2)--;
       }

       /* set initial ZC from:                                  */ 
       /*    scale ~= dist_photo/dist_grnd  ~= (CFL)/(Z1 - Zc)  */
       /*       Zc ~= Z1 + CFL(dist_grnd)/(dist_photo)          */
        
        *ZC =  Z1 +  (CFL*(dist_grnd)/(dist_photo));
      
       /* set initial rotations to zero (radians)*/
        *Omega = *Phi = 0.0;
        
       /* find an initial kappa */
         kappa1 =  atan2((y2-y1),(x2-x1));
         kappa2 =  atan2((Y2-Y1),(X2-X1));
        *Kappa  =  (kappa2 - kappa1);
     
       /* set initial weights */ 
         XC_var = INITIAL_X_VAR; 
         YC_var = INITIAL_Y_VAR; 
         ZC_var = INITIAL_Z_VAR; 
         omega_var = INITIAL_OMEGA_VAR;
         phi_var   = INITIAL_PHI_VAR;
         kappa_var = INITIAL_KAPPA_VAR;
 
         WT1.x[0][0] = (Q1/(XC_var * XC_var));
         WT1.x[1][1] = (Q1/(YC_var * YC_var));
         WT1.x[2][2] = (Q1/(ZC_var * ZC_var));
         WT1.x[3][3] = (Q1/(omega_var * omega_var));
         WT1.x[4][4] = (Q1/(phi_var   * phi_var));
         WT1.x[5][5] = (Q1/(kappa_var * kappa_var));
    }

#ifdef DEBUG
    fprintf (debug_file,  "\nINITIAL CAMERA POSITION:\n");
    fprintf (debug_file,  "\tXC = %f  \tYC = %f \tZC = %f \n", *XC ,*YC,*ZC);
    fprintf (debug_file,  "\tOMEGA = %f  \tPHI = %f \tKAPPA = %f \n", 
			*Omega ,*Phi,*Kappa);
#endif

   /* set initial parameters into epsilon matrix */
   epsilon.x[0][0] = *XC;
   epsilon.x[1][0] = *YC;
   epsilon.x[2][0] = *ZC;
   epsilon.x[3][0] = *Omega;
   epsilon.x[4][0] = *Phi;
   epsilon.x[5][0] = *Kappa;
    
/************************** Start Iterations *****************************/
/* itererate untill convergence */

   for ( iter = 0 ; iter <= MAX_ITERS; iter++ ) 
   { 
        /*  break if converged */
        dx = delta.x[0][0];
        dy = delta.x[1][0];
        dz = delta.x[2][0];
        dd = ((dx*dx)+(dy*dy)+(dz*dz));

        if ((iter > 0) && (dd <=  CONV_LIMIT)) break;

#ifdef DEBUG
        fprintf (debug_file,  "\n\tITERATION = %d \n",iter);
#endif

        /* value of parameters at this iteration */
        *XC = epsilon.x[0][0]; 
        *YC = epsilon.x[1][0];
        *ZC = epsilon.x[2][0];
        *Omega = epsilon.x[3][0];
        *Phi   = epsilon.x[4][0];
        *Kappa = epsilon.x[5][0];     

#ifdef DEBUG
	fprintf (debug_file, "\n\tepsilon:\n\t\tXC = \t%f \n\t\tYC = \t%f \n\t\tZC = \t%f \n\t\tomega = \t%f \n\t\tphi = \t%f \n\t\tkappa = \t%f \n\n",*XC,*YC,*ZC,*Omega,*Phi,*Kappa);
#endif
        

/*  clear NN, CC */
        m_zero (&NN);
        m_zero (&CC);

        /*   Set Oreintaion Matrix from latest vales (Omega, Phi, Kappa); */
        sw = sin (*Omega);
        cw = cos (*Omega);
        sp = sin (*Phi);
        cp = cos (*Phi);
        sk = sin (*Kappa);
        ck = cos (*Kappa);

        M.x[0][0] = (cp*ck); 
        M.x[0][1] = (cw*sk) + (sw*sp*ck);
        M.x[0][2] = (sw*sk) - (cw*sp*ck);
        M.x[1][0] = (-cp*sk);
        M.x[1][1] = (cw*ck) - (sw*sp*sk);
        M.x[1][2] = (sw*ck) + (cw*sp*sk);
        M.x[2][0] = sp;
        M.x[2][1] = (-sw*cp);
        M.x[2][2] = (cw*cp);

        /* just an abbreviation of M */
        m11 = M.x[0][0]; m12 = M.x[0][1]; m13 = M.x[0][2];
        m21 = M.x[1][0]; m22 = M.x[1][1]; m23 = M.x[1][2];
        m31 = M.x[2][0]; m32 = M.x[2][1]; m33 = M.x[2][2];

       /* Form Normal equations by sumation of all active control points */
       for (i = 0; i < cpz->count; i++)
       {    
#ifdef DEBUG
	   fprintf (debug_file, "\n\t\t\tIn Summation count = %d \n",i);
#endif

           x = *((cpz->e1))++;
           y = *((cpz->n1))++;
           /** z = *((cpz->z1))++; */
	   z = - CFL;

           X = *((cpz->e2))++;
           Y = *((cpz->n2))++;
           Z = *((cpz->z2))++;

	   if (cpz->status[i] <= 0)
	       continue;

#ifdef DEBUG
	   fprintf (debug_file, "\n\t\t\timage:\n \t\t\tx = \t%f \n\t\t\ty = \t%f \n\t\t\tz = \t%f \n\t\t\tobject:\n \t\t\tX = \t%f \n\t\t\tY = \t%f \n\t\t\tZ = \t%f \n", x,y,z,X,Y,Z);
#endif
        
           
           /* Compute Obj. Space coordinates */
           XYZ.x[0][0] = X - *XC; 
           XYZ.x[1][0] = Y - *YC; 
           XYZ.x[2][0] = Z - *ZC; 

           /* just an abbreviations */
           lam = XYZ.x[0][0];
           mu  = XYZ.x[1][0];
           nu  = XYZ.x[2][0];

           /* Compute image space coordiantes */
           m_mult (&M, &XYZ, &UVW);

           /*  just an abbreviation */
           U = UVW.x[0][0];
           V = UVW.x[1][0];
           W = UVW.x[2][0];

 
           /* Form Partial derivatives of Normal Equations */
           xbar = x - Xp;
           ybar = y - Yp;

           B.x[0][0] = (-Q1/W) *((xbar*m31)+(CFL*m11));
           B.x[0][1] = (-Q1/W) *((xbar*m32)+(CFL*m12));
           B.x[0][2] = (-Q1/W) *((xbar*m33)+(CFL*m13));
           B.x[0][3] = (Q1/W) *
                       (
                        (xbar*((-m33*mu)+(m32*nu))) +
                        (CFL*((-m13*mu)+(m12*nu)))
                       );
           B.x[0][4] = (Q1/W) *
                       (
                        (xbar*((cp*lam)+(sw*sp*mu)+(-cw*sp*nu))) +
                        (CFL*((-sp*ck*lam)+(sw*cp*ck*mu)+(-cw*cp*ck*nu)))
                       );
           B.x[0][5] = (Q1/W)*
                       (CFL*((m21*lam)+(m22*mu)+(m23*nu)));

           B.x[1][0] = (-Q1/W) *((ybar*m31)+(CFL*m21));
           B.x[1][1] = (-Q1/W) *((ybar*m32)+(CFL*m22));
           B.x[1][2] = (-Q1/W) *((ybar*m33)+(CFL*m23));
           B.x[1][3] = (Q1/W) *
                       (
                        (ybar*((-m33*mu)+(m32*nu))) +
                        (CFL*((-m23*mu)+(m22*nu)))
                       );
           B.x[1][4] = (Q1/W) *
                       (
                        (ybar*((cp*lam)+(sw*sp*mu)+(-cw*sp*nu))) +
                        (CFL*((sp*sk*lam)+(-sw*cp*sk*mu)+(cw*cp*sk*nu)))
                       );
           B.x[1][5] = (Q1/W)*
                       (CFL*((-m11*lam)+(-m12*mu)+(-m13*nu)));

           E.x[0][0] =  (-Q1)*(xbar + (CFL*(U/W)));
           E.x[1][0] =  (-Q1)*(ybar + (CFL*(V/W)));

#ifdef DEBUG
	   fprintf (debug_file, "\n\t\t\tresidual:\n \t\t\tE1 = \t%f \n\t\t\tE2 = \t%f \n", E.x[0][0],E.x[1][0]);
#endif

           /* do the summation into Normal equation and solution matrices */
           /* Find B transpose */
           m_transpose (&B, &BT);
           /* N = BT*B */
           m_mult (&BT, &B, &N); 
           /* NN += N */
           m_add (&NN, &N, &NN);  
           /* C = BT*E */
           m_mult (&BT, &E, &C);   
           /* CC += C */
           m_add (&CC, &C, &CC);            
       } /* end summation loop over all active control points */

       /* reset pointers */
       for (i = 0; i < cpz->count; i++)
       {      
         (cpz->e1)--;
         (cpz->n1)--;
         (cpz->z1)--;
         (cpz->e2)--; 
         (cpz->n2)--;
         (cpz->z2)--;
       }

#ifdef DEBUG
       fprintf (debug_file, "\n\tN: \n");
       for ( i = 0;i < 6; i++)
       fprintf (debug_file, "\t%f \t%f \t%f \t%f \t%f \t%f \n",
           NN.x[i][0], NN.x[i][1], NN.x[i][2],
           NN.x[i][3], NN.x[i][4], NN.x[i][5]);

       fprintf (debug_file, "\n\tC: \n");
       fprintf (debug_file, "\t%f \t%f \t%f \t%f \t%f \t%f \n",
           CC.x[0][0], CC.x[1][0], CC.x[2][0],
           CC.x[3][0], CC.x[4][0], CC.x[5][0]);
#endif


       /* Add weigth matrix of unknowns to NN */
       m_add (&NN, &WT1, &NN);
       /* Solve for delta */
       if (!m_inverse (&NN, &N))
       {  /* control point status becomes zero if matrix is non-invertable */ 
          m_error ("Matrix Inversion (Control Points status modified)");
          for (i = 0; i < cpz->count; i++)
              cpz->status[i] = 0;
          return (-1);	/* cant solve equations */ 
       }


       /* delta = N-1 * CC */
       m_mult (&N, &CC, &delta);
       /* epsilon += delta */
       m_add (&epsilon, &delta, &epsilon);
	
#ifdef DEBUG
       fprintf (debug_file, "\ndelta:\n  \n\t\tXC = \t%f \n\t\tYC = \t%f \n\t\tZC = \t%f \n\t\tomega = \t%f \n\t\tphi = \t%f \n\t\tkappa = \t%f \n",
           delta.x[0][0], delta.x[1][0], delta.x[2][0],
           delta.x[3][0], delta.x[4][0], delta.x[5][0]);
#endif
    
    } /* end ITERATION loop */   
    
    /* This is the solution */ 
    *XC = epsilon.x[0][0]; 
    *YC = epsilon.x[1][0];
    *ZC = epsilon.x[2][0];
    *Omega = epsilon.x[3][0];
    *Phi   = epsilon.x[4][0];
    *Kappa = epsilon.x[5][0];
       
#ifdef DEBUG
    fclose (debug_file);   
#endif

    /* equations solved fine */
    return (1);
}

static void catch (int n)
{
    floating_exception = 1;
    signal (n, catch);
}

/* given ground coordinates (e1,n1,z1) and the solution from above */
/* compute the photo coordinate (e2,n2) position */
int 
I_ortho_ref (double e1, double n1, double z1, double *e2, double *n2, double *z2, Auxillary_Photo *auxil, Coeffs_Photo *coefs)
{

    double XC, YC, ZC;
    double Omega, Phi, Kappa;
    Camera    *cam_info;
    MATRIX UVW, XYZ, M;
    double U,V,W;
    double Xp,Yp,CFL;
    double sw,cw,sp,cp,sk,ck;


   cam_info = &auxil->camera;
   XC = coefs->XC;
   YC = coefs->YC;
   ZC = coefs->ZC;
   Omega = coefs->omega;
   Phi   = coefs->phi;
   Kappa = coefs->kappa;


    /*  Initialize and zero the matrices */
    /*  Object Space Coordinates */
    XYZ.nrows = 3;
    XYZ.ncols = 1;
    m_zero ( &XYZ);
    /*  Image Space coordinates */
    UVW.nrows = 3;
    UVW.ncols = 1;
    m_zero ( &UVW);
    /*  Oreintaion Matrix */
    M.nrows = 3;
    M.ncols = 3;
    m_zero ( &M);

    /************************ Start the work ******************************/
    /* set Xp, Yp, and CFL form cam_info */
    Xp = cam_info->Xp;
    Yp = cam_info->Yp;
    CFL= cam_info->CFL;

        /*  Compute Oreintaion Matrix M from (Omega, Phi, Kappa); */
        sw = sin (Omega);
        cw = cos (Omega);
        sp = sin (Phi);
        cp = cos (Phi);
        sk = sin (Kappa);
        ck = cos (Kappa);

        M.x[0][0] = cp*ck; 
        M.x[0][1] = cw*sk + (sw*sp*ck);
        M.x[0][2] = sw*sk - (cw*sp*ck);
        M.x[1][0] = -(cp*sk);
        M.x[1][1] = cw*ck - (sw*sp*sk);
        M.x[1][2] = sw*ck + (cw*sp*sk);
        M.x[2][0] = sp;
        M.x[2][1] = -(sw*cp);
        M.x[2][2] = cw*cp;

        /* ObjSpace (&XYZ, XC,YC,ZC, X,Y,Z);*/
        XYZ.x[0][0] = e1 - XC; 
        XYZ.x[1][0] = n1 - YC; 
        XYZ.x[2][0] = z1 - ZC; 

        m_mult (&M, &XYZ, &UVW);
      
        /* Image Space */
        U = UVW.x[0][0];
        V = UVW.x[1][0];
        W = UVW.x[2][0];

   /* This is the solution */
   *e2 = (-CFL)*(U/W);
   *n2 = (-CFL)*(V/W);
   return (1);
}

/* given the photo coordiantes (e1,n1) and elevation z2  */
/* and the solution from I_compute_ortho_equation */
/* compute ground position (e2,n2) */
int 
I_inverse_ortho_ref (double e1, double n1, double z1, double *e2, double *n2, double *z2, Auxillary_Photo *auxil, Coeffs_Photo *coefs)
{
    double XC, YC, ZC;
    double Omega, Phi, Kappa;
    Camera    *cam_info;

    MATRIX UVW, XYZ, M;
    double lam,mu,nu;
    double Xp,Yp,CFL;
    double sw,cw,sp,cp,sk,ck;

   cam_info = &auxil->camera;
   XC = coefs->XC;
   YC = coefs->YC;
   ZC = coefs->ZC;
   Omega = coefs->omega;
   Phi   = coefs->phi;
   Kappa = coefs->kappa;

    /*  Initialize and zero matrices */
    /*  Object Space Coordinates */
    XYZ.nrows = 3;
    XYZ.ncols = 1;
    m_zero ( &XYZ);
    /*  Image Space coordinates */
    UVW.nrows = 3;
    UVW.ncols = 1;
    m_zero ( &UVW);
    /*  Oreintaion Matrix */
    M.nrows = 3;
    M.ncols = 3;
    m_zero ( &M);

    /********************** Start the work ********************************/
    /* set Xp, Yp, and CFL form cam_info */
    Xp = cam_info->Xp;
    Yp = cam_info->Yp;
    CFL= cam_info->CFL;

        /* Compute Oreintaion Matrix (Omega, Phi, Kappa); */
        sw = sin (Omega);
        cw = cos (Omega);
        sp = sin (Phi);
        cp = cos (Phi);
        sk = sin (Kappa);
        ck = cos (Kappa);

        /* M Transposed */
        M.x[0][0] = cp*ck; 
        M.x[1][0] = cw*sk + (sw*sp*ck);
        M.x[2][0] = sw*sk - (cw*sp*ck);
        M.x[0][1] = -(cp*sk);
        M.x[1][1] = cw*ck - (sw*sp*sk);
        M.x[2][1] = sw*ck + (cw*sp*sk);
        M.x[0][2] = sp;
        M.x[1][2] = -(sw*cp);
        M.x[2][2] = cw*cp;

        /* ImageSpace */
        UVW.x[0][0] = e1 - Xp; 
        UVW.x[1][0] = n1 - Yp; 
        UVW.x[2][0] = -CFL; 

        m_mult (&M, &UVW, &XYZ);
      
        /* Image Space */
        lam = XYZ.x[0][0];
        mu  = XYZ.x[1][0];
        nu  = XYZ.x[2][0];

   /* This is the solution */
   *e2 = ((z1 - ZC)*(lam/nu)) + XC;
   *n2 = ((z1 - ZC)*(mu/nu))  + YC;
  
   return (1);
}

int matrix_error (char *s)
{
    fprintf (stderr, "WARNING: %s", s);
#ifdef DEBUG
    fclose (debug_file);
#endif
    return 0;
}
