/*  �ļ�����TEST_61F02x_ADC.C
*	���ܣ�  FT61F02x-ADC������ʾ
*   IC:    FT61F023 SOP16
*   ����  16M/2T                    
*   ˵����  �������AN2(PA2)�ڵĵ�ѹ������PWM3P(PC4)��ռ�ձ�
*			AN2�ŵ�ѹԽ��(0-VDD)PWM3P(PC2)ռ�ձ�Խ��(2K,0-99%)	
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
//*************************�궨��***************************
#define  unchar     unsigned char 
#define  unint      unsigned int
#define  unlong     unsigned long

//PWM���������������
#define  PWM3Dir  	TRISC4

volatile unint    	TestADC;
/*-------------------------------------------------
 *  ��������POWER_INITIAL
 *	���ܣ�  �ϵ�ϵͳ��ʼ��
 *  ���룺  ��
 *  �����  ��
 --------------------------------------------------*/	
void POWER_INITIAL (void) 
{ 
    OSCCON = 0B01110001;	//WDT 32KHZ IRCF=111=16MHZ/2=8MHZ,0.125US/T
					 		//Bit0=1,ϵͳʱ��Ϊ�ڲ�����
					 		//Bit0=0,ʱ��Դ��FOSC<2��0>����������ѡ��ʱѡ��
	INTCON = 0;  			//�ݽ�ֹ�����ж�
	PORTA = 0B00000000;		
	TRISA = 0B11111111;		//PAȫ��Ϊ����
	PORTC = 0B00000000; 	
	TRISC = 0B11111111;		//PCȫ������

	WPUA = 0;          		//��ֹ����PA������
	WPUC = 0;       		//��ֹ����PC������
    
	OPTION = 0B00001000;	//Bit3=1 WDT MODE,PS=000=1:1 WDT RATE
					 		//Bit7(PAPU)=0 ENABLED PULL UP PA
	MSCKCON = 0B00000000;	//Bit6->0,��ֹPA4��PC5��ѹ���
					  		//Bit5->0,TIMER2ʱ��ΪFosc
					  		//Bit4->0,��ֹLVR       
	CMCON0 = 0B00000111; 	//�رձȽ�����CxINΪ����IO��
}
/*------------------------------------------------- 
 *	�������ƣ�DelayUs
 *	���ܣ�   ����ʱ���� --16M-2T--��ſ�1%����.
 *	���������Time ��ʱʱ�䳤�� ��ʱʱ��Time Us
 *	���ز������� 
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
 *  ������:  ADC_INITIAL
 *	���ܣ�  ADC��ʼ��
 *  ���룺  ��
 *  �����  ��
 --------------------------------------------------*/
void ADC_INITIAL (void) 
{
	//ADCON1 = 0B00010000; 	//DIVS=0,ʱ��ѡFOSC
					   		//ADCS[2:0]=001,��ƵFOSC/8
	//ADCON1 = 0B01010000; 	//DIVS=0,ʱ��ѡFOSC
					   		//ADCS[2:0]=101,��ƵFOSC/16
	ADCON1 = 0B01100000; 	//DIVS=0,ʱ��ѡFOSC
					   		//ADCS[2:0]=110,��ƵFOSC/64
	ADCON0 = 0B10011101; 	//B7,ADFM=1,����Ҷ���                     
					   		//B6:5,VCFG=00,�ο���ѹVDD
					   		//B6:5,VCFG=01,�ο���ѹ�ڲ�2V
					   		//B6:5,VCFG=11,�ο���ѹVref
					   		//B4:2,CHS=010,ѡ��AN2ͨ��
					   		//B1,GO,ADת��״̬λ
					   		//B0,ADON=1,ADCʹ��
	ANSEL = 0B00000100;   	//ʹ��AN2Ϊģ������	
}                      
/*-------------------------------------------------
 *  ������: GET_ADC_DATA
 *	���ܣ�  ��ȡͨ��ADCֵ
 *  ���룺  AN_CN ͨ�����
 *  �����  INT����ADֵ(���β������˲�)
 --------------------------------------------------*/
