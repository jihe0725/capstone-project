#include <reg52.h>
#include <intrins.h>
#define uint unsigned int
#define uchar unsigned char


sbit DQ1=P3^4;//set the pin 34 of prcessor to connect with the sensor;
sbit DQ2=P3^5;//set the pin 35 of prcessor to connect with the sensor;

sbit LED4   = P2^0;//set the pin 20 of prcessor to connect with the EN of LCD;
sbit LED3   = P2^1;//set the pin 21 of prcessor to connect with the EN of LCD;
sbit LED2   = P2^2;//set the pin 22 of prcessor to connect with the EN of LCD;
sbit LED1   = P2^3;//set the pin 23 of prcessor to connect with the EN of LCD;
sbit RS=P2^5;//set the pin 25 of prcessor to connect with the RS of LCD;
sbit RW=P2^6;//set the pin 26 of prcessor to connect with the RW of LCD;
sbit EN=P2^7;//set the pin 27 of prcessor to connect with the EN of LCD;

sbit SetKey = P1^0;//set the pin 10 of prcessor to connect with input key of alarm;
sbit UpKey  = P1^1;//set the pin 10 of prcessor to connect with upset key of alarm;
sbit DnKey  = P1^2;//set the pin 10 of prcessor to connect with downset key of alarm;
sbit AlarmK = P1^3;//set the pin 10 of prcessor to connect enable pin of alarm;
sbit BEEP   = P2^4;//set the pin 10 of prcessor to connect with beeper;

uchar data disdata[5];
uint CurrentT1=0,CurrentT2=0;// innitial temperature values;
bit tflag1,tflag2,tflag3,tflag4;//the sign of temperature values;
bit ch1=1,ch2=1;
unsigned char chs=0;
unsigned char Alarm1=0,Alarm2=0;//innital alarm value	
unsigned char ts=0;

#define LCD_BUS P0

unsigned char fAlarm=0;

unsigned char AlarmTempLow=20,AlarmTempTop=35,Set=0,NG=0;//set the upper and lower alarm temperature values;

void SendTempToPC(unsigned char ch);

void delayms(uint ms)		 
{
 	uchar i;
	while(ms--)
	{
	 	for(i=0;i<120;i++);
	}
}

void command(uint com)	
{
   RS=0;				  
   LCD_BUS=com;	  
   delayms(5);		 
   EN=1;				
   delayms(5);		
   EN=0;				
}

void write_dat(uchar dat)	 
{
   RS=1;					
   LCD_BUS=dat;		
   delayms(5);			
   EN=1;					 
   delayms(5);			 
   EN=0;					 
}

void writestring(uchar x,uchar y,uchar *s) 
{     
	if (y == 0)  command(0x80 + x);     
 	else 	command(0xC0 + x);     
       
 	while (*s) 			
 	{     
     write_dat( *s);   
 		s ++;     			
 	}
}

void writeChar(uchar x,uchar y,uchar s) 
{     
	if (y == 0)  command(0x80 + x);     
 	else 	command(0xC0 + x);      
       
 	{     
     write_dat( s);    
 	}
}

void LCD_Initial()		//innitial LCD 
{

   EN=0;				  //LCD off
   RW=0;				  //RW off
   command(0x38);		
   command(0x0c);	
   command(0x06);	  
   command(0x01);	  
   command(0x80);	
}

void Usart_Initial(void)
{
   SCON = 0x50;
	TMOD = 0x20;
	PCON = 0x00;
	TH1  = 0xfd;
	TL1  = 0xfd;
	EA   = 1;
	ES   = 1;
	TR1  = 1;
   //TR0=0;
   //SBUF = ' ';
   //while(TI == 0);TI = 0;
   delayms(100);

}

void SendTempToPC(unsigned char ch)// the function of sending signals to PC ;
{
   unsigned char i=0;//set initial char data;
   unsigned achs=0,tflag;// set initial system values;
   unsigned int temp=0;//initial the temperature sensor data;
   //EA=0;
   //ES=0;
   switch(ch) //indicate dfferent situation ;
   {
      case 1:  achs = 'A';tflag=tflag1;temp=CurrentT1;break;// set the situation A;
      case 2:  achs = 'B';tflag=tflag2;temp=CurrentT2;break;// set the situation B;
      default: break; //break the circuit;
   }
   if(ch<5)//indicate if ch is less than 5;
   {
      SBUF = achs;//give the system value to SBUF;
      while(TI == 0);TI = 0;//do delay (100ms);
      SBUF = '=';//give the value = to SBUF;
      while(TI == 0);TI = 0; //do delay(100ms);
      if(tflag==1)  SBUF = '-';// indicate the status of flag and give the system value - to SBUF;
      else SBUF = '+';//give the value + to SBUF;
      while(TI == 0);TI = 0;//do delay(100ms);
      
      SBUF = temp/1000+0x30;    //calculate the Hundreds-digit;
      while(TI == 0);TI = 0;//do delay(100ms);
      SBUF =temp%1000/100+0x30; //calculate the Tens-digit;
      while(TI == 0);TI = 0;//do delay(100ms);
      SBUF =temp%100/10+0x30;   //calculate the Units-digit; 
      while(TI == 0);TI = 0;//do delay(100ms);
      SBUF ='.';   //give the value . to SBUF;
      while(TI == 0);TI = 0;//do delay(100ms);
      SBUF =temp%10+0x30;       //calculate the Decimals-digit;
      while(TI == 0);TI = 0;//do delay(100ms);
      SBUF = '\r'; //refresh to next line;
      while(TI == 0);TI = 0;//do delay(100ms);
   }
   else//indicate if ch is larger than 5;
   {
      for(i=0;i<10;i++) //do cycle ;
      {
         SBUF = '*';//give the value * to SBUF;
         while(TI == 0);TI = 0;//do delay(100ms);
      }
      SBUF = '\r';//refresh to next line
      while(TI == 0);TI = 0;//do delay(100ms);
   }
   //ES=1;
   EA=1;
   delayms(100);
}

