#include <iostream>
#include <wiringPi.h>

using namespace std;

unit16_t listen(){
    /*
    Records the data along the SD line by checking the voltage at timed intervals according to a Baut rate of 115200
    
    ARGs:
        None

    Returns:   
        uint16_t: A 16-bit value to be stored in the appropriate virtual SIOMULTI register

    */
    pinMode(2,INPUT);//Sets GIPO2 Pin to input
    bool endbit = false;
    bool startbit = false;
    int bit_array[16];
    int bit_rate = 8680; //Nano seconds
    uint16_t data = 0;
    while(startbit == false){//checks for the starting bit
        if (digitalRead(2) == 0){
            startbit = true;
            delay(bit_rate);
        }
    }
    for (int i = 0; i < 16; i++){//Reads SD at intervals and converts from binary to decimal
        bit_array[15 - i] = digitalRead(2);
        data = data + digitalRead(2) * pow(2,i);
        delay(bit_rate);
    }
    while(endbit == false){//checks for ending bit
        if (digitalRead(2) == 1){
            endbit = true;
            delay(bit_rate);
        }
    }
    return data;
}

void write(uint16 data){
    /*
    Writes data from SIOMULTI1 register to SD at timed intervals set by the Baut rate (115200)

    ARGs:
        data(uint16_t): data from SIOMULTI1 that is to be written to SD

    Returns:
        NULL
    */
    pinMode(2,OUTPUT); //Sets GPIO2 pin to output
    int bit_array[16];
    int bit_rate = 8680;
    int High = 1;
    int Low = 0;
    for (int i = 0; i < 16; i++){ //converts input from decimal to binary
        if (data >= pow(2,15 - i)) {
            data -= pow(2,15 - i);
            bit_array[i] == 1;
        } else {
            bit_array[i] = 0;
        }
    }
    digitalWrite(2,Low); //Starting bit
    delay(bit_rate);
    for (int i = 0; i < 16; i++){//Outputs the data at correct intervals
        digitalWrite(2,bit_array[15 - i]);
        delay(bit_rate);
    }
    digitalWrite(2,High); //Ending bit
    delay(bit_rate);
}

void main(){
    /*
    Handels 16 multiplayer communication over AGB cables by acting as Slave 1
    Setup required:

         MASTER                                   Ras_PI                          Slave2
    |--------------|                         |--------------|               |--------------|
    |      SD      | ----------------------> |     GPIO2    | ------------> |      SD      |
    |--------------|                         |--------------|               |--------------|
    |      SI      | --------> GND   |-----> |     GPIO3    |        |----> |      SI      |
    |--------------|                 |       |--------------|        |      |--------------|
    |      SO      |-----------------|       |     GPIO4    | -------|      |      SO      |
    |--------------|                         |--------------|               |--------------|
    |      SC      |---------------------->  |     GPIO5    | ------------> |      SC      |
    |--------------|                         |--------------|               |--------------|
    
    The full protocol can be seen here: https://cdn.preterhuman.net/texts/gaming_and_diversion/Gameboy%20Advance%20Programming%20Manual%20v1.1.pdf
    on page 134

    All data is written and read onto the SD chain, where the master and slave take turns writting. Knowing wether it's their turn to write if:
        - SC: is low during data transfere
        - SI: is low when they're allowed to write
        - SO: is set to high before they write then lowered after 

    */

   //Macros
    int High = 1;
    int Low = 0;
    int vir_SD;
    int vir_SI;
    int vir_SC;
    int vir_SO;

    //Initalises GPIO pins for their inital functions
    wiringPiSetup();
    pinMode(vir_SD,INPUT);
    pinMode(vir_SI,INPUT);
    pinMode(vir_SO,OUTPUT);
    pinMode(vir_SC,INPUT);

    //Virtual registers, used to simulate how a real GBA would store comunication data
    uint16_t SIOMULTI0 = 0xFFFF;
    uint16_t SIOMULTI1 = 0xFFFF;
    uint16_t SIOMULTI2 = 0xFFFF;
    uint16_t SIOMULTI3 = 0xFFFF;

    //Variables
    int written_data = Low;
    int M_clock;
    int M_SO;
    int delta;
    int cycle = 0;

    while(1){
        M_clock = digitalRead(vir_SC);
        switch(M_clock){
            //If vir_SC is low then a communication block has started
            case Low:
                //Checks SI to see if master is sending data or not
                M_SO = digitalRead(vir_SI);
                if (M_SO == High){//Master sending data
                    SIOMULTI0 = listen();
                    SIOMULTI1 = process_master_input(SIOMULTI0,SIOMULTI1);
                    cycle++;
                }
                if (written_data == High && M_SO == Low){//Master and Pi has sent data
                    //Listen for Slave 2 & 3
                    switch(cycle){
                        case 2:
                            uint16_t SIOMULTI2 = listen();
                            SIOMULTI1 = process_slave2_input(SIOMULTI2,SIOMULTI1);
                            break;
                        case 3:
                            uint16_t SIOMULTI3 = listen();
                            SIOMULTI1 = process_slave3_input(SIOMULTI3,SIOMULTI1);
                            break;
                        default:
                            break;
                    }
                }
                if (written_data == Low && M_SO = Low){//Only master has sent data
                    write(SIOMULTI1);
                    written_data = High;
                    cycle++;
                    digitalWrite(vir_SO,Low);
                }
                break;
            default:
                //Resets variables for next communication block
                written_data = Low;
                cycle = 0;
                ditigalWrite(vir_SO,High);
                break;

        }
    }
}
