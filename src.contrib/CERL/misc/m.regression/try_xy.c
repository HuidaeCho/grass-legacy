double *vector();
double **matrix();
double rms(), sqrt(), err2(), evaluate();
double gasdev();
double ran1();
double fabs();

static int idum = -1; /* for random number generator */

#define PIXELS 20

int 
main (int argc, char *argv[])
{
    double **u, **v, *w, *a;
    double *x1,*x2,*x3,*y1,*y2,*y3;
    double max,x,y,z,dx,dy;
    double *value;
    void basis();
    int i,j,k;
    int perturb = 1;
    int counts[PIXELS+1];

    int na = 6;	/* number of parameters */
    int ndata = 25; /* number of data points */

    for (i = 1; i < argc; i++)
	if (strcmp(argv[i], "-e") == 0)
	    perturb = 0;

    a = vector (na);    /* parameter array */
    u = matrix (ndata,na);
    v = matrix (na,na);
    w = vector (na);
    value = vector (na);
    x1 = vector (ndata);
    x2 = vector (ndata);
    x3 = vector (ndata);
    y1 = vector (ndata);
    y2 = vector (ndata);
    y3 = vector (ndata);

    k = 0;
    for (i = 0; i < 5; i++)
	for (j = 0; j < 5; j++)
	{
	    x2[k] = x1[k] = i*1000.0;
	    y2[k] = y1[k] = j*1000.0;
	    if (perturb)
	    {
		deviation (&dx, &dy);
		x2[k] += dx;
		y2[k] += dy;
	    }
	    k++;
	}

    fprintf (stdout,"x (rms=%g)\n",rms(x1,x2,ndata));

    for (k=0,i=0; i < 5; i++)
    {
	for (j = 0; j < 5; j++, k++)
	    fprintf (stdout," %g:%g", x1[k],x2[k]);
	fprintf (stdout,"\n");
    }

    fprintf (stdout,"y (rms=%g)\n",rms(y1,y2,ndata));

    for (k=0,i=0; i < 5; i++)
    {
	for (j = 0; j < 5; j++, k++)
	    fprintf (stdout," %g:%g", y1[k],y2[k]);
	fprintf (stdout,"\n");
    }


    svdfit2(x1,y1,x2,ndata,a,na,u,v,w,basis);
    print_eqn ("x",a,w,na);
    for (i=0; i < ndata; i++)
	x3[i] = evaluate(x1[i],y1[i],a,value,na);
    fprintf (stdout,"  rms(x1,x2)=%g rms(x1,x3)=%g rms(x2,x3)=%g\n",
	rms(x1,x2,ndata),rms(x1,x3,ndata),rms(x2,x3,ndata));
    fprintf (stdout,"  x3-x2\n");
    for (i=0; i < ndata; i++)
	fprintf (stdout,"\t%.2lf%s", x3[i]-x2[i], (i+1)%5?"":"\n");

    max = 0.0;
    for (i=0; i <= PIXELS; i++)
	counts[i] = 0;

    for (x=0.0; x <= 5000.0; x += 10.0)
	for (y=0.0; y <= 5000.0; y += 10.0)
	{
	    z = fabs(x-evaluate(x,y,a,value,na));
	    if (z > max) max = z;
	    i = (int)z;
	    if (i > PIXELS) i--;
	    counts[i]++;
	}
    fprintf (stdout,"  deviations:\n");
    for (i = 0; i < PIXELS; i++)
	fprintf (stdout,"\t%d = %d%s", i, counts[i], (i+1)%4?"":"\n");
    fprintf (stdout,"\t>=%d = %d\n", i, counts[i]);
    fprintf (stdout,"\tmax: %g\n", max);

    svdfit2(x1,y1,y2,ndata,a,na,u,v,w,basis);
    print_eqn ("y",a,w,na);
    for (i=0; i < ndata; i++)
	y3[i] = evaluate(x1[i],y1[i],a,value,na);
    fprintf (stdout,"  rms(y1,y2)=%g rms(y1,y3)=%g rms(y2,y3)=%g\n",
	rms(y1,y2,ndata),rms(y1,y3,ndata),rms(y2,y3,ndata));

    max = 0.0;
    for (x=0.0; x <= 5000.0; x += 10.0)
	for (y=0.0; y <= 5000.0; y += 10.0)
	{
	    z = fabs(y-evaluate(x,y,a,value,na));
	    if (z > max) max = z;
	}
    fprintf (stdout,"  max deviation: %g\n", max);
}

void 
basis (
    double x,
    double y,
    double *value,
    int na	/* not used */
)
{
    value[0] = x;
    value[1] = y;
    value[2] = x*x;
    value[3] = x*y;
    value[4] = y*y;
    value[5] = 1.0;
}
char *
bname (int i)
{
    switch (i)
    {
    case 0: return "x";
    case 1: return "y";
    case 2: return "x^2";
    case 3: return "xy";
    case 4: return "y^2";
    default: return "";
    }
}

double 
evaluate (double x, double y, double *a, double *value, int na)
{
    int i;
    double sum;

    basis (x,y,value,na);
    sum = 0.0;
    for (i = 0; i < na; i++)
	sum += a[i]*value[i];
    return sum;
}

double 
err2 (double *x1, double *x2, int n)
{
    double sum;
    double diff;
    int i;

    sum = 0.0;
    for (i=0; i<n;i++)
    {
	diff = x1[i]-x2[i];
	sum += diff*diff;
    }
    return sum;
}

double 
rms (double *x1, double *x2, int ndata)
{
    return sqrt(err2(x1,x2,ndata))/ndata;
}

int 
deviation (double *dx, double *dy)
{
    do
    {
	random_deviation(dx,dy,5.0);
    }
    while (fabs(*dx) > 3.0 || fabs(*dy) > 3.0);
}

int 
random_deviation (double *dx, double *dy, double sig)
{
    *dx = gasdev(&idum) * sig;
    *dy = gasdev(&idum) * sig;
}

int 
circle_deviation (double *dx, double *dy, double r)
{
    double a,b,c;

    do
    {
	a = 2.0 * ran1(&idum) - 1.0;
	b = 2.0 * ran1(&idum) - 1.0;
	c = a*a + b*b;
    } while (c == 0.0);
    c = sqrt(c);

    *dx = a * r /c ;
    *dy = b * r /c ;
}

int 
print_eqn (char *label, double *a, double *w, int na)
{
    int i;
    fprintf (stdout,"%s' = ", label);
    for (i=0; i < na; i++)
    {
	if (i) fprintf (stdout,"%*s", strlen(label)+2, "");
	if (a[i] < 0)
	    fprintf (stdout,"- %g ",-a[i]);
	else
	    fprintf (stdout,"+ %g ",a[i]);
	fprintf (stdout,"%s  (w=%g)\n", bname(i), w[i]);
    }
}