void delay_18B20(unsigned char i)
{
   while(i--);
   //delay_18B20(4); //delay 4ms;
}

bit ds1820rst1(void)//reset the sensor;
{ 
	bit x=0;
	DQ1 = 1;          //reset pin DQ;
	delay_18B20(4); 
	DQ1 = 0;          //DQ off;
	delay_18B20(92); //delay over 80ms;
	DQ1 = 1;          ////reset pin DQ agian;
	delay_18B20(13); 
	x=DQ1;            //if DQ=0, set X=0, initialization sucessed;
	delay_18B20(18);  
	return x;
} 

uchar ds1820rd1(void)//read signals function1;
{ 
	unsigned char i=0;
	unsigned char dat = 0;
	for (i=8;i>0;i--)
	{   
		DQ1 = 0; //pulse sensor signals;
		dat>>=1;
		DQ1 = 1; 
		if(DQ1)
		dat|=0x80;
		delay_18B20(9);
	}
   return(dat);
}

void ds1820wr1(uchar wdata)//write signals function1;
{
	unsigned char i=0;
   for (i=8; i>0; i--)
   { 
		DQ1 = 0;//pulse sensor signals;
		DQ1 = wdata&0x01;
		delay_18B20(9);
		DQ1 = 1;
		wdata>>=1;
   }
}

bit ds1820rst2(void)//reset the sensor 2;
{ 
	bit x=0;
	DQ2 = 1;          //reset pin DQ2;
	delay_18B20(4); //delay 4ms;
	DQ2 = 0;          //DQ2 off;
	delay_18B20(92); //delay over 480us;
	DQ2 = 1;          //reset pin DQ2;
	delay_18B20(13); 
	x=DQ2;            //if DQ2=0, set X=0, initialization sucessed;
	delay_18B20(18);
	return x;
} 

uchar ds1820rd2(void)//read signals function2;
{ 
	unsigned char i=0;
	unsigned char dat = 0;
	for (i=8;i>0;i--)
	{   
		DQ2 = 0; //pulse sensor signals;
		dat>>=1;
		DQ2 = 1; 
		if(DQ2)
		dat|=0x80;
		delay_18B20(9);
	}
   return(dat);
}

void ds1820wr2(uchar wdata)//write signals function2;
{
	unsigned char i=0;
   for (i=8; i>0; i--)
   { 
		DQ2 = 0; //pulse sensor signals;
		DQ2 = wdata&0x01;
		delay_18B20(9);
		DQ2 = 1;
		wdata>>=1;
   }
}

void Read_Temperature1(void)//read the temperature sensor1 and transform signals;
{
	uchar a,b;
	ds1820rst1();    
	ds1820wr1(0xcc);//read signals;
	ds1820wr1(0x44);//start transforming;
	ds1820rst1();    
	ds1820wr1(0xcc);//read signals;
	ds1820wr1(0xbe);//recieve signals;
	a=ds1820rd1();
	b=ds1820rd1();
	CurrentT1=b;
	CurrentT1<<=8;
	CurrentT1=CurrentT1|a;
   if(CurrentT1<0x0fff)   tflag1=0;
   else   {CurrentT1=~CurrentT1+1;tflag1=1;}
	CurrentT1=(unsigned int)(CurrentT1*10/16);//(0.625));//transfer hexadecimal to decimalism data;
}

void Read_Temperature2(void)//read the temperature sensor and transform signals;
{
	uchar a,b;
	ds1820rst2();    
	ds1820wr2(0xcc);//read signals ;
	ds1820wr2(0x44);//start transforming;
	ds1820rst2();    
	ds1820wr2(0xcc);//read signals ;
	ds1820wr2(0xbe);//recieve signals;
	a=ds1820rd2();
	b=ds1820rd2();
	CurrentT2=b;
	CurrentT2<<=8;
	CurrentT2=CurrentT2|a;
   if(CurrentT2<0x0fff)   tflag2=0;
   else   {CurrentT2=~CurrentT2+1;tflag2=1;}
	CurrentT2=(unsigned int)(CurrentT2*10/16);//transfer hexadecimal to decimalism data;
	
}

