	//密码锁单片机课程设计   5122512020080陈兴华 2021/6/1
	#include<reg52.h>
	#include <intrins.h>
	#define uchar unsigned char	 //宏定义类型
	uchar initpassword1=0,initpassword2=0,initpassword3=0,initpassword4=0,initpassword5=0,initpassword6=0; //由于掉电记忆  密码由最后一次的密码决定  初始0
	uchar newpassword1=0,newpassword2=0,newpassword3=0,newpassword4=0,newpassword5=0,newpassword6=0;  //新密码输入 可以经过断电存储		初始0
	uchar position,key,key_temp,number;  //定义判断位置、值选择、暂存值、输入次数
	unsigned int v[]={16,16,16,16,16,16};	//送入数码管灭灯不显示标识初始状态
	unsigned char code wei[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf};  //位选
	//uchar value=0;
	//I2C总线传送
	sbit sda=P2^0;   //数据线         
	sbit scl=P2^1;	 //时钟线
	bit open,change,ok,end,reset,close,root,admin;	 //开锁、更改密码、确定、完毕、重设、关闭、初始化密码为0、重置密码（身份证后六位也行）
	sbit dula=P2^6;	 //段选
	sbit wela=P2^7;		   //位选
	sbit beep=P2^3;				 //蜂鸣器
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
	0x00,														//"熄灭、灭灯"	 16
	0x40														  //"-"类似加密操作	   17
	};	  //数码管的各个值
	//掉电记忆使用EEPROM存储 可实现掉电记忆 I2C总线传送方式
	void delay(unsigned char i)	//延时函数
	{
		uchar j,k;
	  for(j=i;j>0;j--)
	    for(k=125;k>0;k--);
	}
	void nop()	//微秒延时
	{
		_nop_();
		_nop_();
	}
	
	void delay1(unsigned int m)	// 24c02驱动读写的延时
	{	unsigned int n;
	  	for(n=0;n<m;n++);
	}
	void init()  //24c02初始化子程序
	{
		scl=1;
		nop();
		sda=1;
		nop();
	}
	void start()        //启动I2C总线
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
	void stop()         //停止I2C总线
	{
		sda=0;
		nop();
		scl=1;
		nop();
		sda=1;
		nop();
	}
	void writebyte(unsigned char j)  //写入一个字节
	{
		unsigned char i,temp;
	   	temp=j;
	   	for (i=0;i<8;i++)
	   {
		   scl=0;
			_nop_();
			if((temp&0x80)==0x80)
				sda=1;	  //置一
			else
				sda=0;	  //置零
			scl=1;
			_nop_();
			temp<<=1;
	   }
	   scl=0;
	   nop();
	   sda=1;
	   nop();
	}
	unsigned char readbyte()  //读一个字节
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
	
	void clock()   //I2C总线时钟
	{
	   unsigned char i=0;
	   scl=1;
	   nop();
	   while((sda==1)&&(i<255))
	   	  i++;
	   scl=0;
	   nop();
	}
	//指定地址读取一个字节//
	unsigned char at24c02read(unsigned char address)
	{
	   unsigned char i;
	   start();
	   writebyte(0xa0);	 	  //指定地址读操作
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
	//写入一字节//
	void at24c02write(unsigned char address,unsigned char info)
	{
	   start();
	   writebyte(0xa0);	 //指定地址写入
	   clock();
	   writebyte(address);
	   clock();
	   writebyte(info);
	   clock();
	   stop();
	   delay1(5000); 
	}
	void display()	//显示函
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
	  	if(P3!=0xf0){//有按键按下	
	    P3=0xfe;
	    key_temp=P3;
	    key_temp=key_temp&0xf0;
	    if(key_temp!=0xf0)
	    {
	      delay(10);
	      if(key_temp!=0xf0)
	      {	
	        key_temp=~P3;  //取反
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
				   
	          case 0x44:	//s16  更改密码
	               change=1;
				   position=0;
	               break;
	
	          case 0x84:	//s17 确认密码 开锁后才能使用
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
	          case 0x18:   //s18 重设密码
			  	   reset=1;
	               break;
	
	          case 0x28://s19 关闭密码锁
			  	   close=1;
	               break;
			  case 0x48:
			  		if(open)
			  		root=1;	//S20初始化密码	 掉电存储忘记之后 可以使用 开锁后才能使用
			  		break;
			  case 0x88:
					admin=1;//S21解决输入密码三次错误  初始化密码为身份证后六位（这里使用111222）
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
	void inputpw()//输入数据分配
	{
		if(!end)
	 {
		switch(position)	 
		{				//给新密码赋值  对于密码的显示  进行判断 用其他符号代替
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
				   v[position-1]=17;end=1;number++;//输入次数累加
			} else{
				 v[position-1]=key;end=1;
			} break;	//结束标志
		}
	 }
	}
	void check()	  //验证密码是否正确
	{
		if(end)	 //只有当六位密码均输入完毕后方进行验证
		{
		if((newpassword1==initpassword1)&&(newpassword2==initpassword2)&&(newpassword3==initpassword3)&&(newpassword4==initpassword4)&&(newpassword5==initpassword5)&&(newpassword6==initpassword6)){
		open=1;number=0;
				//当输入的密码正确，置一} 
		} 
		}
	}
	void adminmanage(){	//管理员重置密码函数 //按下S21联系管理员更改密码为身份证后六位  111222
			 	at24c02write(0,1);
					at24c02write(1,1);
					at24c02write(2,1);
					at24c02write(3,2);
					at24c02write(4,2);
					at24c02write(5,2);	//每个单元存储
					initpassword1=at24c02read(0);	
		            initpassword2=at24c02read(1);
					initpassword3=at24c02read(2);
					initpassword4=at24c02read(3);
					initpassword5=at24c02read(4);
					initpassword6=at24c02read(5);
					root=0;
					P1=0xff;   //灭灯
					delay(1000);
					P1=0x0f;   //提示 左边四个灯亮提示
					delay(1000);
					delay(1000);
					delay(1000);
					delay(1000);
					delay(1000);
				//	P1=0xff;   //灭灯
					close=1;
			  		number=0;//输入错误次数置零
					admin=0;
	}
	void reatartinpw(){	//初始化密码函数	//S20初始化密码
			 	at24c02write(0,0);
					at24c02write(1,0);
					at24c02write(2,0);
					at24c02write(3,0);
					at24c02write(4,0);
					at24c02write(5,0);	//每个单元存储
					initpassword1=at24c02read(0);	
		            initpassword2=at24c02read(1);
					initpassword3=at24c02read(2);
					initpassword4=at24c02read(3);
					initpassword5=at24c02read(4);
					initpassword6=at24c02read(5);
					root=0;
					P1=0xff;   //灭灯
					delay(1000);
					P1=0xef;   //红灯提示
					delay(1000);
					delay(1000);
					delay(1000);
					delay(1000);
					delay(1000);
				//	P1=0xff;   //灭灯
					close=1;
	}
	void pwset(){//密码确认 保存新密码函数（保存密码）
	unsigned char flag;
		  	ok=0;
				position=0;
				change=0;
				at24c02write(0,newpassword1);
				at24c02write(1,newpassword2);
				at24c02write(2,newpassword3);
				at24c02write(3,newpassword4);
				at24c02write(4,newpassword5);
				at24c02write(5,newpassword6);//每个单元存储
				initpassword1=newpassword1;
				initpassword2=newpassword2;
				initpassword3=newpassword3; 
				initpassword4=newpassword4;
				initpassword5=newpassword5;
				initpassword6=newpassword6;	//新密码替换成功
				for(flag=0;flag<6;flag++){
					 v[flag]=16;
				}	 //重新息屏
	}
	void resetfun(){//重新输入函数	 //当S18被按下，reset置位
	unsigned char flag;
		  reset=0; position=0;end=0; 
			for(flag=0;flag<6;flag++){
					 v[flag]=16;
				}	    //重新息屏
	
	}
	void closed(){ //关锁函数 //S19键按下，close置位
	unsigned char flag;
			 	close=0;change=0;//变量清零，初始化。
				position=0;	end=0;
				open=0;
				P1=0xff;   //关灯
				for(flag=0;flag<6;flag++){
					 v[flag]=16;
				}//重新息屏
	
	}
	void main()
	{  unsigned char flag;
		init();//初始化	
		initpassword1=at24c02read(0);	
		initpassword2=at24c02read(1);
		initpassword3=at24c02read(2);
		initpassword4=at24c02read(3);
		initpassword5=at24c02read(4);
		initpassword6=at24c02read(5);
		//断电记忆
		while(1)
		{
			keyscan();//输入行列扫描
			inputpw();//输入数据选择采集
			check();//开锁验证
			if(open)//open为1，则开锁
			{
				P1=0x00;
				if(!change)
					end=0;
			}
		if(number>=3){//三次输入密码错误
			 for(flag=0;flag<6;flag++){
					 v[flag]=0x76;
				}
			 display();
		}
		  while(admin){//按下S21联系管理员更改密码为身份证后六位  111222
			  	adminmanage();//调用重置密码函数
			  }
			if(change)//当S16按下，change=1
			{
				if(open)//锁打开，才能更改密码
				{
					while(!end)//根据position次数  必须有六次循环
					{
					 keyscan();
					 inputpw();
						if(reset|close)//当S18或者S19被按下时，跳出   重设和关闭
						{	end=1;
							break;
						}
						display();
					}
				}
			}
			if(open){//开锁状态才能进行初始化
				if(root){//S20初始化密码
					 reatartinpw();//调用初始化函数
					}
			}
			if(ok)//按下S17更改密码时，当所有六位新密码均被按下时，可以按下此键，结束密码更改
			{		  //其他时间按下此键无效
				 pwset();//调用保存密码函数
			}
			if(reset)//当S18被按下，reset置位
			{
			resetfun();//调用重新输入函数	
			}
			if(close)//S19键按下，close置位
			{
				closed();//调用关锁函数
			}
			display(); //实时显示
		}
	}