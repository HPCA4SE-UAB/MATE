#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main(int argc,char** argv)
{
  FILE* out;
  int i,j;
  int xc,yc,radi,npunts;
  int x,y;
  float a,ainc;

  if (argc==1)
    {
      printf("Usage: \n");
      printf("cercle <xc> <yc> <radi> <npunts>\n");
    }
  else
    {
      xc=atoi(argv[1]);
      yc=atoi(argv[2]);
      radi=atoi(argv[3]);
      npunts=atoi(argv[4]);

      out=fopen("cercle.out","w");
      
  	/* Un sol front */
      fprintf(out,"%d\n",1);
      fprintf(out,"CLOSEDCURVE\n");
      fprintf(out,"%d\n",npunts);

      a=0.0;
      ainc=(2.0*M_PI)/npunts;
      
      for (i=0;i<npunts;i++)
	{
	  x=rint(xc+radi*sin(a));
	  y=rint(yc+radi*cos(a));

	  fprintf(out,"%d %d \n",x,y);
	  a=a+ainc;
	}
      fclose(out);
    }
  return 0;
}
