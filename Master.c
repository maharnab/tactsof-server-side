#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "gpiolib.h"
#include "ledlib.h"

#define ADC_NODE "/dev/adc"
#define ARRAY_SIZE(array)	sizeof(array)/sizeof(array[0])
#define BUF_SIZE		32768
#define ON			1
#define OFF2 			0
#define SERVER_PORT		2234

void enc_func_10();
void enc_func_25();
void enc_func_50();
void enc_func_100();

void off_down_func_1();
void off_down_func_3();
void off_down_func_6();

void off_up_func_1();
void off_up_func_3();
void off_up_func_6();

void reset_func();

void vcm_main_func_25();
void vcm_main_func_50();
void vcm_main_func_75();
void vcm_main_func_100();

void vcm_work_func_25(void);
void vcm_work_func_50(void);
void vcm_work_func_75(void);
void vcm_work_func_100(void);

void vcm_delay_func_25(void);
void vcm_delay_func_50(void);
void vcm_delay_func_75(void);
void vcm_delay_func_100(void);

char puf[100], quf[100];
int sa, delay_buffer_10sec = 0, tcounter=0;		/* shared resources */
pthread_mutex_t mutex; 					/* mutex */

int main(void)
{
	int input, i, s, b, l, fd, bytes, on=1;
	char buf[BUF_SIZE], fname[255];
	struct sockaddr_in channel;
	for(i = 0; i < 100; i++)
	{
		s=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if(s<0)
		{
			printf("socket creation failure");
			exit(0);
		}
		setsockopt(s,SOL_SOCKET,SO_REUSEADDR,(char *)&on,sizeof(on));
		memset(&channel,0,sizeof(channel));
		channel.sin_family=AF_INET;
		channel.sin_addr.s_addr=htonl(INADDR_ANY);
		channel.sin_port=htons(SERVER_PORT);	
		b=bind(s, (struct sockaddr *)&channel,sizeof(channel));
		if(b<0)
		{
			printf("bind error");
			exit(0);
		}	
		listen(s,5);
		while(1)
		{
			printf("\n	Waiting for choice...\n");
			printf("\n		Select Option	\n");
			printf("	1.	Encoder test\n");
			printf("		101.	10 seconds\n");
			printf("		102.	25 seconds\n");
			printf("		103.	50 seconds\n");
			printf("		104.	100 seconds\n");
			printf("	2.	Move Off Loader up by\n");
			printf("		115.	6 mm\n");
			printf("		116.	3 mm\n");
			printf("		117.	1 mm\n");
			printf("	3.	Move Off Loader down by\n");
			printf("		98.	6 mm\n");
			printf("		99.	3 mm\n");
			printf("		100.	1 mm\n");
			printf("	4.	Reset Drive board\n");
			printf("	5.	VCM test\n");
			printf("		118.	20 seconds\n");
			printf("		119.	40 seconds\n");
			printf("		120.	60 seconds\n");
			printf("		121.	100 seconds\n");
			printf("	6.	Exit\n");

			sa=accept(s,0,0);
			if(sa<0)
			{
				printf("accept failure");
				exit(0);
			}
			memset(&input, 0, sizeof(input));
			read(sa, &input, BUF_SIZE);
			printf("\n	Requested choice is %d\n",input);		
			switch( input )
			{
				case 101:
					enc_func_10();
					continue;
				case 102:
					enc_func_25();
					continue;
				case 103:
					enc_func_50();
					continue;
				case 104:
					enc_func_100();
					continue;

				case 100:
					off_down_func_1();
					continue;
				case 99:
					off_down_func_3();
					continue;
				case 98:
					off_down_func_6();
					continue;

				case 117:
					off_up_func_1();
					continue;
				case 116:
					off_up_func_3();
					continue;
				case 115:
					off_up_func_6();
					continue;

				case 114:
					reset_func();
					continue;
		
				case 118:
					vcm_main_func_25();
					continue;
				case 119:
					vcm_main_func_50();
					continue;
				case 120:
					vcm_main_func_75();
					continue;
				case 121:
					vcm_main_func_100();
					continue;

				case 6:
					printf("\n	Exiting now...\n");
					goto exit_loop;
				default:
					printf("\n	Wrong choice. Try again.\n");
					continue;
			}
			getc(stdin);
		}
		exit_loop: ;	
	}
	system("pause");
	return 0;
}

// Run encoder test for 10 seconds
void enc_func_10()
{
	printf("\n	Encoder test underway...\n");
	
	init_gpio();
	set_gpio_output ("Pa22", 0);
     	usleep (100);
 	set_gpio_output ("Pa22", 1);

	long long sum=0;
	double average=0;
	int i, j, srno=0;
    	unsigned int value = 0;

	for (i=1; i<=56; i++)				//for 10 sec >> 56
	{	
 		for(j=1;j<=20;j++)
 		{
    			set_gpio_output ("Pa24", 0); 	/* OE/ Output Enable on */
			set_gpio_output ("Pa26", 1);	// S1
			set_gpio_output ("Pa27", 0);	// s2
   			usleep (100);

			//$ Encoder Reading
			// Reading Port pins

			int s0,s1,s2,s3,s4,s5,s6,s7; 	// lsb to msb

			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");

			long int portValue1= (s0*1)+(s1*2)+(s2*4)+(s3*8)+(s4*16)+(s5*32)+(s6*64)+(s7*128);
			set_gpio_output ("Pa26", 0);
			set_gpio_output ("Pa27", 0);
   			usleep (100);

			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");

			long int portValue2= (s0*256)+(s1*512)+(s2*1024)+(s3*2048)+(s4*4096)+(s5*8192)+(s6*16384)+(s7*32768);
			set_gpio_output ("Pa26", 1);
			set_gpio_output ("Pa27", 1);
   			usleep (100);

			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");

			long int portValue3= (s0*65536)+(s1*131072)+(s2*262144)+(s3*524288)+(s4*1048576)+(s5*2097152)+(s6*4194304)+(s7*8388608);			
			set_gpio_output ("Pa26", 0);
			set_gpio_output ("Pa27", 1);
			usleep (100);

			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");

			long int portValue4= (s0*16777216)+(s1*33554432)+(s2*67108864)+(s3*134217728)+(s4*268435456)+(s5*536870912)+(s6*1073741824)+(s7*2147483647);
			long int portValue= (portValue1)+(portValue2)+(portValue3)+(portValue4);
			if(portValue>10000000)
			{
 				portValue= 16777216-portValue;
			}
			sum+=portValue;
			set_gpio_output ("Pa24", 1);	/* OE/ output Enable OFF*/
 			usleep (100);  
  		}

		average = (sum/20)*1.22;

		snprintf(puf, 100, "%g", average);
		send(sa, &puf, sizeof(puf), 0);

		printf("	Avg_20_Encoder_Counts= %lf nm\n", average);
		srno++;	
		sum=0;
		average=0;		
	}
	remove_gpio();

	snprintf(puf, 100, "end");
	send(sa, &puf, sizeof(puf), 0);
	close(sa);
}

// Run encoder test for 25 seconds
void enc_func_25()
{
	printf("\n	Encoder test underway...\n");
	
	init_gpio();
	set_gpio_output ("Pa22", 0);
     	usleep (100);
 	set_gpio_output ("Pa22", 1);

	long long sum=0;
	double average=0;
	int i, j, srno=0;
    	unsigned int value = 0;

	for (i=1; i<=140; i++)				//for 10 sec >> 56
	{	
 		for(j=1;j<=20;j++)
 		{
    			set_gpio_output ("Pa24", 0); 	/* OE/ Output Enable on */
			set_gpio_output ("Pa26", 1);	// S1
			set_gpio_output ("Pa27", 0);	// s2
   			usleep (100);

			//$ Encoder Reading
			// Reading Port pins

			int s0,s1,s2,s3,s4,s5,s6,s7; 	// lsb to msb

			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");

			long int portValue1= (s0*1)+(s1*2)+(s2*4)+(s3*8)+(s4*16)+(s5*32)+(s6*64)+(s7*128);
			set_gpio_output ("Pa26", 0);
			set_gpio_output ("Pa27", 0);
   			usleep (100);

			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");

			long int portValue2= (s0*256)+(s1*512)+(s2*1024)+(s3*2048)+(s4*4096)+(s5*8192)+(s6*16384)+(s7*32768);
			set_gpio_output ("Pa26", 1);
			set_gpio_output ("Pa27", 1);
   			usleep (100);

			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");

			long int portValue3= (s0*65536)+(s1*131072)+(s2*262144)+(s3*524288)+(s4*1048576)+(s5*2097152)+(s6*4194304)+(s7*8388608);			
			set_gpio_output ("Pa26", 0);
			set_gpio_output ("Pa27", 1);
			usleep (100);

			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");

			long int portValue4= (s0*16777216)+(s1*33554432)+(s2*67108864)+(s3*134217728)+(s4*268435456)+(s5*536870912)+(s6*1073741824)+(s7*2147483647);
			long int portValue= (portValue1)+(portValue2)+(portValue3)+(portValue4);
			if(portValue>10000000)
			{
 				portValue= 16777216-portValue;
			}
			sum+=portValue;
			set_gpio_output ("Pa24", 1);	/* OE/ output Enable OFF*/
 			usleep (100);  
  		}

		average = (sum/20)*1.22;

		snprintf(puf, 100, "%g", average);
		send(sa, &puf, sizeof(puf), 0);

		printf("	Avg_20_Encoder_Counts= %lf nm\n", average);
		srno++;	
		sum=0;
		average=0;
		
	}
	remove_gpio();
}

