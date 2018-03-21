rm *.txt
echo -n "IAMAT b +34.068930-118.445127 123.435" | nc localhost 8010
sleep 3
echo -n "IAMAT a +34.068930-118.445127 123123.123" | nc localhost 8012
#echo -n "IAMAT c +34.068930-118.445127 324.213" | nc localhost 8010
#echo -n "IAMAT b +34.068930-118.445127 2332.123" | nc localhost 8010
#echo -n "IAMAT d +34.068930-118.445127 300.32" | nc localhost 8010
#echo -n "IAMAT b +34.068930-118.445127 0.123" | nc localhost 8010
#echo -n "IAMAT c +34.068930-118.445127 0.123" | nc localhost 8010