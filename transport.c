#include <string.h>  
#include <errno.h>  
#include <sys/types.h>   
#include <sys/socket.h>  
#include <netpacket/packet.h>  
#include <net/if.h>  
#include <net/if_arp.h>  
#include <sys/ioctl.h>  
#include <arpa/inet.h> //for htons
#include <netinet/if_ether.h>   //for ethhdr
#include <pthread.h>
#include <fcntl.h>

#include "transport.h"
#define LEN     255
#define ETH_2F  0x22ff
uint8_t buf_send[LEN] = {0}; //Tx buffer
uint8_t buf_receive[LEN] = {0}; //Rx buffer
int Tx_fd = 0;
int Rx_fd = 0;
uint16_t Sequence_ID = 0;

//00:0c:29:94:1c:42
const uint8_t myMacAddress[6] = {0x00, 0x0c, 0x29, 0x94, 0x1c, 0x42};
const uint8_t desMacAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};


#define EM_LOG printf 



void create_Tx_socket()
{
    int        result = 0, n, count = 0;  
    uint8_t    buf[LEN];  
    struct sockaddr_ll      sa, sa_recv;  
    struct ifreq    ifr;  
    socklen_t       sa_len = 0;  
    char    if_name[] = "ens33";  
    struct ethhdr *eth; //定义以太网头结构体指针
  
    //create socket  
    Tx_fd = socket(PF_PACKET, SOCK_RAW, htons(0x0003));  
    if (Tx_fd< 0) {  
        perror("socket error\n");  
        exit(1);  
    }  
    memset(&sa, 0, sizeof(sa));  
    sa.sll_family = PF_PACKET;  
    sa.sll_protocol = htons(ETH_P_ALL);   //8902
    
    strcpy(ifr.ifr_name, if_name);  //必须先得到flags，才能再得到index
    result = ioctl(Tx_fd, SIOCGIFFLAGS, &ifr);  
    if (result != 0) {   
        perror("ioctl error, get flags\n");  
        exit(1);  
    }        
    result = ioctl(Tx_fd, SIOCGIFINDEX, &ifr);        //get index  
    if (result != 0) {  
        perror("ioctl error, get index\n");  
        exit(1);  
    }  
    sa.sll_ifindex = ifr.ifr_ifindex;  
    result = bind(Tx_fd, (struct sockaddr*)&sa, sizeof(struct sockaddr_ll));  //bind fd  
    if (result != 0) {  
        perror("bind error\n");  
        exit(1);  
    }
}

void create_Rx_socket()
{
    int        result = 0, n, count = 0;  
    uint8_t    buf[LEN];  
    struct sockaddr_ll       sa,sa_recv;  
    struct ifreq    ifr;  
    socklen_t       sa_len = 0;  
    char    if_name[] = "ens33";  
    struct ethhdr *eth; //定义以太网头结构体指针
  
    //create socket  
    Rx_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_2F));  
    if (Rx_fd < 0) {  
        perror("socket error\n");  
        exit(1);  
    }  
    memset(&sa, 0, sizeof(sa));  
    sa.sll_family = PF_PACKET;  
    sa.sll_protocol = htons(ETH_2F);   //8902
    
    strcpy(ifr.ifr_name, if_name);  //必须先得到flags，才能再得到index
    result = ioctl(Rx_fd, SIOCGIFFLAGS, &ifr);  
    EM_LOG("step1\n");
    if (result != 0) {   
        perror("ioctl error, get flags\n");  
        exit(1);  
    }        
    EM_LOG("step2\n");
    result = ioctl(Rx_fd, SIOCGIFINDEX, &ifr);        //get index  
    if (result != 0) {  
        perror("ioctl error, get index\n");  
        exit(1);  
    }  
    sa.sll_ifindex = ifr.ifr_ifindex;  
    EM_LOG("step3\n");
    result = bind(Rx_fd, (struct sockaddr*)&sa, sizeof(struct sockaddr_ll));  //bind fd  
    if (result != 0) {  
        perror("bind error\n");  
        exit(1);  
    }
    int flags = fcntl(Rx_fd, F_GETFL, 0 );

    fcntl(Rx_fd, F_SETFL, flags|SOCK_NONBLOCK);
}


void init_eth_can()
{
	create_Tx_socket();
	create_Rx_socket();
}


void Tx_Eth(uint8_t *Tx_data,int length)
{   
    //send loop
	int offset = 0;
    	int size = 6;
    	int ret;
    	//00:0c:29:94:1c:42
	/*dst mac*/
	memcpy(buf_send,desMacAddress,sizeof(desMacAddress));
	offset += sizeof(desMacAddress);
	/*src mac*/
	memcpy(buf_send+offset,myMacAddress,sizeof(myMacAddress));
	offset += sizeof(myMacAddress);
	/*packet protocl*/	
	buf_send[offset] = 0x08;
	offset += 1;
	buf_send[offset] = 0x06;
	offset += 1;
	buf_send[offset] = 0x8F;
	offset += 1;
	buf_send[offset] = 0x8F;
	offset += 1;
	
	
	memcpy(buf_send+offset,Tx_data,length);
	offset+=length;
	


	/*cycle send L2 packet*/

	ret = send(Tx_fd, buf_send, offset, 0);
	if (ret <= 0)
	{
		perror("send");
	}

}

void Rx_Eth(uint8_t *Rx_data,int length)
{
        int n;  
        uint16_t ID_num;
        n = recvfrom(Rx_fd, buf_receive, sizeof(buf_receive), 0, NULL, NULL);
        /*  
        */ 
        printf("NUM= %d\n",n);
        if (n >= 13 && n <LEN)
	{		//EM_LOG("%d\n",buf_receive[12]);
			EM_LOG("######################################\n");
		        if(buf_receive[12]==0x22 && buf_receive[13]==0xff)
        		{
				//EM_LOG("######################################\n");
				EM_LOG("%d",buf_receive[16]);
        		}
	}
}

void *TX_thread(void* args)
{
	
	while(1)
	{
		Tx_Eth(&Sequence_ID,2);
		//EM_LOG("###########\n");
		usleep(1000*20);
		Sequence_ID++;
	}
	
}

int main()
{
	pthread_t t1;
	init_eth_can();
	pthread_create(&t1,NULL,TX_thread,NULL);
	uint8_t buf_t[200] = {0};
	while(1)
	{
		Rx_Eth(buf_t,100);
	}
}


