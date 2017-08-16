#NAME: Yunjing Zheng
#EMAIL: jenniezheng321@gmail.com
#ID: 304806663


ID=304806663
DIST_FILES=Makefile lab3a.c README ext2_fs.h
DIST=lab3a-304806663.tar.gz

default: build

build: lab3a.c 
	gcc -o lab3a lab3a.c -g -lm -Wall

clean:
	mkdir -p /tmp/tempBackup.$(ID)
	-mv $(DIST_FILES) /tmp/tempBackup.$(ID)/.
	-rm *
	mv /tmp/tempBackup.$(ID)/* . 
	rmdir /tmp/tempBackup.$(ID)

dist:
	tar -czf $(DIST) $(DIST_FILES)

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

check1: 
	gcc -o lab3a lab3a.c -g -lm
	./lab3a img/trivial.img 

	
check2:
	gcc -o lab3a lab3a.c -g -lm
	./lab3a img/otherimage.img > csv/otherimage_check.csv


diff:
	gcc -o lab3a lab3a.c -g -lm
	./lab3a img/trivial.img > csv/trivial_check.csv
	diff csv/trivial_check.csv csv/trivial.csv
diff2:
	gcc -o lab3a lab3a.c -g -lm
	./lab3a img/trivialV2.img > csv/trivial_check.csv
	diff csv/trivial_check.csv csv/trivial.csv



push:
	git add *
	git commit -am "update"
	git push origin master

tt: 
	make mount2
	sudo touch fs/apple.txt
	make unmount
	./lab3a img/EXTEdited.img >view
	subl view