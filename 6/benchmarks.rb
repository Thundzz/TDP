#!/usr/bin/env ruby

def bench_shared_mem_gnuplot(bs_max, nb_threads)
	input = "bench_shared_mem_omp_#{nb_threads}.dat"
	input2 = "bench_shared_mem_pthread_#{nb_threads}.dat"

	File.open("bench_shared_mem.plt", 'w+') do |f|  
		f.puts("set xlabel \"Dimension du tableau\"\n"+
				"set ylabel \"Speedup\"\n"+
				"set term png\n"+
				"set output \"bench_shared_mem.png\"\n"+
				"set logscale x 2\n"+
				"set key on inside bottom right\n"+
				"plot \""+input+"\" using 1:2 with linespoints title \"OpenMP - #{nb_threads} threads\"," +
				"\""+input2+"\" using 1:2 with linespoints title \"pthreads - #{nb_threads} threads\"")
	end

	system("gnuplot bench_shared_mem.plt")
end


def bench_shared_mem(bs_max, nb_threads, nb_iter)
	nb_loops = 10
	bs = 64

	system("rm -f bench_shared_mem_*.dat*")

	while bs <= bs_max
		cmd = "./life_seq.out #{nb_loops} #{bs}"
		cmd_omp = "./life_omp.out #{nb_loops} #{bs} #{nb_threads}"
		cmd_pthread = "./life_pthread.out #{nb_loops} #{bs} #{nb_threads}"
		
		p cmd
		p cmd_omp
		p cmd_pthread

		time_seq = 0.0
		time_omp = 0.0
		time_pthread = 0.0

		for i in 1..nb_iter do
			system(cmd)
			system(cmd_omp)
			system(cmd_pthread)

			File.open("time_seq.dat", "r") do |file|
				file.each_line do |line|
					time_seq += line.to_s.split(" ")[0].to_f
				end
			end
			File.open("time_omp_#{nb_threads}.dat", "r") do |file|
				file.each_line do |line|
					time_omp += line.to_s.split(" ")[0].to_f
				end
			end
			File.open("time_pthread_#{nb_threads}.dat", "r") do |file|
				file.each_line do |line|
					time_pthread += line.to_s.split(" ")[0].to_f
				end
			end
		end

		time_omp = time_seq/time_omp
		time_pthread = time_seq/time_pthread

		File.open("bench_shared_mem_omp_#{nb_threads}.dat", "a") do |file|
			file.puts("#{bs} #{time_omp}")
		end
		File.open("bench_shared_mem_pthread_#{nb_threads}.dat", "a") do |file|
			file.puts("#{bs} #{time_pthread}")
		end
		bs *= 2
	end
end

def bench_shared_mem_threads_gnuplot()
	system("rm -f bench_shared_mem_threads*.plt*")
	system("rm -f bench_shared_mem_threads*.png*")

	input = "bench_shared_mem_omp.dat"
	input2 = "bench_shared_mem_pthread.dat"

	File.open("bench_shared_mem_threads.plt", 'w+') do |f|  
		f.puts("set xlabel \"Nombre de threads\"\n"+
				"set ylabel \"Speedup\"\n"+
				"set term png\n"+
				"set output \"bench_shared_mem_threads.png\"\n"+
				"set key on inside bottom right\n"+
				"plot \""+input+"\" using 1:2 with linespoints title \"OpenMP\"," +
				"\""+input2+"\" using 1:2 with linespoints title \"pthreads\"")	
	end
	system("gnuplot bench_shared_mem_threads.plt")
end

