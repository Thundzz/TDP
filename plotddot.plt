set   autoscale                        # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set logscale x
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title "Perf de ddot en fonction du nombre d'éléments (MFlops)"
set xlabel "Nombre d'éléments"
set ylabel "Perfs (en MFlop/s)"
#set term png
#set output "figure.png"
plot  [50:9e5][0:3000]  "ddot.txt" using 1:2 with linespoints lc rgb "blue" title "ddot",\
						"ddot2.txt" using 1:2 with linespoints lc rgb "red" title "ddot",\
						"ddot3.txt" using 1:2 with linespoints lc rgb "green" title "ddot",\
						"ddot4.txt" using 1:2 with linespoints lc rgb "orange" title "ddot"

#pause -1
