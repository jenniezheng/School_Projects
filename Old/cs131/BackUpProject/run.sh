for i in Alford Hamilton Welsh Ball Holiday
    do
        python3.6 server.py $i &
    done

function ctrl_c() {
     ./done.sh
     exit 0
}

trap ctrl_c INT

sleep 1000000000