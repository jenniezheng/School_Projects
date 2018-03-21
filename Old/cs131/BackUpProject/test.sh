rm *.txt
echo -n "IAMAT kiwi.cs.ucla.edu +34.068930-118.445127 1479413884.392014450" | nc localhost 8010
sleep 1s
echo -n "WHATSAT kiwi.cs.ucla.edu 10 1" | nc localhost 8010