def bench_shared_mem_threads(bs, max_threads, nb_iter)
	nb_loops = 50
	nb_threads = 1

	system("rm -f bench_shared_mem*.dat*")

	time_seq = 0.0	
	cmd = "./life_seq.out #{nb_loops} #{bs}"
	p cmd
	for i in 1..nb_iter do
		system(cmd)
		File.open("time_seq.dat", "r") do |file|
			file.each_line do |line|
				time_seq += line.to_s.split(" ")[0].to_f
			end
		end
	end

	while nb_threads <= max_threads
		cmd_omp = "./life_omp.out #{nb_loops} #{bs} #{nb_threads}"
		cmd_pthread = "./life_pthread.out #{nb_loops} #{bs} #{nb_threads}"
		
		p cmd_omp
		p cmd_pthread

		time_omp = 0.0
		time_pthread = 0.0

		for i in 1..nb_iter do
			system(cmd_omp)
			system(cmd_pthread)

			File.open("time_omp_#{nb_threads}.dat", "r") do |file|
				file.each_line do |line|
					time_omp += line.to_s.split(" ")[0].to_f
				end
			end
			File.open("time_pthread_#{nb_threads}.dat", "r") do |file|
				file.each_line do |line|
					time_pthread += line.to_s.split(" ")[0].to_f
				end
			end
		end

		time_omp = time_seq/time_omp
		time_pthread = time_seq/time_pthread

		File.open("bench_shared_mem_omp.dat", "a") do |file|
			file.puts("#{nb_threads} #{time_omp}")
		end
		File.open("bench_shared_mem_pthread.dat", "a") do |file|
			file.puts("#{nb_threads} #{time_pthread}")
		end
		nb_threads += 1 
	end
end




def bench_mpi_gnuplot(bs_max, nb_procs)
	system("rm -f bench_mpi*.plt*")
	system("rm -f bench_mpi*.png*")

	input = "bench_mpi_sync_#{nb_procs}.dat"
	input2 = "bench_mpi_async_#{nb_procs}.dat"
	input3 = "bench_mpi_persist_#{nb_procs}.dat"

	File.open("bench_mpi.plt", 'w+') do |f|  
		f.puts("set xlabel \"Dimension du tableau\"\n"+
				"set ylabel \"Speedup\"\n"+
				"set term png\n"+
				"set output \"bench_mpi.png\"\n"+
				"set logscale x 2\n"+
				"set key on inside bottom right\n"+
				"plot \""+input+"\" using 1:2 with linespoints title \"Synchrone - #{nb_procs} processus\"," +
				"\""+input2+"\" using 1:2 with linespoints title \"Asynchrone - #{nb_procs} processus\", "+
				"\""+input3+"\" using 1:2 with linespoints title \"Persistant - #{nb_procs} processus\"")
	
	end

	system("gnuplot bench_mpi.plt")
end


def bench_mpi(bs_max, nb_procs, nb_iter)
	nb_loops = 10
	bs = 64

	system("rm -f bench_mpi*.dat*")

	while bs <= bs_max
		cmd = "./life_seq.out #{nb_loops} #{bs}"
		cmd_sync = "mpiexec -np #{nb_procs} ./life_mpi_sync.out #{nb_loops} #{bs}"
		cmd_async = "mpiexec -np #{nb_procs} ./life_mpi_async.out #{nb_loops} #{bs}"
		cmd_persist = "mpiexec -np #{nb_procs} ./life_mpi_persist.out #{nb_loops} #{bs}"

		p cmd
		p cmd_sync
		p cmd_async
		p cmd_persist

		time_seq = 0.0
		time_sync = 0.0
		time_async = 0.0
		time_persist = 0.0
		
		for i in 1..nb_iter do
			system(cmd)
			system(cmd_sync)
			system(cmd_async)
			system(cmd_persist)

			File.open("time_seq.dat", "r") do |file|
				file.each_line do |line|
					time_seq += line.to_s.split(" ")[0].to_f
				end
			end
			File.open("time_mpi_sync_#{nb_procs}.dat", "r") do |file|
				file.each_line do |line|
					time_sync += line.to_s.split(" ")[0].to_f
				end
			end
			File.open("time_mpi_async_#{nb_procs}.dat", "r") do |file|
				file.each_line do |line|
					time_async += line.to_s.split(" ")[0].to_f
				end
			end
			File.open("time_mpi_persist_#{nb_procs}.dat", "r") do |file|
				file.each_line do |line|
					time_persist += line.to_s.split(" ")[0].to_f
				end
			end
		end

		time_sync = time_seq/time_sync
		time_async = time_seq/time_async
		time_persist = time_seq/time_persist

		File.open("bench_mpi_sync_#{nb_procs}.dat", "a") do |file|
			file.puts("#{bs} #{time_sync}")
		end
		File.open("bench_mpi_async_#{nb_procs}.dat", "a") do |file|
			file.puts("#{bs} #{time_async}")
		end
		File.open("bench_mpi_persist_#{nb_procs}.dat", "a") do |file|
			file.puts("#{bs} #{time_persist}")
		end
		bs *= 2
	end
