
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>


int file_length(FILE * file){
	int count = 0;
	char str[50];
	while(fgets(str,50,file) != NULL){
		count = count + 1;
	}
	return count;
}

void initialize_array(int* array, FILE * file){
	for(int i = 0; i < sizeof(array);i++){
		fscanf(file,"%d\n",&array[i]);
	}
}

void print_voltage(int* array,int target_voltage,int starting_clock,int page){
	int bit = 1;
	int found = 0;
	int count = 0;
	int current_voltage = 0;
	//printf("First clock time is: %d\n",array[1]);
	while(bit < (sizeof(array)/sizeof(array[0])) && found == 0){
		if(array[bit] > starting_clock + 556*(page-1)){
			found = 1;
		}
		bit = bit + 2;
	}
	if(page != 1){
		//printf("triggered\n");
		current_voltage = array[bit-3];
	}
	//printf("first array value is: %d\n",array[bit]);
	//printf("clock range is : %d - %d",starting_clock + 556*(page-1),starting_clock + 556*page);
	for(int j = 0; j < 200; j++){
		if(array[bit] >= (starting_clock + 556*(page-1) + j*(556/200)) && (array[bit] <= (starting_clock + 556*page))){
			//printf("Array value: %d is bigger than clock cycle: %d",array[bit], starting_clock + j*(556/200));
			current_voltage = array[bit-1];
			if(bit < (sizeof(array)/sizeof(array[0]))){
				bit = bit + 2;
			}
		}
		if(current_voltage == target_voltage){
			printf("_");
		}else {
			printf(" ");
		}
	}

}

void graph(int* sd_array,int* sc_array,int* so_array,int* si_array,int starting_clock,int page){
	int bit = 1;
	int found = 0;
	int count = 0;
	int current_voltage = 0;
	for(int i = 0; i < 50; i++){
		switch(i){
			case 3:
				printf("   1  |");
				print_voltage(sd_array,1,starting_clock,page);
				break;
			case 6:
				printf("  SD  ");
				break;
			case 9:
				printf("   0  |");
				print_voltage(sd_array,0,starting_clock,page);
				break;
			case 12:
				printf("      |");
				for(int j = 0; j < 100; j++){
					printf("- ");
				}
				break;
			case 15:
				printf("   1  |");
				print_voltage(sc_array,1,starting_clock,page);
				break;
			case 18:
				printf("  SC  ");
				break;
			case 21:
				printf("   0  |");
				print_voltage(sc_array,0,starting_clock,page);
				break;
			case 24:
				printf("      |");
				for(int j = 0; j < 100; j++){
					printf("- ");
				}
				break;
			case 27:
				printf("   1  |");
				print_voltage(so_array,1,starting_clock,page);
				break;
			case 30:
				printf("  SO  ");
				break;
			case 33:
				printf("   0  |");
				print_voltage(so_array,0,starting_clock,page);
				break;
			case 36:
				printf("      |");
				for(int j = 0; j < 100; j++){
					printf("- ");
				}
				break;
			case 39:
				printf("   1  |");
				print_voltage(si_array,1,starting_clock,page);
				break;
			case 42:
				printf("  SI  ");
				break;
			case 45:
				printf("   0  |");
				print_voltage(si_array,0,starting_clock,page);
				break;
			default:
				printf("      ");
		}
		printf("|");
		printf("\n");
	}
	for(int j = 0; j < 100; j++){
		printf("- ");
	}
}


int main() {
	printf("%d\n",CLOCKS_PER_SEC);
	FILE * sd_file = fopen("transfer_data_line1.txt","r");
	FILE * sc_file = fopen("transfer_data_line2.txt","r");
	FILE * so_file = fopen("transfer_data_line3.txt","r");
	FILE * si_file = fopen("transfer_data_line4.txt","r");

	if (NULL == sd_file || NULL == sc_file || NULL == so_file || NULL == si_file){
		printf("Error opening files\n");
		return 0;
	}

	int sd_array[4];
	int sc_array[file_length(sc_file)];
	int so_array[file_length(so_file)];
	int si_array[file_length(si_file)];

	//Resetting file pointers for another read iteration//
	fclose(sd_file);
	fclose(sc_file);
	fclose(so_file);
	fclose(si_file);

	sd_file = fopen("transfer_data_line1.txt","r");
	sc_file = fopen("transfer_data_line2.txt","r");
	so_file = fopen("transfer_data_line3.txt","r");
	si_file = fopen("transfer_data_line4.txt","r");

	if (NULL == sd_file || NULL == sc_file || NULL == so_file || NULL == si_file){
		printf("Error opening files \n");
		return 0;
	}

	initialize_array(sd_array,sd_file);
	initialize_array(sc_array,sc_file);
	initialize_array(so_array,so_file);
	initialize_array(si_array,si_file);

	//finds the earliest clock cycle//
	int starting_clock;
	if (sizeof(sd_array) > 1){
		starting_clock = sd_array[1];
	}
	if (sizeof(sc_array) > 1){
		if(sc_array[1] < starting_clock){
			starting_clock = sc_array[1];
		}
	}
	if (sizeof(so_array) > 1){
		if(so_array[1] < starting_clock){
			starting_clock = so_array[1];
		}
	}
	if (sizeof(si_array) > 1){
		if(si_array[1] < starting_clock){
			starting_clock = si_array[1];
		}
	}

	//prints out the graph for the appropriate time//
	int page = 1;
	graph(sd_array,sc_array,so_array,si_array,starting_clock,page);

	//input cycle
	int input_cycle = 1;
	while(input_cycle == 1){
		char inBuffer[50];
		printf("\n u - next page \n d - previous page \n s - stop programm \n INPUT: ");
		scanf("%s",inBuffer);
		if(strcmp(inBuffer,"u") == 0){
			page++;
			graph(sd_array,sc_array,so_array,si_array,starting_clock,page);
		}
		/*
		if(strcmp(inBuffer,"d" && page != 1) == 0){
			page--;
			graph(sd_array,sc_array,so_array,si_array,starting_clock,page);
		}
		if(strcmp(inBuffer,"s") == 0){
			input_cycle = 0;
		}
		*/
	}
}