unint GET_ADC_DATA (unchar AN_CH) 
{ 
	unchar i;
	unchar 	ADCON0Buff; 
	unint  tBuffer = 0;
	unint  ADC_DATA=0;
	 
	ADCON0Buff = ADCON0 & 0B11100011; 	//���ͨ��ֵ

	AN_CH <<=2;              
	ADCON0Buff |=  AN_CH;   			//(BIT4-BIT2)CHS=010,ѡ��AN2ͨ��      

	ADCON0 = ADCON0Buff; 				//���¼���ͨ��ֵ
    DelayUs(20); 
	GO_DONE = 1;             			//����ADC 
	while( GO_DONE==1 );    			//�ȴ�ADCת�����

	ADC_DATA =  ADRESH;
	ADC_DATA <<=8;
	ADC_DATA |= ADRESL;      			//10Bit ADCֵ����
	tBuffer =  ADC_DATA;
	return  tBuffer;
} 
/*-------------------------------------------------
 *  ��������PWM_INITIAL
 *	���ܣ�  PWM��ʼ��
 *  ���룺  ��
 *  �����  ��
 --------------------------------------------------*/
void PWM_INITIAL (void) 
{
/*
*����ص�TRISλ��1��ֹPWMx���ŵ������������
*װ��PWMxCR0�Ĵ���������PWMx���ڣ��жϲ�����ʽ�Լ�ѡ��ʱ��Դ��
*���ʵ���ֵװ��PWMxCR1�Ĵ�������ΪPWMģʽ�����ʵ�Ԥ��Ƶ�ȣ�
*װ��PRx�Ĵ�������PWMռ�ձȣ�
*���ò�����TIMERx��
	��PWMxCR1�Ĵ�����TMRXIF�жϱ�־λ����
	��PWMxCR1�Ĵ�����TMRxONλ��1����TIMERx
*���¿�ʼһ��PWM���ں�ʹ��PWM�����
	�ȴ�TIMERx�����PWMxCR1�Ĵ�����TMRxIFλ��1��
	����ص�TRISλ����ʹ��PWMx���ŵ����������
*PWM����=2^TMRxPS * 2^PxPER * [(TxCKDIV+1)/PWMʱ��Դ]
*/
	//-------------------------------
	PWM3Dir =1; 			//PWM3���PIN��Ϊ����ģʽ
	//-------------------------------
	PWM3CR0 = 0B00110010; 	//Bit7 ��ֹ�ж�
							//Bit6:4 ����λѡ��011:8λ
							//Bit3:1 ʱ��ѡ��001:�ڲ�RC��ʱ��/(T3CKDIV+1)
							//Bit0	PWM ���						
    //PWM3CR1 = 0;
	PWM3CR1 = 0B10000000;
                   			//Bit7 =1��TMR3ΪPWM/BUZZERģʽ
	               			//Bit6 =0��PWM3Ϊ�ߵ�ƽ��Ч
	               			//Bit5:3 =000��PWM3Ԥ��Ƶ������Ϊ1:1
	               			//Bit2 =0,�ݹ�TMR3
	               			//Bit1 =0,��ֹTMR3�ж�
	               			//Bit0 =0��TMR3�жϱ�־λֻ��
	TMR3H=0;
	T3CKDIV = 30; 			//���� = 2^0*2^8*[(T3CKDIV+1)/16000000] =(T3CKDIV+1)*16us
				  			//31*16us = 0.496ms �� 2KHz
	PR3L = 128;				//ռ�ձ� 50%
}
/*-------------------------------------------------
 *  ������:  main
 *	���ܣ�  ������
 *  ���룺  ��
 *  �����  ��
 --------------------------------------------------*/
void main()
{
	POWER_INITIAL();		        //ϵͳ��ʼ��
	ADC_INITIAL();			        //ADC��ʼ��	
	PWM_INITIAL();
	TMR3ON = 1;				        //T3����PWM3
	PWM3Dir = 0; 			        //PWM3 PIN��Ϊ���ģʽ����PWM���
	//ADCON0 = 0B10001001; 
	while(1)
	{
		TestADC = GET_ADC_DATA(2); 	//ͨ��2ADֵ
		PR3L =  TestADC >> 2; 		//��ADֵ��ֵ��PWM��PR3L����PWM3P�����ռ�ձ�
							
		NOP();
		NOP();	 
	}
}