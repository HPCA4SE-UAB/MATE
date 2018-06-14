#include <stdlib.h>
#include <stdio.h>

int main(int argc,char** argv)
{
  FILE* in;
  FILE* out;
  int i,j;
  int xdim,ydim,model;
  float x,y,z;
  char namemap[256];
  char nameout[256];

  if (argc==1)
    {
      printf("Usage: \n");
      printf("pendent <filemap> <fileout> \n");
    }
  else
    {
      strcpy(namemap,argv[1]);
      strcpy(nameout,argv[2]);

      in=fopen(namemap,"r");
      out=fopen(nameout,"w");

      fscanf(in,"%d %d",&xdim,&ydim);
  
      for (i=1;i<=xdim;i++)
	for (j=1;j<=ydim;j++)
	  {
	    fscanf(in,"%f %f %f %d",&x,&y,&z,&model);
	    fprintf(out,"%f %f %f\n",x,y,z);
	  }
      fclose(in);
      fclose(out);
    }
  return 0;
}
