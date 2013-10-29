/**********************
* Taylor Sprinkle     *
* 9/23/12             *
* ECE 463             *
* Project 1 Part A    *
**********************/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>

using namespace std;
// initialize cache function
void initialized_Cache(int a, int b, int c, int d, int e, int f) ;
//read cache function prototype
void read_Cache(unsigned int addr);
// write cache function prototype
void write_Cache(unsigned int addr, int miss);
// conversion prototype
unsigned int hex2Int(char* hexNum, int size);
unsigned int mask(unsigned int addr, int size, int shift); 

class CACHE 
{
    private:
	unsigned int comp_tag, comp_index, comp_offset;
	//pointer to the next CACHE in memory hierarchy
    CACHE *nextlevel; 
 
    public:
        //CACHE member functions
        void initialized_Cache(int a, int b, int c, int d, int e, int f);
        void read_Cache(unsigned int addr);
        void write_Cache(unsigned int addr, int miss);
        unsigned int tag_Calculated(unsigned int addr);
        unsigned int index_Calculated(unsigned int addr);
        unsigned int offset_Calculated(unsigned int addr);
		unsigned int hex2Int(char *hexNum, int size);
		unsigned int mask(unsigned int addr, int size, int shift);
		int read, read_hit, read_miss, write, write_hit, write_miss, write_back, total;
		int blocksize, size, set_assoc, replacement_policy, write_policy, sets, bytes_used, used_sets, tag_size, index_size, offset_size;
	
		struct tag_struct 
		{
            int age;
            unsigned int tag, index, offset;
            bool dirty, valid;            
        };
        
        tag_struct **tag_Entry;
};


int main (int argc, char *argv[]) 
{
    ifstream trace;
    CACHE L1;
    string file, trace_input, operation, address, dirty;
    unsigned int addr_hex;
    int blcksz, sz, assc, rplcy, wplcy, sts, address_size;
    double missrate, memtraffic, accesstime, totalaccesstime, misspenalty;
    int iteration = 1;
    
    if (argc != 7) 
    { // 6 inputs plus the program name, needed for correct execution
        printf("Invalid number of arguments\n");
        return 1;
    }
    
    else 
    {        
        blcksz = atoi (argv[1]);
        sz = atoi (argv[2]);
        assc = atoi (argv[3]);
        rplcy = atoi (argv[4]);
        wplcy = atoi(argv[5]);
        sts = sz/(assc*blcksz);
		file = argv[6];
	
	trace.open(&file[0]);
	
        if (((blcksz & (blcksz - 1)) && (sts & (sts - 1))) == 0)
        { //check if power of two then run
         //Initialize CACHE
	    L1.initialized_Cache (blcksz, sz, assc, rplcy, wplcy, sts);
            if ((trace == NULL) || (!trace.is_open())) 
            {
                printf("Error with file\n");
                return 1;
            }
            else 
            {
                while (getline(trace, trace_input)) 
                { //Check if EOF reached
		   		 operation = trace_input.substr(0,1); //extract r or w
		    	 address = trace_input.substr(2,8); //extract up to 8 hex characters
		    	 address_size = int(address.size());
		    	 addr_hex = L1.hex2Int(&address[0], address_size);
		    	if (operation == "r")
		    	 {
                    L1.read_Cache(addr_hex);
					trace_input.clear();
					operation.clear();
					address.clear();
                }
                if (operation == "w")
                {
                	//'0' indicates write w/o read miss
                    L1.write_Cache(addr_hex, 0); 
					trace_input.clear();
					operation.clear();
					address.clear();
                    }
		    iteration++;

                }
		if (trace.eof())
		{
		    trace.close();
		}
printf("===== Simulator configuration ===== \n L1_Blocksize:  %d\n L1_Size:  %d \n L1_Assoc:  %d\n L1_Replacement_Policy:  %d \n L1_Write_Policy: %d \n trace_file: %s \n ===================================\n\n", L1.blocksize, L1.size, L1.set_assoc, L1.replacement_policy, L1.write_policy, &file[0]);

printf("===== L1 contents =====");
	for(int x=0;x<L1.sets;x++){
		printf("\nset   %d:  ", x);
		for(int i=0;i<L1.set_assoc;i++)
		{
			if (L1.tag_Entry[x][i].dirty == true) dirty[0] = 'D';
			if (L1.tag_Entry[x][i].dirty == false) dirty[0] = '\0';
			if (L1.tag_Entry[x][i].valid == false) printf("- %s  ", &dirty[0]);
			if (L1.tag_Entry[x][i].valid == true) printf("%x %s  ",L1.tag_Entry[x][i].tag, &dirty[0]);
		}
	}
misspenalty = 20 + 0.5*(L1.blocksize/16);
totalaccesstime = (((L1.read_hit + L1.read_miss) + L1.write_miss)*(.25+2.5*(L1.size/512)+.025*(L1.blocksize/16)+.025*L1.set_assoc)) + ((L1.read_miss + L1.write_hit)*misspenalty);
missrate = (L1.read_miss + L1.write_miss)/((L1.read_hit + L1.read_miss) + L1.write_miss);
memtraffic = L1.read_miss + L1.write_miss + L1.write_back;
accesstime = (.25+2.5*(L1.size/512)+.025*(L1.blocksize/16)+.025*L1.set_assoc) + (missrate *(20+.5*(L1.blocksize/16)));
printf("\n\n  ====== Simulation results (raw) ====== \n  a. number of L1 reads:     %d\n  b. number of L1 read misses:     %d\n  c. number of L1 writes:   %d\n  d. number of L1 write misses:   %d\n  e. L1 miss rate:  %f\n  f. number of writebacks from L1:   %d\n  g. total memory traffic:   %f\n\n ==== Simulation results (performance) ====\n  1. average access time:     %f ns  ", (L1.read_hit + L1.read_miss), L1.read_miss, L1.write_miss, (L1.write_hit + L1.write_miss), missrate, L1.write_back, memtraffic, accesstime);
																																																																																										
                return 0;
            }            
        }
        else 
        {
            printf("Blocksize and Number of Sets is not a 'Power of Two'\n");
            return 1;
        }        
    }
}

