#!/usr/bin/env ruby


def gen_speedup_file(time_file, speedup_file)
	timeHash = {}
	File.open(time_file, 'r') do |time|
		time.each_line do |line|
			data = line.split(" ")
			timeHash[data[0].to_i] = data[1].to_f
		end
	end
	t0 = timeHash[1]
	File.open(speedup_file, "w+") do |speedup|
		timeHash.each do |key, value|
			speedup.puts("#{key} #{t0/value}")
		end
	end
end

def gen_matrix_file(max, filename)
	size = (1..max).reduce(:lcm)
	File.open(filename, "w") do |f| 
		f.puts(size)
		for i in 1..size
			for j in 1..size
				f.write("#{rand} ")
			end
			f.write("\n")
		end
	end
end

def gen_matrix_size(size, filename)
	puts "Generating a matrix of size #{size}"
	File.open(filename, "w") do |f| 
		f.puts(size)
		for i in 1..size
			for j in 1..size
				if i==j
					f.write("#{rand} ")
				else
					f.write("#{rand} ")
				end
			end
			f.write("\n")
		end
	end
end


def gen_plot_script(input, pltout, pngout)
	File.open(pltout, 'w+') do |f|  
		f.puts("set xlabel \"Nombre de processus\"\n"+
				"set ylabel \"Speedup\"\n"+
				"set term png\n"+
				"set output \"#{pngout}\"\n"+
				"set logscale\n"+
				"plot \"#{input}\" using 1:2 with linespoints title \"speedup\"\n")
	end 
end

def simulate(n, mata, matb, matc, nbIter)
	system("make timeclean")

	for i in 1..n
		nbProcess = i*i
		system("echo mpiexec -np #{nbProcess} ./main.out #{mata} #{matb} #{matc} #{nbIter}")
		system("mpiexec -np #{nbProcess} ./main.out #{mata} #{matb} #{matc} #{nbIter}")
	end
end

def gen_all_proc_var()
	gen_matrix_file(8, "mat.dat")
	simulate(7, "mat.dat", "mat.dat", "out.dat", 10)
	gen_speedup_file("time.dat", "speedup.dat")
	gen_plot_script("speedup.dat", "out.plt", "out.png")
	%x{gnuplot out.plt}
end


def gen_plot_script_triple(input1, input2, input3, pltout, pngout)
	File.open(pltout, 'w+') do |f|  
		f.puts("set xlabel \"Nombre de processus\"\n"+
				"set ylabel \"Speedup\"\n"+
				"set term png\n"+
				"set output \"#{pngout}\"\n"+
				"set logscale\n"+
				"plot \"#{input1}\" using 1:2 with linespoints, "+
				"\"#{input2}\" using 1:2 with linespoints, "+
				"\"#{input3}\" using 1:2 with linespoints")
	end 
end

def gen_proc_cst(max, nbProcess, matc, nbIter)
	time = {}
	matrix_name = "mat.dat"
	speedup_name = "speedup#{nbProcess}.dat"
	for i in 2..max
		gen_matrix_size(2**i, matrix_name)
		system("echo mpiexec -np #{nbProcess} ./main.out #{matrix_name} #{matrix_name} #{matc} #{nbIter}")
		system("mpiexec -np #{nbProcess} ./main.out #{matrix_name} #{matrix_name} #{matc} #{nbIter}")
		File.open("time.dat") do |timefile|
			timefile.each_line do |line|
				time[2**i] = line.split(" ")[1].to_f
			end
		end
	end

	File.open(speedup_name, "w+") do |spdupfile|
		time.each do |key, value|
			spdupfile.puts "#{key} #{value}"
		end
	end
end


def gen_all_triple_proc_cst()
	gen_proc_cst(9, 1, "output.dat", 10)
	gen_proc_cst(9, 4, "output.dat", 10)
	gen_proc_cst(9, 16, "output.dat", 10)
	gen_plot_script_triple("speedup1.dat", "speedup4.dat", "speedup16.dat", "out.plt", "out.png")
	%x{gnuplot out.plt}
end
#gen_all_triple_proc_cst()
gen_all_proc_var();
#gen_matrix_size(10, "mat.dat")