struct ncb  /* neighborhood control block */
{
    DCELL **buf;     /* for reading cell file */
    int *value;     /* neighborhood values */
    int nsize;      /* size of the neighborhood */
    int dist;       /* nsize/2 */
    struct Categories cats;
    char title[1024];
    FILE *out;
    struct
    {
	char *name;
	char *mapset;
    }
    oldcell, newcell;
    long changed;
    DCELL *center;
};

#ifdef MAIN
    struct ncb ncb;
#else
    extern struct ncb ncb;
#endif
