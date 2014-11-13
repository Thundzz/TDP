set term png size 800,600
set output "orbit.png"

set xrange [-2000:2001]
set yrange [-2000:2001]
plot 'datafile0' using 2:3 with linespoints lc rgb 'red' title 'Processeur0, Particule0', 'datafile0' using 4:5 with linespoints lc rgb 'red' title 'Processeur0, Particule1', 'datafile1' using 2:3 with linespoints lc rgb 'green' title 'Processeur1, Particule0', 'datafile1' using 4:5 with linespoints lc rgb 'green' title 'Processeur1, Particule1', 'datafile2' using 2:3 with linespoints lc rgb 'blue' title 'Processeur2, Particule0', 'datafile2' using 4:5 with linespoints lc rgb 'blue' title 'Processeur2, Particule1', 'datafile3' using 2:3 with linespoints lc rgb 'orange' title 'Processeur3, Particule0', 'datafile3' using 4:5 with linespoints lc rgb 'orange' title 'Processeur3, Particule1'
