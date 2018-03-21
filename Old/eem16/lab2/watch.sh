script1=dassign2_1.tb
script2=dassign2_1.v

while true; do
    rm a.out
    iverilog -Wall $script1 $script2
    ./a.out
    #wait for changes to script
    inotifywait -qe modify $script1 $script2 >> /dev/null;
done