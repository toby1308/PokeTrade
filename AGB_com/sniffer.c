#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
/**
-----------------------------------------------------
		Sniffer code
-----------------------------------------------------
This code is used to read the data between a master
and its' slaves. By connecting the PI up such that
it is parrallel to slave 1 it will be able to read
and save all the data from each device so that it
may be analyised later.
Important note is to disconect the SO chain from
the PI due to it not interacting with the chain.
**/


FILE* master_file;
FILE* slave_file;
FILE* test_file;
time_t initial_time;

u_int16_t process_master_input(u_int16_t master_data, u_int16_t slave1_data){
	/**
	Uses the standard function naming scheme for the 16_bit_multi_com
	for processing of the master data.
	In this case it saves the data to the master data file
	ARGS:
	Master_data: u_int16_t number that represents the data from the master
			sent through the SD chain
	Slave1_data: u_int16_t number that represents the data within the
			virtual SIOMULTI1 register
	Returns:
	Slave1_data: u_int16_t number that represents the how the GBA
			would respond to the data sent
	**/
	int time_stamp = time(NULL) - initial_time;
	if(master_data != 0x0000){
	fprintf(master_file,"%d",time_stamp);
	fprintf(master_file," ");
	fprintf(master_file,"%d\n",master_data);
	}
	return slave1_data;
}

u_int16_t process_slave1_input(u_int16_t slave_data){
	/**
	A standard function for the 16_bit_multi_com
	for most cases this function will not be used
	but for the sniffer code it is used to save the
	data the real slave will send.
	ARGS:
	Slave_data: u_int16_t number that represents the data sent by
			the slave 1 within the chian
	Returns: u_int16_t number that is just the sent data as no
			processing would happen duing this time
	**/
	int time_stamp = time(NULL) - initial_time;
	if(slave_data != 0x0000){
	fprintf(slave_file,"%d",time_stamp);
	fprintf(slave_file," ");
	fprintf(slave_file,"%d\n",slave_data);
	}
	return slave_data;
}

u_int16_t process_slave2_input(u_int16_t slave2_data, u_int16_t slave1_data){
	/**
	A standard function for the 16_bit_mutli_com
	In this case it used to save the data from slave2
	ARGS:
	slave2_data: represents the data sent by slave2 down the SD chain
	slave1_data: represnets the data stored in the SIOMULTI1
	Returns:
	slave1_data: u_int16_t number as no processing is done
	**/
	return slave1_data;
}

u_int16_t process_slave3_input(u_int16_t slave3_data, u_int16_t slave1_data){
	/**
	A standard function for the 16_bit_multi_com
	In this case it is used to save the data from slave3
	ARGS:
	slave3_data: u_int16_t number that represents the data sent by
			slave3 down the SD chain
	slave1_data: u_int16_t number that represents the dat stored in
			the SIOMULTI1 register
	Returns:
	slave1_data: u_int16_t number as no processing is done
	**/
	return slave1_data;
}

int start_up(){
	/**
	Optional function for the 16_bit_com
	that sets up any data need for the processing functions
	**/
	master_file = fopen("master_data.txt","a");
	slave_file = fopen("slave_data.txt","a");
	test_file = fopen("test_file.txt","a");
	initial_time = time(NULL);
	if(master_file == NULL){
		printf("Failed to open master_data.txt");
		return 0;
	}
	if(slave_file == NULL){
		printf("Failed to open slave_data.txt");
		return 0;
	}
	//process_slave1_input(0xFFFF);
	//fclose(slave_file);
	return 1;
}

void end(){
	fclose(slave_file);
	fclose(master_file);
}
void longest_input_process(){
	/**
	Function that represents the function that's expected to take the longest
	used for initialising the code and testing how powerful the computer is
	**/
	//printf("longest input test cycle\n");
	u_int16_t temp_val = 0xFFFF;
	int time_stamp = time(NULL) - initial_time;
	if(temp_val != 0x0000){
		fprintf(test_file,"%d",time_stamp);
		fprintf(test_file," ");
		fprintf(test_file,"%d\n",temp_val);
	}
}
