set   autoscale                        # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set logscale x 2
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title "Perf de ddot en fonction de la taille des vecteurs"
set xlabel "Taille (en ko)"
set ylabel "Perfs (en MFlop/s)"
set term png
set output "ddot1.png"
plot [16:4096][] "ddot.txt" using 1:2 with linespoints lc rgb "blue" title "ddot"
#pause -1
