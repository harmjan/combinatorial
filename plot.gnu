set title "Heat map of walshls total runtime"

set xlabel "N"
set ylabel "K"
set zlabel "Walshls runtime"

set hidden3d
unset surface
set view 0,0
set contour
set cntrparam levels 20
set pm3d

splot "results.dat" using 1:2:($6+$7) with linespoints
