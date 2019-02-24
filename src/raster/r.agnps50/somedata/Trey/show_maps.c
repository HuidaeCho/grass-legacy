/****************************************
void show_maps()

MODIFICATIONS by:
Michael Foster, AIWQ Center
501 ASI Building
Penn State University
University Park, PA 16802
Tel./FAX 814-865-3375/3048
Email: mike_foster@agcs.acs.psu.edu

PURPOSE OF FUNCTION:

******************************************/



/*                                       */
/*                                       */
/*  This function is used to display the */
/*  resultant maps of critical area      */
/*  analysis                             */
/*                                       */
/*              Zhian Li, April,1995     */


/*   Include X window libraries:
            LIBS: -lXm -lXt -lX11        */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>     /*added 6/96 */

/******************************
NOT NEEDED IN REVISED VERSION:

Mike Foster 2-21-96

#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include <Xm/Xm.h>
#include <Xm/FileSB.h>
#include <Xm/PushB.h>
#include <Xm/Scale.h>
#include <Xm/SelectioB.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>

*********************************/

/********************************
NOT NEEDED IN REVISED VERSION

Mike Foster 2-21-96

Define map display option     

  int show_opt = 1;
void 
Show_map_option (w, client_data, call_data )
Widget w;
XtPointer client_data;
XmToggleButtonCallbackStruct *call_data;
{

    get the map display option 

    show_opt = (int )client_data;
    return; 
}

**************************************/

/* prototypes for functions added by Dave Peterson, April 1996 */
void show_summary      (int cell, char *runoff, char *sediment, char *n_sediment,
                        char *n_soluble, char *p_sediment, char *p_soluble,
                        int at_outlet);
void pad_string        (char *s, int width);
int  find_outlet_cell  (FILE *f, int *cell);
int  find_max_cell_num (FILE *f, int *max_num);
int  get_cell_data     (FILE *f, int cell, char *x1, char *x2, char *x3, char *x4,
                        char *x5, char *x6);
int  scan_line         (FILE *f);
int  scan_through_word (FILE *f, char *word, int *lines_scanned);
int  parse_field       (FILE *f, int max_width, char *field);

/***** protoypes added by Trey Askew June 1996*****/
void show_pest(char *input_nps);
void get_pest_name(char *input_file, int num, int resolution);

