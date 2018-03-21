
echo -n "IAMAT aa +34.068930-118.445127 123.392014400" | nc localhost 18485
echo -n "IAMAT aa +34.068930-118.445127 123.392014450" | nc localhost 18486
echo -n "IAMAT aa +34.068930-118.445127 123.392014400" | nc localhost 18487
echo -n "WHATSAT ee 10 4" | nc localhost 18485
sleep 5