script1=dassign3_3.tb
script2=dassign3_3.v

while true; do
    rm a.out
    iverilog -Wall test.v
    ./a.out
    #wait for changes to script
    inotifywait -qe modify $script1 $script2 >> /dev/null;
done