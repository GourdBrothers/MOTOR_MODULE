/*  文件名：TEST_61F02x_ADC.C
*	功能：  FT61F02x-ADC功能演示
*   IC:    FT61F023 SOP16
*   晶振：  16M/2T                    
*   说明：  程序根据AN2(PA2)口的电压来调节PWM3P(PC4)的占空比
*			AN2脚电压越高(0-VDD)PWM3P(PC2)占空比越大(2K,0-99%)	
*
*               FT61F023  SOP16 
*               ---------------
*  VDD---------|1(VDD)  (VSS)16|--------GND     
*  NC----------|2(PA7)  (PA0)15|---------NC 
*  NC----------|3(PA6)  (PA1)14|---------NC
*  NC----------|4(PA5)  (PA2)13|--------AN2
*  NC----------|5(PC3)  (PA3)12|---------NC
*  NC----------|6(PC2)  (PC0)11|---------NC
*  NC----------|7(PA4)  (PC1)10|---------NC
*  NC----------|8(PC5)  (PC4)09|-------PWM3
*			    ---------------
*/
#include "SYSCFG.h"


//**********************************************************
//*************************宏定义***************************
#define  unchar     unsigned char 
#define  unint      unsigned int
#define  unlong     unsigned long

//PWM引脚输入输出控制
#define  PWM3Dir  	TRISC4

volatile unint    	TestADC;
/*-------------------------------------------------
 *  函数名：POWER_INITIAL
 *	功能：  上电系统初始化
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/	
void POWER_INITIAL (void) 
{ 
    OSCCON = 0B01110001;	//WDT 32KHZ IRCF=111=16MHZ/2=8MHZ,0.125US/T
					 		//Bit0=1,系统时钟为内部振荡器
					 		//Bit0=0,时钟源由FOSC<2：0>决定即编译选项时选择
	INTCON = 0;  			//暂禁止所有中断
	PORTA = 0B00000000;		
	TRISA = 0B11111111;		//PA全部为输入
	PORTC = 0B00000000; 	
	TRISC = 0B11111111;		//PC全部输入

	WPUA = 0;          		//禁止所有PA口上拉
	WPUC = 0;       		//禁止所有PC口上拉
    
	OPTION = 0B00001000;	//Bit3=1 WDT MODE,PS=000=1:1 WDT RATE
					 		//Bit7(PAPU)=0 ENABLED PULL UP PA
	MSCKCON = 0B00000000;	//Bit6->0,禁止PA4，PC5稳压输出
					  		//Bit5->0,TIMER2时钟为Fosc
					  		//Bit4->0,禁止LVR       
	CMCON0 = 0B00000111; 	//关闭比较器，CxIN为数字IO口
}
/*------------------------------------------------- 
 *	函数名称：DelayUs
 *	功能：   短延时函数 --16M-2T--大概快1%左右.
 *	输入参数：Time 延时时间长度 延时时长Time Us
 *	返回参数：无 
 -------------------------------------------------*/
void DelayUs(unsigned char Time)
{
	unsigned char a;
	for(a=0;a<Time;a++)
	{
		NOP();
	}
}  
/*-------------------------------------------------
 *  函数名:  ADC_INITIAL
 *	功能：  ADC初始化
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/
void ADC_INITIAL (void) 
{
	//ADCON1 = 0B00010000; 	//DIVS=0,时钟选FOSC
					   		//ADCS[2:0]=001,分频FOSC/8
	//ADCON1 = 0B01010000; 	//DIVS=0,时钟选FOSC
					   		//ADCS[2:0]=101,分频FOSC/16
	ADCON1 = 0B01100000; 	//DIVS=0,时钟选FOSC
					   		//ADCS[2:0]=110,分频FOSC/64
	ADCON0 = 0B10011101; 	//B7,ADFM=1,结果右对齐                     
					   		//B6:5,VCFG=00,参考电压VDD
					   		//B6:5,VCFG=01,参考电压内部2V
					   		//B6:5,VCFG=11,参考电压Vref
					   		//B4:2,CHS=010,选择AN2通道
					   		//B1,GO,AD转换状态位
					   		//B0,ADON=1,ADC使能
	ANSEL = 0B00000100;   	//使能AN2为模拟输入	
}                      
/*-------------------------------------------------
 *  函数名: GET_ADC_DATA
 *	功能：  读取通道ADC值
 *  输入：  AN_CN 通道序号
 *  输出：  INT类型AD值(单次采样无滤波)
 --------------------------------------------------*/