// Run encoder test for 50 seconds
void enc_func_50()
{
	printf("\n	Encoder test underway...\n");
	
	init_gpio();
	set_gpio_output ("Pa22", 0);
     	usleep (100);
 	set_gpio_output ("Pa22", 1);

	long long sum=0;
	double average=0;
	int i, j, srno=0;
    	unsigned int value = 0;

	for (i=1; i<=280; i++)				//for 10 sec >> 56
	{	
 		for(j=1;j<=20;j++)
 		{
    			set_gpio_output ("Pa24", 0); 	/* OE/ Output Enable on */
			set_gpio_output ("Pa26", 1);	// S1
			set_gpio_output ("Pa27", 0);	// s2
   			usleep (100);

			//$ Encoder Reading
			// Reading Port pins

			int s0,s1,s2,s3,s4,s5,s6,s7; 	// lsb to msb

			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");

			long int portValue1= (s0*1)+(s1*2)+(s2*4)+(s3*8)+(s4*16)+(s5*32)+(s6*64)+(s7*128);
			set_gpio_output ("Pa26", 0);
			set_gpio_output ("Pa27", 0);
   			usleep (100);

			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");

			long int portValue2= (s0*256)+(s1*512)+(s2*1024)+(s3*2048)+(s4*4096)+(s5*8192)+(s6*16384)+(s7*32768);
			set_gpio_output ("Pa26", 1);
			set_gpio_output ("Pa27", 1);
   			usleep (100);

			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");

			long int portValue3= (s0*65536)+(s1*131072)+(s2*262144)+(s3*524288)+(s4*1048576)+(s5*2097152)+(s6*4194304)+(s7*8388608);			
			set_gpio_output ("Pa26", 0);
			set_gpio_output ("Pa27", 1);
			usleep (100);

			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");

			long int portValue4= (s0*16777216)+(s1*33554432)+(s2*67108864)+(s3*134217728)+(s4*268435456)+(s5*536870912)+(s6*1073741824)+(s7*2147483647);
			long int portValue= (portValue1)+(portValue2)+(portValue3)+(portValue4);
			if(portValue>10000000)
			{
 				portValue= 16777216-portValue;
			}
			sum+=portValue;
			set_gpio_output ("Pa24", 1);	/* OE/ output Enable OFF*/
 			usleep (100);  
  		}

		average = (sum/20)*1.22;

		snprintf(puf, 100, "%g", average);
		send(sa, &puf, sizeof(puf), 0);

		printf("	Avg_20_Encoder_Counts= %lf nm\n", average);
		srno++;	
		sum=0;
		average=0;
	}
	remove_gpio();
}

// Run encoder test for 100 seconds
void enc_func_100()
{
	printf("\n	Encoder test underway...\n");
	
	init_gpio();
	set_gpio_output ("Pa22", 0);
     	usleep (100);
 	set_gpio_output ("Pa22", 1);

	long long sum=0;
	double average=0;
	int i, j, srno=0;
    	unsigned int value = 0;

	for (i=1; i<=560; i++)				//for 10 sec >> 56
	{	
 		for(j=1;j<=20;j++)
 		{
    			set_gpio_output ("Pa24", 0); 	/* OE/ Output Enable on */
			set_gpio_output ("Pa26", 1);	// S1
			set_gpio_output ("Pa27", 0);	// s2
   			usleep (100);

			//$ Encoder Reading
			// Reading Port pins

			int s0,s1,s2,s3,s4,s5,s6,s7; 	// lsb to msb

			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");

			long int portValue1= (s0*1)+(s1*2)+(s2*4)+(s3*8)+(s4*16)+(s5*32)+(s6*64)+(s7*128);
			set_gpio_output ("Pa26", 0);
			set_gpio_output ("Pa27", 0);
   			usleep (100);

			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");

			long int portValue2= (s0*256)+(s1*512)+(s2*1024)+(s3*2048)+(s4*4096)+(s5*8192)+(s6*16384)+(s7*32768);
			set_gpio_output ("Pa26", 1);
			set_gpio_output ("Pa27", 1);
   			usleep (100);

			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");

			long int portValue3= (s0*65536)+(s1*131072)+(s2*262144)+(s3*524288)+(s4*1048576)+(s5*2097152)+(s6*4194304)+(s7*8388608);			
			set_gpio_output ("Pa26", 0);
			set_gpio_output ("Pa27", 1);
			usleep (100);

			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");

			long int portValue4= (s0*16777216)+(s1*33554432)+(s2*67108864)+(s3*134217728)+(s4*268435456)+(s5*536870912)+(s6*1073741824)+(s7*2147483647);
			long int portValue= (portValue1)+(portValue2)+(portValue3)+(portValue4);
			if(portValue>10000000)
			{
 				portValue= 16777216-portValue;
			}
			sum+=portValue;
			set_gpio_output ("Pa24", 1);	/* OE/ output Enable OFF*/
 			usleep (100);  
  		}

		average = (sum/20)*1.22;

		snprintf(puf, 100, "%g", average);
		send(sa, &puf, sizeof(puf), 0);

		printf("	Avg_20_Encoder_Counts= %lf nm\n", average);
		srno++;	
		sum=0;
		average=0;		
	}
	remove_gpio();
}

// Move off loader down by 1 mm
void off_down_func_1()
{
	printf("\n	Off Loader is going down...\n");

	unsigned long int portValue1,portValue2,portValue3,portValue4,portValue;
    	unsigned int s0,s1,s2,s3,s4,s5,s6,s7; // lsb to msb
	long long sum=0,average=0;
	int fd, rd, i, j, k;
        struct input_event ev[64];

	init_gpio();
	set_gpio_output ("pa29", 1);		//Enable
	set_gpio_output ("pa30", 0);		//Step
	set_gpio_output ("pa31", 0);		//Direction
	usleep (2000);
	led_ctrl ("D9", OFF2);
	led_ctrl ("D6", OFF2);
        set_gpio_output ("Pa22", 0);
	usleep (100);
	set_gpio_output ("Pa22", 1);
	set_gpio_output ("pa24", 1);	      
	led_ctrl ("D9", ON);
	set_gpio_output ("pa29", 0);		//Enable
	set_gpio_output ("pa31", 0);		//Direction=0	
	usleep(2);
	for(i=1;i<=109;i++)		//400 gives approx. 510000 counts i.e.,3.665mm @ VCM;; For 3mm @ VCM, we use 328 that should give 417490 counts 
	{
		set_gpio_output ("pa30", 1);		//
		usleep (1);
		set_gpio_output ("pa30", 0);		//
		usleep (1);
	}
	sleep(1);		
	for(j=1;j<=20;j++)
        {
		set_gpio_output ("Pa24", 0); // OE/ Output Enable on 			
		set_gpio_output ("Pa26", 1);	// S1   // Byte 1
		set_gpio_output ("Pa27", 0);	// s2
		usleep(100);
		
		s0=get_gpio_input ("pb29");
		s1=get_gpio_input ("pb30");
		s2=get_gpio_input ("pb23");
		s3=get_gpio_input ("pb24");
		s4=get_gpio_input ("pd12");
		s5=get_gpio_input ("pd13");
		s6=get_gpio_input ("pd14");
		s7=get_gpio_input ("pd28");
		usleep(100);
		portValue1= (s0*1)+(s1*2)+(s2*4)+(s3*8)+(s4*16)+(s5*32)+(s6*64)+(s7*128);
		
		set_gpio_output ("Pa26", 0);    // Byte 2
		set_gpio_output ("Pa27", 0);
		usleep(100);
		
		s0=get_gpio_input ("pb29");
		s1=get_gpio_input ("pb30");
		s2=get_gpio_input ("pb23");
		s3=get_gpio_input ("pb24");
		s4=get_gpio_input ("pd12");
		s5=get_gpio_input ("pd13");
		s6=get_gpio_input ("pd14");
		s7=get_gpio_input ("pd28");
   		usleep(100);
		portValue2= (s0*256)+(s1*512)+(s2*1024)+(s3*2048)+(s4*4096)+(s5*8192)+(s6*16384)+(s7*32768);	
		
		set_gpio_output ("Pa26", 1);      // Byte 3
		set_gpio_output ("Pa27", 1);
		usleep(100);

		s0=get_gpio_input ("pb29");
		s1=get_gpio_input ("pb30");
		s2=get_gpio_input ("pb23");
		s3=get_gpio_input ("pb24");
		s4=get_gpio_input ("pd12");
		s5=get_gpio_input ("pd13");
		s6=get_gpio_input ("pd14");
		s7=get_gpio_input ("pd28");
		usleep(100);
		portValue3= (s0*65536)+(s1*131072)+(s2*262144)+(s3*524288)+(s4*1048576)+(s5*2097152)+(s6*4194304)+(s7*8388608);
			
		set_gpio_output ("Pa26", 0);       // Byte 4
		set_gpio_output ("Pa27", 1);
		usleep(100);

		s0=get_gpio_input ("pb29");
		s1=get_gpio_input ("pb30");
		s2=get_gpio_input ("pb23");
		s3=get_gpio_input ("pb24");
		s4=get_gpio_input ("pd12");
		s5=get_gpio_input ("pd13");
		s6=get_gpio_input ("pd14");
		s7=get_gpio_input ("pd28");
 		usleep(100);

		portValue4= (s0*16777216)+(s1*33554432)+(s2*67108864)+(s3*134217728)+(s4*268435456)+(s5*536870912)+(s6*1073741824)+(s7*2147483647);			
		portValue= (portValue1)+(portValue2)+(portValue3);

		if(portValue>10000000)
		{
			portValue= 16777216-portValue;
		}
		sum+=portValue;
		set_gpio_output ("Pa24", 1);   // OE/ output Enable OFF	
		usleep (100);
	}

	average = (sum/20);

	snprintf(puf, 100, "%g", average*1.22);
	send(sa, &puf, sizeof(puf), 0);

	printf("	Avg_20_Encoder_Counts= %lf nm\n", average*1.22);	
	sum=0;
	average=0;

    	remove_gpio();
 	led_ctrl ("D9", OFF2);
	led_ctrl ("D6", OFF2);
}