/* parameter "input_nps_filename" added by Dave Peterson, April 1996; parameter
   contains input filename entered by user with .nps extension added
*/
void 
show_maps (char *input_nps_filename)
{
    FILE *input_nps_file;
    int outlet_cell_num, max_cell_num, cell_choice;
    char cell_choice_str[81], runoff[11], sediment[11], n_soluble[11],
         n_sediment[11], p_soluble[11], p_sediment[11];

    char cmd[100];
    char *file_selected;
    int getstring_ok;
    int  slength, t, i, maps;
    char directory[100];
    extern int pollut, show_opt;
    extern int mapflag1,mapflag2,mapflag3,mapflag4,mapflag5,mapflag6;
    int mapflag=0;
    
    /* 
    The next 4 variables were added by Mike Foster 
    */
    int res=0;              /*cell size*/
    char resstring[2];      /* String to read in cell size */ 
    char mapflagstring[2];  /* string to read in desired map type */
    
    
    char cellnummapname[20]; /*cell_num.map name, e.g cell_num.map.100*/

/*  Initialize the com arrary                        */

    for(i=0; i<=99; i++){ 
      cmd[i] = ' ';
      }


/*  Get the selected file name from the file selector*/
/******************************************************
NOT NEEDED: Mike Foster 2-21-96

    slength = 0;
    if(call_data->length == 0) {
       fprintf(stderr,"No file has been selected\n");
       return;
       }
      else
       {
       getstring_ok = XmStringGetLtoR(call_data->value,
          XmSTRING_DEFAULT_CHARSET,&file_selected);
       fprintf(stderr,"the filename is %s \n",file_selected);
       }

*********************************************************/


  /* Get the directory name from the selected simulation*/

/********************************************************
NOT NEEDED: Mike Foster 2-21-96

      slength = strlen(file_selected);
      fprintf(stderr,"the string length %6d\n",slength);

      slength = slength +1;
      slength =slength - 6;
      strncpy(directory, file_selected, slength);

      directory[slength]='\0';
      t = strlen(directory);
      fprintf(stderr,"the string length %d %d\n",slength,t);

***********************************************************/


  /* Show maps according to the selection user made   */

/***********************************************************
NOT NEEDED: Mike Foster 2-12-96

      fprintf(stderr,"the value of show_opt %i \n",show_opt);
      fprintf(stderr,"the value of map_type %i \n",mapflag);
*************************************************************/

for (; ; )
 { system("clear");
   for (; ; )
    { 
      printf("\nWhat type of AGNPS model output would you like to view?\n\n");
      printf("  1=Sediment-attached nitrogen (lbs/acre)\n");
      printf("  2=Total soluble nitrogen (lbs/acre)\n");
      printf("  3=Sediment-attached phosphorus (lbs/acre)\n");
      printf("  4=Total soluble phosphorus (lbs/acre)\n");
      printf("  5=Total soluble COD (lbs/acre)\n");
      printf("  6=Cell erosion (tons/acre)\n");
      printf("  7=Sediment yield (tons)\n");
      printf("  8=Summary of nutrient and sediment yields at outlet cell\n");      
      printf("  9=Pesticide in runoff(lbs/acre)\n");
      printf(" 10=Pesticide in sediment(lbs/acre)\n");
      printf(" 11=Percolation within cell(lbs/acre)\n");
      printf(" 12=Summary of pesticide yields at outlet cell\n");
      printf("  0=Quit\n\n");
      printf("Please enter 0-9 for your choice of contaminant>");              
      mapflag=atoi(gets(mapflagstring)); 
      
      printf("\nThe choice you entered was %d\n",mapflag);
      
      if (mapflag >=0 && mapflag <= 12) break;
      else {
        printf("Please review the choices and make a new selection\n");
        printf("Press any key to continue> "); 
        gets(mapflagstring); 
        
        system("clear");
      }
    }

   if (mapflag == 0) break;
   
   if(mapflag>0 && mapflag<12 && mapflag!=8) {
   strcpy(resstring,"   "); 
   strcpy(cellnummapname,"                    ");
   
      for (; ; )
       { 
         printf("\nPlease enter your cell size in meters (1-9999)>");
         res=atoi(gets(resstring)); 
         printf("\nThe resolution you entered was %d\n",res);
                 
         if (res >= 1 && res <= 9999) break;

         printf("The number entered should be 1-9999\n");
         printf("Press any key to continue> ");
         gets(resstring); 
         system("clear");
       }
      strcpy(cellnummapname,"cell_num.map.");
      strcat(cellnummapname,resstring);

   if (find_old_map(cellnummapname) == 1);
   /* Otherwise a repeated warning is generated that the
   cell map cannot be found */
    }
       
/******Start of section added by Trey Askew 6-13-96*********/

   if(mapflag>=9 && mapflag <=11)
      get_pest_name(input_nps_filename,mapflag,res);
   
   if (mapflag == 12) {
      show_pest(input_nps_filename); 
      gets(mapflagstring);   /*put in to grab extra character */
      continue; 
      } 
/**************End section added by Trey Askew**************/

            
   if (mapflag == 8)
    { 

/************************************************************/
/* start of code section added by Dave Peterson, April 1996 */
/************************************************************/

      /* open input file for reading */
      if ( (input_nps_file = fopen(input_nps_filename, "rb")) == NULL )
        { printf("Error opening file %s.\n", input_nps_filename);
          exit(1);
        }

       /* find outlet cell number and max cell number; then get data for cell */
       /* to display                                                          */
       if ( find_outlet_cell(input_nps_file, &outlet_cell_num) ||
            find_max_cell_num(input_nps_file, &max_cell_num)   ||
            get_cell_data(input_nps_file, outlet_cell_num, runoff, sediment,
                          n_sediment, n_soluble, p_sediment, p_soluble) )
        { printf("Error: file %s has unrecognized format.\n",
                 input_nps_filename);
          fclose(input_nps_file);
          exit(1);
        }

       /* display data for outlet cell */
       show_summary(outlet_cell_num, runoff, sediment, n_sediment, n_soluble,
                    p_sediment, p_soluble, 1);

       /* allow user to display other cells until user is done */
       for (; ; )
        { /* allow user to view another cell */
          printf(
          "To see output from another cell, enter the cell number [1 - %d],\n",
          max_cell_num);
          printf("or enter 0 when done viewing cells ==========================> ");
          cell_choice = atoi(gets(cell_choice_str));

          system("clear"); /* clear the screen */

          if (cell_choice == 0) break;

          if (cell_choice >= 1 && cell_choice <= max_cell_num)
           { /* get data for another cell */
             if (get_cell_data(input_nps_file, cell_choice, runoff, sediment,
                               n_sediment, n_soluble, p_sediment, p_soluble))
              { printf("Error: file %s has unrecognized format.\n",
                       input_nps_filename);
                fclose(input_nps_file);
                exit(1);
              }

             /* display data for cell */
             show_summary(cell_choice, runoff, sediment, n_sediment, n_soluble,
                          p_sediment, p_soluble, 0);
           }
          else
           printf("You have entered an invalid choice.  Please try again.\n\n");
        }

       fclose(input_nps_file);

/**********************************************************/
/* end of code section added by Dave Peterson, April 1996 */
/**********************************************************/

    }
   else
    { if (mapflag<=2)
       sprintf(cmd,"show_N.csh %d %d",mapflag,res);
      else if (mapflag>=3 && mapflag<=4)
       sprintf(cmd,"show_P.csh %d %d",mapflag,res);
      else if (mapflag==5)
       sprintf(cmd,"show_COD.csh %d %d",mapflag,res);
      else if (mapflag>=6 && mapflag<8)
       sprintf(cmd,"show_S.csh %d %d",mapflag,res);  
      system(cmd);   

    }
 }
}

