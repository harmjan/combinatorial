reset

set xrange [100:2000]
set xlabel "N"
set ylabel "K"

set hidden3d
unset surface
set view 0,0
set contour
set cntrparam levels 7
set pm3d

unset key
set format z ''

set terminal postscript eps colour size 18cm,12.5cm

set output "numberOfIterations.eps"
splot "results_alpha_100.dat" using 1:2:4 with linespoints

set output "walshlsIterationRuntime.eps"
splot "results_alpha_100.dat" using 1:2:($7/$8) with linespoints

set output "walshlsTotalIterationRuntime.eps"
splot "results_alpha_100.dat" using 1:2:(($6+$7)/$8) with linespoints

set output "naiveIterationRuntime.eps"
splot "results_alpha_100.dat" using 1:2:($3/$4) with linespoints

set output "walshlsRuntime.eps"
splot "results_alpha_100.dat" using 1:2:($6+$7) with linespoints

set output "naiveRuntime.eps"
splot "results_alpha_100.dat" using 1:2:3 with linespoints

set cntrparam levels discrete 0.25,0.5,1,2,4

set output "comparisonRuntime.eps"
splot "results_alpha_100.dat" using 1:2:($3/($6+$7)) with linespoints

set output "alphaComparison.eps"
set multiplot
set multiplot layout 1,2
splot "results_alpha_100.dat" using 1:2:($6+$7) with linespoints
splot "results_alpha_1000000.dat" using 1:2:($3+$4) with linespoints
unset multiplot