// Move off loader down by 3 mm
void off_down_func_3()
{
	printf("\n	Off Loader is going down...\n");

	unsigned long int portValue1,portValue2,portValue3,portValue4,portValue;
    	unsigned int s0,s1,s2,s3,s4,s5,s6,s7; // lsb to msb
	long long sum=0,average=0;
	int fd, rd, i, j, k;
        struct input_event ev[64];

	init_gpio();
	set_gpio_output ("pa29", 1);		//Enable
	set_gpio_output ("pa30", 0);		//Step
	set_gpio_output ("pa31", 0);		//Direction
	usleep (2000);
	led_ctrl ("D9", OFF2);
	led_ctrl ("D6", OFF2);
        set_gpio_output ("Pa22", 0);
	usleep (100);
	set_gpio_output ("Pa22", 1);
	set_gpio_output ("pa24", 1);	      
	led_ctrl ("D9", ON);
	set_gpio_output ("pa29", 0);		//Enable
	set_gpio_output ("pa31", 0);		//Direction=0	
	usleep(2);
	for(i=1;i<=328;i++)		//400 gives approx. 510000 counts i.e.,3.665mm @ VCM;; For 3mm @ VCM, we use 328 that should give 417490 counts 
	{
		set_gpio_output ("pa30", 1);		//
		usleep (1);
		set_gpio_output ("pa30", 0);		//
		usleep (1);
	}
	sleep(1);		
	for(j=1;j<=20;j++)
        {
		set_gpio_output ("Pa24", 0); // OE/ Output Enable on 			
		set_gpio_output ("Pa26", 1);	// S1   // Byte 1
		set_gpio_output ("Pa27", 0);	// s2
		usleep(100);
		
		s0=get_gpio_input ("pb29");
		s1=get_gpio_input ("pb30");
		s2=get_gpio_input ("pb23");
		s3=get_gpio_input ("pb24");
		s4=get_gpio_input ("pd12");
		s5=get_gpio_input ("pd13");
		s6=get_gpio_input ("pd14");
		s7=get_gpio_input ("pd28");
		usleep(100);
		portValue1= (s0*1)+(s1*2)+(s2*4)+(s3*8)+(s4*16)+(s5*32)+(s6*64)+(s7*128);
		
		set_gpio_output ("Pa26", 0);    // Byte 2
		set_gpio_output ("Pa27", 0);
		usleep(100);
		
		s0=get_gpio_input ("pb29");
		s1=get_gpio_input ("pb30");
		s2=get_gpio_input ("pb23");
		s3=get_gpio_input ("pb24");
		s4=get_gpio_input ("pd12");
		s5=get_gpio_input ("pd13");
		s6=get_gpio_input ("pd14");
		s7=get_gpio_input ("pd28");
   		usleep(100);
		portValue2= (s0*256)+(s1*512)+(s2*1024)+(s3*2048)+(s4*4096)+(s5*8192)+(s6*16384)+(s7*32768);	
		
		set_gpio_output ("Pa26", 1);      // Byte 3
		set_gpio_output ("Pa27", 1);
		usleep(100);

		s0=get_gpio_input ("pb29");
		s1=get_gpio_input ("pb30");
		s2=get_gpio_input ("pb23");
		s3=get_gpio_input ("pb24");
		s4=get_gpio_input ("pd12");
		s5=get_gpio_input ("pd13");
		s6=get_gpio_input ("pd14");
		s7=get_gpio_input ("pd28");
		usleep(100);
		portValue3= (s0*65536)+(s1*131072)+(s2*262144)+(s3*524288)+(s4*1048576)+(s5*2097152)+(s6*4194304)+(s7*8388608);
			
		set_gpio_output ("Pa26", 0);       // Byte 4
		set_gpio_output ("Pa27", 1);
		usleep(100);

		s0=get_gpio_input ("pb29");
		s1=get_gpio_input ("pb30");
		s2=get_gpio_input ("pb23");
		s3=get_gpio_input ("pb24");
		s4=get_gpio_input ("pd12");
		s5=get_gpio_input ("pd13");
		s6=get_gpio_input ("pd14");
		s7=get_gpio_input ("pd28");
 		usleep(100);

		portValue4= (s0*16777216)+(s1*33554432)+(s2*67108864)+(s3*134217728)+(s4*268435456)+(s5*536870912)+(s6*1073741824)+(s7*2147483647);	
		
		portValue= (portValue1)+(portValue2)+(portValue3);

		if(portValue>10000000)
		{
			portValue= 16777216-portValue;
		}
		sum+=portValue;
		set_gpio_output ("Pa24", 1);   // OE/ output Enable OFF	
		usleep (100);
	}

	average = (sum/20);

	snprintf(puf, 100, "%g", average*1.22);
	send(sa, &puf, sizeof(puf), 0);

	printf("	Avg_20_Encoder_Counts= %lf nm\n", average*1.22);	
	sum=0;
	average=0;

    	remove_gpio();
 	led_ctrl ("D9", OFF2);
	led_ctrl ("D6", OFF2);
}

