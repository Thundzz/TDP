set   autoscale                        # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
#set logscale x
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title "Perf de dgemm en fonction du nombre d'éléments (MFlops)"
set xlabel "Dimension"
set ylabel "Perfs (en MFlop/s)"
set term png
set key below
set output "dgemm_block.png"
plot "dgemmblock.txt" using 1:2 with linespoints lc rgb "blue" title "dgemm_blocked",\
	 "dgemmijk.txt" using 1:2 with linespoints lc rgb "red" title "dgemm (i,j,k)"#,\
	 "dgemmkij.txt" using 1:2 with linespoints lc rgb "blue" title "dgemm (k,i,j)",\
	 "dgemmikj.txt" using 1:2 with linespoints lc rgb "green" title "dgemm (i,k,j)",\
	 "dgemmjik.txt" using 1:2 with linespoints lc rgb "orange" title "dgemm (j,i,k)",\
 	 "dgemmblock100.txt" using 1:2 with linespoints lc rgb "purple" title "dgemmblock100",\
  	 "dgemmscal.txt" using 1:2 with linespoints lc rgb "black" title "dgemmscal"

