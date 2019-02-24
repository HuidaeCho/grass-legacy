#include "imagery.h"

I_get_cam_title (camera, title, n)
    char *camera;
    char *title;
{
    FILE *fd;

    *title = 0;
    G_suppress_warnings(1);
    fd = I_fopen_cam_file_old (camera, "TITLE");
    G_suppress_warnings(0);
    if (fd != NULL)
    {
	G_getl (title, n, fd);
	fclose (fd);
    }
    return fd != NULL;
}

I_put_camera_title (camera, title)
    char *camera;
    char *title;
{
    FILE *fd;
    fd = I_fopen_cam_file_new (camera, "TITLE");
    if (fd != NULL)
    {
	fprintf (fd, "%s\n", title);
	fclose (fd);
    }
    return fd != NULL;
}