// Move off loader down by 6 mm
void off_down_func_6()
{
	printf("\n	Off Loader is going down...\n");

	unsigned long int portValue1,portValue2,portValue3,portValue4,portValue;
    	unsigned int s0,s1,s2,s3,s4,s5,s6,s7; // lsb to msb
	long long sum=0,average=0;
	int fd, rd, i, j, k;
        struct input_event ev[64];

	init_gpio();
	set_gpio_output ("pa29", 1);		//Enable
	set_gpio_output ("pa30", 0);		//Step
	set_gpio_output ("pa31", 0);		//Direction
	usleep (2000);
	led_ctrl ("D9", OFF2);
	led_ctrl ("D6", OFF2);
        set_gpio_output ("Pa22", 0);
	usleep (100);
	set_gpio_output ("Pa22", 1);
	set_gpio_output ("pa24", 1);	      
	led_ctrl ("D9", ON);
	set_gpio_output ("pa29", 0);		//Enable
	set_gpio_output ("pa31", 0);		//Direction=0	
	usleep(2);
	for(i=1;i<=656;i++)		//400 gives approx. 510000 counts i.e.,3.665mm @ VCM;; For 3mm @ VCM, we use 328 that should give 417490 counts 
	{
		set_gpio_output ("pa30", 1);		//
		usleep (1);
		set_gpio_output ("pa30", 0);		//
		usleep (1);
	}
	sleep(1);		
	for(j=1;j<=20;j++)
        {
		set_gpio_output ("Pa24", 0); // OE/ Output Enable on 			
		set_gpio_output ("Pa26", 1);	// S1   // Byte 1
		set_gpio_output ("Pa27", 0);	// s2
		usleep(100);
		
		s0=get_gpio_input ("pb29");
		s1=get_gpio_input ("pb30");
		s2=get_gpio_input ("pb23");
		s3=get_gpio_input ("pb24");
		s4=get_gpio_input ("pd12");
		s5=get_gpio_input ("pd13");
		s6=get_gpio_input ("pd14");
		s7=get_gpio_input ("pd28");
		usleep(100);
		portValue1= (s0*1)+(s1*2)+(s2*4)+(s3*8)+(s4*16)+(s5*32)+(s6*64)+(s7*128);
		
		set_gpio_output ("Pa26", 0);    // Byte 2
		set_gpio_output ("Pa27", 0);
		usleep(100);
		
		s0=get_gpio_input ("pb29");
		s1=get_gpio_input ("pb30");
		s2=get_gpio_input ("pb23");
		s3=get_gpio_input ("pb24");
		s4=get_gpio_input ("pd12");
		s5=get_gpio_input ("pd13");
		s6=get_gpio_input ("pd14");
		s7=get_gpio_input ("pd28");
   		usleep(100);
		portValue2= (s0*256)+(s1*512)+(s2*1024)+(s3*2048)+(s4*4096)+(s5*8192)+(s6*16384)+(s7*32768);	
		
		set_gpio_output ("Pa26", 1);      // Byte 3
		set_gpio_output ("Pa27", 1);
		usleep(100);

		s0=get_gpio_input ("pb29");
		s1=get_gpio_input ("pb30");
		s2=get_gpio_input ("pb23");
		s3=get_gpio_input ("pb24");
		s4=get_gpio_input ("pd12");
		s5=get_gpio_input ("pd13");
		s6=get_gpio_input ("pd14");
		s7=get_gpio_input ("pd28");
		usleep(100);
		portValue3= (s0*65536)+(s1*131072)+(s2*262144)+(s3*524288)+(s4*1048576)+(s5*2097152)+(s6*4194304)+(s7*8388608);
			
		set_gpio_output ("Pa26", 0);       // Byte 4
		set_gpio_output ("Pa27", 1);
		usleep(100);

		s0=get_gpio_input ("pb29");
		s1=get_gpio_input ("pb30");
		s2=get_gpio_input ("pb23");
		s3=get_gpio_input ("pb24");
		s4=get_gpio_input ("pd12");
		s5=get_gpio_input ("pd13");
		s6=get_gpio_input ("pd14");
		s7=get_gpio_input ("pd28");
 		usleep(100);

		portValue4= (s0*16777216)+(s1*33554432)+(s2*67108864)+(s3*134217728)+(s4*268435456)+(s5*536870912)+(s6*1073741824)+(s7*2147483647);			
		portValue= (portValue1)+(portValue2)+(portValue3);

		if(portValue>10000000)
		{
			portValue= 16777216-portValue;
		}
		sum+=portValue;
		set_gpio_output ("Pa24", 1);   // OE/ output Enable OFF	
		usleep (100);
	}

	average = (sum/20);

	snprintf(puf, 100, "%g", average*1.22);
	send(sa, &puf, sizeof(puf), 0);

	printf("	Avg_20_Encoder_Counts= %lf nm\n", average*1.22);	
	sum=0;
	average=0;

    	remove_gpio();
 	led_ctrl ("D9", OFF2);
	led_ctrl ("D6", OFF2);
}

// Move off loader up by 1 mm
void off_up_func_1()
{
	printf("\n	Off Loader is going up...\n");

	unsigned long int portValue1,portValue2,portValue3,portValue4,portValue;
    	unsigned int s0,s1,s2,s3,s4,s5,s6,s7;
	long long sum=0,average=0;
	int fd, rd, i, j, k;
        struct input_event ev[64];

	init_gpio();
	set_gpio_output ("pa29", 1);		//Enable
	set_gpio_output ("pa30", 0);		//Step
	set_gpio_output ("pa31", 0);		//Direction
	usleep (2000);
	led_ctrl ("D9", OFF2);
	led_ctrl ("D6", OFF2);
        set_gpio_output ("Pa22", 0);
	usleep (100);
	set_gpio_output ("Pa22", 1);
	set_gpio_output ("pa24", 1);
	led_ctrl ("D9", ON);
	set_gpio_output ("pa29", 0);		//Enable
	set_gpio_output ("pa31", 1);		//Direction=0	
	usleep(2);
	for(i=1;i<=109;i++)		//400 gives approx. 510000 counts i.e.,3.665mm @ VCM;; For 3mm @ VCM, we use 328 that should give 417490 counts 
	{
		set_gpio_output ("pa30", 1);		//
		usleep (1);
		set_gpio_output ("pa30", 0);		//
		usleep (1);
	}
	sleep(1);	
	for(j=1;j<=20;j++)
         {
		set_gpio_output ("Pa24", 0); // OE/ Output Enable on 		
		set_gpio_output ("Pa26", 1);	// S1   // Byte 1
		set_gpio_output ("Pa27", 0);	// s2
		usleep(100);
		
		s0=get_gpio_input ("pb29");
		s1=get_gpio_input ("pb30");
		s2=get_gpio_input ("pb23");
		s3=get_gpio_input ("pb24");
		s4=get_gpio_input ("pd12");
		s5=get_gpio_input ("pd13");
		s6=get_gpio_input ("pd14");
		s7=get_gpio_input ("pd28");
		usleep(100);
		portValue1= (s0*1)+(s1*2)+(s2*4)+(s3*8)+(s4*16)+(s5*32)+(s6*64)+(s7*128);
		
		set_gpio_output ("Pa26", 0);    // Byte 2
		set_gpio_output ("Pa27", 0);
		usleep(100);
		
		s0=get_gpio_input ("pb29");
		s1=get_gpio_input ("pb30");
		s2=get_gpio_input ("pb23");
		s3=get_gpio_input ("pb24");
		s4=get_gpio_input ("pd12");
		s5=get_gpio_input ("pd13");
		s6=get_gpio_input ("pd14");
		s7=get_gpio_input ("pd28");
   		usleep(100);
		portValue2= (s0*256)+(s1*512)+(s2*1024)+(s3*2048)+(s4*4096)+(s5*8192)+(s6*16384)+(s7*32768);
				
		set_gpio_output ("Pa26", 1);      // Byte 3
		set_gpio_output ("Pa27", 1);
		usleep(100);

		s0=get_gpio_input ("pb29");
		s1=get_gpio_input ("pb30");
		s2=get_gpio_input ("pb23");
		s3=get_gpio_input ("pb24");
		s4=get_gpio_input ("pd12");
		s5=get_gpio_input ("pd13");
		s6=get_gpio_input ("pd14");
		s7=get_gpio_input ("pd28");
		usleep(100);
		portValue3= (s0*65536)+(s1*131072)+(s2*262144)+(s3*524288)+(s4*1048576)+(s5*2097152)+(s6*4194304)+(s7*8388608);
				
		set_gpio_output ("Pa26", 0);       // Byte 4
		set_gpio_output ("Pa27", 1);
		usleep(100);

		s0=get_gpio_input ("pb29");
		s1=get_gpio_input ("pb30");
		s2=get_gpio_input ("pb23");
		s3=get_gpio_input ("pb24");
		s4=get_gpio_input ("pd12");
		s5=get_gpio_input ("pd13");
		s6=get_gpio_input ("pd14");
		s7=get_gpio_input ("pd28");
 		usleep(100);

		portValue4= (s0*16777216)+(s1*33554432)+(s2*67108864)+(s3*134217728)+(s4*268435456)+(s5*536870912)+(s6*1073741824)+(s7*2147483647);				
		portValue= (portValue1)+(portValue2)+(portValue3);	

		if(portValue>10000000)
		{
			portValue= 16777216-portValue;
		}
		sum+=portValue;
		set_gpio_output ("Pa24", 1);   // OE/ output Enable OFF	
		usleep (100);
	}

	average = (sum/20);
	average = average*(-1);
	snprintf(puf, 100, "%g", average*1.22);
	send(sa, &puf, sizeof(puf), 0);

	printf("	Avg_20_Encoder_Counts= %lf nm\n", average*1.22);	
	sum=0;
	average=0;

    	remove_gpio();
 	led_ctrl ("D9", OFF2);
	led_ctrl ("D6", OFF2);
}

