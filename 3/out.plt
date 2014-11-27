set xlabel "Nombre de processus"
set ylabel "Speedup"
set term png
set output "out.png"
plot "speedup.dat" using 1:2 with linespoints title "speedup"
