#!/usr/bin/python
# I am not sure about the offsets I am guessing this is what the spec
#wants from the test cases and block no 6 and 7 are not referenced
#but are supposed to be counted as referenced or i am doing it wrong
import sys, string, locale, getopt,csv
import os.path
#print 'ARGV :', sys.argv[1]

def main():
    block_cap = 0;
    reserved_blocks = list()
    blocks_referenced = list()
    free_blocks = list()
    allocated_inodes = list()
    free_inodes = list()
    first_inode = 2
    last_inode = 0

    #reserved_blocks.insert(0,"0")
    reserved_blocks.insert(1,"1")
    blocks_referenced.insert(0,0)

    #print"Hello"
    if len(sys.argv) == 2:
        filename = sys.argv[1]
        if not os.path.isfile(filename):
            exit_str= "file " + filename + " does not exist"
            sys.exit(exit_str)
    else:
        sys.exit("no csv argument exiting")
            #csv_file = open(filename,"r")

   
    with open(filename,"r") as csv_file:
        entries = csv.reader(csv_file)
        for fields in entries:
            if(fields[0]=="SUPERBLOCK"):
                first_inode = int(fields[7])
                block_cap = int(fields[1])
                last_inode = int(fields[2])
   
            if(fields[0]=="GROUP"):
                index = len(reserved_blocks)
                reserved_blocks.insert(index,fields[6])
                index = len(reserved_blocks)
                reserved_blocks.insert(index,fields[7])
                index = len(reserved_blocks)
                reserved_blocks.insert(index,fields[8])
                b_no = int(fields[6])
                if (b_no!=0):
                    index = len(blocks_referenced)
                    blocks_referenced.insert(index,int(fields[6]))
                b_no = int(fields[7])
                if (b_no!=0):
                    index = len(blocks_referenced)
                    blocks_referenced.insert(index,int(fields[7]))
                b_no = int(fields[8])
                if (b_no!=0):
                    index = len(blocks_referenced)
                    blocks_referenced.insert(index,int(fields[8]))

            if(fields[0]=="BFREE"):
                b_no = int(fields[1])
                if (b_no!=0):
                    index = len(blocks_referenced)
                    blocks_referenced.insert(index,int(fields[1]))
                    index = len(free_blocks)
                    free_blocks.insert(index,b_no)
            if(fields[0]=="INDIRECT"):
                b_no = int(fields[5])
                if (b_no!=0):
                    blocks_referenced.insert(index,b_no)
            if(fields[0]=="INODE"):
                y = 0
                if(fields[2]!="?"):
                    index = len(allocated_inodes)
                    b_no = int(fields[1])
                    allocated_inodes.insert(index,b_no)
                while(y<15):
                    index = len(blocks_referenced)
                    b_no = int(fields[12+y])
                    if (b_no!=0):
                        blocks_referenced.insert(index,b_no)
                        if b_no in free_blocks:
                            print "ALLOCATED BLOCK " + fields[12+y] + " ON FREELIST"
                    y = y + 1
            if(fields[0]=="IFREE"):
                index = len(free_inodes)
                b_no = int(fields[1])
                free_inodes.insert(index,b_no)
                    
        #print block_cap
        #print blocks_referenced.count(8)
        it = 0
        while(it < block_cap-1):
            if it not in blocks_referenced:
                print "UNREFERENCED BLOCK " + str(it) #Block numbers 6 and 7 are never in this list but they are supposed to be refered i dont know why cos inode block begin from 5
            it = it + 1
        it = first_inode
        print first_inode

        while ( it < last_inode ):
            if it not in allocated_inodes:
                if it not in free_inodes:
                    print "UNALLOCATED INODE " + str(it) + " NOT ON FREELIST"
            if it in free_inodes:
                if it in allocated_inodes:
                    print "ALLOCATED INODE " + str(it) + " ON FREELIST"
            it = it + 1
        it = 0
        while ( it < len(allocated_inodes)):
            if allocated_inodes[it] in free_inodes:
                print "ALLOCATED INODE " + str(allocated_inodes[it]) + " ON FREELIST"
            it = it + 1
    
    with open(filename,"r") as csv_file:
        entries = csv.reader(csv_file)
        for fields in entries:
            #print fields
            if(fields[0]=="INODE"):
                x = 0
                while(x<15):
                    if(int(fields[12+x])>block_cap or int(fields[12+x])<0):
                        if(x==12):
                            print "INVALID INDIRECT BLOCK " + fields[12+x] + " IN INODE " + fields[1] + " AT OFFSET 12"
                        elif (x==13):
                            print "INVALID DOUBLE INDIRECT BLOCK " + fields[12+x] + " IN INODE " + fields[1] + " AT OFFSET 268"
                        elif (x==14):
                            print "INVALID TRIPPLE INDIRECT BLOCK " + fields[12+x] + " IN INODE " + fields[1] + " AT OFFSET 65804"
                        else:
                            print "INVALID BLOCK " + fields[12+x] + " IN INODE " + fields[1] + " AT OFFSET " + str(x)
                    if fields[12+x] in reserved_blocks:
                        if(x==12):
                            print "RESERVED INDIRECT BLOCK " + fields[12+x] + " IN INODE " + fields[1] + " AT OFFSET 12"
                        elif (x==13):
                            print "RESERVED DOUBLE INDIRECT BLOCK " + fields[12+x] + " IN INODE " + fields[1] + " AT OFFSET 268"
                        elif (x==14):
                            print "RESERVED TRIPPLE INDIRECT BLOCK " + fields[12+x] + " IN INODE " + fields[1] + " AT OFFSET 65804"
                        else:
                            print "RESERVED BLOCK " + fields[12+x] + " IN INODE " + fields[1] + " AT OFFSET " + str(x)
                    if (blocks_referenced.count(int(fields[12+x]))>1):
                        if(x==12):
                            print "DUPLICATE INDIRECT BLOCK " + fields[12+x] + " IN INODE " + fields[1] + " AT OFFSET 12"
                        elif(x==13):
                            print "DUPLICATE DOUBLE INDIRECT BLOCK " + fields[12+x] + " IN INODE " + fields[1] + " AT OFFSET 268"
                        elif(x==14):
                            print "DUPLICATE TRIPPLE INDIRECT BLOCK " + fields[12+x] + " IN INODE " + fields[1] + " AT OFFSET 65804"
                        else:
                            print "DUPLICATE BLOCK " + fields[12+x] + " IN INODE " + fields[1] + " AT OFFSET " + str(x)
                    x = x + 1
   
    #################################
    #I-node Allocation Audits########
    #################################
    with open(filename,"r") as csv_file:
        csv_entries = csv.reader(csv_file)
       

        #convert to array
        entries=[]
        for entry in csv_entries:
            entries.append(entry);

        """
        check that each allocated I-node should be referred to by the a number of directory 
        entries that is equal to the reference count recorded in the I-node
        """
        #create dictionary of inode num to link count
        inode_num_to_link_count_dict={}
        for fields in entries:
            if(fields[0]=="DIRENT"):
                referenced_file_inode_num=fields[3];
                if referenced_file_inode_num in inode_num_to_link_count_dict:
                    inode_num_to_link_count_dict[referenced_file_inode_num]+=1;
                else:
                    inode_num_to_link_count_dict[referenced_file_inode_num]=1;
        for fields in entries:
            if(fields[0]=="INODE"):
                inode_num=fields[1];
                link_count_stored_in_inode=fields[6];
                link_count_based_on_dirents=str(inode_num_to_link_count_dict[inode_num]);
                if link_count_based_on_dirents != link_count_stored_in_inode:
                    print "INODE " + inode_num + " HAS " + link_count_based_on_dirents + " LINKS BUT LINKCOUNT IS " + link_count_stored_in_inode
        
        """
        Check that directory entries should only refer to valid and allocated I-nodes. 
        While scanning the directory entries, check the validity and allocation status of each referenced I-node. 
        """

        #create set of free inodes inodes
        #root directory is always inode 2 
        free_inode_nums=set();
        for fields in entries:
            if(fields[0]=="IFREE"):
                inode_num=fields[1];
                free_inode_nums.add(inode_num);
        #valid inodes are from 2 (root inode) to the total number of inodes aka last_inode
        first_valid_inode=2;
        for fields in entries:
            if(fields[0]=="DIRENT"):
                parent_inode_num=fields[1];
                referenced_file_inode_num=fields[3];
                direct_entry_name=fields[6];
                if int(referenced_file_inode_num) < first_valid_inode or int(referenced_file_inode_num) > last_inode:
                    print "DIRECTORY INODE "+parent_inode_num +" NAME " +direct_entry_name+ " INVALID INODE "+referenced_file_inode_num
                elif referenced_file_inode_num in free_inode_nums:
                    print "DIRECTORY INODE "+parent_inode_num +" NAME " +direct_entry_name+ " UNALLOCATED INODE "+referenced_file_inode_num

        #Check that each directory begins with two links, 
        #one to itself (.) and one to its parent (..). 
        #While scanning each directory, check for the validity of these two links and, 
        #for each detected inconsistency, a message like one of the following should be generated to stdout:
        #collect list of directories
        directory_inode_nums=set()
        for fields in entries:
            if(fields[0]=="INODE" and fields[2]=='d'):
                directory_inode_num=fields[1]
                directory_inode_nums.add(directory_inode_num);

        #map child to parent according to dirent entries
        child_to_parent_dict={}
        child_to_parent_dict['2']='2'; #root's parent is always root
        for fields in entries:
            if(fields[0]=="DIRENT"):
                parent_inode_num=fields[1];
                referenced_file_inode_num=fields[3];
                direct_entry_name=fields[6];
                if(referenced_file_inode_num not in directory_inode_nums):
                    #not a directory
                    continue;
                elif(direct_entry_name == "'.'" or direct_entry_name=="'..'"):
                    #not a child directory
                    continue;
                else:
                    child_to_parent_dict[referenced_file_inode_num]=parent_inode_num;
        
        directory_entry_check_status={}
        for d in directory_inode_nums:
            directory_entry_check_status[d]=3;
        #check directories
        for fields in entries:
            if(fields[0]=="DIRENT"):
                parent_inode_num=fields[1];
                referenced_file_inode_num=fields[3];
                direct_entry_name=fields[6];
                check_status=directory_entry_check_status[parent_inode_num];
                if(direct_entry_name == "'.'"):
                    if(not check_status&1):
                        directory_entry_check_status[parent_inode_num]=-1;
                    else:
                        directory_entry_check_status[parent_inode_num]-=1;
                    if(referenced_file_inode_num != parent_inode_num):
                        print "DIRECTORY INODE "+parent_inode_num+" NAME '.' LINK TO INODE "+referenced_file_inode_num+" SHOULD BE "+parent_inode_num
                    
                elif(direct_entry_name=="'..'"):
                    if(not check_status&2):
                        directory_entry_check_status[parent_inode_num]=-1;
                    else:
                        directory_entry_check_status[parent_inode_num]-=2;
                    if(child_to_parent_dict[parent_inode_num] != referenced_file_inode_num):
                        print "DIRECTORY INODE "+parent_inode_num+" NAME '..' LINK TO INODE "+referenced_file_inode_num+" SHOULD BE "+child_to_parent_dict[parent_inode_num] 
        #check that all hit 
        #(I think this is a good check, but not included in specs so nevermind)
        #for key in directory_entry_check_status:
            #if(directory_entry_check_status[key]!=0):
                #print "DIRECTORY INODE "+str(key)+" HAS EITHER TOO MANY OR TOO FEW '.' OR '..' ENTRIES"
                #continue

              





if __name__ == "__main__":
    main()