// Move off loader up by 3 mm
void off_up_func_3()
{
	printf("\n	Off Loader is going up...\n");

	unsigned long int portValue1,portValue2,portValue3,portValue4,portValue;
    	unsigned int s0,s1,s2,s3,s4,s5,s6,s7;
	long long sum=0,average=0;
	int fd, rd, i, j, k;
        struct input_event ev[64];

	init_gpio();
	set_gpio_output ("pa29", 1);		//Enable
	set_gpio_output ("pa30", 0);		//Step
	set_gpio_output ("pa31", 0);		//Direction
	usleep (2000);
	led_ctrl ("D9", OFF2);
	led_ctrl ("D6", OFF2);
        set_gpio_output ("Pa22", 0);
	usleep (100);
	set_gpio_output ("Pa22", 1);
	set_gpio_output ("pa24", 1);
	led_ctrl ("D9", ON);
	set_gpio_output ("pa29", 0);		//Enable
	set_gpio_output ("pa31", 1);		//Direction=0	
	usleep(2);
	for(i=1;i<=328;i++)		//400 gives approx. 510000 counts i.e.,3.665mm @ VCM;; For 3mm @ VCM, we use 328 that should give 417490 counts 
	{
		set_gpio_output ("pa30", 1);		//
		usleep (1);
		set_gpio_output ("pa30", 0);		//
		usleep (1);
	}
	sleep(1);	
	for(j=1;j<=20;j++)
         {
		set_gpio_output ("Pa24", 0); // OE/ Output Enable on 		
		set_gpio_output ("Pa26", 1);	// S1   // Byte 1
		set_gpio_output ("Pa27", 0);	// s2
		usleep(100);
		
		s0=get_gpio_input ("pb29");
		s1=get_gpio_input ("pb30");
		s2=get_gpio_input ("pb23");
		s3=get_gpio_input ("pb24");
		s4=get_gpio_input ("pd12");
		s5=get_gpio_input ("pd13");
		s6=get_gpio_input ("pd14");
		s7=get_gpio_input ("pd28");
		usleep(100);
		portValue1= (s0*1)+(s1*2)+(s2*4)+(s3*8)+(s4*16)+(s5*32)+(s6*64)+(s7*128);
		
		set_gpio_output ("Pa26", 0);    // Byte 2
		set_gpio_output ("Pa27", 0);
		usleep(100);
		
		s0=get_gpio_input ("pb29");
		s1=get_gpio_input ("pb30");
		s2=get_gpio_input ("pb23");
		s3=get_gpio_input ("pb24");
		s4=get_gpio_input ("pd12");
		s5=get_gpio_input ("pd13");
		s6=get_gpio_input ("pd14");
		s7=get_gpio_input ("pd28");
   		usleep(100);
		portValue2= (s0*256)+(s1*512)+(s2*1024)+(s3*2048)+(s4*4096)+(s5*8192)+(s6*16384)+(s7*32768);
				
		set_gpio_output ("Pa26", 1);      // Byte 3
		set_gpio_output ("Pa27", 1);
		usleep(100);

		s0=get_gpio_input ("pb29");
		s1=get_gpio_input ("pb30");
		s2=get_gpio_input ("pb23");
		s3=get_gpio_input ("pb24");
		s4=get_gpio_input ("pd12");
		s5=get_gpio_input ("pd13");
		s6=get_gpio_input ("pd14");
		s7=get_gpio_input ("pd28");
		usleep(100);
		portValue3= (s0*65536)+(s1*131072)+(s2*262144)+(s3*524288)+(s4*1048576)+(s5*2097152)+(s6*4194304)+(s7*8388608);
				
		set_gpio_output ("Pa26", 0);       // Byte 4
		set_gpio_output ("Pa27", 1);
		usleep(100);

		s0=get_gpio_input ("pb29");
		s1=get_gpio_input ("pb30");
		s2=get_gpio_input ("pb23");
		s3=get_gpio_input ("pb24");
		s4=get_gpio_input ("pd12");
		s5=get_gpio_input ("pd13");
		s6=get_gpio_input ("pd14");
		s7=get_gpio_input ("pd28");
 		usleep(100);

		portValue4= (s0*16777216)+(s1*33554432)+(s2*67108864)+(s3*134217728)+(s4*268435456)+(s5*536870912)+(s6*1073741824)+(s7*2147483647);				
		portValue= (portValue1)+(portValue2)+(portValue3);	

		if(portValue>10000000)
		{
			portValue= 16777216-portValue;
		}
		sum+=portValue;
		set_gpio_output ("Pa24", 1);   // OE/ output Enable OFF	
		usleep (100);
	}

	average = (sum/20);
	average = average*(-1);
	snprintf(puf, 100, "%g", average*1.22);
	send(sa, &puf, sizeof(puf), 0);

	printf("	Avg_20_Encoder_Counts= %lf nm\n", average*1.22);	
	sum=0;
	average=0;

    	remove_gpio();
 	led_ctrl ("D9", OFF2);
	led_ctrl ("D6", OFF2);
}

// Move off loader up by 6 mm
void off_up_func_6()
{
	printf("\n	Off Loader is going up...\n");

	unsigned long int portValue1,portValue2,portValue3,portValue4,portValue;
    	unsigned int s0,s1,s2,s3,s4,s5,s6,s7;
	long long sum=0,average=0;
	int fd, rd, i, j, k;
        struct input_event ev[64];

	init_gpio();
	set_gpio_output ("pa29", 1);		//Enable
	set_gpio_output ("pa30", 0);		//Step
	set_gpio_output ("pa31", 0);		//Direction
	usleep (2000);
	led_ctrl ("D9", OFF2);
	led_ctrl ("D6", OFF2);
        set_gpio_output ("Pa22", 0);
	usleep (100);
	set_gpio_output ("Pa22", 1);
	set_gpio_output ("pa24", 1);
	led_ctrl ("D9", ON);
	set_gpio_output ("pa29", 0);		//Enable
	set_gpio_output ("pa31", 1);		//Direction=0	
	usleep(2);
	for(i=1;i<=656;i++)		//400 gives approx. 510000 counts i.e.,3.665mm @ VCM;; For 3mm @ VCM, we use 328 that should give 417490 counts 
	{
		set_gpio_output ("pa30", 1);		//
		usleep (1);
		set_gpio_output ("pa30", 0);		//
		usleep (1);
	}
	sleep(1);	
	for(j=1;j<=20;j++)
         {
		set_gpio_output ("Pa24", 0); // OE/ Output Enable on 		
		set_gpio_output ("Pa26", 1);	// S1   // Byte 1
		set_gpio_output ("Pa27", 0);	// s2
		usleep(100);
		
		s0=get_gpio_input ("pb29");
		s1=get_gpio_input ("pb30");
		s2=get_gpio_input ("pb23");
		s3=get_gpio_input ("pb24");
		s4=get_gpio_input ("pd12");
		s5=get_gpio_input ("pd13");
		s6=get_gpio_input ("pd14");
		s7=get_gpio_input ("pd28");
		usleep(100);
		portValue1= (s0*1)+(s1*2)+(s2*4)+(s3*8)+(s4*16)+(s5*32)+(s6*64)+(s7*128);
		
		set_gpio_output ("Pa26", 0);    // Byte 2
		set_gpio_output ("Pa27", 0);
		usleep(100);
		
		s0=get_gpio_input ("pb29");
		s1=get_gpio_input ("pb30");
		s2=get_gpio_input ("pb23");
		s3=get_gpio_input ("pb24");
		s4=get_gpio_input ("pd12");
		s5=get_gpio_input ("pd13");
		s6=get_gpio_input ("pd14");
		s7=get_gpio_input ("pd28");
   		usleep(100);
		portValue2= (s0*256)+(s1*512)+(s2*1024)+(s3*2048)+(s4*4096)+(s5*8192)+(s6*16384)+(s7*32768);
				
		set_gpio_output ("Pa26", 1);      // Byte 3
		set_gpio_output ("Pa27", 1);
		usleep(100);

		s0=get_gpio_input ("pb29");
		s1=get_gpio_input ("pb30");
		s2=get_gpio_input ("pb23");
		s3=get_gpio_input ("pb24");
		s4=get_gpio_input ("pd12");
		s5=get_gpio_input ("pd13");
		s6=get_gpio_input ("pd14");
		s7=get_gpio_input ("pd28");
		usleep(100);
		portValue3= (s0*65536)+(s1*131072)+(s2*262144)+(s3*524288)+(s4*1048576)+(s5*2097152)+(s6*4194304)+(s7*8388608);
				
		set_gpio_output ("Pa26", 0);       // Byte 4
		set_gpio_output ("Pa27", 1);
		usleep(100);

		s0=get_gpio_input ("pb29");
		s1=get_gpio_input ("pb30");
		s2=get_gpio_input ("pb23");
		s3=get_gpio_input ("pb24");
		s4=get_gpio_input ("pd12");
		s5=get_gpio_input ("pd13");
		s6=get_gpio_input ("pd14");
		s7=get_gpio_input ("pd28");
 		usleep(100);

		portValue4= (s0*16777216)+(s1*33554432)+(s2*67108864)+(s3*134217728)+(s4*268435456)+(s5*536870912)+(s6*1073741824)+(s7*2147483647);				
		portValue= (portValue1)+(portValue2)+(portValue3);	

		if(portValue>10000000)
		{
			portValue= 16777216-portValue;
		}
		sum+=portValue;
		set_gpio_output ("Pa24", 1);   // OE/ output Enable OFF	
		usleep (100);
	}

	average = (sum/20);
	average = average*(-1);

	snprintf(puf, 100, "%g", average*1.22);
	send(sa, &puf, sizeof(puf), 0);

	printf("	Avg_20_Encoder_Counts= %lf nm\n", average*1.22);	
	sum=0;
	average=0;

    	remove_gpio();
 	led_ctrl ("D9", OFF2);
	led_ctrl ("D6", OFF2);
}

