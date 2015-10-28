
/*****************************************************************
*
*
*
******************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <includes.h>
#ifdef   _MBUS_DETACH_
#define I2C_M_RD		(1 << 0)	/* read data, from slave to master */
#define I2C_M_SIM           (1 << 1)
#define I2C_M_GSTOP       (1 << 2)

OS_EVENT *I2cMsgRetSem;
OS_EVENT *I2cMsgSem;
 struct i2c_msg_t
{
   uint16 addr;
   uint16 subAddr;
   uint16 flag;
   uint16 len;
   uint16 msg_ptr;
   uint8  buf[255]; 
   uint16 result;
 };

struct i2c_msg_t i2c_msg;

void I2C_delay(void)		  //�����ʱ���Ǿ�ȷ��
{	
   uint16_t i=20000;     
   while(i) 
   { 
     i--; 
   } 
}
/*******************************************************************************
* Function Name  : I2C_WriteByte
* Description    : дһ�ֽ�����
* Input          : - SendByte: ��д������
*           	   - WriteAddress: ��д���ַ
*                  - DeviceAddress: ��������
* Output         : None
* Return         : ����Ϊ:=1�ɹ�д��,=0ʧ��
*******************************************************************************/           
uint8 I2C_Write(uint8 *pSendByte, uint8 len, uint8 DeviceAddress)
{	
 uint8 err;
  OSSemPend(I2cMsgSem, 0, &err);
  i2c_msg.addr = (DeviceAddress & 0xFE);
  i2c_msg.flag = I2C_M_GSTOP;
  i2c_msg.msg_ptr = 0;
  i2c_msg.len = len;
  i2c_msg.result = 0;
  memcpy(i2c_msg.buf,pSendByte,len);
  I2C_ITConfig(I2C1, I2C_IT_BUF, ENABLE);
  I2C_ITConfig(I2C1, I2C_IT_EVT, ENABLE);
  I2C_AcknowledgeConfig(I2C1, ENABLE);
   while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
  I2C_GenerateSTART(I2C1, ENABLE);
  OSSemPend(I2cMsgRetSem, 0, &err);
  OSSemPost(I2cMsgSem);
   I2C_delay();			 
   return i2c_msg.result;
}									 

/*******************************************************************************
* Function Name  : I2C_ReadByte
* Description    : ��ȡһ������
* Input          : - pBuffer: ��Ŷ�������
*           	   - length: ����������
*                  - ReadAddress: ��������ַ
*                  - DeviceAddress: ��������
* Output         : None
* Return         : ����Ϊ:=1�ɹ�����,=0ʧ��
*******************************************************************************/          
uint8 I2C_ReadByte(uint8* pBuffer,   uint8 length,   uint8 ReadAddress,  uint8 DeviceAddress)
{		

 uint8 err;
  OSSemPend(I2cMsgSem, 0, &err);
  i2c_msg.addr = (DeviceAddress & 0xFE);
  i2c_msg.flag = I2C_M_SIM;
  i2c_msg.msg_ptr = 0;
  i2c_msg.len = length;
   i2c_msg.buf[0] = ReadAddress & 0xF0;//yaotong 20131105
   	I2C_ITConfig(I2C1, I2C_IT_BUF, ENABLE);
	 I2C_ITConfig(I2C1, I2C_IT_EVT, ENABLE);
   I2C_AcknowledgeConfig(I2C1, ENABLE);	
   while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
   I2C_GenerateSTART(I2C1, ENABLE);  
   OSSemPend(I2cMsgRetSem, 0, &err);	 
   OSSemPost(I2cMsgSem);
   memcpy(pBuffer,i2c_msg.buf+1,length - 1);
   
   
   return i2c_msg.result;
}

/**
  * @brief  This function handles I2C1 Event interrupt request.
  * @param  None
  * @retval : None
  */
