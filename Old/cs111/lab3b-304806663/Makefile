#NAME: Yunjing Zheng,Kunjan Patel
#EMAIL: jenniezheng321@gmail.com,kunjan@ucla.edu
#ID: 304806663,904563044


ID=304806663
DIST_FILES=Makefile lab3b README 
DIST=lab3b-304806663.tar.gz

default:
	echo "No need to compile"

clean:
	mkdir -p /tmp/tempBackup.$(ID)
	-mv $(DIST_FILES) /tmp/tempBackup.$(ID)/.
	-rm *
	mv /tmp/tempBackup.$(ID)/* . 
	rmdir /tmp/tempBackup.$(ID)

dist:
	tar -czf $(DIST) $(DIST_FILES)


#########################################################


#other stuff
mount:
	-mkdir fs
	sudo mount -o ro,loop img/EXT2_test.img fs


mount2:
	-mkdir fs
	sudo mount -o rw,loop img/lite.img fs

unmount:
	sudo umount fs
	rmdir fs

debugfs:
	debugfs img/EXT2_test.img 
	#try stats, stat, bd, testi, and testb

view:
	xxd img/EXT2_test.img | head -20


tt: 
	make mount2
	sudo touch fs/apple.txt
	make unmount
	./lab3a img/EXTEdited.img >view
	subl view
