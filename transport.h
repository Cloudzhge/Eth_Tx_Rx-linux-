#include <stdint.h>
#include <stdio.h>  
#include <stdlib.h>	
#include <unistd.h>


void init_eth_can();
void Tx_Eth(uint8_t *Tx_data,int length);
void Rx_Eth(uint8_t *Rx_data,int length);


/*seconds: the seconds; mseconds: the micro seconds*/

