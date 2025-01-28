#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include "sniffer.h"
#include <wiringPi.h>

#define VIR_SD 1
#define VIR_SI 0
#define VIR_SC 2
#define VIR_SO 3

#define LOW 0
#define HIGH 1
#define SNIFFER 0
#define SPOOFER 1

int baut_rate = 115200; //Adjust for application
int baut_time = 8680; //Nano second delay between signals
int gpio_delay; //Nano second delay from reading/writing from/to pins

/**

u_int16_t process_master_input(u_int16_t alpha, u_int16_t beta){
	return 0xFFFF;
}

u_int16_t process_slave1_input(u_int16_t alpha){
	return 0xFFFF;
}

u_int16_t process_slave2_input(u_int16_t alpha, u_int16_t beta){
	return 0xFFFF;
}

u_int16_t process_slave3_input(u_int16_t alpha, u_int16_t beta){
	return 0xFFFF;
}


**/

int vir_SD_read(){
	pinMode(VIR_SD,INPUT);
	return digitalRead(VIR_SD);
}
void vir_SD_up(){
	pinMode(VIR_SD,OUTPUT);
	digitalWrite(VIR_SD,HIGH);
}

void vir_SD_down(){
	pinMode(VIR_SD,OUTPUT);
	digitalWrite(VIR_SD,LOW);
}

int vir_SI_read(){
	pinMode(VIR_SI,INPUT);
	return digitalRead(VIR_SI);
}

void vir_SI_up(){
	pinMode(VIR_SI,OUTPUT);
	digitalWrite(VIR_SI,HIGH);
}

void vir_SI_down(){
	pinMode(VIR_SI,OUTPUT);
	digitalWrite(VIR_SI,LOW);
}

int vir_SC_read(){
	pinMode(VIR_SC,INPUT);
	return digitalRead(VIR_SC);
}

void vir_SC_up(){
	pinMode(VIR_SC,OUTPUT);
	digitalWrite(VIR_SC,HIGH);
}

void vir_SC_down(){
	pinMode(VIR_SC,OUTPUT);
	digitalWrite(VIR_SC,LOW);
}

int vir_SO_read(){
	pinMode(VIR_SO,INPUT);
	return digitalRead(VIR_SO);
}
void vir_SO_up(){
	pinMode(VIR_SO,OUTPUT);
	digitalWrite(VIR_SO,HIGH);
}

void vir_SO_down(){
	pinMode(VIR_SO,OUTPUT);
	digitalWrite(VIR_SO,LOW);
}

u_int16_t listen_16(){
	/**
	Records the data along the SD line by checking the voltage at timed intervals acoording to a Baut rate of 115200

	ARGs:
	None

	Returns:
	u_int16_t: A 16-bit value to be stored in the apropirate virtual SIOMULTI register

	**/
	int endbit = 0;
	int startbit = 0;
	//int bit_array[16];
	double bit_rate = (baut_time - gpio_delay) / pow(10,3); //micro seconds
	u_int16_t data = 0;
	//checks for start bit
	while(startbit == 0){
		if (vir_SC_read() == 0){
			startbit = 1;
			usleep(bit_rate);
		}
	}
	//reads data bits
	for (int i = 0; i <16; i++){
		//bit_array[i - 15] = vir_SC_read();
		data = data + vir_SC_read() * pow(2,i);
		usleep(bit_rate);
	}
	//checks for endbit
	while(endbit == 0){
		if (vir_SC_read() == 1){
			endbit = 1;
			usleep(bit_rate);
		}
	}
	//returns the sent data
	return data;
}

void write_16(u_int16_t data){
	/**
	Writes daata from SIOMULTI1 register to SD at time intervals set by the Baut rate (115200)

	ARGs:
		data(u_int16_t): data from SIOMULTI1 that is to be written to the SD chain

	Returns:
		NULL
	**/

	int bit_array[16];
	double bit_rate = (baut_time - gpio_delay) / pow(10,3);
	for (int i = 0; i< 16; i++){
		if (data >= pow(2,15 - i)){
			data -= pow(2,15 - i);
			bit_array[i] = i;
		} else {
			bit_array[i] = 0;
		}
	}
	vir_SD_down();
	usleep(bit_rate);
	for (int i = 0; i < 16; i++){
		if (bit_array[i] = 1){
			vir_SD_up();
		} else {
			vir_SD_down();
		}
		usleep(bit_rate);
	}
	vir_SD_up();
	usleep(bit_rate);
	pinMode(VIR_SD,INPUT);
	/**
	for (int i = 0;i < 16; i++){
		printf("%d",bit_array[i]);
	}
	**/
}

