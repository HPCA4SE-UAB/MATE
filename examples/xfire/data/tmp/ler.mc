    fscanf (fp,"%d  %d ", &ni, &nj ); 
    for (i=1;i<=ni;i++)
      for (j=1;j<=nj;j++)
	  { 
            fscanf ( fp,"%f %f %f %d", &x[i]   , &y[j], &z[i][j], &mdpop[i][j] ) ;
           } ;

