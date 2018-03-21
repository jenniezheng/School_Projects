#! /usr/bin/gnuplot
#
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab2_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#
# output:

#lab2b_1.png ... throughput vs number of threads for mutex and spin-lock synchronized list operations.
#lab2b_2.png ... mean time per mutex wait and mean time per operation for mutex-synchronized list perations.
#lab2b_3.png ... successful iterations vs threads for each synchronization method.
#lab2b_4.png ... throughput vs number of threads for mutex synchronized partitioned lists.
#lab2b_4.png ... throughput vs number of threads for spin-lock synchronized partitioned lists.
#
#columns: list-id-m,10,1000,5,30000,118393703,3946,25313
#1: name 
#2: num threads
#3: num iterations
#4: num lists
#5: total operations 
#6: elapsed time
#7: time per operation 
#8: wait-time

# general plot parameters
set terminal png
set datafile separator ","

set title "List-1: Throughput vs Number of Threads"
set xlabel "Number of Threads"
set xrange [0.75:]
set logscale x 10
set ylabel "Throughput (operations/sec)"
set logscale y 10
set output 'lab2b_1.png'

#long long throughput=operations*1E9/elapsed_time;
plot \
     "< grep 'list-none-s,' lab2b_1.csv" using 2:($5*1000000000/$6) \
	 with linespoints lc rgb 'blue'  title "sync=spin-lock", \
	 "< grep 'list-none-m,' lab2b_1.csv" using 2:($5*1000000000/$6) \
	 with linespoints lc rgb 'green'  title "sync=mutex" 

set title "List-2: Mean Wait Time and Mean Operation time for Mutex-Sync"
set ylabel "Time (ns)"
set output 'lab2b_2.png'

plot \
    "lab2b_2.csv" using 2:7 \
	 with linespoints lc rgb 'blue'  title "Operation Time", \
	 "lab2b_2.csv" using 2:8 \
	 with linespoints lc rgb 'red'  title "Wait-For-Lock Time"

#lab2b_3.png ... successful iterations vs threads for each synchronization method.

set title "List-3: Successful Iterations vs Threads for Each Sync Type"
set ylabel "Number of Iterations"
set xlabel "Number of Threads"
set output 'lab2b_3.png'

plot \
    "< grep 'id-m,' lab2b_3.csv" using 2:3 \
	 with points lc rgb 'red'  title "Mutex", \
	"< grep 'id-s,' lab2b_3.csv" using 2:3 \
	 with points lc rgb 'blue'  title "Spin-lock", \
	"< grep 'id-none,' lab2b_3.csv" using 2:3 \
	 with points lc rgb 'green'  title "None"

set title "List-4: Throughput vs Number of Threads for Mutex Sync"
set ylabel "Throughput (operations/sec)"
set xlabel "Number of Threads"
set output 'lab2b_4.png'

plot \
    "< grep 'none-m,[0-9]*,[0-9]*,1,' lab2b_4.csv" using 2:($5*1000000000/$6) \
	 with linespoints lc rgb 'red'  title "1 List", \
	"< grep 'none-m,[0-9]*,[0-9]*,4,' lab2b_4.csv" using 2:($5*1000000000/$6) \
	 with linespoints lc rgb 'blue'  title "4 Lists", \
	"< grep 'none-m,[0-9]*,[0-9]*,8,' lab2b_4.csv" using 2:($5*1000000000/$6) \
	 with linespoints lc rgb 'green'  title "8 Lists", \
	 "< grep 'none-m,[0-9]*,[0-9]*,16,' lab2b_4.csv" using 2:($5*1000000000/$6) \
	 with linespoints lc rgb 'violet'  title "16 Lists"

set title "List-5: Throughput vs Number of Threads for Spin-Lock Sync"
set ylabel "Throughput (operations/sec)"
set xlabel "Number of Threads"
set output 'lab2b_5.png'

plot \
    "< grep 'none-s,[0-9]*,[0-9]*,1,' lab2b_5.csv" using 2:($5*1000000000/$6) \
	 with linespoints lc rgb 'red'  title "1 List", \
	"< grep 'none-s,[0-9]*,[0-9]*,4,' lab2b_5.csv" using 2:($5*1000000000/$6) \
	 with linespoints lc rgb 'blue'  title "4 Lists", \
	"< grep 'none-s,[0-9]*,[0-9]*,8,' lab2b_5.csv" using 2:($5*1000000000/$6) \
	 with linespoints lc rgb 'green'  title "8 Lists", \
	 "< grep 'none-s,[0-9]*,[0-9]*,16,' lab2b_5.csv" using 2:($5*1000000000/$6) \
	 with linespoints lc rgb 'violet'  title "16 Lists"
