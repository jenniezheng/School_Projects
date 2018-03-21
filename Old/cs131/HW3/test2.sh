javac *.java

declare -a States=('Null' 'Synchronized' 'Unsynchronized' 'GetNSet' 'BetterSafe' 'BetterSafeTwo');
declare -a Threads=(4 8 16 28 32);
declare -a Trials=(1000000 10000000);
#default trials = 10000000
#default length = 5
#default maxval = 10
#default threads = 28


for state in "${States[@]}"
do
    echo "testing $state with $trial trials "
    # arguments are STATE THREADS TRIALS MAXVAL ARR0 ARR1 ARR2
    java UnsafeMemory $state 28 10000000 20 1 2 3 4 5 6 7 8 9 10 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 10 1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 10 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 10 1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 10 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 10 1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 10 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 10 1 2 3 4 5 6 7 8 9
done