void I2C1_EV_IRQHandler(void)
{
  u32 u32Event;
  u32Event = I2C_GetLastEvent(I2C1);
  switch (u32Event)
  {
    case I2C_EVENT_MASTER_MODE_SELECT:                 /* EV5 */

	if(!(i2c_msg.flag & I2C_M_RD))
      {
        /* Master Transmitter ----------------------------------------------*/
        I2C_Send7bitAddress(I2C1, i2c_msg.addr, I2C_Direction_Transmitter);
      }
      else
      {
        /* Master Receiver -------------------------------------------------*/  
        I2C_Send7bitAddress(I2C1, i2c_msg.addr, I2C_Direction_Receiver); 
      }
      break;
        
    /* Master Transmitter --------------------------------------------------*/
    case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:  
      
 	   I2C_SendData(I2C1, i2c_msg.buf[i2c_msg.msg_ptr++]); 
	   if(i2c_msg.flag & I2C_M_SIM)
	   	I2C_ITConfig(I2C1, I2C_IT_BUF, DISABLE);
	   if(i2c_msg.len == 1)
	   	I2C_ITConfig(I2C1, I2C_IT_BUF, DISABLE);
      break;

    /* Test on I2C1 EV8 and clear it */
  case I2C_EVENT_MASTER_BYTE_TRANSMITTING:  /* Without BTF, EV8 */     
      if(i2c_msg.msg_ptr < i2c_msg.len)
      {
        /* Transmit I2C1 data */
        I2C_SendData(I2C1, i2c_msg.buf[i2c_msg.msg_ptr++]);
      
      }
      else
      {
        I2C_ITConfig(I2C1, I2C_IT_BUF, DISABLE);

      }        
      break;

    case I2C_EVENT_MASTER_BYTE_TRANSMITTED: /* With BTF EV8-2 */
	
	 if(i2c_msg.flag & I2C_M_GSTOP)
	 {
	   
        I2C_GenerateSTOP(I2C1, ENABLE);	
	    i2c_msg.result = 1;
	    OSSemPost(I2cMsgRetSem);
	 }
	 else
	 {
	    i2c_msg.flag = I2C_M_RD;	   
	    I2C_ITConfig(I2C1, I2C_IT_BUF, ENABLE);
	    I2C_GenerateSTART(I2C1, ENABLE);
	 }	
      break;

    /* Master Receiver -------------------------------------------------------*/
    case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:
 	 if((i2c_msg.msg_ptr + 1) == (i2c_msg.len ))
     {
       /* Disable I2C1 acknowledgement */
       I2C_AcknowledgeConfig(I2C1, DISABLE);
	    I2C_GenerateSTOP(I2C1, ENABLE); //��ģʽ
     }
      break;

   /* Test on I2C1 EV7 and clear it */
   case I2C_EVENT_MASTER_BYTE_RECEIVED: 	
     i2c_msg.buf[i2c_msg.msg_ptr++] = I2C_ReceiveData (I2C1); 	   
	  if((i2c_msg.msg_ptr + 1) == (i2c_msg.len ))
     {
       /* Disable I2C1 acknowledgement */
       I2C_AcknowledgeConfig(I2C1, DISABLE);
	    I2C_GenerateSTOP(I2C1, ENABLE); //��ģʽ
     }
      if((i2c_msg.msg_ptr ) == (i2c_msg.len ))
     {
     
	   i2c_msg.result = 1;
	   OSSemPost(I2cMsgRetSem);
     }
     /* Disable ACK and send I2C1 STOP condition before receiving the last data */	  
	 
     break;
   case 0x40:
   	  i2c_msg.buf[i2c_msg.msg_ptr] = I2C_ReceiveData (I2C1); 
     if((i2c_msg.msg_ptr +1) >= (i2c_msg.len ))
     {
       /* Disable I2C1 acknowledgement */
		 i2c_msg.result = 1;
	   OSSemPost(I2cMsgRetSem);
     } 
	 break;
    default:
	I2C_ReadRegister(I2C1, I2C_Register_SR1);
	I2C_ReadRegister(I2C1, I2C_Register_SR2);
      break;
  }
}
#else
/* Private define ------------------------------------------------------------*/
#define SCL_H         GPIO_SetBits(GPIOB , GPIO_Pin_6)
#define SCL_L         GPIO_ResetBits(GPIOB , GPIO_Pin_6)
   
#define SDA_H         GPIO_SetBits(GPIOB , GPIO_Pin_7)
#define SDA_L         GPIO_ResetBits(GPIOB , GPIO_Pin_7)

#define SDA_read      GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_7)

//ʹ��SysTick����ͨ����ģʽ���ӳٽ��й���
//����delay_us,delay_ms 
//static uint8  fac_us = 0;  //us��ʱ������
//static uint16 fac_ms = 0;  //ms��ʱ������

/*
//��ʼ���ӳٺ���
void delay_init(uint8 SYSCLK)
{
 SysTick->CTRL &= 0xfffffffb; //ѡ���ڲ�ʱ�� HCLK/8
 fac_us = SYSCLK / 8;     
 fac_ms = (uint16)fac_us * 1000;
} 
          
//��ʱNms
//ע��Nms�ķ�Χ
//Nms<=0xffffff*8/SYSCLK
//��72M������,Nms<=1864
void delay_ms(uint16 nms)
{   
  SysTick->LOAD = (u32)nms * fac_ms;   //ʱ����� 
  SysTick->CTRL |= 0x01;               //��ʼ����   
  while(!(SysTick->CTRL & (1<<16)));   //�ȴ�ʱ�䵽��
  SysTick->CTRL &= 0XFFFFFFFE;         //�رռ�����
  SysTick->VAL = 0X00000000;           //��ռ�����    
} 
 
//��ʱus          
void delay_us(u32 Nus)
{ 
  SysTick->LOAD = Nus*fac_us;          //ʱ�����     
  SysTick->CTRL |= 0x01;               //��ʼ����   
  while(!(SysTick->CTRL & (1<<16)));   //�ȴ�ʱ�䵽��
  SysTick->CTRL = 0X00000000;          //�رռ�����
  SysTick->VAL = 0X00000000;           //��ռ�����    
}*/  