end



def bench_mpi_procs_gnuplot()
	system("rm -f bench_mpi_procs*.plt*")
	system("rm -f bench_mpi_procs*.png*")

	input = "bench_mpi_procs_sync.dat"
	input2 = "bench_mpi_procs_async.dat"
	input3 = "bench_mpi_procs_persist.dat"

	File.open("bench_mpi_procs.plt", 'w+') do |f|  
		f.puts("set xlabel \"Nombre de processus\"\n"+
				"set ylabel \"Speedup\"\n"+
				"set term png\n"+
				"set output \"bench_mpi_procs.png\"\n"+
				"set key on inside bottom right\n"+
				"plot \""+input+"\" using 1:2 with linespoints title \"Synchrone\"," +
				"\""+input2+"\" using 1:2 with linespoints title \"Asynchrone\", "+
				"\""+input3+"\" using 1:2 with linespoints title \"Persistant\"")
	
	end
	system("gnuplot bench_mpi_procs.plt")
end

def bench_mpi_procs(bs, max_procs, nb_iter)
	nb_loops = 50
	k = 1

	system("rm -f bench_mpi_procs*.dat*")

	time_seq = 0.0	
	cmd = "./life_seq.out #{nb_loops} #{bs}"
	p cmd
	for i in 1..nb_iter do
		system(cmd)
		File.open("time_seq.dat", "r") do |file|
			file.each_line do |line|
				time_seq += line.to_s.split(" ")[0].to_f
			end
		end
	end

	while k*k <= max_procs
		nb_procs = k*k

		cmd_sync = "mpiexec -np #{nb_procs} ./life_mpi_sync.out #{nb_loops} #{bs}"
		cmd_async = "mpiexec -np #{nb_procs} ./life_mpi_async.out #{nb_loops} #{bs}"
		cmd_persist = "mpiexec -np #{nb_procs} ./life_mpi_persist.out #{nb_loops} #{bs}"

		p cmd_sync
		p cmd_async
		p cmd_persist

		time_sync = 0.0
		time_async = 0.0
		time_persist = 0.0
		
		for i in 1..nb_iter do
			system(cmd)
			system(cmd_sync)
			system(cmd_async)
			system(cmd_persist)

			File.open("time_mpi_sync_#{nb_procs}.dat", "r") do |file|
				file.each_line do |line|
					time_sync += line.to_s.split(" ")[0].to_f
				end
			end
			File.open("time_mpi_async_#{nb_procs}.dat", "r") do |file|
				file.each_line do |line|
					time_async += line.to_s.split(" ")[0].to_f
				end
			end
			File.open("time_mpi_persist_#{nb_procs}.dat", "r") do |file|
				file.each_line do |line|
					time_persist += line.to_s.split(" ")[0].to_f
				end
			end
		end

		time_sync = time_seq/time_sync
		time_async = time_seq/time_async
		time_persist = time_seq/time_persist

		File.open("bench_mpi_procs_sync.dat", "a") do |file|
			file.puts("#{nb_procs} #{time_sync}")
		end
		File.open("bench_mpi_procs_async.dat", "a") do |file|
			file.puts("#{nb_procs} #{time_async}")
		end
		File.open("bench_mpi_procs_persist.dat", "a") do |file|
			file.puts("#{nb_procs} #{time_persist}")
		end
		k += 1 
	end
end

# bench_shared_mem(40000, 16, 3)
# bench_shared_mem_gnuplot(40000, 16)

# bench_mpi(40000, 16, 3)
# bench_mpi_gnuplot(40000, 16)

# bench_mpi_procs(8400, 36, 1)
# bench_mpi_procs_gnuplot()

bench_shared_mem_threads(4000, 10, 1)
bench_shared_mem_threads_gnuplot()
