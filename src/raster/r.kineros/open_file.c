#include <stdio.h>

FILE *open_file(string,type)

/******************************************************************/
/*  Open a file named by string and associate a stream with it.   */
/*  Type has one of the associated values with it:                */
/*                                                                */
/*  "r"    open for reading                                       */
/*                                                                */
/*  "w"    truncate or create for writing                         */
/*                                                                */
/*  "a"    append; open for writing at end of file, or create     */
/*         for writing                                            */
/*                                                                */
/*  "r+"   open for update (reading and writing)                  */
/*                                                                */
/*  "w+"   truncate or create for update                          */
/*                                                                */
/*  "a+"   append; open or create for update at end-of-file       */
/*                                                                */
/*  Open_file returns a pointer to the file structure associated  */
/*  with the stream.                                              */
/******************************************************************/

char *string;
char *type;

{

	FILE *stream;

	stream = fopen(string,type);

	fprintf(stderr,"\n");
	if (stream == NULL)
	{
	    fprintf(stderr,"\n Error opening \"%s\".",string);
	    fprintf(stderr,"\n");
	    exit(-1);
	}
	else
	{
	    fprintf(stderr,"\n \"%s\" has been",string);

	    if(strcmp(type,"r") == 0)
		fprintf(stderr,"\n  opened for reading.");

	    if(strcmp(type,"w") == 0)
		fprintf(stderr,"\n  truncated or created for writing.");

	    if(strcmp(type,"a") == 0)
	    {
		fprintf(stderr,"\n  opened or created for writing at");
		fprintf(stderr,"\n  the end-of-file.");
	    }

	    if(strcmp(type,"r+") == 0)
		fprintf(stderr,"\n  opened for update (reading and writing).");

	    if(strcmp(type,"w+") == 0)
		fprintf(stderr,"\n  truncated or updated for update.");

	    if(strcmp(type,"a+") == 0)
	    {
		fprintf(stderr,"\n  opened or created for updating at");
		fprintf(stderr,"\n  the end-of-file.");
	    }

	    fprintf(stderr,"\n");
	}

	return stream;
}
