
#include "digit.h"

/*
**  watch how routines work if A == B.  Will have to make this work
**  on a line against itself
*/

static struct line_pnts APoints;
static struct line_pnts BPoints;
static int first = 1;

intersect (Map)
    struct Map_info *Map;
{
    int A, B;
    P_LINE *Aline, *Bline;
    char *p, *getenv ();


    if (first)
    {
	APoints.alloc_points = 0;
	BPoints.alloc_points = 0;
	first = 0;
    }
    for (A = 1 ; A <= Map->n_lines ; A++)
    {
	Aline = &(Map->Line[A]);
	if (!LINE_ALIVE (Aline))
	    continue;

	for (B = A ; B <= Map->n_lines ; B++)
	{
	    Bline = &(Map->Line[B]);
	    if (!LINE_ALIVE (Bline))
		continue;

	    if (bboxes_cross (Aline, Bline))
	    {
		if (cross_lines (Map, A, B))
		    break;
	    }
	}
    }
}

cross_lines (Map, A, B)
    struct Map_info *Map;
    int A, B;
{
    P_LINE *Aline, *Bline;
    double X, Y;
    int cnt = 0;
    int a, b, end;
    int ret;

    Aline = &(Map->Line[A]);
    Bline = &(Map->Line[B]);

    dig__Read_line (&APoints, Map->digit, Aline->offset);
    dig__Read_line (&BPoints, Map->digit, Bline->offset);

    end = APoints.n_points-1;
    for (a = 1 ; a < APoints.n_points ; a++)
    {
	for (b = 1 ; b < BPoints.n_points ; b++)
	{
	    if 
	    (
	      (BPoints.x[b-1]==APoints.x[a]  &&BPoints.y[b-1]==APoints.y[a])  ||
	      (BPoints.x[b]  ==APoints.x[a]  &&BPoints.y[b]  ==APoints.y[a])  ||
	      (BPoints.x[b]  ==APoints.x[a-1]&&BPoints.y[b]  ==APoints.y[a-1])||
	      (BPoints.x[b-1]==APoints.x[a-1]&&BPoints.y[b-1]==APoints.y[a-1])
	    ) 
		continue;

	    if (ret = find_intersection (
		APoints.x[a-1], APoints.y[a-1], APoints.x[a], APoints.y[a], 
		BPoints.x[b-1], BPoints.y[b-1], BPoints.x[b], BPoints.y[b],
		&X, &Y))
	    {
		/* test for nodal intersection */
		if ((X == APoints.x[0] && Y == APoints.y[0]) ||
		    (X == APoints.x[end] && Y == APoints.y[end]))
		    continue;
/*DEBUG*/ fprintf (stderr, "Cross lines  %d and %d\n", A, B);
		break_line_w_point (Map, A, X, Y);
		if (A != B)
		    break_line_w_point (Map, B, X, Y);
		return (1);
	    }
	}
    }
    return (0);
}
