set terminal postscript
set output "| lp"
set time
replot
set terminal X11
set output
set notime
replot