// initialize cache function
void CACHE::initialized_Cache(int a, int b, int c, int d, int e, int f) 
{
    //Initialize Cache properties   
    blocksize = a;
    size = b;
    set_assoc = c;
    replacement_policy = d;
    write_policy = e;
    sets = f;
    
    read = 0;
    read_hit = 0;
    read_miss = 0;
    write = 0;
    write_hit = 0;
    write_miss = 0;
    write_back = 0;
    total = 0;
    
    index_size = int(log2(sets) + 0.5);
    offset_size = int(log2(blocksize) + 0.5);
     //assuming 32bit address
    tag_size = int((32 - index_size - offset_size));
    
    
    //Create Cache
    //creates rows [sets]
    tag_Entry = new tag_struct* [sets]; 
    for (int h = 0; h < sets; h++) 
    {
    	//creates columns [sets][set_assoc]
        tag_Entry[h] = new tag_struct [set_assoc]; 
        for (int i = 0; i < set_assoc; i++) 
        {
            tag_Entry[h][i].age = 0;
	    tag_Entry[h][i].dirty = false;
	    tag_Entry[h][i].valid = false;
        }
    }
}

//read cache function
void CACHE::read_Cache(unsigned int addr)
{ 		
    int c_age = 0;
    
    comp_tag = mask(addr, tag_size, (index_size + offset_size));
    comp_index = mask(addr, index_size, offset_size);
    comp_offset = mask(addr, offset_size, 0);
    
    read += 1;

    for (int j = 0; j  <= set_assoc; j++) 
    {    
	if (j == set_assoc) 
	{	//Read Miss
	    read_miss += 1;
	    total += 1;
	    write_Cache(addr, 1); //1 indicates write w/ read miss
	}
        if((j < set_assoc) && (tag_Entry[comp_index][j].valid == true))
        { 			
	    if(tag_Entry[comp_index][j].tag == comp_tag) 
	    {
		if (tag_Entry[comp_index][j].index == comp_index) 
		{	//Read Hit
		    //Incr Read Hit and pull info
		    read_hit += 1;
		    
		    c_age = tag_Entry[comp_index][j].age;
		    if (c_age != 1) {
			for (int k = 0; k < set_assoc; k++) {  		
			    if (tag_Entry[comp_index][k].valid == true) {
				if (tag_Entry[comp_index][k].age < c_age) {
				    tag_Entry[comp_index][k].age += 1;
			        }			    
			        if (tag_Entry[comp_index][k].age > set_assoc) {
				    tag_Entry[comp_index][k].age = set_assoc;
				}
			    }
			}
		    }
		    tag_Entry[comp_index][j].age = 1;
		    break;
		}
	    }
	}	
    }
}


