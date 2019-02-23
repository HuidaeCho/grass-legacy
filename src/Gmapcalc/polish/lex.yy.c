# include "stdio.h"
# define U(x) x
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX 200
# define output(c) putc(c,yyout)
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
# define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng; extern char yytext[];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
FILE *yyin = {stdin}, *yyout = {stdout};
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;
# define YYNEWLINE 10
yylex(){
int nstr; extern int yyprevious;
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:
 ;
break;
case 2:
             { return AND; }
break;
case 3:
             { return OR; }
break;
case 4:
               { return GT; }
break;
case 5:
             { return GE; }
break;
case 6:
               { return LT; }
break;
case 7:
             { return LE; }
break;
case 8:
             { return EQ; }
break;
case 9:
             { return NE; }
break;
case 10:
case 11:
{
		yylval = store (yytext);
		return FLOAT ;
	       }
break;
case 12:
    {
		yylval = atoi (yytext);
		return INTEGER;
	    }
break;
case 13:
      { return COLOR_RED;}
break;
case 14:
      { return COLOR_GREEN;}
break;
case 15:
      { return COLOR_BLUE;}
break;
case 16:
          { return COLOR_GRAY;}
break;
case 17:
 {
	        yylval = store (yytext);
		begin_function();
		return FUNCTION; 
	    }
break;
case 18:
             {
		      yylval = store (yytext);
		      return NAME; 
	             }
break;
case 19:
{
		      yylval = store (yytext);
		      return STRING; 
	             }
break;
case 20:
  case 21:
  {
		yytext[yyleng-1] = 0;
		yylval = store (yytext+1);
		return STRING; 
	    }
break;
case 22:
     case 23:
         { return (int) *yytext; }
break;
case -1:
break;
default:
fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */
int yyvstop[] = {
0,

12,
0,

12,
0,

18,
22,
-17,
0,

1,
22,
0,

23,
0,

22,
0,

22,
0,

16,
22,
0,

22,
0,

22,
0,

22,
0,

18,
22,
-17,
0,

12,
18,
22,
-17,
0,

6,
22,
0,

22,
0,

4,
22,
0,

18,
22,
-17,
0,

18,
22,
-17,
0,

18,
22,
-17,
0,

22,
0,

18,
-17,
0,

17,
0,

18,
-17,
0,

9,
0,

2,
0,

11,
18,
-17,
0,

10,
18,
-17,
0,

12,
18,
-17,
0,

7,
0,

8,
0,

5,
0,

15,
0,

14,
0,

13,
0,

3,
0,

18,
-17,
0,

20,
0,

21,
0,

10,
11,
18,
-17,
0,

18,
19,
-17,
0,

18,
19,
-17,
0,

19,
0,

19,
0,

18,
19,
-17,
0,

19,
0,
0};
# define YYTYPE char
struct yywork { YYTYPE verify, advance; } yycrank[] = {
0,0,	0,0,	1,3,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,4,	1,5,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
22,22,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,6,	1,7,	
1,8,	26,41,	1,9,	1,10,	
1,11,	1,9,	1,9,	1,9,	
1,9,	1,9,	1,9,	1,12,	
1,9,	1,13,	10,27,	22,22,	
28,42,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	22,23,	
0,0,	1,14,	1,15,	1,16,	
6,25,	1,9,	14,32,	1,17,	
15,33,	16,34,	0,0,	0,0,	
1,18,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	2,8,	
0,0,	2,9,	2,10,	1,19,	
2,9,	2,9,	2,9,	2,9,	
2,9,	2,9,	2,12,	2,9,	
1,9,	0,0,	1,9,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
2,14,	2,15,	2,16,	0,0,	
2,9,	0,0,	3,21,	39,44,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	3,22,	3,0,	
0,0,	0,0,	0,0,	0,0,	
22,39,	1,20,	20,38,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	2,9,	
0,0,	2,9,	0,0,	0,0,	
0,0,	0,0,	3,0,	3,0,	
3,0,	0,0,	3,0,	3,0,	
3,0,	3,23,	3,0,	3,0,	
3,0,	3,0,	3,0,	0,0,	
3,0,	3,21,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
2,20,	3,0,	3,0,	3,0,	
0,0,	3,0,	7,26,	3,21,	
0,0,	11,28,	0,0,	0,0,	
3,21,	0,0,	7,26,	7,26,	
0,0,	11,28,	11,28,	0,0,	
0,0,	0,0,	0,0,	3,21,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
3,0,	0,0,	3,0,	0,0,	
0,0,	0,0,	7,26,	7,0,	
0,0,	11,28,	11,28,	0,0,	
7,26,	0,0,	3,24,	11,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	7,26,	0,0,	0,0,	
11,28,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	3,0,	0,0,	0,0,	
12,0,	0,0,	0,0,	7,26,	
0,0,	0,0,	11,28,	0,0,	
7,26,	0,0,	0,0,	11,28,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	7,26,	
0,0,	0,0,	11,28,	12,0,	
12,0,	12,0,	0,0,	12,0,	
12,0,	12,0,	12,23,	12,0,	
12,0,	12,0,	12,0,	12,0,	
13,0,	12,0,	12,29,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	12,0,	12,0,	
12,0,	0,0,	12,0,	0,0,	
0,0,	0,0,	0,0,	13,0,	
13,0,	13,0,	0,0,	13,0,	
13,0,	13,0,	13,23,	13,0,	
13,0,	13,0,	13,0,	13,0,	
13,30,	13,0,	13,31,	17,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	12,0,	0,0,	12,0,	
0,0,	0,0,	13,0,	13,0,	
13,0,	0,0,	13,0,	0,0,	
0,0,	0,0,	0,0,	12,24,	
0,0,	0,0,	17,0,	17,0,	
17,35,	0,0,	17,0,	17,0,	
17,0,	17,23,	17,0,	17,0,	
17,0,	17,0,	17,0,	18,0,	
17,0,	0,0,	12,0,	0,0,	
0,0,	13,0,	0,0,	13,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	17,0,	17,0,	17,0,	
0,0,	17,0,	0,0,	13,24,	
0,0,	0,0,	18,0,	18,0,	
18,36,	0,0,	18,0,	18,0,	
18,0,	18,23,	18,0,	18,0,	
18,0,	18,0,	18,0,	0,0,	
18,0,	19,0,	13,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
17,0,	0,0,	17,0,	0,0,	
0,0,	18,0,	18,0,	18,0,	
0,0,	18,0,	0,0,	0,0,	
0,0,	0,0,	17,24,	0,0,	
19,0,	19,0,	19,37,	0,0,	
19,0,	19,0,	19,0,	19,23,	
19,0,	19,0,	19,0,	19,0,	
19,0,	21,0,	19,0,	0,0,	
0,0,	17,0,	0,0,	0,0,	
18,0,	0,0,	18,0,	0,0,	
0,0,	0,0,	0,0,	19,0,	
19,0,	19,0,	0,0,	19,0,	
0,0,	0,0,	18,24,	0,0,	
21,0,	21,0,	21,0,	0,0,	
21,0,	21,0,	21,0,	21,23,	
21,0,	21,0,	21,0,	21,0,	
21,0,	0,0,	21,0,	24,0,	
0,0,	18,0,	0,0,	0,0,	
0,0,	0,0,	19,0,	0,0,	
19,0,	0,0,	0,0,	21,0,	
21,0,	21,0,	0,0,	21,0,	
0,0,	0,0,	0,0,	0,0,	
19,24,	0,0,	24,0,	24,0,	
24,0,	0,0,	24,0,	24,0,	
24,0,	24,23,	24,0,	24,0,	
24,0,	24,0,	24,0,	29,0,	
24,0,	0,0,	0,0,	19,0,	
0,0,	0,0,	21,0,	0,0,	
21,0,	0,0,	0,0,	0,0,	
0,0,	24,0,	24,0,	24,0,	
0,0,	24,0,	0,0,	0,0,	
21,24,	0,0,	29,0,	29,0,	
29,0,	0,0,	29,0,	29,0,	
29,0,	29,23,	29,0,	29,0,	
29,0,	29,0,	29,0,	0,0,	
29,0,	29,29,	0,0,	21,0,	
0,0,	30,0,	0,0,	0,0,	
24,0,	0,0,	24,0,	0,0,	
0,0,	29,0,	29,0,	29,0,	
0,0,	29,0,	0,0,	0,0,	
0,0,	0,0,	24,24,	0,0,	
0,0,	0,0,	0,0,	24,40,	
30,0,	30,0,	30,0,	0,0,	
30,0,	30,0,	30,0,	30,23,	
30,0,	30,0,	30,0,	30,0,	
30,0,	24,0,	30,0,	30,43,	
29,0,	0,0,	29,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	30,0,	
30,0,	30,0,	29,24,	30,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	31,0,	0,0,	
0,0,	29,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	30,0,	0,0,	
30,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	31,0,	31,0,	31,0,	
30,24,	31,0,	31,0,	31,0,	
31,23,	31,0,	31,0,	31,0,	
31,0,	31,0,	31,30,	31,0,	
31,31,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	30,0,	
0,0,	0,0,	0,0,	0,0,	
31,0,	31,0,	31,0,	0,0,	
31,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	40,45,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	40,46,	40,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	31,0,	
0,0,	31,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	40,0,	40,0,	
40,0,	31,24,	40,0,	40,0,	
40,0,	40,23,	40,0,	40,0,	
40,0,	40,0,	40,0,	0,0,	
40,0,	40,45,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
31,0,	0,0,	0,0,	0,0,	
0,0,	40,0,	40,0,	40,0,	
0,0,	40,0,	0,0,	40,45,	
0,0,	0,0,	0,0,	0,0,	
40,45,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
43,0,	0,0,	0,0,	40,45,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
40,0,	0,0,	40,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	43,0,	
43,0,	43,0,	40,47,	43,0,	
43,0,	43,0,	43,23,	43,0,	
43,0,	43,0,	43,0,	43,0,	
0,0,	43,0,	43,43,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	40,0,	0,0,	0,0,	
0,0,	0,0,	43,0,	43,0,	
43,0,	0,0,	43,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
44,48,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
44,44,	44,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	43,0,	0,0,	43,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
44,0,	44,0,	44,0,	43,24,	
44,0,	44,0,	44,0,	44,0,	
44,0,	44,0,	44,0,	44,0,	
44,0,	0,0,	44,0,	44,48,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	43,0,	0,0,	
0,0,	0,0,	0,0,	44,0,	
44,0,	44,0,	0,0,	44,0,	
0,0,	44,48,	0,0,	0,0,	
0,0,	0,0,	44,48,	0,0,	
0,0,	45,22,	45,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	44,48,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	44,0,	0,0,	
44,0,	0,0,	0,0,	0,0,	
0,0,	45,0,	45,0,	45,0,	
0,0,	45,0,	45,0,	45,0,	
45,23,	45,0,	45,0,	45,0,	
45,0,	45,0,	0,0,	45,0,	
46,46,	46,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	44,0,	
45,0,	45,0,	45,0,	0,0,	
45,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
46,0,	46,0,	46,0,	0,0,	
46,0,	46,0,	46,0,	46,23,	
46,0,	46,0,	46,0,	46,0,	
46,0,	0,0,	46,0,	47,22,	
47,0,	0,0,	0,0,	45,0,	
0,0,	45,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	46,0,	
46,0,	46,0,	0,0,	46,0,	
0,0,	45,47,	0,0,	0,0,	
0,0,	0,0,	0,0,	47,0,	
47,0,	47,0,	0,0,	47,0,	
47,0,	47,0,	47,23,	47,0,	
47,0,	47,0,	47,0,	47,0,	
45,0,	47,0,	48,0,	48,0,	
0,0,	0,0,	46,0,	0,0,	
46,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	47,0,	47,0,	
47,0,	0,0,	47,0,	0,0,	
46,49,	0,0,	0,0,	0,0,	
0,0,	48,0,	48,0,	48,0,	
48,0,	0,0,	48,0,	48,0,	
48,0,	48,0,	48,0,	48,0,	
48,0,	48,0,	48,0,	46,0,	
48,0,	0,0,	0,0,	0,0,	
0,0,	47,0,	0,0,	47,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	48,0,	48,0,	48,0,	
0,0,	48,0,	0,0,	47,47,	
0,0,	0,0,	0,0,	0,0,	
47,50,	0,0,	0,0,	49,0,	
49,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	47,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
48,0,	0,0,	48,0,	0,0,	
0,0,	0,0,	49,0,	49,0,	
49,0,	49,0,	0,0,	49,0,	
49,0,	49,0,	49,0,	49,0,	
49,0,	49,0,	49,0,	49,0,	
50,0,	49,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	48,0,	49,0,	49,0,	
49,0,	0,0,	49,0,	0,0,	
0,0,	0,0,	0,0,	50,0,	
50,0,	50,0,	0,0,	50,0,	
50,0,	50,0,	50,23,	50,0,	
50,0,	50,0,	50,0,	50,0,	
0,0,	50,0,	51,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	49,0,	0,0,	49,0,	
0,0,	0,0,	50,0,	50,0,	
50,0,	0,0,	50,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	51,0,	51,0,	51,0,	
49,51,	51,0,	51,0,	51,0,	
51,0,	51,0,	51,0,	51,0,	
51,0,	51,0,	0,0,	51,0,	
0,0,	0,0,	49,0,	0,0,	
0,0,	50,0,	0,0,	50,0,	
0,0,	0,0,	0,0,	0,0,	
51,0,	51,0,	51,0,	0,0,	
51,0,	0,0,	0,0,	50,47,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	50,0,	0,0,	
0,0,	0,0,	0,0,	51,0,	
0,0,	51,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
51,0,	0,0,	0,0,	0,0,	
0,0};
struct yysvf yysvec[] = {
0,	0,	0,
yycrank+-1,	0,		yyvstop+1,
yycrank+-44,	yysvec+1,	yyvstop+3,
yycrank+-109,	0,		yyvstop+5,
yycrank+0,	0,		yyvstop+9,
yycrank+0,	0,		yyvstop+12,
yycrank+3,	0,		yyvstop+14,
yycrank+-173,	0,		yyvstop+16,
yycrank+0,	0,		yyvstop+18,
yycrank+0,	0,		yyvstop+21,
yycrank+12,	0,		yyvstop+23,
yycrank+-176,	0,		yyvstop+25,
yycrank+-226,	yysvec+3,	yyvstop+27,
yycrank+-262,	yysvec+3,	yyvstop+31,
yycrank+5,	0,		yyvstop+36,
yycrank+7,	0,		yyvstop+39,
yycrank+8,	0,		yyvstop+41,
yycrank+-301,	yysvec+3,	yyvstop+44,
yycrank+-337,	yysvec+3,	yyvstop+48,
yycrank+-375,	yysvec+3,	yyvstop+52,
yycrank+2,	0,		yyvstop+56,
yycrank+-411,	yysvec+3,	yyvstop+58,
yycrank+19,	0,		0,	
yycrank+0,	0,		yyvstop+61,
yycrank+-449,	yysvec+3,	yyvstop+63,
yycrank+0,	0,		yyvstop+66,
yycrank+-3,	yysvec+7,	0,	
yycrank+0,	0,		yyvstop+68,
yycrank+-13,	yysvec+11,	0,	
yycrank+-485,	yysvec+3,	yyvstop+70,
yycrank+-527,	yysvec+3,	yyvstop+74,
yycrank+-596,	yysvec+3,	yyvstop+78,
yycrank+0,	0,		yyvstop+82,
yycrank+0,	0,		yyvstop+84,
yycrank+0,	0,		yyvstop+86,
yycrank+0,	0,		yyvstop+88,
yycrank+0,	0,		yyvstop+90,
yycrank+0,	0,		yyvstop+92,
yycrank+0,	0,		yyvstop+94,
yycrank+1,	0,		0,	
yycrank+-665,	0,		yyvstop+96,
yycrank+0,	0,		yyvstop+99,
yycrank+0,	0,		yyvstop+101,
yycrank+-734,	yysvec+3,	yyvstop+103,
yycrank+-803,	0,		0,	
yycrank+-868,	yysvec+40,	yyvstop+108,
yycrank+-907,	yysvec+44,	0,	
yycrank+-946,	yysvec+40,	yyvstop+112,
yycrank+-985,	yysvec+44,	yyvstop+116,
yycrank+-1050,	yysvec+44,	yyvstop+118,
yycrank+-1086,	yysvec+40,	yyvstop+120,
yycrank+-1124,	yysvec+44,	yyvstop+124,
0,	0,	0};
struct yywork *yytop = yycrank+1248;
struct yysvf *yybgin = yysvec+1;
char yymatch[] = {
00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,011 ,012 ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
011 ,'!' ,'"' ,'!' ,01  ,'!' ,'!' ,047 ,
'!' ,'!' ,'!' ,'!' ,'!' ,'!' ,01  ,'!' ,
'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,
'0' ,'0' ,01  ,01  ,'!' ,'!' ,'!' ,01  ,
'!' ,01  ,'B' ,01  ,01  ,01  ,01  ,'G' ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,'R' ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,'!' ,01  ,'!' ,01  ,01  ,
01  ,01  ,'B' ,01  ,01  ,01  ,01  ,'G' ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,'R' ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,'!' ,01  ,01  ,01  ,
0};
char yyextra[] = {
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,1,0,0,0,0,0,0,
0};
/* #ident	"@(#)libl:lib/ncform	1.3" */
#ident	"$Header: /net/siouxsie/d2/3.2A/source/root/usr/src/lib/libl/RCS/ncform,v 1.4 86/09/30 14:55:39 bruce Exp $"
int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
char yytext[YYLMAX];
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
yylook(){
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych, yyfirst;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
	char *yylastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	yyfirst=1;
	if (!yymorfg)
		yylastch = yytext;
	else {
		yymorfg=0;
		yylastch = yytext+yyleng;
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = yystate->yystoff;
			if(yyt == yycrank && !yyfirst){  /* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
				}
			*yylastch++ = yych = input();
			yyfirst=0;
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( (int)yyt > (int)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((int)yyt < (int)yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
					sprint(yytext);
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
		if (yytext[0] == 0  /* && feof(yyin) */)
			{
			yysptr=yysbuf;
			return(0);
			}
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
		yylastch=yytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
yyback(p, m)
	int *p;
{
if (p==0) return(0);
while (*p)
	{
	if (*p++ == m)
		return(1);
	}
return(0);
}
	/* the following are only used in the lex library */
yyinput(){
	return(input());
	}
yyoutput(c)
  int c; {
	output(c);
	}
yyunput(c)
   int c; {
	unput(c);
	}
