set   autoscale                        # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
#set logscale x
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title "Perf de dgemm en fonction du nombre d'éléments (MFlops)"
set xlabel "Nombre d'éléments"
set ylabel "Perfs (en MFlop/s)"
#set term png
#set output "figure.png"
plot "dgemmkij.txt" using 1:2 with linespoints lc rgb "blue" title "dgemmkij",\
	 "dgemmijk.txt" using 1:2 with linespoints lc rgb "red" title "dgemmijk",\
	 "dgemmikj.txt" using 1:2 with linespoints lc rgb "green" title "dgemmikj",\
	 "dgemmjik.txt" using 1:2 with linespoints lc rgb "orange" title "dgemmjik",\
 	 "dgemmblock.txt" using 1:2 with linespoints lc rgb "pink" title "dgemmblock",\