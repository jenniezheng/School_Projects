//NAME: Yunjing Zheng,Kunjan Patel
//EMAIL: jenniezheng321@gmail.com,kunjan@ucla.edu
//ID: 304806663,904563044


#include <fcntl.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "ext2_fs.h"



int file_system_image_fd;
long unsigned num_groups;
long long unsigned block_size;
long long unsigned byte_offset;
struct ext2_super_block super_block;
struct ext2_group_desc *group_descs;

int process_arg(int argc, char** argv);
char* time_to_string(unsigned raw);
void super_block_read();
void group_read();
void super_block_print();
void single_group_print(int group_num);
void block_directory_entry_print(int block_number, int inode_number);
void single_inode_print(struct ext2_inode inode, unsigned inode_number);
void group_inode_bitmap_print(int group_num);
void group_block_bitmap_print(int group_num);
char file_type_to_char(uint16_t file_type);
void inode_info_print(int group_num);
void recurse_through_indirects(int block_number, int indirection_level, int parent_inode_number, int directory, unsigned long long logical_block_general_offset);

int process_arg(int argc, char** argv){
	//accepts exactly one argument: the file_system_image_name
	if (argc != 2) {
		fprintf(stderr,"Wrong number of arguments.\nUsage: lab3a <file_system_image_name>.\n"); exit(1);
	}
	char* file_system_image_name = argv[1];
	file_system_image_fd = open(file_system_image_name, O_RDONLY);

	if(file_system_image_fd<0){
		fprintf(stderr,"Failed to open file %s.\n", file_system_image_name); exit(1);
	}
	return file_system_image_fd;
}

char* time_to_string(unsigned raw){
    struct tm *gtime;
    time_t raw_time=(time_t)raw;
    gtime = gmtime(&raw_time);
    char* buffer=malloc(100);
    /* Display the time in GMT */
    strftime(buffer,100,"%D %X", gtime);
    return buffer; 
}

char file_type_to_char(uint16_t raw_type){
	char file_type;

	if ((raw_type & 0xA000) == 0xA000 ) file_type='s';
	else if(raw_type& 0x8000) file_type='f';
	else if(raw_type & 0x4000) file_type='d';
	else file_type='?';
	return file_type;
}

void super_block_read(){
	// superblock always starts at offset 2^10 
	const int super_block_offset = 1024;
	if (pread(file_system_image_fd, &super_block, sizeof(struct ext2_super_block), super_block_offset) == -1) {
		fprintf(stderr,"Failed read the superblock.\n"); exit(1);
	}
	num_groups = ceil( (double)super_block.s_blocks_count / super_block.s_blocks_per_group ); // Find number of full groups
	block_size = EXT2_MIN_BLOCK_SIZE << super_block.s_log_block_size;
}

void group_read(){
	const int block_group_desc_table_offset=2048; 
	group_descs=malloc(sizeof(struct ext2_group_desc)*num_groups);
	int i;
	for(i = 0; i < num_groups; i++){
		if (pread(file_system_image_fd, group_descs+i, sizeof(struct ext2_group_desc), 
			block_group_desc_table_offset + i * sizeof(struct ext2_group_desc)) == -1) //offset
		{
			fprintf(stderr,"Failed read block group %d.\n",i); exit(1);
		}
	}
}

void super_block_print(){
	//print in order, separated by commas, no space
	printf("SUPERBLOCK,%u,%u,%llu,%d,%d,%d,%d",
		super_block.s_blocks_count, //total number of blocks (decimal)
		super_block.s_inodes_count, //total number of i-nodes (decimal)
		block_size, //block size (in bytes, decimal)
		super_block.s_inode_size, //i-node size (in bytes, decimal)
		super_block.s_blocks_per_group, //blocks per group (decimal)
		super_block.s_inodes_per_group, //i-nodes per group (decimal)
		super_block.s_first_ino //first non-reserved i-node (decimal)
	);
}

void block_directory_entry_print(int block_number, int inode_number){
	unsigned local_byte_offset=0;
	int count=0;
	while (local_byte_offset < block_size){
		struct ext2_dir_entry dir_entry;
		if (pread(file_system_image_fd, &dir_entry, sizeof(dir_entry), 
			block_number*block_size+local_byte_offset) == -1){
			fprintf(stderr,"Failed to read directory block for inode %d.\n",inode_number); exit(1);
		}
		
		if(dir_entry.inode!=0){
			dir_entry.name[dir_entry.name_len]=0; // end it
			printf("\nDIRENT,%u,%llu,%u,%u,%u,'%s'",
			inode_number, //parent inode number (decimal) ... 
			byte_offset, //logical byte offset (decimal) of this entry within the directory
			dir_entry.inode,//inode number of the referenced file (decimal)
			dir_entry.rec_len,//entry length (decimal)
			dir_entry.name_len,//name length (decimal)
			dir_entry.name//name (string, surrounded by single-quotes). Don't worry about escaping, we promise there will be no single-quotes or commas in any of the file names.
			);
		}
		count++;
		byte_offset+=dir_entry.rec_len;
		local_byte_offset+=dir_entry.rec_len;
	}
}