//Drive board reset here
void reset_func()
{
	printf("\n	Resetting the drive board now...\n");

	init_gpio();
	set_gpio_output ("Pa22", 0);    	// Reset pin of Encoder
	usleep (100);
	set_gpio_output ("pa29", 0);		//Enable pin of Stepper
	usleep (100);
	set_gpio_output ("PA28", 0);		//Latch Enable=0 SPI of DAC

	int a,fd1;				//fd1 for SPI
	char wr_buf[]={0x00,0x00};     	
	fd1 = open("/dev/spidev1.3", O_RDWR);
	if (fd1<=0) 
	{
		printf("%s: Device %s not found\n", "vcm_test","/dev/spidev1.3");
		exit(1);
	}
	printf("spi device opened\n");
	a=0000;
	wr_buf[0]=a/256;
	wr_buf[1]=a%256;
	set_gpio_output ("PA28", 1);		//Latch Enable=1
	{
		if (write(fd1, wr_buf, ARRAY_SIZE(wr_buf)) != ARRAY_SIZE(wr_buf))
		perror("Write Error");
	}
	set_gpio_output ("PA28", 0);		//Latch Enable=0 SPI of DAC
	usleep (100);
}

//Run vcm test for 25 seconds
void vcm_main_func_25()
{
	printf("\n	VCM test underway...\n");
	
	pthread_t id1, id2;				//, id3; /* define the thread identifier */
	pthread_mutex_init(&mutex, NULL);		/* initialize mutex */
	
	/* create thread */
	pthread_create(&id1, NULL, (void*)&vcm_work_func_25, NULL);
	pthread_create(&id2, NULL, (void*)&vcm_delay_func_25, NULL);
	
	init_gpio();
	set_gpio_output ("Pa22", 0);    		/* case not sensitive */  // Reset pin
	usleep (100);
	set_gpio_output ("Pa22", 1);    		/* case not sensitive */
	set_gpio_output ("PA28", 0);

	pthread_join(id2, NULL);
       	pthread_join(id1, NULL);	
}

void vcm_work_func_25()
{
	int a, k, fd1, srno=0, j;					//fd1 for SPI
	int ad = -1, errcode = 0, nread = 0;
	unsigned int value = 0;	
	char wr_buf[]={0x00,0x00};
	long long sum=0;
	double average=0;

	fd1 = open("/dev/spidev1.3", O_RDWR);
	if (fd1<=0) 
	{
		printf("%s: Device %s not found\n", "vcm_test","/dev/spidev1.3");
		exit(1);
	}
	printf("\n	SPI device opened\n");

	ad = open (ADC_NODE, O_RDONLY);
	if (ad < 0) 
	{
        	fprintf (stderr, "open fail\n");
        	errcode = 1;
        	goto out;
   	}
	while(tcounter <= 2)
	{	
		set_gpio_output ("Pa26", 0);			// S1   // Byte 1
		set_gpio_output ("Pa27", 0);			// s2
		usleep(200000);

		if ((nread = read (ad, &value, sizeof(value))) < 0) 
		{
       			fprintf (stderr, "read error\n");
        	 	errcode = 1;
        	 	goto out;
    		}	

		set_gpio_output ("Pa26", 1);			// S1   // Byte 1
		set_gpio_output ("Pa27", 0);			// s2
		usleep(200000);

		if ((nread = read (ad, &value, sizeof(value))) < 0) 
		{
       			fprintf (stderr, "read error\n");
        	 	errcode = 1;
        	 	goto out;
    		}	
  	 	printf ("%d\n", value);

 		if(delay_buffer_10sec==0)
		{
			led_ctrl ("D9", OFF2);
		   	a=0000;
		   	wr_buf[0]=a/256;
	   	   	wr_buf[1]=a%256;
		   	set_gpio_output ("PA28", 1);		//Latch Enable=1
		   	{
		   		if (write(fd1, wr_buf, ARRAY_SIZE(wr_buf)) != ARRAY_SIZE(wr_buf))
		   		perror("Write Error");
		   	}
		   	set_gpio_output ("PA28", 0);		//Latch Enable=0
		}
 		else if(delay_buffer_10sec==1)
		{
			led_ctrl ("D9", ON);
			a=9340;				//0x1113/4371 for 0.4V (For Lab Actuator),9340 for 0.891V (For Avsarala Actuator)
			wr_buf[0]=a/256;
			wr_buf[1]=a%256;
		 	set_gpio_output ("PA28", 1);		//Latch Enable=1
		 	{
		 		if (write(fd1, wr_buf, ARRAY_SIZE(wr_buf)) != ARRAY_SIZE(wr_buf))
			   	perror("Write Error");
			}
			set_gpio_output ("PA28", 0);		//Latch Enable=0
		}
		sleep(1);			//for Encoder values to settle
     		for(j=1;j<=20;j++)
      		{
			unsigned int s0,s1,s2,s3,s4,s5,s6,s7; // lsb to msb
				
			set_gpio_output ("Pa24", 0); // OE/ Output Enable on 		
			set_gpio_output ("Pa26", 1);	// S1   // Byte 1
			set_gpio_output ("Pa27", 0);	// s2
			usleep(100);
			
			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");
			usleep(100);
			unsigned long int portValue1= (s0*1)+(s1*2)+(s2*4)+(s3*8)+(s4*16)+(s5*32)+(s6*64)+(s7*128);
			
			set_gpio_output ("Pa26", 0);    // Byte 2
			set_gpio_output ("Pa27", 0);
			usleep(100);
			
			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");
   			usleep(100);
			unsigned long int portValue2= (s0*256)+(s1*512)+(s2*1024)+(s3*2048)+(s4*4096)+(s5*8192)+(s6*16384)+(s7*32768);
			
			set_gpio_output ("Pa26", 1);      // Byte 3
			set_gpio_output ("Pa27", 1);
			usleep(100);
	
			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");
			usleep(100);
			unsigned long portValue3= (s0*65536)+(s1*131072)+(s2*262144)+(s3*524288)+(s4*1048576)+(s5*2097152)+(s6*4194304)+(s7*8388608);
			
			set_gpio_output ("Pa26", 0);       // Byte 4
			set_gpio_output ("Pa27", 1);
			usleep(100);
	
			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");
 			usleep(100);
			unsigned long portValue4= (s0*16777216)+(s1*33554432)+(s2*67108864)+(s3*134217728)+(s4*268435456)+(s5*536870912)+(s6*1073741824)+(s7*2147483647);		
			unsigned long int portValue= (portValue1)+(portValue2)+(portValue3);
			if(portValue>10000000)
			{
				portValue= 16777216-portValue;
			}
			sum+=portValue;
			unsigned long int Result_new=portValue;
			set_gpio_output ("Pa24", 1);   // OE/ output Enable OFF	
			usleep (100);
    	  	}
		average = (sum/20)*1.22;

		snprintf(puf, 10, "%lf", average*1.22);
		send(sa, &puf, sizeof(puf), 0);

		snprintf(quf, 100, "%d", value+10000000);
		send(sa, &quf, sizeof(quf), 0);

		printf("	Avg_20_Encoder_Counts= %lf nm\n", average);	
		srno++;	
		sum=0;
		average=0;	
		set_gpio_output ("PA28", 1);		//Latch Enable=1
	}
	out:
  		if (ad > 0) close (ad);	
}

void vcm_delay_func_25()
{
	tcounter=0;
	while(tcounter <= 2)				// 2 for 20sec
	{		
		for(tcounter=0; tcounter<= 2; tcounter++)
		{	
 			sleep(5);
			delay_buffer_10sec=0;
			sleep(5);
			delay_buffer_10sec=1;	
		}
	}
}

//Run vcm test for 50 seconds
void vcm_main_func_50()
{
	printf("\n	VCM test underway...\n");
	
	pthread_t id1, id2;				//, id3; /* define the thread identifier */
	pthread_mutex_init(&mutex, NULL);		/* initialize mutex */
	
	/* create thread */
	pthread_create(&id1, NULL, (void*)&vcm_work_func_25, NULL);
	pthread_create(&id2, NULL, (void*)&vcm_delay_func_25, NULL);
	
	init_gpio();
	set_gpio_output ("Pa22", 0);    		/* case not sensitive */  // Reset pin
	usleep (100);
	set_gpio_output ("Pa22", 1);    		/* case not sensitive */
	set_gpio_output ("PA28", 0);

	pthread_join(id2, NULL);
       	pthread_join(id1, NULL);	
}

