/* %W% %G% */
#include "imagery.h"

#define NFILES 10

ask_oldfiles (ref1, ref2, group)
    struct Ref *ref1, *ref2;
    char *group;
{
    char use[NFILES][3];
    char name[NFILES][80];
    int i,f,p,n,k;
    int repeat;
    int row;
    char title[80];
    char instructions[80];

    sprintf (instructions, "GROUP: %s", group);
    I_location_info (title, instructions);
    sprintf (instructions, "If you wish to delete a file from group [%s]", group);

    f = 0;
    while(f < ref1->nfiles)
    {
	V_clear();
	V_line (0, title);
	V_line (2, instructions);
	V_line (3, "remove the 'x' from in front of the file name");
	for (i=0; i < NFILES; i++)
	    strcpy(use[i], "x");
	row = 6;
	p = f;
	for (i=0; i < NFILES && f < ref1->nfiles; i++, f++)
	{
	    sprintf (name[i], "%s in %s", ref1->file[f].name, ref1->file[f].mapset);
	    if (G_find_cell (ref1->file[f].name, ref1->file[f].mapset) == NULL)
	    {
		strcat(name[i], "  <- cell file does not exist");
		use[i][0] = 0;
	    }
	    V_const (name[i], 's', row, 3, 70);
	    V_ques (use[i], 's', row, 0, 2);
	    row++;
	}
	if (i==0) break;
	if (f < ref1->nfiles)
	    V_line (row+2,"(more on next page)");

	V_intrpt_ok();
	if(!V_call())
	    return 0;

	for (n=0; n < i; n++)
	    if (use[n][0])
		I_transfer_group_ref_file (ref1, p+n, ref2);
    }
    return 1;
}