void Display_Temperature(uint vt,uchar tg,uchar x,uchar y)		// the function of displaying temp;
{
	uchar flagdat;
   disdata[0]=vt/1000+0x30;//calculate and set the Hundreds-digit as disdata0;
   disdata[1]=vt%1000/100+0x30;//calculate and set the Tens-digit as disdata1;
   disdata[2]=vt%100/10+0x30;//calculate and set the Unis-digit as disdata2;
   disdata[3]=vt%10+0x30;//calculate and set the Decimals-digit as disdata2;
   if(tg==0)   flagdat=43;//if the temperature is positive, no plus sign;
   else   flagdat=0x2d;//if the temperature is negative, display minus sign;
	writeChar(x,y,flagdat);//display sign on x,y position;
	writeChar(x+1,y,disdata[0]);//display Hundreds-digit on x+1,y position;
	writeChar(x+2,y,disdata[1]);//display Tens-digiton x+2,y position;
	writeChar(x+3,y,disdata[2]);//display Unis-digit on x+3,y position;
	writeChar(x+4,y,0X2E);//display a point on x+4,y position;
	writeChar(x+5,y,disdata[3]);//display Decimals-digit on x+5,y position;
} 

void OneSensor(void) //only one sensor connnected 
{
	if(ch1==0)	// if sensor1 connected 
	{
		Read_Temperature1();	//go to read function1
		Display_Temperature(CurrentT1,tflag1,7,0);		//go to display function1
      SendTempToPC(1); //go to send to PC function 1
	}
	else(ch2==0)	// else if sensor2 connected 
	{
		Read_Temperature2();	//go to read function2
		Display_Temperature(CurrentT2,tflag2,7,0);		//go to display function2
      SendTempToPC(2);//go to send to PC function 2
	}
}

void TwoSensor(void) //two sensors connnected 
{
	if(ch1==0) // if sensor1 connected 
	{
		Read_Temperature1();	//go to read function1
		Display_Temperature(CurrentT1,tflag1,1,0);		//go to display function1
      SendTempToPC(1);//go to send to PC function 1
	else(ch2==0) // else if sensor2 connected 
		{
			Read_Temperature2();	//go to read function2
			Display_Temperature(CurrentT2,tflag2,10,0);		//go to display function2
         SendTempToPC(2);//go to send to PC function 2
		}
	}
  if(ch2==0)  // if sensor2 connected 
	{
		Read_Temperature2();	//go to read function2
		Display_Temperature(CurrentT2,tflag2,1,0);		//go to display function2
      SendTempToPC(2);//go to send to PC function 2
	}
}

void DisplaySensor(void)
{
	switch(chs)
	{
		case 1:	
		{
			writestring(0,0,"  CurT=         ");
		}break;
		case 2:	
		{
			if(ch1==0) 
			{
				writestring(0,0,"A");
				if(ch2==0) writestring(9,0,"B");
			}
			else if(ch2==0) 
			{
				writestring(0,0,"B");
			}
		}break;
	}
}

void CheckSensor(void)
{
	ch1=0;ch2=0;ch3=0;ch4=0;
	chs=0;
	ch1=ds1820rst1();
	ch2=ds1820rst2();
	delayms(200);
	
	if(ch1==0) {chs++;}
	if(ch2==0) {chs++;}

   writestring(0,1,"  Get   Sensor  ");writeChar(6,1,chs+'0');delayms(1000);
   while(chs==0) writestring(0,0,"   No Sensor    ");
	writestring(0,0,"                ");
   writestring(0,1,"                ");
	DisplaySensor();
}

void SendRunToPC(void)
{
   unsigned char i=0;
   SBUF = 'S';
   while(TI == 0);TI = 0;//delayms(100);
   SBUF = 't';
   while(TI == 0);TI = 0;//delayms(100);
   SBUF = 'a';
   while(TI == 0);TI = 0;//delayms(100);
   SBUF = 'r';
   while(TI == 0);TI = 0;//delayms(100);
   SBUF = 't';
   while(TI == 0);TI = 0;//delayms(100);
   SBUF = '\r';
   while(TI == 0);TI = 0;//delayms(100); 
}


void main(void) //main function
{
	unsigned char i=0;
	P2=0xff;

 	LCD_Initial();	 //initial LCD
   
	Read_Temperature1(); //read function 1
	Read_Temperature2(); //read function 2
	
	writestring(0,0," DS18B20 Alarmer");	  //display ¡°DS18B20 Alarmer¡±*/
   Usart_Initial();
   SendRunToPC();
	delayms(1000);		//delay 1S
	writestring(0,0,"                "); //dispaly space on x=0,y=0
	writestring(0,1,"                "); //dispaly space on x=0,y=1
	CheckSensor();
	
	NG=0;
	while(1)
	{
		if(Set==0)
		{
			if(chs==1) //one sensor only
			{
				OneSensor(); //go to onesensor function
			}
			if(chs==2) //two sensors 
			{
				TwoSensor(); //go to twosensor function
			}
		}
		SetFun();
		Alarm();
	}
}




