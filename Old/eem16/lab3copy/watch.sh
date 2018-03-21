script1=dassign3_1.tb
script2=dassign3_1.v

while true; do
    rm a.out
    iverilog -Wall $script1 lab_modules.v building_blocks.v $script2
    ./a.out
    #wait for changes to script
    inotifywait -qe modify $script1 $script2 >> /dev/null;
done