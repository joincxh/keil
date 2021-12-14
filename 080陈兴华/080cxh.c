	//��������Ƭ���γ����   5122512020080���˻� 2021/6/1
	#include<reg52.h>
	#include <intrins.h>
	#define uchar unsigned char	 //�궨������
	uchar initpassword1=0,initpassword2=0,initpassword3=0,initpassword4=0,initpassword5=0,initpassword6=0; //���ڵ������  ���������һ�ε��������  ��ʼ0
	uchar newpassword1=0,newpassword2=0,newpassword3=0,newpassword4=0,newpassword5=0,newpassword6=0;  //���������� ���Ծ����ϵ�洢		��ʼ0
	uchar position,key,key_temp,number;  //�����ж�λ�á�ֵѡ���ݴ�ֵ���������
	unsigned int v[]={16,16,16,16,16,16};	//�����������Ʋ���ʾ��ʶ��ʼ״̬
	unsigned char code wei[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf};  //λѡ
	//uchar value=0;
	//I2C���ߴ���
	sbit sda=P2^0;   //������         
	sbit scl=P2^1;	 //ʱ����
	bit open,change,ok,end,reset,close,root,admin;	 //�������������롢ȷ������ϡ����衢�رա���ʼ������Ϊ0���������루���֤����λҲ�У�
	sbit dula=P2^6;	 //��ѡ
	sbit wela=P2^7;		   //λѡ
	sbit beep=P2^3;				 //������
	unsigned char code datatable[]={0x3f,	//"0"  0
	0x06,//"1"	  1
	0x5b,	 //"2"		2
	0x4f,		 //"3"		3
	0x66,			 //"4"	  4
	0x6d,				 //"5"	 5
	0x7d,					 //"6"	  6
	0x07,						 //"7"		 7
	0x7f,							 //"8"		8
	0x6f,								 //"9"	  9
	0x77,								//"A"		 10
	0x7c,									//"B"	   11
	0x39,										//"C"	  12
	0x5e,											//"D"	  13
	0x79,												//"E"	  14
	0x71,													//"F"	   15
	0x00,														//"Ϩ�����"	 16
	0x40														  //"-"���Ƽ��ܲ���	   17
	};	  //����ܵĸ���ֵ
	//�������ʹ��EEPROM�洢 ��ʵ�ֵ������ I2C���ߴ��ͷ�ʽ
	void delay(unsigned char i)	//��ʱ����
	{
		uchar j,k;
	  for(j=i;j>0;j--)
	    for(k=125;k>0;k--);
	}
	void nop()	//΢����ʱ
	{
		_nop_();
		_nop_();
	}
	
	void delay1(unsigned int m)	// 24c02������д����ʱ
	{	unsigned int n;
	  	for(n=0;n<m;n++);
	}
	void init()  //24c02��ʼ���ӳ���
	{
		scl=1;
		nop();
		sda=1;
		nop();
	}
	void start()        //����I2C����
	{
		sda=1;
		nop();
		scl=1;
		nop();
		sda=0;
		nop();
		scl=0;
		nop();
	}
	void stop()         //ֹͣI2C����
	{
		sda=0;
		nop();
		scl=1;
		nop();
		sda=1;
		nop();
	}
	void writebyte(unsigned char j)  //д��һ���ֽ�
	{
		unsigned char i,temp;
	   	temp=j;
	   	for (i=0;i<8;i++)
	   {
		   scl=0;
			_nop_();
			if((temp&0x80)==0x80)
				sda=1;	  //��һ
			else
				sda=0;	  //����
			scl=1;
			_nop_();
			temp<<=1;
	   }
	   scl=0;
	   nop();
	   sda=1;
	   nop();
	}
	unsigned char readbyte()  //��һ���ֽ�
	{
	   unsigned char i,k=0;
	   scl=0; nop(); sda=1;
	   for (i=0;i<8;i++)
	   {  
		scl=1;
			_nop_();
			if(sda)
				k|=0x01;
			if(i<7)
			 	k<<=1;
			scl=0;
			_nop_();	
	
		}
	   	nop();
		return(k);
	}
	
	void clock()   //I2C����ʱ��
	{
	   unsigned char i=0;
	   scl=1;
	   nop();
	   while((sda==1)&&(i<255))
	   	  i++;
	   scl=0;
	   nop();
	}
	//ָ����ַ��ȡһ���ֽ�//
	unsigned char at24c02read(unsigned char address)
	{
	   unsigned char i;
	   start();
	   writebyte(0xa0);	 	  //ָ����ַ������
	   clock();
	   writebyte(address);
	   clock();
	   start();
	   writebyte(0xa1);
	   clock();
	   i=readbyte();
	   stop();
	   delay1(100);
	   return(i);
	}
	//д��һ�ֽ�//
	void at24c02write(unsigned char address,unsigned char info)
	{
	   start();
	   writebyte(0xa0);	 //ָ����ַд��
	   clock();
	   writebyte(address);
	   clock();
	   writebyte(info);
	   clock();
	   stop();
	   delay1(5000); 
	}
	void display()	//��ʾ��
	{
	unsigned char i;
	for(i=0;i<6;i++){
		dula=0;
	   P0=datatable[v[i]];
	   dula=1;
	   dula=0;		  
	   wela=0;
	   P0=wei[i];
	   wela=1;
	   wela=0;
	   delay(5);
		}
	}
	void keyscan()
	{
	  	if(P3!=0xf0){//�а�������	
	    P3=0xfe;
	    key_temp=P3;
	    key_temp=key_temp&0xf0;
	    if(key_temp!=0xf0)
	    {
	      delay(10);
	      if(key_temp!=0xf0)
	      {	
	        key_temp=~P3;  //ȡ��
	        switch(key_temp)
	        {
	          case 0x11:   //s6
	               key=0;
				   position++;
	               break;
	
	          case 0x21:	 //s7
	               key=1;
				   position++;
	               break;
	
	          case 0x41:	 //s8
	               key=2;
				   position++;
	               break;
	
	          case 0x81:   //s9
	               key=3;
				   position++;
	               break;
	         }
	         while(key_temp!=0xf0) 
	        {
	           key_temp=P3;
	           key_temp=key_temp&0xf0;
	           beep=0;
	         }
	         beep=1;
	      }
	    }
	    P3=0xfd;
	    key_temp=P3;
	    key_temp=key_temp&0xf0;
	    if(key_temp!=0xf0)
	    {
	      delay(10);
	      if(key_temp!=0xf0)
	      {
	        key_temp=~P3;
	        switch(key_temp)
	        {
	          case 0x12:	//s10
	               key=4;
				   position++;
	               break;
	
	          case 0x22:	//s11
	               key=5;
				   position++;
	               break;
	
	          case 0x42:	//s12
	               key=6;
				   position++;
	               break;
	
	          case 0x82:	//s13
	               key=7;
				   position++;
	               break;
	         }
	         while(key_temp!=0xf0)
	         {
	           key_temp=P3;
	           key_temp=key_temp&0xf0;
	           beep=0;
	         }
	         beep=1;
	      }
	      }
	    P3=0xfb;
	    key_temp=P3;
	    key_temp=key_temp&0xf0;
	    if(key_temp!=0xf0)
	    {
	      delay(10);
	      if(key_temp!=0xf0)
	      {
	        key_temp=~P3;
	        switch(key_temp)
	        {
	          case 0x14:   //s14
	               key=8;
				   position++;
	               break;
	
	          case 0x24:   //s15
	               key=9;
				   position++;
	               break;
				   
	          case 0x44:	//s16  ��������
	               change=1;
				   position=0;
	               break;
	
	          case 0x84:	//s17 ȷ������ ���������ʹ��
			  	   if(open)
	               ok=1;
	               break;
	         }
	        while(key_temp!=0xf0)
	         {
	           key_temp=P3;
	           key_temp=key_temp&0xf0;
	           beep=0;
	         }
	         beep=1;
	      }
	      }
		  P3=0xf7;
	    key_temp=P3;
	    key_temp=key_temp&0xf0;
	    if(key_temp!=0xf0)
	    {
	      delay(10);
	      if(key_temp!=0xf0)
	      {
	        key_temp=~P3;
	        switch(key_temp)
	        {
	          case 0x18:   //s18 ��������
			  	   reset=1;
	               break;
	
	          case 0x28://s19 �ر�������
			  	   close=1;
	               break;
			  case 0x48:
			  		if(open)
			  		root=1;	//S20��ʼ������	 ����洢����֮�� ����ʹ�� ���������ʹ��
			  		break;
			  case 0x88:
					admin=1;//S21��������������δ���  ��ʼ������Ϊ���֤����λ������ʹ��111222��
					break;
	         }
	        while(key_temp!=0xf0)
	         {
	           key_temp=P3;
	           key_temp=key_temp&0xf0;
	           beep=0;
	         }
	         beep=1;
	      	}
	      }
		}
	}
	void inputpw()//�������ݷ���
	{
		if(!end)
	 {
		switch(position)	 
		{				//�������븳ֵ  �����������ʾ  �����ж� ���������Ŵ���
			case 1:newpassword1=key; 
			if(!open){
				   v[position-1]=17;
			} else{
				 v[position-1]=key;
			} 	break;
			case 2:newpassword2=key;if(v[position-2]==17){
				   v[position-1]=17;
			} else{
				 v[position-1]=key;
			} 	break;
			case 3:newpassword3=key;if(v[position-3]==17){
				   v[position-1]=17;
			} else{
				 v[position-1]=key;
			} 	break;
			case 4:newpassword4=key;if(v[position-4]==17){
				   v[position-1]=17;
			} else{
				 v[position-1]=key;
			} 	break;
			case 5:newpassword5=key; if(v[position-5]==17){
				   v[position-1]=17;
			} else{
				 v[position-1]=key;
			} 	break;
			case 6:newpassword6=key; 
			if(v[position-6]==17){
				   v[position-1]=17;end=1;number++;//��������ۼ�
			} else{
				 v[position-1]=key;end=1;
			} break;	//������־
		}
	 }
	}
	void check()	  //��֤�����Ƿ���ȷ
	{
		if(end)	 //ֻ�е���λ�����������Ϻ󷽽�����֤
		{
		if((newpassword1==initpassword1)&&(newpassword2==initpassword2)&&(newpassword3==initpassword3)&&(newpassword4==initpassword4)&&(newpassword5==initpassword5)&&(newpassword6==initpassword6)){
		open=1;number=0;
				//�������������ȷ����һ} 
		} 
		}
	}
	void adminmanage(){	//����Ա�������뺯�� //����S21��ϵ����Ա��������Ϊ���֤����λ  111222
			 	at24c02write(0,1);
					at24c02write(1,1);
					at24c02write(2,1);
					at24c02write(3,2);
					at24c02write(4,2);
					at24c02write(5,2);	//ÿ����Ԫ�洢
					initpassword1=at24c02read(0);	
		            initpassword2=at24c02read(1);
					initpassword3=at24c02read(2);
					initpassword4=at24c02read(3);
					initpassword5=at24c02read(4);
					initpassword6=at24c02read(5);
					root=0;
					P1=0xff;   //���
					delay(1000);
					P1=0x0f;   //��ʾ ����ĸ�������ʾ
					delay(1000);
					delay(1000);
					delay(1000);
					delay(1000);
					delay(1000);
				//	P1=0xff;   //���
					close=1;
			  		number=0;//��������������
					admin=0;
	}
	void reatartinpw(){	//��ʼ�����뺯��	//S20��ʼ������
			 	at24c02write(0,0);
					at24c02write(1,0);
					at24c02write(2,0);
					at24c02write(3,0);
					at24c02write(4,0);
					at24c02write(5,0);	//ÿ����Ԫ�洢
					initpassword1=at24c02read(0);	
		            initpassword2=at24c02read(1);
					initpassword3=at24c02read(2);
					initpassword4=at24c02read(3);
					initpassword5=at24c02read(4);
					initpassword6=at24c02read(5);
					root=0;
					P1=0xff;   //���
					delay(1000);
					P1=0xef;   //�����ʾ
					delay(1000);
					delay(1000);
					delay(1000);
					delay(1000);
					delay(1000);
				//	P1=0xff;   //���
					close=1;
	}
	void pwset(){//����ȷ�� ���������뺯�����������룩
	unsigned char flag;
		  	ok=0;
				position=0;
				change=0;
				at24c02write(0,newpassword1);
				at24c02write(1,newpassword2);
				at24c02write(2,newpassword3);
				at24c02write(3,newpassword4);
				at24c02write(4,newpassword5);
				at24c02write(5,newpassword6);//ÿ����Ԫ�洢
				initpassword1=newpassword1;
				initpassword2=newpassword2;
				initpassword3=newpassword3; 
				initpassword4=newpassword4;
				initpassword5=newpassword5;
				initpassword6=newpassword6;	//�������滻�ɹ�
				for(flag=0;flag<6;flag++){
					 v[flag]=16;
				}	 //����Ϣ��
	}
	void resetfun(){//�������뺯��	 //��S18�����£�reset��λ
	unsigned char flag;
		  reset=0; position=0;end=0; 
			for(flag=0;flag<6;flag++){
					 v[flag]=16;
				}	    //����Ϣ��
	
	}
	void closed(){ //�������� //S19�����£�close��λ
	unsigned char flag;
			 	close=0;change=0;//�������㣬��ʼ����
				position=0;	end=0;
				open=0;
				P1=0xff;   //�ص�
				for(flag=0;flag<6;flag++){
					 v[flag]=16;
				}//����Ϣ��
	
	}
	void main()
	{  unsigned char flag;
		init();//��ʼ��	
		initpassword1=at24c02read(0);	
		initpassword2=at24c02read(1);
		initpassword3=at24c02read(2);
		initpassword4=at24c02read(3);
		initpassword5=at24c02read(4);
		initpassword6=at24c02read(5);
		//�ϵ����
		while(1)
		{
			keyscan();//��������ɨ��
			inputpw();//��������ѡ��ɼ�
			check();//������֤
			if(open)//openΪ1������
			{
				P1=0x00;
				if(!change)
					end=0;
			}
		if(number>=3){//���������������
			 for(flag=0;flag<6;flag++){
					 v[flag]=0x76;
				}
			 display();
		}
		  while(admin){//����S21��ϵ����Ա��������Ϊ���֤����λ  111222
			  	adminmanage();//�����������뺯��
			  }
			if(change)//��S16���£�change=1
			{
				if(open)//���򿪣����ܸ�������
				{
					while(!end)//����position����  ����������ѭ��
					{
					 keyscan();
					 inputpw();
						if(reset|close)//��S18����S19������ʱ������   ����͹ر�
						{	end=1;
							break;
						}
						display();
					}
				}
			}
			if(open){//����״̬���ܽ��г�ʼ��
				if(root){//S20��ʼ������
					 reatartinpw();//���ó�ʼ������
					}
			}
			if(ok)//����S17��������ʱ����������λ�������������ʱ�����԰��´˼��������������
			{		  //����ʱ�䰴�´˼���Ч
				 pwset();//���ñ������뺯��
			}
			if(reset)//��S18�����£�reset��λ
			{
			resetfun();//�����������뺯��	
			}
			if(close)//S19�����£�close��λ
			{
				closed();//���ù�������
			}
			display(); //ʵʱ��ʾ
		}
	}