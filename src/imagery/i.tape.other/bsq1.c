/* read each file as a complete band */
#include "tape.h"

int bsq1 (void)
{
    int band;
    int lastband = 0;

    for (band=0; band < nbands; band++)
	if (wantband[band])
		lastband = band;
    for (band=0; band <= lastband; band++)
    {
	if (wantband[band])
	{
	    I_tape_advance (tapefd, skiprecords);
	    first[band] = 1;
	    if(!readbsq (band))
		break;
	}
	if (band < lastband)
	    I_tape_advance (tapefd, -999);
    }

    return 0;
}
