#include <stdio.h>
#include <string.h>
struct POINTS {
	double x,y;
	};

main (int argc, char *argv[])
{
int i,rows,cols,zone,copy,j;
char buf[100],buf2[100],buf3[100],name[100],pdatum[100],sdatum[100],proj[100],*ptr;
double resol;
struct POINTS p,s,nwp,nws,swp,sws,nep,nes,sep,ses;


while (fgets(buf,100,stdin) != NULL) {
if (strstr(buf,"END_USGS_HEADER") != NULL) break;

/********** do NAME ************************/
if (strstr(buf,"QUADRANGLE_NAME") != NULL) {
	copy = 0;
	j = 0;
	for (i=0;i<strlen(buf);i++){
		if(buf[i] == '"') copy +=1;
		else if(copy == 1) name[j++] = buf[i];
		}
	name[j] = (char)NULL;
	}
else if (strstr(buf,"RASTER_ORDER") != NULL) {
/*** Check data ordering ******/
	if (strstr(buf,"LEFT_RIGHT/TOP_BOTTOM") == NULL) {
		fprintf(stderr,"Unable to handle this data ordering: %s\n",buf);
		exit();
		}
	}
else if (strstr(buf,"SAMPLES_AND_LINES") != NULL) {
/********* Get rows and columns *************/
	sscanf(buf,"%s %d %d %s",buf2,&cols,&rows,buf3);
	}
else if (strstr(buf,"BAND_CONTENT") != NULL) {
/*********** Get # bands now handlws one ***************/
	if (strstr(buf,"BLACK&WHITE") == NULL) {
		fprintf(stderr,"There are multiple bands in this image.\n This routine presently handles only one\n");
		exit();
		}
	}
else if (strstr(buf,"HORIZONTAL_DATUM") != NULL) {
/******* Get Datums ***************/
	if (strstr(buf,"SECONDARY_HORIZONTAL_DATUM") != NULL) {
		sscanf(buf,"%s %s %s",buf2,sdatum,buf3);
		}
	else sscanf(buf,"%s %s %s",buf2,pdatum,buf3);
	}
else if (strstr(buf,"HORIZONTAL_COORDINATE_SYSTEM") != NULL) {
/************** Get projection *************/
	sscanf(buf,"%s %s %s",buf2,proj,buf3);
	}
else if (strstr(buf,"COORDINATE_ZONE") != NULL) {
/************** Get projection *************/
	sscanf(buf,"%s %d %s",buf2,&zone,buf3);
	}

/*********** Get primary tick values ***************/
else if (strstr(buf,"NW_QUAD_CORNER_XY") != NULL) {
	sscanf(buf,"%s %lf %lf %s",buf2,&nwp.x,&nwp.y,buf3);
	}
else if (strstr(buf,"NE_QUAD_CORNER_XY") != NULL) {
	sscanf(buf,"%s %lf %lf %s",buf2,&nep.x,&nep.y,buf3);
	}
else if (strstr(buf,"SW_QUAD_CORNER_XY") != NULL) {
	sscanf(buf,"%s %lf %lf %s",buf2,&swp.x,&swp.y,buf3);
	}
else if (strstr(buf,"SE_QUAD_CORNER_XY") != NULL) {
	sscanf(buf,"%s %lf %lf %s",buf2,&sep.x,&sep.y,buf3);
	}

/******************* Read secondary cordinate values ***********/
else if (strstr(buf,"SECONDARY_NW_QUAD_XY") != NULL) {
	sscanf(buf,"%s %lf %lf %s",buf2,&nws.x,&nws.y,buf3);
	}
else if (strstr(buf,"SECONDARY_NE_QUAD_XY") != NULL) {
	sscanf(buf,"%s %lf %lf %s",buf2,&nes.x,&nes.y,buf3);
	}
else if (strstr(buf,"SECONDARY_SW_QUAD_XY") != NULL) {
	sscanf(buf,"%s %lf %lf %s",buf2,&sws.x,&sws.y,buf3);
	}
else if (strstr(buf,"SECONDARY_SE_QUAD_XY") != NULL) {
	sscanf(buf,"%s %lf %lf %s",buf2,&ses.x,&ses.y,buf3);
	}

/*********** Get ground resolution *************/
else if (strstr(buf,"HORIZONTAL_RESOLUTION") != NULL) {
	sscanf(buf,"%s %lf %s",buf2,&resol,buf3);
	}

/************** Get ground coordinates of pixel 1,1 ***********/
else if (strstr(buf,"XY_ORIGIN") != NULL) {
	if (strstr(buf,"SECONDARY_XY_ORIGIN") != NULL) {
		sscanf(buf,"%s %lf %lf %s",buf2,&s.x,&s.y,buf3);
		}
	else sscanf(buf,"%s %lf %lf %s",buf2,&p.x,&p.y,buf3);
	}


}
	printf("NAME: %s\n",name);
	printf("Primary datum: %s   ",pdatum);
	printf("Secondary datum: %s\n",sdatum);
	printf("The coordinate system: %s   ZONE: %d\n",proj,zone);
	printf("ROWS: %d  COLUMNS: %d\n",rows,cols);
	printf("ORIGIN: X: %.1lf   Y: %.1lf  [primary coordinates]\n",p.x,p.y);
	printf("ORIGIN: X: %.1lf   Y: %.1lf  [secondary coordinates]\n",s.x,s.y);
	printf("GROUND RESOLUTION: %.1lf\n",resol);

printf("Last char %s\n--%c--%c--\n",buf,buf[strlen(buf) -2],buf[strlen(buf) -1]);
}