void vcm_work_func_50()
{
	int a, k, fd1, srno=0, j;					//fd1 for SPI
	int ad = -1, errcode = 0, nread = 0;
	unsigned int value = 0;	
	char wr_buf[]={0x00,0x00};
	long long sum=0;
	double average=0;

	fd1 = open("/dev/spidev1.3", O_RDWR);
	if (fd1<=0) 
	{
		printf("%s: Device %s not found\n", "vcm_test","/dev/spidev1.3");
		exit(1);
	}
	printf("\n	SPI device opened\n");

	ad = open (ADC_NODE, O_RDONLY);
	if (ad < 0) 
	{
        	fprintf (stderr, "open fail\n");
        	errcode = 1;
        	goto out;
   	}
	while(tcounter <= 4)
	{	
		set_gpio_output ("Pa26", 0);			// S1   // Byte 1
		set_gpio_output ("Pa27", 0);			// s2
		usleep(200000);

		if ((nread = read (ad, &value, sizeof(value))) < 0) 
		{
       			fprintf (stderr, "read error\n");
        	 	errcode = 1;
        	 	goto out;
    		}	

		set_gpio_output ("Pa26", 1);			// S1   // Byte 1
		set_gpio_output ("Pa27", 0);			// s2
		usleep(200000);

		if ((nread = read (ad, &value, sizeof(value))) < 0) 
		{
       			fprintf (stderr, "read error\n");
        	 	errcode = 1;
        	 	goto out;
    		}	
  	 	printf ("%d\n", value);

 		if(delay_buffer_10sec==0)
		{
			led_ctrl ("D9", OFF2);
		   	a=0000;
		   	wr_buf[0]=a/256;
	   	   	wr_buf[1]=a%256;
		   	set_gpio_output ("PA28", 1);		//Latch Enable=1
		   	{
		   		if (write(fd1, wr_buf, ARRAY_SIZE(wr_buf)) != ARRAY_SIZE(wr_buf))
		   		perror("Write Error");
		   	}
		   	set_gpio_output ("PA28", 0);		//Latch Enable=0
		}
 		else if(delay_buffer_10sec==1)
		{
			led_ctrl ("D9", ON);
			a=9340;				//0x1113/4371 for 0.4V (For Lab Actuator),9340 for 0.891V (For Avsarala Actuator)
			wr_buf[0]=a/256;
			wr_buf[1]=a%256;
		 	set_gpio_output ("PA28", 1);		//Latch Enable=1
		 	{
		 		if (write(fd1, wr_buf, ARRAY_SIZE(wr_buf)) != ARRAY_SIZE(wr_buf))
			   	perror("Write Error");
			}
			set_gpio_output ("PA28", 0);		//Latch Enable=0
		}
		sleep(1);			//for Encoder values to settle
     		for(j=1;j<=20;j++)
      		{
			unsigned int s0,s1,s2,s3,s4,s5,s6,s7; // lsb to msb
				
			set_gpio_output ("Pa24", 0); // OE/ Output Enable on 		
			set_gpio_output ("Pa26", 1);	// S1   // Byte 1
			set_gpio_output ("Pa27", 0);	// s2
			usleep(100);
			
			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");
			usleep(100);
			unsigned long int portValue1= (s0*1)+(s1*2)+(s2*4)+(s3*8)+(s4*16)+(s5*32)+(s6*64)+(s7*128);
			
			set_gpio_output ("Pa26", 0);    // Byte 2
			set_gpio_output ("Pa27", 0);
			usleep(100);
			
			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");
   			usleep(100);
			unsigned long int portValue2= (s0*256)+(s1*512)+(s2*1024)+(s3*2048)+(s4*4096)+(s5*8192)+(s6*16384)+(s7*32768);
			
			set_gpio_output ("Pa26", 1);      // Byte 3
			set_gpio_output ("Pa27", 1);
			usleep(100);
	
			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");
			usleep(100);
			unsigned long portValue3= (s0*65536)+(s1*131072)+(s2*262144)+(s3*524288)+(s4*1048576)+(s5*2097152)+(s6*4194304)+(s7*8388608);
			
			set_gpio_output ("Pa26", 0);       // Byte 4
			set_gpio_output ("Pa27", 1);
			usleep(100);
	
			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");
 			usleep(100);
			unsigned long portValue4= (s0*16777216)+(s1*33554432)+(s2*67108864)+(s3*134217728)+(s4*268435456)+(s5*536870912)+(s6*1073741824)+(s7*2147483647);		
			unsigned long int portValue= (portValue1)+(portValue2)+(portValue3);
			if(portValue>10000000)
			{
				portValue= 16777216-portValue;
			}
			sum+=portValue;
			unsigned long int Result_new=portValue;
			set_gpio_output ("Pa24", 1);   // OE/ output Enable OFF	
			usleep (100);
    	  	}
		average = (sum/20)*1.22;

		snprintf(puf, 10, "%lf", average*1.22);
		send(sa, &puf, sizeof(puf), 0);

		snprintf(quf, 100, "%d", value+10000000);
		send(sa, &quf, sizeof(quf), 0);

		printf("	Avg_20_Encoder_Counts= %lf nm\n", average);	
		srno++;	
		sum=0;
		average=0;	
		set_gpio_output ("PA28", 1);		//Latch Enable=1
	}
	out:
  		if (ad > 0) close (ad);	
}

void vcm_delay_func_50()
{
	tcounter=0;
	while(tcounter <= 4)				// 4 for 40sec
	{		
		for(tcounter=0; tcounter<= 4; tcounter++)
		{	
 			sleep(5);
			delay_buffer_10sec=0;
			sleep(5);
			delay_buffer_10sec=1;	
		}
	}
}

//Run vcm test for 75 seconds
void vcm_main_func_75()
{
	printf("\n	VCM test underway...\n");
	
	pthread_t id1, id2;				//, id3; /* define the thread identifier */
	pthread_mutex_init(&mutex, NULL);		/* initialize mutex */
	
	/* create thread */
	pthread_create(&id1, NULL, (void*)&vcm_work_func_25, NULL);
	pthread_create(&id2, NULL, (void*)&vcm_delay_func_25, NULL);
	
	init_gpio();
	set_gpio_output ("Pa22", 0);    		/* case not sensitive */  // Reset pin
	usleep (100);
	set_gpio_output ("Pa22", 1);    		/* case not sensitive */
	set_gpio_output ("PA28", 0);

	pthread_join(id2, NULL);
       	pthread_join(id1, NULL);	
}

void vcm_work_func_75()
{
	int a, k, fd1, srno=0, j;					//fd1 for SPI
	int ad = -1, errcode = 0, nread = 0;
	unsigned int value = 0;	
	char wr_buf[]={0x00,0x00};
	long long sum=0;
	double average=0;

	fd1 = open("/dev/spidev1.3", O_RDWR);
	if (fd1<=0) 
	{
		printf("%s: Device %s not found\n", "vcm_test","/dev/spidev1.3");
		exit(1);
	}
	printf("\n	SPI device opened\n");

	ad = open (ADC_NODE, O_RDONLY);
	if (ad < 0) 
	{
        	fprintf (stderr, "open fail\n");
        	errcode = 1;
        	goto out;
   	}
	while(tcounter <= 6)
	{	
		set_gpio_output ("Pa26", 0);			// S1   // Byte 1
		set_gpio_output ("Pa27", 0);			// s2
		usleep(200000);

		if ((nread = read (ad, &value, sizeof(value))) < 0) 
		{
       			fprintf (stderr, "read error\n");
        	 	errcode = 1;
        	 	goto out;
    		}	

		set_gpio_output ("Pa26", 1);			// S1   // Byte 1
		set_gpio_output ("Pa27", 0);			// s2
		usleep(200000);

		if ((nread = read (ad, &value, sizeof(value))) < 0) 
		{
       			fprintf (stderr, "read error\n");
        	 	errcode = 1;
        	 	goto out;
    		}	
  	 	printf ("%d\n", value);

 		if(delay_buffer_10sec==0)
		{
			led_ctrl ("D9", OFF2);
		   	a=0000;
		   	wr_buf[0]=a/256;
	   	   	wr_buf[1]=a%256;
		   	set_gpio_output ("PA28", 1);		//Latch Enable=1
		   	{
		   		if (write(fd1, wr_buf, ARRAY_SIZE(wr_buf)) != ARRAY_SIZE(wr_buf))
		   		perror("Write Error");
		   	}
		   	set_gpio_output ("PA28", 0);		//Latch Enable=0
		}
 		else if(delay_buffer_10sec==1)
		{
			led_ctrl ("D9", ON);
			a=9340;				//0x1113/4371 for 0.4V (For Lab Actuator),9340 for 0.891V (For Avsarala Actuator)
			wr_buf[0]=a/256;
			wr_buf[1]=a%256;
		 	set_gpio_output ("PA28", 1);		//Latch Enable=1
		 	{
		 		if (write(fd1, wr_buf, ARRAY_SIZE(wr_buf)) != ARRAY_SIZE(wr_buf))
			   	perror("Write Error");
			}
			set_gpio_output ("PA28", 0);		//Latch Enable=0
		}
		sleep(1);			//for Encoder values to settle
     		for(j=1;j<=20;j++)
      		{
			unsigned int s0,s1,s2,s3,s4,s5,s6,s7; // lsb to msb
				
			set_gpio_output ("Pa24", 0); // OE/ Output Enable on 		
			set_gpio_output ("Pa26", 1);	// S1   // Byte 1
			set_gpio_output ("Pa27", 0);	// s2
			usleep(100);
			
			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");
			usleep(100);
			unsigned long int portValue1= (s0*1)+(s1*2)+(s2*4)+(s3*8)+(s4*16)+(s5*32)+(s6*64)+(s7*128);
			
			set_gpio_output ("Pa26", 0);    // Byte 2
			set_gpio_output ("Pa27", 0);
			usleep(100);
			
			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");
   			usleep(100);
			unsigned long int portValue2= (s0*256)+(s1*512)+(s2*1024)+(s3*2048)+(s4*4096)+(s5*8192)+(s6*16384)+(s7*32768);
			
			set_gpio_output ("Pa26", 1);      // Byte 3
			set_gpio_output ("Pa27", 1);
			usleep(100);
	
			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");
			usleep(100);
			unsigned long portValue3= (s0*65536)+(s1*131072)+(s2*262144)+(s3*524288)+(s4*1048576)+(s5*2097152)+(s6*4194304)+(s7*8388608);
			
			set_gpio_output ("Pa26", 0);       // Byte 4
			set_gpio_output ("Pa27", 1);
			usleep(100);
	
			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");
 			usleep(100);
			unsigned long portValue4= (s0*16777216)+(s1*33554432)+(s2*67108864)+(s3*134217728)+(s4*268435456)+(s5*536870912)+(s6*1073741824)+(s7*2147483647);		
			unsigned long int portValue= (portValue1)+(portValue2)+(portValue3);
			if(portValue>10000000)
			{
				portValue= 16777216-portValue;
			}
			sum+=portValue;
			unsigned long int Result_new=portValue;
			set_gpio_output ("Pa24", 1);   // OE/ output Enable OFF	
			usleep (100);
    	  	}
		average = (sum/20)*1.22;

		snprintf(puf, 10, "%lf", average*1.22);
		send(sa, &puf, sizeof(puf), 0);

		snprintf(quf, 100, "%d", value+10000000);
		send(sa, &quf, sizeof(quf), 0);

		printf("	Avg_20_Encoder_Counts= %lf nm\n", average);	
		srno++;	
		sum=0;
		average=0;	
		set_gpio_output ("PA28", 1);		//Latch Enable=1

	}
	out:
  		if (ad > 0) close (ad);	
}

