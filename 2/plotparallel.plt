set terminal gif animate delay 10
set output 'particles.gif'
stats 'datafile0' nooutput
set xrange [-2000:2001]
set yrange [-2000:2001]

do for [i=1:int(STATS_blocks)] {
    plot 'datafile0' index (i-1) using 2:3 with circle lc rgb "red",\
    'datafile0' index (i-1) using 4:5 with circle lc rgb "red",\
    'datafile1' index (i-1) using 2:3 with circle lc rgb "blue",\
    'datafile1' index (i-1) using 4:5 with circle lc rgb "blue"
}