int start_communication(){
	/**
	Handels 16 bit multiplayer communicaiton over AGV cables by acting as Slave 1
	Setup required:


		Master
	|--------------------|			     |--------------------|
	|	  SD	     |---------------------> |	       GPIO02     |
	|--------------------|			     |--------------------|
	|	  SI	     |--------->GND  |-----> |	       GPIO03     |
	|--------------------|               |       |--------------------|
	|	  SO	     |---------------|       |	       GPIO04     |
	|--------------------|                       |--------------------|
	|	  SC 	     |---------------------> |	       GPIO05     |
 	|--------------------|			     |--------------------|

	The full protocol can be seen here: https://cdn.preterhuman.net/texts/gaming_and_diversion/Gameboy%20Advance%20Programming%20Manual%20v1.1.pdf (page134)

	All data is written and read onto the SD chain, where the mater and slaves take turns writting. Knowing wether it's their turn to write if:
		-SC: is low during data transfere
		-SI: is low
		-SO: is low (set to high after)

	Uses a time out mechanic to exit the code, better options to be added
	Ideas for exit code:
	-Error codes from AGB manual
	-No communication after set time
	-Exit code sent down SD (specific to application)
	**/
	//Setup
	wiringPiSetup();

	//Starting timer
	time_t start_time;
	time(&start_time);
	time_t max_time = 600;

	//Initialising virtual registers
	u_int16_t siomulti0 = 0xFFFF;
	u_int16_t siomulti1 = 0xFFFF;
	u_int16_t siomulti2 = 0xFFFF;
	u_int16_t siomulti3 = 0xFFFF;
	int input = 1;
	int cycle_check = 0;
	int initial_check = 0;

	//Variables
	int written_data = LOW;
	int m_clock;
	int m_so;
	int delta;
	int cycle = 0;

	//switches
	int mode = SNIFFER;
	int sc_high = FALSE;
	int data_transfere = FALSE;

	//checks if SC is high to confirm connection to AGB cable
	printf("Waiting for SC connection\n");
	while(sc_high == FALSE){
		/**
		cycle_check++;
		if(cycle_check == pow(10,9)){
			delta = time(NULL) - start_time;
			return delta;
		}
		**/
		//time out check
		delta = time(NULL) - start_time;
		if(delta > max_time){
			return initial_check;
		}
		if (vir_SC_read() == 1){
			sc_high = TRUE;
			data_transfere = TRUE;
		}
	}

	printf("SC connection made");
	while(data_transfere == TRUE){
		delta = time(NULL) - start_time;
		if(delta > max_time){
			data_transfere = FALSE;
		}
		m_clock = vir_SC_read();
		switch(m_clock){
			//If vir_SC is low then a communication block has started
			case LOW:
				//Checks SI to see if master is sending data or not
				m_so = vir_SI_read();
				if(m_so == HIGH){//Master sending data
					siomulti0 = listen_16();
					siomulti1 = process_master_input(siomulti1,siomulti0);
					cycle++;
				}
				if(written_data == HIGH && m_so == LOW){//PI and master has sent data
					//Listen for slave 2 & 3
					switch(cycle){
						case 2:
							siomulti2 = listen_16();
							siomulti1 = process_slave2_input(siomulti2,siomulti1);
							break;
						case 3:
							siomulti3 = listen_16();
							siomulti1 = process_slave3_input(siomulti3,siomulti1);
							break;
						default:
							break;
					}
					cycle++;
				}
				if (written_data == LOW && m_so == LOW && mode == SPOOFER){//Only master has sent data
					write_16(siomulti1);
					written_data = HIGH;
					cycle++;
					vir_SO_down();
				}
				if (written_data == LOW && m_so == LOW && mode == SNIFFER){//If Pi is used as a sniffer, read data instead
					siomulti1 = listen_16();
					siomulti1 = process_slave1_input(siomulti1);
					cycle++;
					written_data = HIGH;
				}
				break;
			default:
				//Resets variables for next communication block
				written_data = LOW;
				cycle = 0;
				vir_SO_up();
				break;
		}
	}
	return 0;
}

int initial_test(){
	/**
	Runs 2 test to customise variables for computer
	Test 1: Checks how long computer takes to read GPIO pins, sets and saves baut_rate approriately
	Test 2: Checks the expected longest prcess time [function written by the processing file] and sees if
		run time is less than the baut rate
	**/
	wiringPiSetup();
	time_t start_time = time(NULL);
	int process_delay;
	u_int16_t data = 0;
	printf("0");
	printf("%%\n");
	for(int i = 0; i < 10; i++){
		for(int j = 0; j < pow(10,7); j++){
			data = vir_SD_read() * pow(2,15);
			data++;
		}
		printf("%d",(i + 1) * 10);
		printf("%%\n");
	}
	printf("Testing phase 1 complete\n");
	gpio_delay = (time(NULL) - start_time) * 10;//nano second delay for reading / writing data;
	start_time = time(NULL);
	printf("Starting phase 2\n");
	printf("0");
	printf("%%\n");
	for(int i = 0; i < 10; i++){
		for(int j = 0; j < pow(10,7); j++){
			process_slave1_input(0xFFFF);
		}
		printf("%d",(i + 1) * 10);
		printf("%%\n");
	}
	printf("phase 2 complete\n");
	process_delay = (time(NULL) - start_time) * 10;//Nano second processing time
	if(process_delay > baut_time){
		printf("Your computer is not strong enough to run this program");
	}
	else {
		printf("Tests successful\n");
	}
	printf("Testing finsished, saving data\n");
	return gpio_delay;
}

int main(){
	if(start_up() == 0){
		printf("Error setting up processing file\n");
		return 0;
	}
	FILE* custom_file = fopen("initial_data.txt","r");
	if(custom_file == NULL){
		printf("No file found\n");
		printf("Initial testing beginning\n");
		printf("Please wait, proces should only take a couple of minutes\n");
		custom_file = fopen("initial_data.txt","a");
		fprintf(custom_file,"%d",initial_test());
		printf("Data saved");
		fclose(custom_file);
	}
	else {
		fscanf(custom_file,"%d",&gpio_delay);
		printf("gpio delay is: ");
		printf("%1d\n",gpio_delay);
	}
	printf("%d",start_communication());
}