void single_inode_print(struct ext2_inode inode, unsigned inode_number){
	char inode_file_type=file_type_to_char(inode.i_mode);
	char* mtime=time_to_string(inode.i_mtime);
	char* atime=time_to_string(inode.i_atime);
	/*
		not sure how to get time of last I-node change 
	*/
	printf("\nINODE,%u,%c,%o,%u,%u,%u,%s,%s,%s,%u,%u",
		inode_number, //inode number (decimal)
		inode_file_type, //file type ('f' for file, 'd' for directory, 's' for symbolic link, '?" for anything else)
		inode.i_mode & ((1<<12)-1), //mode (low order 12-bits, octal ... suggested format "0%o")
		inode.i_uid,//owner (decimal)
		inode.i_gid,//group (decimal)
		inode.i_links_count,//link count (decimal)
		mtime,//time of last I-node change (ta said this was ok...)
		mtime,//modification time (mm/dd/yy hh:mm:ss, GMT)
		atime,//time of last access (mm/dd/yy hh:mm:ss, GMT)
		inode.i_size,//file size (decimal)
		inode.i_blocks //number of blocks (decimal)
	);
	free(mtime);
	free(atime);

	int address_index;
	int num_blocks_to_print= (inode_file_type=='s') ? ceil( (double)inode.i_size/ block_size) : 15;
	 
	for(address_index=0; address_index<num_blocks_to_print; address_index++){
		printf(",%u",inode.i_block[address_index]);
	}
}

void single_group_print(int group_num){
	struct ext2_group_desc group_desc=group_descs[group_num];
	unsigned int total_blocks_in_group;
	unsigned int total_inodes_in_group;

	if(group_num==num_groups-1){ // if last group
		total_blocks_in_group=super_block.s_blocks_count - super_block.s_blocks_per_group * (num_groups-1);
		total_inodes_in_group=super_block.s_inodes_count - super_block.s_inodes_per_group * (num_groups-1);
	}
	else {
		total_blocks_in_group=super_block.s_blocks_per_group; //number of blocks (decimal)
		total_inodes_in_group=super_block.s_inodes_per_group; //number of free i-nodes (decimal)
	}
	//PRINTING GROUP
	printf("\nGROUP,%u,%u,%u,%u,%u,%u,%u,%u",
	group_num, //group number (decimal, starting from zero)
	total_blocks_in_group, //total number of blocks in this group (decimal)
	total_inodes_in_group,	//total number of i-nodes in this group (decimal)
	group_desc.bg_free_blocks_count,//number of free blocks (decimal)
	group_desc.bg_free_inodes_count, //number of free i-nodes (decimal)
	group_desc.bg_block_bitmap, //block number of free block bitmap for this group (decimal)
	group_desc.bg_inode_bitmap, //block number of free i-node bitmap for this group (decimal)
	group_desc.bg_inode_table //block number of first block of i-nodes in this group (decimal)
	); 
}

void group_block_bitmap_print(int group_num){
	struct ext2_group_desc group_desc=group_descs[group_num];
	unsigned int total_blocks_in_group;

	if(group_num==num_groups-1) // if last group
		total_blocks_in_group=super_block.s_blocks_count - super_block.s_blocks_per_group * (num_groups-1);
	else 
		total_blocks_in_group=super_block.s_blocks_per_group; //number of blocks (decimal)
	//PRINTING BFREE
	char* block_bitmap=malloc(block_size);
	if (pread(file_system_image_fd, block_bitmap, block_size, group_desc.bg_block_bitmap * block_size) == -1) {
		fprintf(stderr,"Failed to read the block bitmap of group %d.\n",group_num); exit(1);
	}
	
	int block_offset=group_num*super_block.s_blocks_per_group;
	int i;
	for (i=0; i<total_blocks_in_group; i++){
		unsigned int index=i/8;
		unsigned int offset=i%8;
		if(!(block_bitmap[index] & (1 << offset))) printf("\nBFREE,%d",1+i+block_offset);
	}
	free(block_bitmap);
}

void group_inode_bitmap_print(int group_num){
	struct ext2_group_desc group_desc=group_descs[group_num];
	unsigned int total_inodes_in_group;

	if(group_num==num_groups-1) // if last group
		total_inodes_in_group=super_block.s_inodes_count - super_block.s_inodes_per_group * (num_groups-1);
	
	else total_inodes_in_group=super_block.s_inodes_per_group; //number of i-nodes (decimal)
	
	//PRINTING IFREE
	unsigned char* inode_bitmap=malloc(block_size);
	if (pread(file_system_image_fd, inode_bitmap, block_size, group_desc.bg_inode_bitmap  * block_size) == -1) {
		fprintf(stderr,"Failed to read the inode bitmap of group %d.\n",group_num); exit(1);
	}
	
	int inode_offset=group_num*super_block.s_inodes_per_group;
	int i;
	for (i=0; i<total_inodes_in_group; i++){
		unsigned int index=i/8;
		unsigned int offset=i%8;
		unsigned int inode_number = 1+i+inode_offset;
		if(!(inode_bitmap[index] & (1 << offset))) printf("\nIFREE,%d",inode_number);
	}
	free(inode_bitmap);
}


