rm *.txt
echo -n "IAMAT dd +34.068930-118.445127 123.392014450" | nc localhost 8012
echo -n "IAMAT aa +34.068930-118.445127 123.392014450" | nc localhost 8010
echo -n "IAMAT bb +34.068930-118.445127 123.392014450" | nc localhost 8010
sleep 5