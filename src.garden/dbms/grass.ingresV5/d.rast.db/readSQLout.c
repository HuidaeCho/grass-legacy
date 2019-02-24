/* readSQLout.c - Reads output from SQL query, converts to GRASS reclass
 * 		  rules, displays the result.
 *
 * INGRES modifications - The INGRES output is in a "nice looking" table
 *			  format. The routine ingresUnTable has been added
 * 			  to remove all characters that make up the table
 *    			  frames etc, as well as any lines of text (except
 *			  column headers).
 *
 * 			  The ingresUnTable routine differs slightly for 
 *			  INGRES ver5.0 and ver6. The currently implemented
 * 			  routine applies to INGRES ver5.0
 *
 * Enhancements - Fancy display with title and legend added.
 *
 * KJ 930411
 */
#include "gis.h"
#include "ingresUtils.h"
#include "displayUtils.h"

#define TRUE 0
#define FALSE 1

int readSQLout(tmpfile_out, outFlag, key, col, input, output)

	char *tmpfile_out;
	int outFlag;
	char *key, *col, *input, *output;
{

    FILE *fp, *fpin, *fpout;
    int i,  cat, TMP;
    char sysbuf[1024];          /* buffer to issue system cmd   */
    char buf[1024];             /* value in key column          */
    char label[1024];           /* current value in column      */
    char prev_label[1024] ;             /* running value for column     */
    int  colFlag, x;
    char *tmpfile_rules;
    char *reduced_col, *reduced_key;

/*  Does not apply to INGRES, KJ Nov 4, 1992

    if ( (reduced_col = rindex(col,'.')) != NULL )
       col = reduced_col + 1;

    if ( (reduced_key = rindex(key,'.')) != NULL ) 
       key = reduced_key + 1;

*/

    i = 1;
    colFlag =0;
    cat = 0;
    TMP = FALSE;


        /* Open file containing output from SQL command */
        if((fpin = fopen(tmpfile_out,"r")) == NULL) {
            fprintf(stderr, "File read error on temporary file (output)\n");
            exit(-1);
           }


        /* Open file for reclass rules 			*/
	tmpfile_rules = G_tempfile() ;
	if((fpout = fopen(tmpfile_rules,"w")) == NULL) {
            fprintf(stderr, "File write error on temporary file (rules)\n");
	    exit(-1);
           }



	/* Read SQL output and generate reclass rules	*/

	   *prev_label = 0;
	   printf("Creating GRASS reclass rules ... \n");
	   while(G_getl(buf, sizeof(buf), fpin) ) {
		G_squeeze(buf);
		ingresUnTable(buf); 
		if (*buf == 0) continue;
		if ((outFlag == 1) && (colFlag == 0)) { colFlag = 1; continue; } /* skip 1st line in col output (ie the column headers) */

                if (outFlag == 1 && *buf > 0)           /* Read column type output */
                        if(sscanf(buf,"%d %[^\n]",&x, label) != 2) {
                                fprintf(stderr,"Col error %s\n", buf); exit(-1);
                          }
                         else {
                                G_squeeze(label);
                                if (strcmp(label,prev_label) != 0) cat++;
                                fprintf(fpout,"%d=%d %s\n", x, cat, label);
                                strcpy(prev_label, label);
                        }

/* Row type output does not apply to INGRES, KJ 930411 */

                if (outFlag == 2 && *buf > 0)           /* Read row type output */
                        if (strncmp(buf,key,strlen(key)) == 0)  {         /* Read: Key Keyval     */
                                if(sscanf(buf,"%*s %d", &x) != 1) {
                                        fprintf(stderr,"Row error %s\n", buf); exit(-1);
                                }
			}
			 
			if (strncmp(buf,col,strlen(col)) == 0)
                                if(sscanf(buf,"%*s %[^\n]", label) != 1) { 	/* Read: Col Colval     */
                                        fprintf(stderr,"error %s\n", buf); exit(-1);
                                }
                                else {
                                        G_squeeze(label);
                                        if (strcmp(label,prev_label) != 0) cat++;
                                        fprintf(fpout,"%d=%d %s\n", x, cat, label);
                                        strcpy(prev_label, label);
                                }

                          }
	
        fclose(fpin);
        fclose(fpout);

	/* If user didn't specify output file		*/
	if (!output) {
	    output="tmp.recl" ;
	    TMP = TRUE;
	}


	/* Execute GRASS reclass			*/
	sprintf(sysbuf,"r.reclass input=%s output=%s < %s\n",
		input, output, tmpfile_rules);
	system(sysbuf);
	   
	/* Display reclass map				*/
	printf("Displaying the result ... \n");
/* Enhanced display. Save for later KJ 930411 
	sprintf(sysbuf,"/data1/home/kjohnsso/Grass/Scripts/display.sh %s\n",output);
	system(sysbuf);
*/

/*
	sprintf(sysbuf,"d.rast %s\n", output);
	system(sysbuf);
*/

 	fancyDisplay(output);

	/* Remove temporay dot files			*/
	unlink(tmpfile_out);
	unlink(tmpfile_rules);

	/* if user didn't specify raster output file - cleanup */
	if (TMP==TRUE) {
		G_remove("cell","tmp.recl") ;
		G_remove("cats","tmp.recl") ;
		G_remove("cellhd","tmp.recl") ;
		G_remove("hist","tmp.recl") ;
		G_remove("cell_misc","tmp.recl") ;
		G_remove("colr","tmp.recl") ;
	}
		
return 0;
}