/* display data for a particular cell */
void show_summary(int cell, char *runoff, char *sediment, char *n_sediment,
                  char *n_soluble, char *p_sediment, char *p_soluble,
                  int at_outlet)
 { /* pad display strings (right justify) to correct widths */
   pad_string(runoff, 6);
   pad_string(sediment, 8);
   pad_string(n_sediment, 8);
   pad_string(n_soluble, 7);
   pad_string(p_sediment, 8);
   pad_string(p_soluble, 7);

   /* display data for cell */
   if (at_outlet) printf("\n\nSummary of Yields at Watershed Outlet:\n\n");
   else           printf("\n\nSummary of Yields:\n\n");
   printf("                           (    nitrogen    )   (   phosphorus   )\n");
   printf("cell   runoff   sediment   sediment   soluble   sediment   soluble\n");
   printf("        (in)     (tons)    (lb/ac)    (lb/ac)   (lb/ac)    (lb/ac)\n");
   printf("------------------------------------------------------------------\n");
   printf("%4i   %s   %s   %s   %s   %s   %s\n\n", cell, runoff, sediment,
          n_sediment, n_soluble, p_sediment, p_soluble);
 }

/* pads string to a given witdh (text gets right-justified) */
void pad_string(char *s, int width)
 { int i, j, k;

   /* string length in i */
   for (i = 0; s[i]; i++);

   /* return if string is already at least as wide as width */
   if (i >= width) return;

   /* terminating NULL char */
   s[width] = 0;

   /* move string to the right, working from right to left */
   for (j = i - 1, k = width - 1; j >= 0; j--, k--)
    s[k] = s[j];

   /* fill in positions to left of string with spaces */
   for (; k >= 0; k--)
    s[k] = ' ';
 }

/* get outlet cell number from data file; outlet cell number gets passed back
   in "cell"; file handle f should be opened for reading before calling this
   function; return 0 on success or 1 if file has incorrect format
*/
int find_outlet_cell(FILE *f, int *cell)
 { int  i, skip_line, found_word;
   char c, field[21], *ptr;

   /* file pointer to start of file */
   fseek(f, 0, SEEK_SET);

   /* scan past line labeled "INITIAL" and line immediately following this line */
   if (scan_through_word(f, "INITIAL", &i)) return 1;
   if (scan_line(f))                        return 1;

   /* get outlet cell # from fifth field in line */
   for (i = 5; i; i--)
    if (parse_field(f, 10, field)) return 1;
   *cell = strtol(field, &ptr, 10);
   if (ptr == field) return 1;

   return 0;
 }
/* pass back maximum cell number in "max_num"; file handle f should be opened
   for reading before calling this function; return 0 on success or 1 if file
   has incorrect format
*/
int find_max_cell_num(FILE *f, int *max_num)
 { int i;

   /* file pointer to start of file */
   fseek(f, 0, SEEK_SET);

   /* scan past line labeled "INITIAL" */
   if (scan_through_word(f, "INITIAL", &i))   return 1;

   /* scan past next line */
   if (scan_line(f))                          return 1;

   /* scan past line labeled "SOIL_LOSS" */
   if (scan_through_word(f, "SOIL_LOSS", &i)) return 1;

   /* scan past line labeled "NUTRIENT" */
   if (scan_through_word(f, "NUTRIENT", &i))  return 1;
   if (i < 9) return 1;
   /* i should now equal (7 * number of cells) + 2 */

   *max_num = (i - 2) / 7; /* pass back max cell number */

   return 0;
 }