void I2C_delay(void)		  //�����ʱ���Ǿ�ȷ��
{	
   uint8_t i=200;     
   while(i) 
   { 
     i--; 
   } 
}

uint8 I2C_Start(void)	 //I2C��ʼλ
{
	SDA_H;
	SCL_H;
	I2C_delay();
	if(!SDA_read)
	  return 0;	                    //SDA��Ϊ�͵�ƽ������æ,�˳�
	SDA_L;
	I2C_delay();
	if(SDA_read) 
	  return 0;	                    //SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
	SDA_L;							//SCLΪ�ߵ�ƽʱ��SDA���½��ر�ʾֹͣλ
	I2C_delay();
	return 1;
}

void I2C_Stop(void)			   //I2Cֹͣλ
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;					   //SCLΪ�ߵ�ƽʱ��SDA�������ر�ʾֹͣλ
	I2C_delay();
}

static void I2C_Ack(void)		//I2C��Ӧλ
{	
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

static void I2C_NoAck(void)		//I2C����Ӧλ
{	
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

uint8 I2C_WaitAck(void) 	  //I2C�ȴ�Ӧ��λ
{
	SCL_L;
	I2C_delay();
	SDA_H;			
	I2C_delay();
	SCL_H;
	I2C_delay();
	if(SDA_read)
	{
      SCL_L;
      return 0;
	}
	SCL_L;
	return 1;
}

 /*******************************************************************************
* Function Name  : I2C_SendByte
* Description    : ���ݴӸ�λ����λ
* Input          : - SendByte: ���͵�����
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_SendByte(uint8 SendByte) 
{
    uint8 i;

    for(i = 0;i < 8; i++) 
    {
      SCL_L;
      I2C_delay();
      if(SendByte & 0x80)
        SDA_H;  			  //��SCLΪ�͵�ƽʱ������SDA���ݸı�
      else 
        SDA_L;   
      SendByte <<= 1;
      I2C_delay();
      SCL_H;
      I2C_delay();
    }
    SCL_L;
}

/*******************************************************************************
* Function Name  : I2C_ReceiveByte
* Description    : ���ݴӸ�λ����λ
* Input          : None
* Output         : None
* Return         : I2C���߷��ص�����
*******************************************************************************/
uint8 I2C_ReceiveByte(void)  
{ 
    uint8 i,ReceiveByte = 0;

    SDA_H;				
    for(i = 0;i < 8; i++) 
    {
      ReceiveByte <<= 1;      
      SCL_L;
      I2C_delay();
	  SCL_H;
      I2C_delay();	
      if(SDA_read)				   //��SCLΪ�ߵ�ƽʱ��SDA�ϵ����ݱ��ֲ��䣬���Զ�����
      {
        ReceiveByte |= 0x01;
      }
    }
    SCL_L;
    return ReceiveByte;
}

/*******************************************************************************
* Function Name  : I2C_WriteByte
* Description    : дһ�ֽ�����
* Input          : - SendByte: ��д������
*           	   - WriteAddress: ��д���ַ
*                  - DeviceAddress: ��������
* Output         : None
* Return         : ����Ϊ:=1�ɹ�д��,=0ʧ��
*******************************************************************************/           
uint8 I2C_Write(uint8 *pSendByte, uint8 len, uint8 DeviceAddress)
{	
	
    if(!I2C_Start())
	  return 0;

    I2C_SendByte(DeviceAddress & 0xFE); //������ַ + W

    if(!I2C_WaitAck())
	{
	  I2C_Stop(); 
	  return 0;
	}
	
	while(len--)
	{
		I2C_SendByte(*pSendByte++);
    	I2C_WaitAck();
	}
    
	   
    I2C_Stop(); 

    return 1;
}									 

/*******************************************************************************
* Function Name  : I2C_ReadByte
* Description    : ��ȡһ������
* Input          : - pBuffer: ��Ŷ�������
*           	   - length: ����������
*                  - ReadAddress: ��������ַ
*                  - DeviceAddress: ��������
* Output         : None
* Return         : ����Ϊ:=1�ɹ�����,=0ʧ��
*******************************************************************************/          
uint8 I2C_ReadByte(uint8* pBuffer,   uint8 length,   uint8 ReadAddress,  uint8 DeviceAddress)
{		
	//I2C_WriteByte(ReadAddress, DeviceAddress);
	if(!I2C_Start())
	  return 0;

    I2C_SendByte(DeviceAddress & 0xFE); //������ַ + W
	I2C_WaitAck();

	I2C_SendByte(ReadAddress | 0x04); 	//��ݶ�ȡ
	I2C_WaitAck();
	length--;

    while(length)
    {
      *pBuffer = I2C_ReceiveByte();
      if(length == 1)
	    I2C_NoAck();
      else 
	    I2C_Ack(); 
      pBuffer++;
      length--;
    }

    I2C_Stop();

    return 1;
}
#endif	
