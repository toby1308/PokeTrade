#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

int start_up();
u_int16_t process_master_input(u_int16_t master_data,u_int16_t slave_data);
u_int16_t process_slave1_input(u_int16_t slave1_data);
u_int16_t process_slave2_input(u_int16_t slave2_data,u_int16_t slave1_data);
u_int16_t process_slave3_input(u_int16_t slave3_data,u_int16_t slave1_data);
void end();
void longest_input_process();

#endif
