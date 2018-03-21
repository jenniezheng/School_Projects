javac *.java

declare -a States=('Null' 'Synchronized' 'Unsynchronized' 'GetNSet' 'BetterSafe');
declare -a Threads=(4 8 16 28 32);
declare -a Trials=(100 1000 10000 100000 1000000 10000000);
#default trials = 10000000
#default length = 5
#default maxval = 10
#default threads = 28

for trial in "${Trials[@]}"
do
    for state in "${States[@]}"
    do
        echo "testing $state with $trial trials on array of length 5, maxval 10"
        # arguments are STATE THREADS TRIALS MAXVAL ARR0 ARR1 ARR2


        java UnsafeMemory $state 28 $trial 20 1 2 15 10 10 1 2 15 10 10
    done
done


for thread in "${Threads[@]}"
do
    for state in "${States[@]}"
    do
        echo "testing $state with $thread threads on array of length 5, maxval 10"
        # arguments are STATE THREADS TRIALS MAXVAL ARR0 ARR1 ARR2
        java UnsafeMemory $state $thread 10000000 20 1 12 15 10 5
    done
done

for thread in "${Threads[@]}"
do
    for state in "${States[@]}"
    do
        echo "testing $state with $thread threads on array of length 100, maxval 10"
        # arguments are STATE THREADS TRIALS MAXVAL ARR0 ARR1 ARR2


        java UnsafeMemory $state $thread 10000000 20 1 2 3 4 5 6 7 8 9 10 1 2 3\
 4 5 6 7 8 9 0 1 2 3 4 \
5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 10 1 2 3 4 5 6 7 8 9 15 1 2 3 4 5 6 7 8 9 15 1 2 \
3 4 5 6 7 8 9 0 1 2 3 4\
 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 10 1 2 3 4 5 6 7 8 9 0
    done
done


declare -a Maxvals=(5 50 127);

for maxval in "${Maxvals[@]}"
do
    for state in "${States[@]}"
    do
        echo "testing $state with 28 threads and maxval $maxval, array length 10"
        # arguments are STATE THREADS TRIALS MAXVAL ARR0 ARR1 ARR2


        java UnsafeMemory $state 28 10000000 $maxval 1 2 3 4 5 1 2 3 4 5
    done
done
