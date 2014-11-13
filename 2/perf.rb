#!/usr/bin/env ruby

def gen_plot_cmd(nbProc, nbParticle)
	color = ["red", "green", "blue", "orange", "purple"]
	cmd = "plot "
	for i in 1..nbProc
		s= 2
		e = 3
		for j in 1..nbParticle
			cmd += "\'datafile#{i-1}\' index (i-1) using #{s}:#{s+1} with circle lc rgb \'#{color[i-1% color.size]}\'"
			s+=2
			unless (i== nbProc && j==nbParticle)
				cmd+= ', '
			end
		end
	end
	cmd
end

def print_on_file(filename, s)
	system("echo \"#{s}\" " + ">> #{filename}")	
end

def gen_plot_script(filename, nbProc, nbParticle)
	system("cp  header.plt #{filename}.plt")
	print_on_file filename+".plt", gen_plot_cmd(nbProc, nbParticle)
	system("cat footer.plt >> #{filename}.plt")
end


def simulate(nbProcess, nbParticle)
	filename = "particles#{nbProcess}#{nbParticle}"
	plotfile = filename+".plt"
	giffile = filename+".gif"
	system("mpiexec -np #{nbProcess} ./simulation.out")
	gen_plot_script(filename, nbProcess, nbParticle)
	system("gnuplot #{plotfile}")
	system("eog #{giffile}")
end

simulate(4, 2)

#gen_plot_script("plot42", 4,1)