/* get data fields for cell specified by "cell"; pass fields back in x1, ..., x6;
   file handle f should be opened for reading before calling this function;
   return 0 on success or 1 if file has incorrect format
*/
int get_cell_data(FILE *f, int cell, char *x1, char *x2, char *x3, char *x4,
                  char *x5, char *x6)
 { int   i, j;

   /* file pointer to start of file */
   fseek(f, 0, SEEK_SET);

   /* scan past line labeled "INITIAL" */
   if (scan_through_word(f, "INITIAL", &i))   return 1;

   /* scan past next line */
   if (scan_line(f))                          return 1;

   /* scan past line labeled "SOIL_LOSS" */
   if (scan_through_word(f, "SOIL_LOSS", &i)) return 1;

   /* scan past line immediately before first line for cell */
   for (i = 1; i < cell; i++)
    for (j = 7; j; j--)
     if (scan_line(f)) return 1;

   /* get 7th field in first line of cell data */
   for (i = 7; i; i--)
    if (parse_field(f, 10, x1)) return 1;

   /* scan through end of sixth line of cell data */
   for (i = 6; i; i--)
    if (scan_line(f)) return 1;

   /* get 4th field in 7th line of cell data */
   for (i = 4; i; i--)
    if (parse_field(f, 10, x2)) return 1;

   /* scan past line labeled "NUTRIENT" */
   if (scan_through_word(f, "NUTRIENT", &i)) return 1;

   /* scan past line immediately before first line for cell */
   for (i = 1; i < cell; i++)
    { if (scan_line(f)) return 1;
      if (scan_line(f)) return 1;
    }

   /* get 5th field in first line of cell data */
   for (i = 5; i; i--)
    if (parse_field(f, 10, x3)) return 1;

   /* get 7th field in first line of cell data */
   if (parse_field(f, 10, x4)) return 1;
   if (parse_field(f, 10, x4)) return 1;

   /* scan past end of line */
   if (scan_line(f)) return 1;

   /* get 2nd field in 2nd line of cell data */
   if (parse_field(f, 10, x5)) return 1;
   if (parse_field(f, 10, x5)) return 1;

   /* get 4th field in 2nd line of cell data */
   if (parse_field(f, 10, x6)) return 1;
   if (parse_field(f, 10, x6)) return 1;

   return 0;
 }

/* advance file pointer past end of current line in file */
int scan_line(FILE *f)
 { char c;

   do if (fread(&c, 1, 1, f) == 0)
       return 1;
   while (c != '\n');

   return 0;
 }

/* advance file pointer past next line starting with "word" in file; pass back
   # of lines scanned in the process 
*/
int scan_through_word(FILE *f, char *word, int *lines_scanned)
 { char c;
   int  i, found_word, skip_line, len, lines_done;

   /* length of string in i */
   for (len = 0; word[len]; len++);

   /* each iteration scans one line */
   lines_done = 0;   
   found_word = 0;
   do { skip_line  = 0;
        lines_done++;
        for (i = 0; ; i++)
         { if (fread(&c, 1, 1, f) == 0) return 1;

           if (c == '\n') break;

           if (i >= len)
            { if (c != ' ' && c != '\t') skip_line = 1; }
           else if (c != word[i]) skip_line  = 1;
           else if (i == len - 1) found_word = 1;

           if (skip_line)
            { found_word = 0;
              do if (fread(&c, 1, 1, f) == 0)
                  return 1;
              while (c != '\n');

              break;
            }
         }
      }
   while (found_word == 0);

   *lines_scanned = lines_done;

   return 0;
 }

/* get next field in current line (delimited by surrounding whitespace) in file

   return value:
      0: success
      1: end of file encountered
      2: end of line encountered
      3: max width for field reached
*/
int parse_field(FILE *f, int max_width, char *field)
 { char c;
   int  i;

   field[0] = 0;

   /* scan past leading whitespace */
   do if (fread(&c, 1, 1, f) == 0)
       return 1;
   while (c == ' ' || c == '\t');

   /* return if end of line reached */
   if (c == '\n') return 2;

   /* copy up to "max_width" chars from field */
   i = 0;
   do { field[i++] = c;
        if (fread(&c, 1, 1, f) == 0)
         { field[i] = 0;
           return 1;
         }
        if (i == max_width)
         { field[i] = 0;
           return 3;
         }
      }
   while (c != ' ' && c != '\t' && c != '\n');

   field[i] = 0;

   if (c == '\n') return 2;

   return 0;
 }
