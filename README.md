# Eth_Tx_Rx-linux-
链路层以太网收发，发（广播），收（22ff协议）。（用于测试传输can数据）
Version1.0 2021/4/12

/*Tx socket 初始化*/
void create_Tx_socket()
/*Rx socket 初始化*/
void create_Rx_socket()
/*Tx 函数*/
void Tx_Eth(uint8_t *Tx_data,int length)
/*Rx 函数*/
void Rx_Eth(uint8_t *Rx_data,int length)