void group_inode_info_print(int group_num){
	struct ext2_group_desc group_desc=group_descs[group_num];
	unsigned int total_inodes_in_group;
	if(group_num==num_groups-1) total_inodes_in_group=super_block.s_inodes_count - super_block.s_inodes_per_group * (num_groups-1);
	else total_inodes_in_group=super_block.s_inodes_per_group; //number of i-nodes (decimal)
	int inode_offset=group_num*super_block.s_inodes_per_group;
	//print info about inodes
	int i;
	for (i=0; i<total_inodes_in_group; i++){
		unsigned int inode_number = 1+i+inode_offset;
		// read in inode
		struct ext2_inode inode;
		if (pread(file_system_image_fd, &inode, super_block.s_inode_size, 
			group_desc.bg_inode_table * block_size + i * super_block.s_inode_size ) == -1){
			fprintf(stderr,"Failed to read inode %d.\n",inode_number); exit(1);
		}
		
		if(inode.i_mode && inode.i_links_count){
			single_inode_print(inode,inode_number);
			char inode_file_type=file_type_to_char(inode.i_mode);

			//indirect pointer active
			int directory= (inode_file_type=='d') ? 1 : 0;
			int block_index;
			for (block_index=0; block_index<12; block_index++){
				recurse_through_indirects(inode.i_block[block_index],0,inode_number,directory,block_index); 
			}
			unsigned long num_indirect_blocks = block_size / sizeof(unsigned int);  
			recurse_through_indirects(inode.i_block[12],1,inode_number,directory, 12);
			recurse_through_indirects(inode.i_block[13],2,inode_number,directory, 12+(unsigned long long)num_indirect_blocks);
			recurse_through_indirects(inode.i_block[14],3,inode_number,directory, 12+num_indirect_blocks+(unsigned long long)num_indirect_blocks*num_indirect_blocks);
			byte_offset = 0; //reset byte offset
		}
	}
}

void recurse_through_indirects(int block_number, int indirection_level, int parent_inode_number, int directory,  unsigned long long logical_block_general_offset){
	if(block_number==0)
		return;
	else if(indirection_level==0) {
		if(!directory) return;
		block_directory_entry_print(block_number, parent_inode_number);
	}
	else{
		unsigned long num_blocks = block_size / 4; 
		unsigned int* block_pointers = malloc(block_size);
		
		if (pread(file_system_image_fd, block_pointers, block_size, block_number * block_size) == -1) {
			fprintf(stderr,"Failed to read the allocated indirect block.\n"); exit(1);
		}

		int logical_block_offset_multiplier; //(might want to check this)
		if(indirection_level==1)logical_block_offset_multiplier=1;
		else if (indirection_level==2)logical_block_offset_multiplier=block_size;
		else if (indirection_level==3)logical_block_offset_multiplier=block_size*block_size;
		int block_index;
		for (block_index = 0; block_index < num_blocks; block_index++) {

			if (block_pointers[block_index] == 0) 
				continue;
			// check that the indirect block pointer is valid
			if (block_pointers[block_index] > super_block.s_blocks_count) {
				fprintf(stderr,"Indirect block number is invalid"); exit(2);
			}
			unsigned long long total_offset=logical_block_general_offset+block_index*logical_block_offset_multiplier;
			printf("\nINDIRECT,%u,%u,%llu,%u,%u", 
			parent_inode_number, //I-node number of the owning file (decimal)
			indirection_level, //(decimal) level of indirection for the block being scanned ... 1 single indirect, 2 double indirect, 3 tripple
			total_offset, //logical block offset (decimal) represented by the referenced block
			block_number,//block number of the (1,2,3) indirect block being scanned (decimal) ... not the highest level block (in the recursive scan), but the lower level block that contains the block reference reported by this entry.
			block_pointers[block_index]//block number of the referenced block (decimal)
			);	
			//continue to lower indirection level
			recurse_through_indirects(block_pointers[block_index],indirection_level-1,parent_inode_number,directory,total_offset);
		}
		free(block_pointers);
	}
}




int main(int argc, char** argv){
	process_arg(argc,argv);
	super_block_read();
	group_read();
	super_block_print();
	int i;
	for(i = 0; i < num_groups; i++){	
		single_group_print(i);
		group_block_bitmap_print(i);
		group_inode_bitmap_print(i);
		group_inode_info_print(i);
	}
	free(group_descs);
	return 0;
}
