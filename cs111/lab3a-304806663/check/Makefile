#NAME: Yunjing Zheng
#EMAIL: jenniezheng321@gmail.com
#ID: 304806663


ID=304806663
DIST_FILES=Makefile lab3a.c README ext2_fs.h
DIST=lab3a-304806663.tar.gz

default: build

build: lab3a.c 
	gcc -o lab3a lab3a.c -g -lm

clean:
	mkdir -p /tmp/tempBackup.$(ID)
	-mv $(DIST_FILES) /tmp/tempBackup.$(ID)/.
	-rm *
	mv /tmp/tempBackup.$(ID)/* . 
	rmdir /tmp/tempBackup.$(ID)

dist:
	tar -czf $(DIST) $(DIST_FILES)

mount:
	mkdir fs
	sudo mount -o ro,loop img/EXT2_test.img fs

unmount:
	sudo umount fs
	rmdir fs

debugfs:
	debugfs img/EXT2_test.img 
	#try stats, stat, bd, testi, and testb

view:
	xxd img/EXT2_test.img | head -20

check:
	gcc -o lab3a lab3a.c -g -lm
	./lab3a img/trivial.img 

diff:
	gcc -o lab3a lab3a.c -g -lm
	./lab3a img/trivial.img > csv/trivial_check.csv
	diff -u csv/trivial_check.csv csv/trivial.csv

check2:
	gcc -o lab3a lab3a.c -g -lm
	./lab3a img/otherimage.img > csv/otherimage_check.csv