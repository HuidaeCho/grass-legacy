#ifndef lint
static char *SCCSID = "@(#)Dhp7475.c	OEMG v.3.2";
#endif
/* hp7475A - desktop plotter */
/* the following defines must be included for all drivers */
/* modified for multiple paper sizes */
#include <varargs.h>
#include "plotter.h"
#define file stdout
#define ifile stdin
static char s[] = "\0";		/* required structure for return */
static XYS cursor = {0, 0, s};	/* data.  "s" may be larger, if needed */
# include <termio.h>
/*	select appropriate baud rate for your system */
# define BAUD B9600
	static int
oldpen;
	static char
*spen;
	static struct {
int	xlim,  ylim;
} lims[6] = {
	10020, 7400,		/* A paper */
	15900, 10020,		/* B paper */
	21200, 15900,		/* C paper */
	33200, 21200,		/* D paper */
	33200, 21200,		/* E paper */
	33200, 21200,		/* direct connect */
};
	XYS *
Ddmp60(va_alist) va_dcl {
	va_list vap;
	int cmd, i, pen=1, px1,py1,px2,py2;
	long	x, y, t;
	XYS *ret = &cursor;

	va_start(vap); cmd = va_arg(vap, int);
	switch(cmd) {
	case D_SCALE:
		if (Dglobal.scale <= 0.)
			Dglobal.scale = 1.;
		goto scaleit;
	case D_INIT:
		if ( Dglobal.model_no == 5 && !(isatty(fileno(file)) || isatty(fileno(file)))){
			fprintf(stderr,"Error: Paper size not specified\n");
			exit();
			}
		if (isatty(i = fileno(file))) { /* set baud rate */
			struct termio term;
			if (ioctl(i, TCGETA, &term) == -1)
				return NULL;
			term.c_iflag |= IXON+IXANY;
			term.c_cflag = (term.c_cflag & ~CBAUD) + BAUD;
			if (ioctl(i, TCSETA, &term)== -1)
				return NULL;
		}
		fputs("IN;\033.I81;;17:\033.N;19:\nSP1;PU;\n",file);
		fprintf(file,"IP;\n");
		oldpen = 1;
                if (Dglobal.model_no == 5) {
			struct termio term;
                        if (ioctl(i, TCGETA, &term) == -1)
                                return NULL;
                        term.c_iflag |= IXON+IXANY;
                        term.c_cflag = (term.c_cflag & ~CBAUD) + BAUD;
                        if (ioctl(i, TCSETA, &term)== -1)
                                return NULL;
                        fputs("OP;\n",file);
                        scanf("%d,%d,%d,%d",&px1,&py1,&px2,&py2);
                        lims[Dglobal.model_no].xlim = px2 - px1 +300;
                        lims[Dglobal.model_no].ylim = py2 - py1 +300;
                }
scaleit:
		if (Dglobal.reverse) {
			cursor.x = lims[Dglobal.model_no].ylim / Dglobal.scale;
			cursor.y = lims[Dglobal.model_no].xlim / Dglobal.scale;
		} else {
			cursor.x = lims[Dglobal.model_no].xlim / Dglobal.scale;
			cursor.y = lims[Dglobal.model_no].ylim / Dglobal.scale;
		}
		break;
	case D_DONE:
		fputs("SP;\n",file);
		fclose(file);
		break;
	case D_MOVE:
		spen = "PU";
		goto moveit;
	case D_LINE:
		spen = "PD";
moveit:
		x = (va_arg(vap, long)) * Dglobal.scale;
		y = (va_arg(vap, long)) * Dglobal.scale;

		x = x - (lims[Dglobal.model_no].xlim / 2);
		y = y - (lims[Dglobal.model_no].ylim / 2);
		if (Dglobal.reverse) {
			t = x;
			x = -(y + ((lims[Dglobal.model_no].ylim - lims[Dglobal.model_no].xlim) / 2));
			y = t + ((lims[Dglobal.model_no].xlim - lims[Dglobal.model_no].ylim) / 2);
			}
		fprintf(file,"%s%d,%d;\n", spen, x, y);
		break;
	case D_PEN:
		pen = (va_arg(vap, long)) & 7;
		pen %= 8;  /* only 8 pens */
		if (pen != oldpen) {
			oldpen = pen;
			fprintf(file,"SP%d;PU;\n",pen+1);
		}
	}
	va_end(vap);
	return ret;
}
