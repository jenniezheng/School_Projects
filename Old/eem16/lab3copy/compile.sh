script1=dassign2_1.tb
script2=dassign2_1.v


iverilog -Wall $script1 $script2
echo $?