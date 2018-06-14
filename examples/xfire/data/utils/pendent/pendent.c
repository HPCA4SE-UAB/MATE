#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main(int argc,char** argv)
{
  FILE* out;
  char name[256];
  int alpha;
  int i,j;
  int model;
  float x,y,z;
  float zmin,zmax;
  float xmax=562300;
  float xmin=555600;
  float ymax=4456400;
  float ymin=4451300;
  float xdelta=50;
  float ydelta=50;
  int xdim=135;
  int ydim=103;

  if (argc==1)
    {
      printf("Usage: \n");
      printf("pendent <file> <alpha> <model>\n");
    }
  else
    {
      strcpy(name,argv[1]);
      alpha=atoi(argv[2]);
      model=atoi(argv[3]);

      out=fopen(name,"w");
      
      fprintf(out,"%d %d\n",xdim,ydim);

      x=xmin;
      y=ymin;
      for (i=1;i<=xdim;i++)
	{
	  for (j=1;j<=ydim;j++)
	    {
	      zmax=(xmax-xmin)*tan((float)(alpha*M_PI)/180);
	      z=(zmax*(xmax-x))/(xmax-xmin);
	      fprintf(out,"%f %f %f %d\n",x,y,z,model);
	      y=y+ydelta;
	    }
	  x=x+xdelta;
	  y=ymin;
	}
      fclose(out);
    }
  return 0;
}
