#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <math.h>
#include <unistd.h>
#include <time.h>


int main(){
	FILE* data1 = fopen("transfer_data_line1.txt","a");
	FILE* data2 = fopen("transfer_data_line2.txt","a");
	FILE* data3 = fopen("transfer_data_line3.txt","a");
	FILE* data4 = fopen("transfer_data_line4.txt","a");
	int signal_value[] = {0,0,0,0};
	time_t start_up_time = time(NULL);
	time_t delta_time = 0;
	clock_t start_time;
	start_time = clock();
	int clock_cycle;
	wiringPiSetup();
	printf("%d",CLOCKS_PER_SEC);
	clock_cycle = clock() - start_time;
	while(delta_time < 300){
		clock_cycle = clock() - start_time;
		for(int i = 0;i < 4;i++){
			if (digitalRead(i) != signal_value[i]){
				signal_value[i] = digitalRead(i);
				switch(i) {
					case 0:
						fprintf(data1,"%d\n",signal_value[i]);
						fprintf(data1,"%d\n",clock_cycle);
						break;
					case 1:
						fprintf(data2,"%d\n",signal_value[i]);
						fprintf(data2,"%d\n",clock_cycle);
						break;
					case 2:
						fprintf(data3,"%d\n",signal_value[i]);
						fprintf(data3,"%d\n",clock_cycle);
						break;
					case 3:
						fprintf(data4,"%d\n",signal_value[i]);
						fprintf(data4,"%d\n",clock_cycle);
						break;
				}

			}
		}
		delta_time = time(NULL) - start_up_time;
	}
	fclose(data1);
	fclose(data2);
	fclose(data3);
	fclose(data4);
}



