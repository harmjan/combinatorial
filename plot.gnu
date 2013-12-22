reset

set xlabel "N"
set ylabel "K"
set zlabel "Walshls runtime"

set hidden3d
unset surface
set view 0,0
set contour
set cntrparam levels 20
set pm3d

set multiplot
set multiplot layout 2,3
splot "results.dat" using 1:2:3 title "Naive runtime" with linespoints
splot "results.dat" using 1:2:($6+$7) title "Walshls runtime" with linespoints
splot "results.dat" using 1:2:($3-($6+$7)) title "Naive total runtime minus walshls total runtime" with linespoints
splot "results.dat" using 1:2:($3/$4) title "Naive runtime per iteration" with linespoints
splot "results.dat" using 1:2:($7/$8) title "Walshls runtime per iteration (without preprocessing)" with linespoints
splot "results.dat" using 1:2:($3/($6+$7)) title "Naive runtime diveded by walshls total time" with linespoints
unset multiplot
