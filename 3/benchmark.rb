

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


gen_matrix_file(8, "mat.dat")