unint GET_ADC_DATA (unchar AN_CH) 
{ 
	unchar i;
	unchar 	ADCON0Buff; 
	unint  tBuffer = 0;
	unint  ADC_DATA=0;
	 
	ADCON0Buff = ADCON0 & 0B11100011; 	//清空通道值

	AN_CH <<=2;              
	ADCON0Buff |=  AN_CH;   			//(BIT4-BIT2)CHS=010,选择AN2通道      

	ADCON0 = ADCON0Buff; 				//重新加载通道值
    DelayUs(20); 
	GO_DONE = 1;             			//启动ADC 
	while( GO_DONE==1 );    			//等待ADC转换完成

	ADC_DATA =  ADRESH;
	ADC_DATA <<=8;
	ADC_DATA |= ADRESL;      			//10Bit ADC值整合
	tBuffer =  ADC_DATA;
	return  tBuffer;
} 
/*-------------------------------------------------
 *  函数名：PWM_INITIAL
 *	功能：  PWM初始化
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/
void PWM_INITIAL (void) 
{
/*
*将相关的TRIS位置1禁止PWMx引脚的输出驱动器；
*装裁PWMxCR0寄存器以设置PWMx周期，中断产生方式以及选择时钟源；
*用适当的值装载PWMxCR1寄存器配置为PWM模式，合适的预分频比；
*装载PRx寄存器设置PWM占空比；
*配置并启动TIMERx：
	将PWMxCR1寄存器的TMRXIF中断标志位清零
	将PWMxCR1寄存器的TMRxON位置1启动TIMERx
*重新开始一个PWM周期后，使能PWM输出：
	等待TIMERx溢出（PWMxCR1寄存器的TMRxIF位置1）
	将相关的TRIS位清零使能PWMx引脚的输出驱动器
*PWM周期=2^TMRxPS * 2^PxPER * [(TxCKDIV+1)/PWM时钟源]
*/
	//-------------------------------
	PWM3Dir =1; 			//PWM3输出PIN暂为输入模式
	//-------------------------------
	PWM3CR0 = 0B00110010; 	//Bit7 禁止中断
							//Bit6:4 周期位选择011:8位
							//Bit3:1 时钟选择001:内部RC快时钟/(T3CKDIV+1)
							//Bit0	PWM 输出						
    //PWM3CR1 = 0;
	PWM3CR1 = 0B10000000;
                   			//Bit7 =1，TMR3为PWM/BUZZER模式
	               			//Bit6 =0，PWM3为高电平有效
	               			//Bit5:3 =000，PWM3预分频比设置为1:1
	               			//Bit2 =0,暂关TMR3
	               			//Bit1 =0,禁止TMR3中断
	               			//Bit0 =0，TMR3中断标志位只读
	TMR3H=0;
	T3CKDIV = 30; 			//周期 = 2^0*2^8*[(T3CKDIV+1)/16000000] =(T3CKDIV+1)*16us
				  			//31*16us = 0.496ms ≈ 2KHz
	PR3L = 128;				//占空比 50%
}
/*-------------------------------------------------
 *  函数名:  main
 *	功能：  主函数
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/
void main()
{
	POWER_INITIAL();		        //系统初始化
	ADC_INITIAL();			        //ADC初始化	
	PWM_INITIAL();
	TMR3ON = 1;				        //T3开启PWM3
	PWM3Dir = 0; 			        //PWM3 PIN设为输出模式允许PWM输出
	//ADCON0 = 0B10001001; 
	while(1)
	{
		TestADC = GET_ADC_DATA(2); 	//通道2AD值
		PR3L =  TestADC >> 2; 		//将AD值赋值给PWM的PR3L调节PWM3P输出的占空比
							
		NOP();
		NOP();	 
	}
}