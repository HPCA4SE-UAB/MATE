set autoscale
set parametric
set nocontour

set data style points
splot "mapa.out"
pause -1 "Apreta Return"

set data style lines
splot "mapa.out"
pause -1 "Apreta Return"

set dgrid3d 50,50,2
set contour
set data style lines
splot "mapa.out"
pause -1 "Apreta Return"

