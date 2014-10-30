set   autoscale                        # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set logscale x 2
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title "Perf de ddot en fonction du nombre d'éléments (MFlops)"
set xlabel "Nombre d'éléments"
set ylabel "Perfs (en MFlop/s)"
#set term png
#set output "figure.png"
plot [][] "ddot.txt" using 1:2 with linespoints lc rgb "blue" title "ddot"
#pause -1