// write cache function
void CACHE::write_Cache(unsigned int addr, int miss) 
{ 	
    int c_age = 0;
    int t = 0;

    comp_tag = mask(addr, tag_size, (index_size + offset_size));
    comp_index = mask(addr, index_size, offset_size);
    comp_offset = mask(addr, offset_size, 0);
    
    write += 1;

    if (miss > 0) 
    { 								//Read Miss or Write Miss
	for (int i = 0; i  < set_assoc; i++) 
	{  
	   if ((tag_Entry[comp_index][i].age == 0) || (tag_Entry[comp_index][i].age == set_assoc)) 
	   { //Find first free or oldest tag to evict
		t = i;
		break;
	    }
	    if (tag_Entry[comp_index][i].age > tag_Entry[comp_index][t].age) 
	    {
		t = i;
	    }
	}        
	if ((tag_Entry[comp_index][t].dirty == true) && (write_policy == 0)) 
	{ //WBWA
	    //Update next level information** (i.e. writeback to next level)
	    write_back += 1;
	    total += 1;
	    tag_Entry[comp_index][t].dirty = false;
	}
	tag_Entry[comp_index][t].tag = comp_tag; 
        tag_Entry[comp_index][t].index = comp_index;
        tag_Entry[comp_index][t].offset = comp_offset;
        tag_Entry[comp_index][t].valid = true;
	
	c_age = tag_Entry[comp_index][t].age;	
	
	//If using evicted set
	if (c_age > 1) 
	{  	
	    for (int k = 0; k < set_assoc; k++) 
	    {  //Incr Age
		if (tag_Entry[comp_index][k].valid == true)
		{
		    if (tag_Entry[comp_index][k].age < c_age) 
		    {
		        tag_Entry[comp_index][k].age += 1;
		    }
		    if (tag_Entry[comp_index][k].age > set_assoc) 
		    {
		        tag_Entry[comp_index][k].age = set_assoc;
		    }
		}

	    }
	    tag_Entry[comp_index][t].age = 1;
	}
	if (c_age == 0) 
	{ 	//If using new, previously unallocated set
	    for (int k = 0; k < set_assoc; k++) 
	    {  				
		if (tag_Entry[comp_index][k].valid == true) 
		{
		    tag_Entry[comp_index][k].age += 1;
		    if (tag_Entry[comp_index][k].age > set_assoc) 
		    {
		        tag_Entry[comp_index][k].age = set_assoc;
		    }
		}

	    }
	    tag_Entry[comp_index][t].age = 1;
	}	    
    //Get next level address information
	if (miss == 1) 
	{  	//Read Miss
	    tag_Entry[comp_index][t].dirty = false;
	}
	if (miss == 2 && write_policy == 0) 
	{    				//WBWA  //Event of recursive Write Miss
	    tag_Entry[comp_index][t].dirty = true;
	}    
    }
    if (miss == 0) 
    {              						

	for (int j = 0; j  <= set_assoc; j++) 
	{
	    if (j == set_assoc) 
	    {	//Write Miss
		write_miss += 1;
		if (write_policy == 1) 
		{ //WTNA do not allocate new memory therefore propegate to next level
		    //Update next level address information**
		    write_back += 1;
		    total += 1;
		}
		if (write_policy == 0) 
		{ //WBWA allocate new memory but don't propegate unless evicted therefore causes dirty areas.
		    write_Cache (addr, 2);  //recursively calls itself during the event of a miss
		}
		break;
	    }
	    if ((j < set_assoc) && (tag_Entry[comp_index][j].valid == true)) 
	    {
		if (tag_Entry[comp_index][j].tag == comp_tag) 
		{
		    if (tag_Entry[comp_index][j].index == comp_index) 
		    { //Write Hit
			//Update next level address information
			write_hit += 1;
			
			tag_Entry[comp_index][j].tag = comp_tag;
			tag_Entry[comp_index][j].index = comp_index;
			tag_Entry[comp_index][j].offset = comp_offset;
			if (write_policy == 1) 
			{  //WTNA
			    //Update next level address information
			    write_back += 1;
			    total += 1;
			    tag_Entry[comp_index][j].dirty = false;
			}
			if (write_policy == 0) { 				//WBWA
			    tag_Entry[comp_index][j].dirty = true;
			}
			
			c_age = tag_Entry[comp_index][j].age;
			if (c_age != 1) 
			{
			    for (int k = 0; k < sets; k++) 
			    {  		
			        if (tag_Entry[comp_index][k].valid == true) 
			        {
				    if (tag_Entry[comp_index][k].age < c_age)
				     {
				        tag_Entry[comp_index][k].age += 1;
				    }		
				    if (tag_Entry[comp_index][k].age > set_assoc) 
				    {
				        tag_Entry[comp_index][k].age = set_assoc;
				    }
				}

			    }
			tag_Entry[comp_index][j].age = 1;
			break;
			}
		    }
		}
	    }
        }
    }
}

// conversion
unsigned int CACHE::hex2Int(char* hexNum, int size) 
{
    char c;
    double r = 0.0;

    for (int k = 1; k <= size; k++) 
    {
	c = *hexNum++;
	switch(c) {
	    case 'a': r += 10*pow(16.0,(size - k)); break;
	    case 'b': r += 11*pow(16.0,(size - k)); break;
	    case 'c': r += 12*pow(16.0,(size - k)); break;
	    case 'd': r += 13*pow(16.0,(size - k)); break;
	    case 'e': r += 14*pow(16.0,(size - k)); break;
	    case 'f': r += 15*pow(16.0,(size - k)); break;
	    default : r += atoi(&c)*pow(16.0,(size - k)); break;
       }
    }
    // return a typecast r
    return (unsigned int)r;
}
    
unsigned int CACHE::mask(unsigned int addr, int size, int shift) 
{
    unsigned int temp;
    unsigned int mask = 1;

    addr = addr >> shift;

    for (int k = 0; k < (size - 1); k++)
 {
	mask = mask << 1;
       mask = (mask | 1);
    }

    temp = (addr & mask);
    return temp;
}



