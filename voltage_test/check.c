#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define VIR_SD 1
#define VIR_SI 0
#define VIR_SC 2
#define VIR_SO 3

int main(){
	wiringPiSetup();
	pinMode(VIR_SD,INPUT);
	pinMode(VIR_SI,INPUT);
	pinMode(VIR_SC,INPUT);
	pinMode(VIR_SO,INPUT);

	int duck = 0;
	while(duck == 0){
		if (digitalRead(VIR_SD) == 1){
			printf("VIR_SD triggered");
		}
		if (digitalRead(VIR_SI) == 1){
			printf("VIR_SI triggered");
		}
		if (digitalRead(VIR_SC) == 1){
			printf("VIR_SC triggered");
		}
		if (digitalRead(VIR_SO) == 1){
			printf("VIR_SO triggered");
		}
	}
	printf("HELLO");
}

