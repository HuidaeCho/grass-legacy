#include "gis.h"
#include "report.h"
#define FD  report->fd
#define BUF report->buf
#define NFIELDS report->nfields
#define FIELD   report->field
#define OFFSET  report->offset
#define MATRIX  report->matrix

REPORT *
report_open (report_file)

    char *report_file;
{
    REPORT *report;

    report = (REPORT *) G_malloc (sizeof(REPORT));
    if (FD = fopen (report_file,"r"))
    {
        NFIELDS = 0;
        BUF[0]  = 0;
        report_scan (report);
        return report;
    }

    printf("can't open report file\n");
    perror(report_file);
    exit(-1);
}

REPORT *
report_open_ref (report_file, ref)

    char *report_file;
    REPORT *ref;
{
    REPORT *report;
    FILE *fd;

    if (fd = fopen (report_file,"r"))
    {
        report = (REPORT *) G_malloc (sizeof(REPORT));
        memcopy (report, ref, sizeof(REPORT));
        report->fd = fd;
        fseek (fd, ftell (ref->fd), 0);
        return report;
    }

    printf("can't open report file\n");
    perror(report_file);
    exit(-1);
}

report_close (report)

    REPORT *report;
{
    fclose (FD);
    free (report);
}

report_read (report)

    REPORT *report;
{
    int size;

    size = sizeof (FIELD) / sizeof (FIELD[0]);

    OFFSET.next = ftell(FD);
    while (G_getl(BUF, sizeof (BUF), FD))
    {
        OFFSET.cur = OFFSET.next;
        OFFSET.next = ftell(FD);
        NFIELDS = parse (BUF, FIELD, size, "|");
        if (NFIELDS <= 0)
            continue;
        if (FIELD[0][0] != '#')
            return 1;
    }
    BUF[0] = 0;
    NFIELDS = 0;
    return 0;
}

report_record (report, type)

    REPORT *report;
    char *type;
{
    return (strcmp(report->field[0],type) == 0) ;
}

report_read_record (report, type)
    REPORT *report;
    char *type;
{
    return (report_read (report) && report_record (report, type));
}

report_scan (report)

    REPORT *report;
{
    unsigned int flags; 
    double ctof();

    flags = 0;
    fseek (report->fd, 0L, 0);

    report->nlayers = 0;
    report->npoints = 0;
    MATRIX.n = 0;
    MATRIX.size = 0;
    MATRIX.center = -1;

    while (report_read(report))
    {
        if (report_record(report,"location"))
        {
            report->location = G_store (report->field[1]);
            report->fullname = G_store (report->field[2]);
            flags |= 001;
        }
        else if (report_record(report,"mapset"))
        {
            report->mapset = G_store (report->field[1]);
            flags |= 002;
        }
        else if (report_record(report,"north"))
        {
            report->north = ctof(report->field[1]);
            flags |= 004;
        }
        else if (report_record(report,"south"))
        {
            report->south = ctof(report->field[1]);
            flags |= 010;
        }
        else if (report_record(report,"east"))
        {
            report->east = ctof(report->field[1]);
            flags |= 020;
        }
        else if (report_record(report,"west"))
        {
            report->west = ctof(report->field[1]);
            flags |= 040;
        }
        else if (report_record(report,"nsres"))
        {
            report->ns_res = ctof(report->field[1]);
            flags |= 0100;
        }
        else if (report_record(report,"ewres"))
        {
            report->ew_res = ctof(report->field[1]);
            flags |= 0200;
        }
        else if (report_record(report,"layer"))
        {
            report->nlayers++;
            if (!(flags & 0400))
            {
                flags |= 0400;
                OFFSET.layers      = OFFSET.cur;
                OFFSET.layers_next = OFFSET.cur;
            }
        }
        else if (report_record(report,"cat"))
        {
            if (!(flags & 01000))
            {
                flags |= 01000;
                OFFSET.cats      = OFFSET.cur;
                OFFSET.cats_next = OFFSET.cur;
            }
        }
        else if (report_record(report,"point"))
        {
            report->npoints++;
            if (!(flags & 02000))
            {
                flags |= 02000;
                OFFSET.points      = OFFSET.cur;
                OFFSET.points_next = OFFSET.cur;
            }
        }
        else if (report_record(report,"data"))
        {
            if (!(flags & 04000))
            {
                flags |= 04000;
                OFFSET.data      = OFFSET.cur;
                OFFSET.data_next = OFFSET.cur;
            }
            break;  /* stop reading report */
        }
        else if (report_record(report,"matrix size"))
        {
            flags |= 010000;
            MATRIX.size = atoi(FIELD[1]);
        }
        else if (report_record(report,"matrix"))
        {
            report_matrix (report);
            flags |= 020000;
        }
        if (report_record(report,"site list"))
        {
            report->site_list_name = G_store (report->field[1]);
            report->site_list_desc = G_store (report->field[2]);
            flags |= 040000;
        }
    }

    if (flags != 077777)
        die("** error - raw report incomplete");
}

report_matrix (report)

    REPORT *report;
{
    int i;

    if (NFIELDS % 2 != 1)
        die("** error - raw report matrix record invalid");

    if (MATRIX.n == 0)
    {
        MATRIX.down  = (int *) G_malloc (MATRIX.size * sizeof(int));
        MATRIX.right = (int *) G_malloc (MATRIX.size * sizeof(int));
    }

    for (i = 1; i < NFIELDS; i++)
    {
        int right;
        int down;

        right = atoi (FIELD[i++]);
        down  = atoi (FIELD[i++]);

        MATRIX.right[MATRIX.n] = right;
        MATRIX.down[MATRIX.n]  = down;

        if (right == 0 && down == 0)
            MATRIX.center = MATRIX.n;

        MATRIX.n++;
    }
}

static double ctof (buf)
    char *buf;
{
    double x;
    sscanf (buf, "%lf", &x);
    return x;
}
