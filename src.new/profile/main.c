
/*
 *   main.c 
 *
 *   cell-file profiling.
 *
 */

#define DEBUG 
#define MAIN
#include "profile.h"
#include "math.h"
#include "stdio.h"

struct Profile profile;

main()
{
char   *old_mapset,
       old_mapname[80];
double cur_ux, cur_uy ;
double D_d_to_u_row(), D_d_to_u_col() ;
double hypot() ;
double length ;
double ux, uy ;
char   ltr[10];
int    text_width, text_height;
int    err;
int    button ;
int    cur_screen_x, cur_screen_y ;
int    screen_x, screen_y ;
struct Cell_head window ;
int    t, b, l, r ;
char   title_buf[1024];
char   txt_buf[1024];
int    i,CurrentWin=0;
int    min,max;
int    row,col; 

/* Initialize the GIS library */
G_gisinit("profile") ;

/* prompt user for cell-file to be profiled */ 
G_clear_screen();
printf("\nprofile\n\n");
printf("This program allows a user to view profiles of a cell-file.\n\n");
old_mapset = G_ask_cell_old("Enter name of cell-file",old_mapname); 
if (old_mapset == NULL)
   exit(0);
sprintf(title_buf,"%s in mapset %s",old_mapname,old_mapset);

/* get cell-file range */
WindowRange(old_mapname,old_mapset,&min,&max);

/* 
if (!quick_range(old_mapname,old_mapset,&min,&max))
   {
   if (!slow_range(old_mapname,old_mapset,&min,&max))
      {
      G_fatal_error("Unable to read from cell-file");
      exit(-1);
      }
   }
if (min > 0) min = 0;
if (max < 0) max = 0;
*/

printf("\n\nUse mouse to choose action\n");
 
/* establish connection with graphics driver */
R_open_driver();

/* Make sure screen is clear */
Dclearscreen() ;

/* Establish windows on screen */
Dnew(MOU.name, MOU.bot, MOU.top, MOU.left, MOU.right) ;
Dnew(STA.name, STA.bot, STA.top, STA.left, STA.right) ;
Dnew(MAP.name, MAP.bot, MAP.top, MAP.left, MAP.right) ;
for (i=0; i<=3; i++)
   Dnew(profiles[i].name,profiles[i].bot,profiles[i].top,
        profiles[i].left,profiles[i].right);

/* Plot cell-file in map window */
Dchoose(MAP.name);
Dcell(old_mapname,old_mapset,0);
   
/* loop until user wants to quit */
for(;;)
   {
   /* display mouse-menu in mouse-menu window */
   Dchoose(MOU.name);
   R_standard_color(D_translate_color("black"));
   D_erase_window();
   R_standard_color(D_translate_color("red"));
   R_flush();
   DrawText(22,1,1,"GRASS PROGRAM: profile");
   R_standard_color(D_translate_color("white"));
   DrawText(22,2,1,"MOUSE  | Left:   Where am I?");
   DrawText(22,3,1,"BUTTON | Middle: Set FIRST point");
   DrawText(22,4,1,"MENU   | Right:  Quit this\n");

   /* LOOP to get first point of line */
   do
      {

      /* choose map window and set up conversion factors */
      Dchoose(MAP.name);
      G_get_set_window(&window) ;
      D_get_screen_window(&t, &b, &l, &r) ;
      screen_y  = (t + b) / 2 ;
      screen_x  = (l + r) / 2 ;
      D_do_conversions(&window, t, b, l, r) ;

      /* get a point from the mouse */
      R_get_location_with_pointer(&screen_x, &screen_y, &button);
      
      /* exit if user hit left mouse button */
      if(button == 3)
	 return(0) ;

      /* convert to (easting,northing) coordinates */
      cur_uy = D_d_to_u_row((double)screen_y); 
      cur_ux = D_d_to_u_col((double)screen_x);

      if (cur_ux > window.east || cur_ux < window.west ||
          cur_uy > window.north || cur_uy < window.south)
         {
         Dchoose(STA.name);
         R_standard_color(D_translate_color("black"));
         D_erase_window();
         R_standard_color(D_translate_color("red"));
         R_flush();
         DrawText(25,1,1,"OUTSIDE CURRENT WINDOW");
         button = 1;
         }
      else
         {
         /* print "earth" coords. and category info. in status window */
         Dchoose(STA.name);
         What(old_mapname,old_mapset,window,cur_ux,cur_uy);

         }

      }
   while (button != 2); 

   /* display mouse-menu in mouse-menu window */
   Dchoose(MOU.name);
   R_standard_color(D_translate_color("black"));
   D_erase_window();
   R_standard_color(D_translate_color("red"));
   R_flush();
   DrawText(22,1,1,"GRASS PROGRAM: profile");
   R_standard_color(D_translate_color("white"));
   DrawText(22,2,1,"MOUSE   | Left:   Where am I?");
   DrawText(22,3,1,"BUTTON  | Middle: Set SECOND point");
   DrawText(22,4,1,"MENU    | Right:  Quit this\n");

   /* move graphics position to first point chosen */
   R_move_abs(screen_x, screen_y) ;
   cur_screen_x = screen_x ;
   cur_screen_y = screen_y ;

   /* LOOP to get second point of line */
   do
      {
      /* choose map window and set up conversion factors */
      Dchoose(MAP.name);
      G_get_window(&window) ;
      D_get_screen_window(&t, &b, &l, &r) ;
      D_do_conversions(&window, t, b, l, r) ;

      R_get_location_with_line(cur_screen_x,cur_screen_y,
                               &screen_x, &screen_y, &button) ;
      uy = D_d_to_u_row((double)screen_y);
      ux = D_d_to_u_col((double)screen_x);
      if (ux > window.east || ux < window.west ||
          uy > window.north || uy < window.south)
         {
         Dchoose(STA.name);
         R_standard_color(D_translate_color("black"));
         D_erase_window();
         R_standard_color(D_translate_color("red"));
         R_flush();
         DrawText(25,1,1,"OUTSIDE CURRENT WINDOW");
         button = 1;
         }
      else
         {
         switch (button)
         {
         case 1:
            /* print "earth" coords. and category info. in status window */
            Dchoose(STA.name);
            What(old_mapname,old_mapset,window,ux,uy);
            break;
         case 2:
            /* get profile data */
            InitProfile(&profile,window,cur_uy,cur_ux,uy,ux);
            if (err=ExtractProfile(&profile,old_mapname,old_mapset)==-1)
               {
               Dchoose(STA.name);
               R_standard_color(D_translate_color("black"));
               D_erase_window();
               R_standard_color(D_translate_color("red"));
               R_flush();
               DrawText(25,1,1,"ERROR: end-point outside");
               DrawText(25,2,1,"       of current window");
               }
            else if (err==-2)
               {
               printf("Error opening cell-file\n");
               exit(-1);
               }
            else if (err==-3)
               {
               printf("Error reading from cell-file\n");
               exit(-1);
               }
            else if (err==-4)
               {
               printf("Mysterious window inconsistancy error\n");
               exit(-1);
               }
            else
             {
             /* draw profile line on cell-file */
             black_and_white_line(screen_x,screen_y,cur_screen_x,cur_screen_y);

               /* select letter for current profile label */
               switch (CurrentWin)
                  {
                  case 0: ltr[0]='A'; ltr[1]=0; break;
                  case 1: ltr[0]='B'; ltr[1]=0; break;
                  case 2: ltr[0]='C'; ltr[1]=0; break;
                  case 3: ltr[0]='D'; ltr[1]=0; break;
                  default: ltr[0]='?'; ltr[1]=0; break;
                  }
               
               /* plot label in black */
               text_height=(int)(0.03*(b-t));
               text_width=(int)(0.03*(r-l));
               Dchoose(MAP.name);
               R_move_abs(screen_x,screen_y);
               if (screen_x<=cur_screen_x && screen_y>=cur_screen_y)
                  R_move_rel(-(text_width+2),(text_height+2));
               else if (screen_x<cur_screen_x && screen_y<=cur_screen_y)
                  R_move_rel(-(text_width+2),2);
               else if (screen_x>cur_screen_x)
                  R_move_rel(3,0);
               R_standard_color(D_translate_color("black"));
               R_text_size(text_width,text_height);
               R_text(ltr); 
               R_standard_color(D_translate_color("white"));
               
               /* plot label in white */
               R_move_abs(screen_x,screen_y);
               if (screen_x<=cur_screen_x && screen_y>=cur_screen_y)
                  R_move_rel(-(text_width+2),(text_height+2));
               else if (screen_x<cur_screen_x && screen_y<=cur_screen_y)
                  R_move_rel(-(text_width+2),2);
               else if (screen_x>cur_screen_x)
                  R_move_rel(3,0);
               R_move_rel(1,1);
               R_text(ltr); 
               R_standard_color(D_translate_color("black"));
               
               /*length = hypot(cur_ux - ux, cur_uy - uy);*/
            
               /* tell user about profile being plotted */ 
               Dchoose(STA.name);
               R_standard_color(D_translate_color("black"));
               D_erase_window();
               R_standard_color(D_translate_color("red"));
               R_flush();
               DrawText(25,1,1,"PLOTTING PROFILE");

               /* plot profile data in profile window */
               Dchoose(profiles[CurrentWin++].name);
               PlotProfile(profile,ltr,min,max);
               if (CurrentWin > 3) CurrentWin = 0;

               cur_screen_x = screen_x;
               cur_screen_y = screen_y;
               cur_ux = ux ;
               cur_uy = uy ;
               break;
               }
         default:
            break;
         }
        }
      }
   while (button != 3 && button != 2);

   /* display mouse-menu in mouse-menu window */
   Dchoose(MOU.name);
   R_standard_color(D_translate_color("black"));
   D_erase_window();
   R_standard_color(D_translate_color("red"));
   R_flush();
   DrawText(22,1,1,"GRASS PROGRAM: profile");
   R_standard_color(D_translate_color("white"));
   DrawText(22,2,1,"MOUSE   | Left:   DO ANOTHER");
   DrawText(22,3,1,"BUTTON  | Middle: CLEAR DISPLAY");
   DrawText(22,4,1,"MENU    | Right:  QUIT");

   R_get_location_with_pointer(&screen_x, &screen_y, &button) ;
   if (button == 3)
      return(0);
   else if (button == 2)
      {
      Dchoose(MAP.name);
      Derase(D_translate_color("black"));
      Dcell(old_mapname,old_mapset,0);
      for (i=0; i<=3; i++)
         {
         Dchoose(profiles[i].name);
         Derase(D_translate_color("black"));
         }
      CurrentWin=0;
      }
   else
      cfree(profile.ptr);
   }
}
