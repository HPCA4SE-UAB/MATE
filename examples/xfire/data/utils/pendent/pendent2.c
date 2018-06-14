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
  float xmax=10000;
  float xmin=1000;
  float ymax=10000;
  float ymin=1000;
  float xdelta=2;
  float ydelta=2;
  int xdim=5000;
  int ydim=5000;

  if (argc==1)
    {
      printf("Usage: \n");
      printf("pendent2 <file> <alpha> <model>\n");
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
	      fprintf(out,"%.1f %.1f %.1f %d\n",x,y,z,model);
	      y=y+ydelta;
	    }
	  x=x+xdelta;
	  y=ymin;
	}
      fclose(out);
    }
  return 0;
}