void vcm_delay_func_75()
{
	tcounter=0;
	while(tcounter <= 6)				// 6 for 60sec
	{		
		for(tcounter=0; tcounter<= 6; tcounter++)
		{	
 			sleep(5);
			delay_buffer_10sec=0;
			sleep(5);
			delay_buffer_10sec=1;	
		}
	}
}

//Run vcm test for 100 seconds
void vcm_main_func_100()
{
	printf("\n	VCM test underway...\n");
	
	pthread_t id1, id2;				//, id3; /* define the thread identifier */
	pthread_mutex_init(&mutex, NULL);		/* initialize mutex */
	
	/* create thread */
	pthread_create(&id1, NULL, (void*)&vcm_work_func_100, NULL);
	pthread_create(&id2, NULL, (void*)&vcm_delay_func_100, NULL);
	
	init_gpio();
	set_gpio_output ("Pa22", 0);    		/* case not sensitive */  // Reset pin
	usleep (100);
	set_gpio_output ("Pa22", 1);    		/* case not sensitive */
	set_gpio_output ("PA28", 0);

	pthread_join(id2, NULL);
       	pthread_join(id1, NULL);	
}

void vcm_work_func_100()
{
	int a, k, fd1, srno=0, j;					//fd1 for SPI
	int ad = -1, errcode = 0, nread = 0;
	unsigned int value = 0;	
	char wr_buf[]={0x00,0x00};
	long long sum=0;
	double average=0;

	fd1 = open("/dev/spidev1.3", O_RDWR);
	if (fd1<=0) 
	{
		printf("%s: Device %s not found\n", "vcm_test","/dev/spidev1.3");
		exit(1);
	}
	printf("\n	SPI device opened\n");

	ad = open (ADC_NODE, O_RDONLY);
	if (ad < 0) 
	{
        	fprintf (stderr, "open fail\n");
        	errcode = 1;
        	goto out;
   	}
	while(tcounter <= 10)
	{	
		set_gpio_output ("Pa26", 0);			// S1   // Byte 1
		set_gpio_output ("Pa27", 0);			// s2
		usleep(200000);

		if ((nread = read (ad, &value, sizeof(value))) < 0) 
		{
       			fprintf (stderr, "read error\n");
        	 	errcode = 1;
        	 	goto out;
    		}	

		set_gpio_output ("Pa26", 1);			// S1   // Byte 1
		set_gpio_output ("Pa27", 0);			// s2
		usleep(200000);

		if ((nread = read (ad, &value, sizeof(value))) < 0) 
		{
       			fprintf (stderr, "read error\n");
        	 	errcode = 1;
        	 	goto out;
    		}	
  	 	printf ("%d\n", value);

 		if(delay_buffer_10sec==0)
		{
			led_ctrl ("D9", OFF2);
		   	a=0000;
		   	wr_buf[0]=a/256;
	   	   	wr_buf[1]=a%256;
		   	set_gpio_output ("PA28", 1);		//Latch Enable=1
		   	{
		   		if (write(fd1, wr_buf, ARRAY_SIZE(wr_buf)) != ARRAY_SIZE(wr_buf))
		   		perror("Write Error");
		   	}
		   	set_gpio_output ("PA28", 0);		//Latch Enable=0
		}
 		else if(delay_buffer_10sec==1)
		{
			led_ctrl ("D9", ON);
			a=9340;				//0x1113/4371 for 0.4V (For Lab Actuator),9340 for 0.891V (For Avsarala Actuator)
			wr_buf[0]=a/256;
			wr_buf[1]=a%256;
		 	set_gpio_output ("PA28", 1);		//Latch Enable=1
		 	{
		 		if (write(fd1, wr_buf, ARRAY_SIZE(wr_buf)) != ARRAY_SIZE(wr_buf))
			   	perror("Write Error");
			}
			set_gpio_output ("PA28", 0);		//Latch Enable=0
		}
		sleep(1);			//for Encoder values to settle
     		for(j=1;j<=20;j++)
      		{
			unsigned int s0,s1,s2,s3,s4,s5,s6,s7; // lsb to msb
				
			set_gpio_output ("Pa24", 0); // OE/ Output Enable on 		
			set_gpio_output ("Pa26", 1);	// S1   // Byte 1
			set_gpio_output ("Pa27", 0);	// s2
			usleep(100);
			
			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");
			usleep(100);
			unsigned long int portValue1= (s0*1)+(s1*2)+(s2*4)+(s3*8)+(s4*16)+(s5*32)+(s6*64)+(s7*128);
			
			set_gpio_output ("Pa26", 0);    // Byte 2
			set_gpio_output ("Pa27", 0);
			usleep(100);
			
			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");
   			usleep(100);
			unsigned long int portValue2= (s0*256)+(s1*512)+(s2*1024)+(s3*2048)+(s4*4096)+(s5*8192)+(s6*16384)+(s7*32768);
			
			set_gpio_output ("Pa26", 1);      // Byte 3
			set_gpio_output ("Pa27", 1);
			usleep(100);
	
			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");
			usleep(100);
			unsigned long portValue3= (s0*65536)+(s1*131072)+(s2*262144)+(s3*524288)+(s4*1048576)+(s5*2097152)+(s6*4194304)+(s7*8388608);
			
			set_gpio_output ("Pa26", 0);       // Byte 4
			set_gpio_output ("Pa27", 1);
			usleep(100);
	
			s0=get_gpio_input ("pb29");
			s1=get_gpio_input ("pb30");
			s2=get_gpio_input ("pb23");
			s3=get_gpio_input ("pb24");
			s4=get_gpio_input ("pd12");
			s5=get_gpio_input ("pd13");
			s6=get_gpio_input ("pd14");
			s7=get_gpio_input ("pd28");
 			usleep(100);
			unsigned long portValue4= (s0*16777216)+(s1*33554432)+(s2*67108864)+(s3*134217728)+(s4*268435456)+(s5*536870912)+(s6*1073741824)+(s7*2147483647);		
			unsigned long int portValue= (portValue1)+(portValue2)+(portValue3);
			if(portValue>10000000)
			{
				portValue= 16777216-portValue;
			}
			sum+=portValue;
			unsigned long int Result_new=portValue;
			set_gpio_output ("Pa24", 1);   // OE/ output Enable OFF	
			usleep (100);
    	  	}
		average = (sum/20)*1.22;

		snprintf(puf, 10, "%lf", average*1.22);
		send(sa, &puf, sizeof(puf), 0);

		snprintf(quf, 100, "%d", value+10000000);
		send(sa, &quf, sizeof(quf), 0);

		printf("	Avg_20_Encoder_Counts= %lf nm\n", average);	
		srno++;	
		sum=0;
		average=0;	
		set_gpio_output ("PA28", 1);		//Latch Enable=1
	}
	out:
  		if (ad > 0) close (ad);	
}

void vcm_delay_func_100()
{
	tcounter=0;
	while(tcounter <= 10)					// 10 for 100 sec
	{		
		for(tcounter=0; tcounter<= 10; tcounter++)
		{	
 			sleep(5);
			delay_buffer_10sec=0;
			sleep(5);
			delay_buffer_10sec=1;	
		}
	}
}


