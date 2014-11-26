#!/usr/bin/env ruby

def gen_matrix_file(max, filename)
	size = (1..max).reduce(:lcm)
	puts(size)
	File.open(filename, "w") do |f| 
		f.puts(size)
		for i in 1..size
			for j in 1..size
				if i==j
					f.write("1 ")
				else
					f.write("0 ")
				end
			end
			f.write("\n")
		end
	end
end

def gen_plot_script(input, seqtime, pltout, pngout)
	File.open(pltout, 'w+') do |f|  
		f.puts("set xlabel \"Nombre de processus\"\n"+
				"set ylabel \"Speedup\"\n"+
				"set term png\n"+
				"set output \"#{pngout}\"\n"+
				"plot \"#{input}\" using 1:(#{seqtime}/$2) with linespoints title \"speedup\"")
	end 
end

def gnuplotit(matc)
	f = File.open("seqtime.dat", 'r')
	seqtime = f.gets.to_f
	
	plotfile = matc+".gp"
	pngfile = matc+".png"
	gen_plot_script("time.dat", seqtime, plotfile, pngfile)
	system("gnuplot #{plotfile}")
end

def simulate(n, mata, matb, matc, nbIter)
	system("make timeclean")

	for i in 1..n
		nbProcess = i*i
		system("echo mpiexec -np #{nbProcess} ./test_grid.out #{mata} #{matb} #{matc} #{nbIter}")
		system("mpiexec -np #{nbProcess} ./test_grid.out #{mata} #{matb} #{matc} #{nbIter}")
	end
	gnuplotit(matc)
end

gen_matrix_file(8, "mat.dat")
simulate(7, "mat.dat", "mat.dat", "out.dat", 10)
gnuplotit("out.dat")

