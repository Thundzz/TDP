set terminal gif animate delay 10
set output 'particles.gif'
stats 'datafile' nooutput
set xrange [-2000:2001]
set yrange [-2000:2001]
do for [i=1:int(STATS_blocks)] {
