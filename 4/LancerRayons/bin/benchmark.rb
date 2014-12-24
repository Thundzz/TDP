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

def gen_plot_script(input, pltout, pngout)
	File.open(pltout, 'w+') do |f|  
		f.puts("set xlabel \"Nombre de processus\"\n"+
				"set ylabel \"Speedup\"\n"+
				"set term png\n"+
				"set output \"#{pngout}\"\n"+
				"plot \"#{input}\" using 1:2 with linespoints title \"speedup\"\n")
	end 
end

def perf(nbProcess, file)
	time = {}
	speedup_name = "speedup#{nbProcess}.dat"
	for i in 1..nbProcess
		system("echo mpiexec -np #{i} ./lanceur #{file}")
		system("mpiexec -np #{i} ./lanceur #{file}")
		File.open("dyn-time.dat") do |timefile|
			timefile.each_line do |line|
				time[i] = line.split(" ")[1].to_f
			end
		end
	end

	gen_speedup_file("dyn-time.dat", speedup_name);
	gen_plot_script(speedup_name, "plot_"+speedup_name, speedup_name+".png")
end

perf(16, "jouet");