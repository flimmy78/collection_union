#include <includes.h>
#define  IN_LCMDRV
#include "lcmdrv.h"
#include "ASCIICode.h"
#include "HZTable.h"
#include "PictureCode.h"
#include "app_down.h"
#include "PublicFunc.h"

#define LCMRST_ENABLE	GPIO_ResetBits(GPIOG , LCM_RESET)
#define LCMRST_DISABLE	GPIO_SetBits(GPIOG , LCM_RESET)

#define BCD_TO_HEX(x) ((x/0x10)*0x0A+(x%0x10))


BLT_CTR gBltCtr;

OS_EVENT *KeyMessageQ;
OS_EVENT *LcmSem;

extern uint8 KeyMessageFlag;
extern uint8 ShowType_Flag;

uint8 GetKeyStatus(void)
{
	uint8 err;
	uint32 Status;
	//Status=(uint32)OSQPend(KeyMessageQ, OS_TICKS_PER_SEC/50, &err);   //�õ�����״̬���ϡ��¡�����
	Status=(uint32)OSQPend(KeyMessageQ, 0, &err);   //�õ�����״̬���ϡ��¡�����
	KeyMessageFlag = 1;
	return (uint8)(Status);
}

uint8 GetKeyStatuschaobiao(void)
{
	uint8 err;
	uint32 Status;
	Status=(uint32)OSQPend(KeyMessageQ, 1500, &err);   //�õ�����״̬������
	KeyMessageFlag = 1;
	return (uint8)(Status);
}



void  delay(uint32  dly)
{  uint32  i;

   for(; dly>0; dly--) 
      for(i=0; i<2; i++);
}

uint8 readi(void)
{
    uint8 data;
    data=*((uint8*)(LDM_R_ADDR));
    return data;
    
}
uint8 readd(void)
{
    uint8 data;
    data=*((uint8*)(LDM_D_ADDR));
    delay(0);
    return data;
    
}


void writei(unsigned char ins)
{
   
   	*((uint8*)(LDM_R_ADDR))=ins;

    delay(0);
}

void writed(unsigned char dat)
{
     
   	*((uint8*)(LDM_D_ADDR))=dat;
   	//delay(0);
}

void lcd_initial(void)
{
//	uint8 Err 	= 0;

	delay(0x00000fff);
    LCMRST_ENABLE;
    delay(0x000fffff);
    LCMRST_DISABLE;
    delay(0x00000fff);
	
    writei(0xe2);  //set system reset
    delay(0x000fffff);   //delay 200ms

    writei(0x24);  //set Temp. command  TC1TC0 00=-0.00%
    writei(0x2b);  //internal pump

#ifdef UPDOWN_DIS    
	  writei(0xC2);  //set lcd mapping  MY=0,Mx=1,LC0=0
      writei(0x8D);//set AC[2-0]
#else
	  writei(0xC4);  //set lcd mapping  MY=1,Mx=0,LC0=0
#endif

      writei(0xA1);  //set line rate
      writei(0xc8);  //set n-line
      writei(0x1f);

      writei(0xD1);  //set color pattern RGB

      writei(0xD5);  //set 4K color mode

      writei(0xE9);  //set lcd bias ratio: 1/10bias

      writei(0x81);  //set Vbias potentiometer
      writei(0xc3);  //set contrast level:0-255
	
      writei(0xde);  //set com scan function
      
      LCM_LIGHT_ENABLE;
      gBltCtr.Sta = TRUE; //��ʾ�����Ѵ�
	  gBltCtr.DogCnt = 0;
	  gBltCtr.DogCntThrd = BLIGHT_DOG_CNT_THRD;
        
	  display_white();
	  //ShowPicture(28,9,104,104,&StartPic[0]);
	  
	  SetContrast(LCD_CON_VALUE);//���öԱ�ֵ
	  writei(0xAd);  //display enable
	  
	  if(clock_state == FALSE)
	  {
		  ShowLineChar_HZ(4,10,&ClockErr[0],7);//������������ά��
		  delay(0x00200000);
	  }  
      delay(0x00000500);
 }


void window_program(void)      //set window
{


    writei(0xF4);  //set start column
    writei(0x23);  //start seg106

    writei(0xF5);  //set start row
    writei(0x00);  //start com1

    writei(0xF6);  //set end column
   writei(0x6b);//Ӧ������Ϊ0x58����Ϊ��ʹ��ʾ������ַ����ܴ�ͷ��ʾ�����������ر����ô�һЩ
    
    writei(0xF7);  //set end row
    writei(0x9F);  //end com160
     
}

void display_black(void)
{
  int i;
  int j;
  window_program();
  for(i=0;i<160;i++)
     {

		for(j=0;j<54;j++)
		{
		   writed(0xff);
		   writed(0xff);
		   writed(0xff);
		}
		
		
	}
}

void display_white(void)
{
  int i;
  int j;
  window_program();
  for(i=0;i<160;i++)
     {

		for(j=0;j<54;j++)
		{
		   writed(0x00);
           writed(0x00);
		   writed(0x00);
		}
		}
}

void snow()
{
      int i;
      int j;
      window_program();
     for(i=0;i<160;i++)
     {
		for(j=0;j<54;j++)
		{
		 writed(0xf0);
                 writed(0xf0);
                 writed(0xf0);
		}
		for(j=0;j<54;j++)
		{
		 writed(0x0f);
                 writed(0x0f);
                 writed(0x0f);
		}

     }
}

void network()
{
	  int i;
      int j;
      window_program();
     for(i=0;i<160;i++)
     {
		for(j=0;j<54;j++)
		{
		 writed(0xf0);
                 writed(0xf0);
                 writed(0xf0);
		}
		for(j=0;j<54;j++)
		{
		         writed(0xff);
                 writed(0xff);
                 writed(0xff);
		}
     }
}


void horizontal()
{
  int i;
  int j;
  window_program();

  for(i=0;i<160;i++)
     {
		for(j=0;j<54;j++)
		{  writed(0xff);
           writed(0xff);
		   writed(0xff);
		}

		for(j=0;j<54;j++)
		{  writed(0xff);
           writed(0xff);
		   writed(0xff);
		}

	    for(j=0;j<54;j++)
		{
		   writed(0x00);
           writed(0x00);
		   writed(0x00);
		}
     }
}

void horizontal1()
{
  int i;
  int j;
  window_program();

  for(i=0;i<160;i++)
     {
		for(j=0;j<54;j++)
		{  writed(0xff);
           writed(0xff);
		   writed(0xff);
		}

		for(j=0;j<54;j++)
		{  writed(0xff);
           writed(0xff);
		   writed(0xff);
		}

	    for(j=0;j<54;j++)
		{
		   writed(0x00);
           writed(0x00);
		   writed(0x00);
		}
     }
}

void horizontal2()
{
  int i;
  int j;
  window_program();

  for(i=0;i<160;i++)
     {
	
		for(j=0;j<54;j++)
		{  writed(0xff);
           writed(0xff);
		   writed(0xff);
		}

	    for(j=0;j<54;j++)
		{
		   writed(0x00);
           writed(0x00);
		   writed(0x00);
		}
     }
}

void vertical()
{
      int i;
      int j;
      window_program();
     for(i=0;i<160;i++)
     {
		for(j=0;j<54;j++)
		{
		   writed(0xff);
           writed(0x0f);
		   writed(0xf0);
		}
     }
}

void vertical1()
{
      int i;
      int j;
      window_program();
     for(i=0;i<160;i++)
     {
		for(j=0;j<54;j++)
		{
		   writed(0x0f);
           writed(0xf0);
		   writed(0xff);
		}
     }
}

void frame()
{
      int i,j;
      window_program();
     for(i=0;i<160;i++)
	{
		if(i==0||i==159)
	        {
                 for(j=0;j<54;j++)
		    {writed(0xff);
             writed(0xff);
		     writed(0xff);}
		}

		else

		for(j=0;j<54;j++)
		{
		 if(j==44)
		   {writed(0xf0);
		    writed(0x00);
		    writed(0x00);}
                 else if(j==17)
		   {writed(0x00);
		    writed(0x0f);
		    writed(0x00);}
                 else
                       { writed(0x00);
                         writed(0x00);
		                 writed(0x00);
			}
		}
	}
}
/***********************************************************************************
*��������SetContrast()
*���ܣ�����Һ���Աȶ�
*��ڲ�����ContrastValue:�Աȶ�ֵ 0--255 
*���ز�������
************************************************************************************/
void SetContrast(uint8 ContrastValue)
{
      writei(0x81);  //set Vbias potentiometer
      writei(ContrastValue);  //set contrast level:0-255
}
/***********************************************************************************
*��������SetLine1()
*���ܣ������е�ַrow    0<=line<=159 
*��ڲ�����chip:оƬѡ��
 		   line:���õ��У�ҳ��	
*���ز������������󷵻�0����ȷ����1
************************************************************************************/

void SetLine1(uint16 line) // 0<=line<=159    1 line = 1 row
{
	uint8 LineLSB;
	uint8 LineMSB;
	LineLSB = (line&0x0F)|0x60;
	LineMSB = (line >> 4)&0x0F;
	LineMSB = LineMSB | 0x70;
	
	writei(LineLSB);
	writei(LineMSB);
}

/***********************************************************************************
*��������SetLine()
*���ܣ������е�ַҳ  0<=line<=19 
*��ڲ�����
 		   line:���õ��У�ҳ��	
*���ز�������
************************************************************************************/

void SetLine(uint16 line)  // 0<=line<=19    1 line = 8 row
{
	uint8 LineLSB;
	uint8 LineMSB;
	
	line = line % 20;
	line = line << 3;
	LineLSB = (line&0x0F)|0x60;
	LineMSB = (line >> 4)&0x0F;
	LineMSB = LineMSB | 0x70;
	
	writei(LineLSB);
	writei(LineMSB);
	
}

/***********************************************************************************
*��������SetColumn()
*���ܣ������е�ַ  --Y  0--53
*��ڲ�����chip:оƬѡ��
 		   column:���õ���	
*���ز������������󷵻�0����ȷ����1
************************************************************************************/
void SetColumn(uint16 column)  //0<=column<=53   1 column = R G B = 3 ��
{
	uint8 ColumnLSB;
	uint8 ColumnMSB;
	column = column % 54;
	column = column + COLUMN_OFFSET; // Һ����106�п�ʼ
	ColumnLSB = (column & 0x0F)| 0x00;
	ColumnMSB = (column>>4) & 0x07;
	ColumnMSB = ColumnMSB | 0x10;
	writei(ColumnLSB);
	writei(ColumnMSB);
}

uint32 ConvertByte(uint8 Bytedata) //�ֽڰ�λת����0ת��Ϊ0��1ת��ΪF��ת����Ϊ32λ
{
	uint8 i,flag;
	uint32 converdata = 0;
	for(i=0;i<8;i++)
	{
		flag = Bytedata&0x01;
		if(flag == 1)
		{
	//		converdata |= 0x0F; 
			converdata = converdata | (0x0F << (4*i));
		}
		
		Bytedata = Bytedata >> 1;
	}
	return converdata;
}

void Convert(uint8* Daddr, const uint8* Saddr,uint8 ByteNum) //
{
	uint8 i,j;
	uint8 temp1,temp2;
	uint8 flag;
	uint8 tempdata;
	
	for(i = 0;i < ByteNum;i++)
	{
		if(Saddr[i] == 0x00) //�ֽ�ȫΪ0�������
		{
			//memset(&Daddr[4*i],0,4);
			for(j =0;j<4;j++)
				Daddr[4*i+j] = 0x00;
			continue;
		}
		
		tempdata = Saddr[i];
		for(j = 0;j < 4;j++) //��ȫ0�������
		{
			flag = tempdata&0x80;
			if(flag == 0x80)
			{
				temp1 = 0xF0;
				//Daddr[i*8+j] = 0x0F;
			}
			else
			{
				temp1 = 0x00;
			}
			
			//Daddr[i*8+j] = 0x00;
			tempdata = tempdata << 1;
			
			flag = tempdata&0x80;
			if(flag == 0x80)
			{
				temp2 = 0x0f;
			}
			else
			{
				temp2 = 0x00;
			}
				
			Daddr[i*4+j] = temp1|temp2;	
			
			tempdata = tempdata << 1;			
		}
	}
	
}

uint8 UpDown_Convert(uint8* Daddr,uint8* Saddr,uint8 ByteNum)
{
	uint8 i,j;
	uint8 temp;
	uint8 flag;
	uint8 tempdata, len;
	
	for(i = 0;i < ByteNum;i++)
	{
		if(Saddr[i] == 0x00) //�ֽ�ȫΪ0�������
		{
			//memset(&Daddr[4*i],0,4);
			for(j =0;j<8;j++)
			{
				Daddr[8*i+j] = 0x00;
			}	
			len = i*8+j;
			continue;
		}
		
		tempdata = Saddr[i];
		for(j = 0;j < 8;j++) //��ȫ0�������
		{
			flag = tempdata&0x80;
			if(flag == 0x80)
			{
				Daddr[i*8+j] = 0xFF;
			}
			else
			{
				Daddr[i*8+j] = 0x00;
			}
			tempdata = tempdata << 1;
		}
		len = i*8+j;
	}
	
	for(i=0; (i+2)<len; i+=3)
	{
		tempdata = Daddr[i];
		Daddr[i] = Daddr[i+2];
		Daddr[i+2] = tempdata;
	}
	
	temp = len%3;
	if(temp == 1)
	{
		Daddr[i+2] = Daddr[i];
		Daddr[i+1] = 0x00;
		Daddr[i] = 0x00;	
		len = len + 2;
	}
	else if(temp == 2)
	{
		Daddr[i+3] = 0x00;
		Daddr[i+2] = Daddr[i+1];
		Daddr[i+1] = Daddr[i];
		Daddr[i] = 0x00;
		len = len + 2;
	}
	return len;
}

uint32 ConvertRevertShow(uint32 data)//����洢��ɨ��ת������
{
	uint32 tmpdata;
	uint8 bit1;
	uint8 bit3;
	uint8 bit4;
	uint8 bit6;
	uint8 bit7;
	uint8 bit8;
	bit1 = (data & 0x0000000F);
	bit3 = (data & 0x00000F00)>>8;
	bit4 = (data & 0x0000F000)>>12;
	bit6 = (data & 0x00F00000)>>20;
	bit7 = (data & 0x0F000000)>>24;
	bit8 = (data & 0xF0000000)>>28;
	
	tmpdata = data & 0x000F00F0;
	tmpdata = tmpdata | (bit1<<8);
	tmpdata = tmpdata | (bit3);
	tmpdata = tmpdata | (bit4<<20);
	tmpdata = tmpdata | (bit6<<12);
	tmpdata = tmpdata | (bit7<<28);
	tmpdata = tmpdata | (bit8<<24);
	
	return tmpdata;
}


/***********************************************************************************
*��������Show88()
*���ܣ���ʾ8*8����
*��ڲ�����line:��(0-19)
 		   column: ��(0-54)
           address����ģ���׵�ַ
*���ز�������
************************************************************************************/
void Show98(uint16 line,uint16 column,const uint8* addr)
{
	uint8 i,j,temp;
	//uint8 temp1,temp2;
	uint32 tempdata;
		
	SetLine(line);
	SetColumn(column);
	line = line << 3;
	for(i=1;i<=8;i++)
	{	
		if(addr[i-1]==0x00)
		{
			writed(0x00);
			writed(0x00);
			writed(0x00);
			writed(0x00);
			writed(0x00);
		}
		else
		{
			tempdata = ConvertByte(addr[i-1]);
	#ifdef	UPDOWN_DIS
			tempdata = ConvertRevertShow(tempdata);
			for(j=0;j<3;j++)
			{
				temp = tempdata&0x000000FF;
				temp = (temp << 4)|(temp >> 4);
				writed(temp);
				tempdata = tempdata >> 8;
			}	

			temp = tempdata&0x000000FF;		
			temp = (temp << 4)|(temp >> 4);			
			writed(0x00|(temp>>4));//0x00+��7λ
			//writed(0x00);//д�������B������һ���ڵ�RG�ܹ���ʾ
			writed((temp&0x0F)<<4);
	#else
			for(j=0;j<4;j++)
			{
				temp = tempdata&0x000000FF;
				temp = (temp << 4)|(temp >> 4);
				writed(temp);

				tempdata = tempdata >> 8;
			}			
			writed(0x00);//д�������B������һ���ڵ�RG�ܹ���ʾ	
	#endif
		}

		SetLine1(line + i);
		SetColumn(column);
	}
}

/***********************************************************************************
*��������ShowChar()
*���ܣ���ʾ8*16
*��ڲ�����line:��(0-9)
 		   column: ��(0-54)
           character:�ַ�����(��׼ASCII��)
*���ز�������
************************************************************************************/ 
void ShowChar(uint16 line,uint16 column,uint8 character)
{
	line = line << 1;
	Show98(line, column, &ASCIICode[character-0x20][0]);
	Show98(line + 1,column, &ASCIICode[character-0x20][8]);
}

void ShowChar1(uint16 line,uint16 column,uint16 character)
{
	line = line << 1;
	Show98(line, column, &ASCIICode[character-0x20][0]);
	Show98(line + 1,column, &ASCIICode[character-0x20][8]);
}
/***********************************************************************************
*��������ShowHZ()
*���ܣ���ʾһ��16*16���֣�����Ļ��ʵ����ʾ16*15����16*18��
*��ڲ�����line:��(0-9)
 		   column: ��(0-53)
           hzcode:���ֱ��룬���ֵı���Ϊ�������������е����+0x90(144)
*���ز�������
************************************************************************************/ 
void ShowHZ(uint8 line,uint8 column,uint16 hzcode)
{
	uint8 i,j;
	uint8 convertdata[162];
	//uint8 len;
	
	line = line % 10;
	line = line << 1;
	column = column % 54;
	
	//һ�㲻�ܳ����������������ʾһ���ַ������ֺ��ַ���ϣ���ʱ��������ʾ���󣬵���ʾ��������ʱ������ȷ��ʾ
	if(hzcode<0x90)
	{
		hzcode += 0x90;
	}
	
	SetLine(line);
	SetColumn(column);
	
	line = line << 3;
	for(i=0;i<16;i++)
	{
#ifdef UPDOWN_DIS
		len = UpDown_Convert(&convertdata[0],&HZTable[hzcode-0x90][i*2],2);
		for(j=0;j<len/2;j++)
		{
			writed((convertdata[2*j]<<4)|(convertdata[2*j+1]>>4));
		}
#else
		Convert(&convertdata[0], &HZTable[hzcode-0x90][i*2],2);	
		for(j=0;j<8;j++)
		{
			writed(convertdata[j]);
		}
		writed(0x00);//д������ݣ���ʾ���ĵ�R,ȥ����������ʾ18���ص�
#endif		 
		SetLine1(line + i + 1);
		SetColumn(column);			
	}
	
}

/***********************************************************************************
*��������ShowLineHZ()
*���ܣ���ʾһ��16*16����
*��ڲ�����line:��(0-9)
		   column:�У�0--53��
		   number:��ʾ�ĺ��ָ���
 		   linehzaddr:���ִ����ַ
 		   len:���ִ�������ĳ���
*���ز�������
************************************************************************************/ 
void ShowLineHZ(uint8 line,uint8 column,uint16* linehzaddr,uint8 len)
{
	uint8 temp;
	uint8 i;
	//uint8 len;
	
	//line=line<<1; //lin*2
	line = line % 10;
	column = column%54; //��ֹ��������
	//temp = column/5;
	temp = 54 - column;
	temp = temp / 5;
    if(len>temp)
    {
    	len = temp;    //��ֹҪ������ָ������ڱ���ʣ��������ռ䣬�Ӷ���ͷ��ʼ����
    }   
     
    for(i=0;i<len;i++)
    {
     	ShowHZ(line,column,linehzaddr[i]);
     	column = column + 5;
    }
}
//��ʾһ��ASCII���ַ�
void ShowLineChar(uint8 line,uint8 column,uint8* linecharaddr,uint8 len)
{
	uint8 temp;
	uint8 i;
	//uint8 len;
	
	//line=line<<1; //lin*2
	line = line % 10;
	column = column%54; //��ֹ��������
	//temp = column/5;
	temp = 54 - column;
	temp = temp / 3;
    if(len>temp)
    {
    	len = temp;    //��ֹҪ����ַ��������ڱ���ʣ��������ռ䣬�Ӷ���ͷ��ʼ����
    }   
     
    for(i=0;i<len;i++)
    {
     	ShowChar(line,column,linecharaddr[i]);
     	column = column + 3;
    }

}

void ShowLineChar1(uint8 line,uint8 column,const uint8* linecharaddr,uint8 len)
{
	uint8 temp;
	uint8 i;
	//uint8 len;
	
	//line=line<<1; //lin*2
	line = line % 10;
	column = column%54; //��ֹ��������
	//temp = column/5;
	temp = 54 - column;
	temp = temp / 3;
    if(len>temp)
    {
    	len = temp;    //��ֹҪ����ַ��������ڱ���ʣ��������ռ䣬�Ӷ���ͷ��ʼ����
    }   
     
    for(i=0;i<len;i++)
    {
     	ShowChar(line,column,linecharaddr[i]);
     	column = column + 3;
    }

}

void ShowLineChar2(uint8 line,uint8 column, uint16* linecharaddr,uint8 len)
{
	uint8 temp;
	uint8 i;
	//uint8 len;
	
	//line=line<<1; //lin*2
	line = line % 10;
	column = column%54; //��ֹ��������
	//temp = column/5;
	temp = 54 - column;
	temp = temp / 3;
    if(len>temp)
    {
    	len = temp;    //��ֹҪ����ַ��������ڱ���ʣ��������ռ䣬�Ӷ���ͷ��ʼ����
    }   
     
    for(i=0;i<len;i++)
    {
     	ShowChar(line,column,linecharaddr[i]);
     	column = column + 3;
    }

}

//��ʾ���ⳤ�ȵ��ַ������Զ�����
void ShowAnyChar(uint8 line,uint8 column,uint8* linecharaddr,uint8 len)
{
	uint8 temp;
	uint8 i;
	
	line = line % 10;
	column = column%54; //��ֹ��������

/*
	temp = 54 - column;
	temp = temp / 3;
    if(len <= temp)
    {
	    for(i=0;i<len;i++)
	    {
	     	ShowChar(line,column,linecharaddr[i]);
	     	column = column + 3;
	    }
	    return;
    }
*/    
	temp = column / 3;
	for(i=0;i<len;i++)
	{
		if(line < 10)
		{
			if(temp < 18)
			{
				ShowChar(line,3*temp,linecharaddr[i]);
				//temp++;
			}
			else
			{
				temp = 0;
				line++;
				ShowChar(line,3*temp,linecharaddr[i]);
				//temp++;	
			}
			temp++;
		}
	}
    return;

}

//��ʾʱ������ --- ����ʱ��
void ShowTimeChar(uint8 line,uint8 column,uint8* linecharaddr,uint8 len)
{
	uint8 temp;
	uint8 i;
	
	line = line % 10;
	column = column%54; //��ֹ��������
	temp = 54 - column;
	temp = temp / 3;
	i=0;
	
    if(len>temp)
    {
    	len = temp;    //��ֹҪ����ַ��������ڱ���ʣ��������ռ䣬�Ӷ���ͷ��ʼ����
    }   
    if(len < 8)
    {
    	ShowChar(line,column,0x30);					//��-- ʮλ
    	column += 3;
   	}
   	else
   	{
   		ShowChar(line,column,linecharaddr[i++]);	//��-- ʮλ
    	column += 3;
   	}
    ShowChar(line,column,linecharaddr[i++]);	//��-- ��λ
    column += 3;
    ShowChar(line,column,0x2D);					//��ʾ " - "
    column += 3;
    ShowChar(line,column,linecharaddr[i++]);	//��-- ʮλ
    column += 3;
    ShowChar(line,column,linecharaddr[i++]);	//��-- ��λ
    column += 3;   
    ShowChar(line,column,0x20);					//��ʾ "  "
    column += 3;
    ShowChar(line,column,linecharaddr[i++]);	//Сʱ-- ʮλ
    column += 3;
    ShowChar(line,column,linecharaddr[i++]);	//Сʱ-- ��λ
    column += 3;   
    ShowChar(line,column,0x3A);					//��ʾ " : "
    column += 3;
    ShowChar(line,column,linecharaddr[i++]);	//��-- ʮλ
    column += 3;
    ShowChar(line,column,linecharaddr[i++]);	//��-- ��λ
    column += 3;   

}

//��ʾһ��ASCII���ַ��ͺ��ֵĻ��
void ShowLineChar_HZ(uint8 line,uint8 column,const uint16* linecharaddr,uint8 len)
{
	uint8 temp;
	uint8 i;
	uint8 temp1 = 0;
	
	line = line % 10;        // �ܹ�10�У����й���	  
	column = column % 54;	 //�ܹ�54�У����й���

	temp = 54 - column;		//

//begin:�鿴ָ�����ܷ���ʾ��ָ����len���ַ���������ܣ���ı�len.
	for( i = 0; i<len; i++ )	//lenָÿһ����ʾ�ĺ��ָ���
	{
		if( linecharaddr[i]<0x90 )	temp1 = temp1 + 3; 	 //�����ASCII�ַ�
		else	temp1 = temp1 + 5; //����Ǻ���
		if( temp <= temp1 )
		{
			len = i+1;
		}
	}
//end:�鿴ָ�����ܷ���ʾ��ָ����len���ַ���������ܣ���ı�len.

//ѭ��������ʾ�ַ�		
	for( i = 0; i < len; i++ )
	{
		if( linecharaddr[i] >= 0x20 && linecharaddr[i] < 0x90 )	//��ʾASCII�ַ�
		{
			ShowChar( line, column, linecharaddr[i] );
			column = column + 3;
		}
		else if( linecharaddr[i] >= 0x90 )	  	//��ʾ����
		{
			ShowHZ( line, column, linecharaddr[i] );
			column = column + 5;
		}
	}
	return;
}

/**********************************************************************************
*��������ReverseShow31
*���ܣ�����ʾһ��3*1�ֿ鵥Ԫ,ǰ������COLOR MODEΪ64K��ģʽ�£���LC[7��6]=10B��RRRRR-GGGGGG-BBBBB
*��ڲ�����line:����ʾ����(0--160)
		   column:����ʾ����(0--54)
*���ز�������
*˵����COLOR MODE�����úͻָ����Է��������ú����У���Ϊ��ʹ��㺯���ĵ��ò��ٿ���������ã�����
	   ���ڱ������У���ȱ����������ָ����������ö�Σ��˷Ѷ�д����
***********************************************************************************/
void ReverseShow31(uint8 line, uint8 column)
{
	uint8 temp[2];
	uint32 i;
//	uint8 temp1;
	
	line = line % 160;
	column = column % 54;
	
	writei(0xd6);//����COLOR MODEΪ64K��ģʽ����LC[7��6]=10B��RRRRR-GGGGGG-BBBBB
	
	SetLine1(line);
	SetColumn(column);
	temp[0] = readd();
	for(i=0;i<2;i++)
	{
		temp[i] = ~readd();
	}
	
	SetLine1(line);
	SetColumn(column);
	for(i=0;i<2;i++)
	{
		writed(temp[i]);
	}
	writei(0xd5);//�ָ�����COLOR MODEΪ4K��ģʽ����LC[7��6]=01B��RRRR-GGGG-BBBB
	
}


void ReverseShow38(uint8 line,uint8 column)
{
	uint8 i;
	
	line = line << 3;
	for(i=0;i<8;i++)
	{
		ReverseShow31(line+i,column);
	}
	
}
/**********************************************************************************
*��������ReverseShow98
*���ܣ�����ʾһ��9*8�ֿ鵥Ԫ(9��8��)
*��ڲ�����line:����ʾ����(0--19)
		   column:����ʾ����(0--54)
*���ز�������
***********************************************************************************/
void ReverseShow98(uint8 line, uint8 column)
{
	uint8 i;
	
	line = line % 20;
	column = column % 54;
	line = line << 3;
	
	for(i = 0;i < 8;i++)
	{
		ReverseShow31(line+i,column);
		ReverseShow31(line+i,column+1);
		ReverseShow31(line+i,column+2);
	}
}

/**********************************************************************************
*��������ReverseShowChar
*���ܣ�����ʾһ��9*16�ֿ鵥Ԫ(9��16��)
*��ڲ�����line:����ʾ����(0--10)
		   column:����ʾ����(0--54)
*���ز�������
***********************************************************************************/
void ReverseShowChar(uint8 line, uint8 column)
{
//	uint8 i;
	
	line = line % 10;
	column = column % 54;
	line = line << 1;
	
//	writei(0xd6);//����COLOR MODEΪ64K��ģʽ����LC[7��6]=10B��RRRRR-GGGGGG-BBBBB

	ReverseShow98(line,column);
	ReverseShow98(line+1,column);
	
//	writei(0xd5);//�ָ�����COLOR MODEΪ4K��ģʽ����LC[7��6]=01B��RRRR-GGGG-BBBB
}

/**********************************************************************************
*��������ReverseShowHZ
*���ܣ�����ʾһ��15*16�ֿ鵥Ԫ(15��16��)
*��ڲ�����line:����ʾ����(0--10)
		   column:����ʾ����(0--54)
*���ز�������
***********************************************************************************/
void ReverseShowHZ(uint8 line, uint8 column)
{
	uint8 i,j;
	
	line = line % 10;
	column = column % 54;
	line = line << 4;
	
//	writei(0xd6);//����COLOR MODEΪ64K��ģʽ����LC[7��6]=10B��RRRRR-GGGGGG-BBBBB
	
	for(i=0;i<16;i++)
	{
		for(j=0;j<5;j++)
		{
			ReverseShow31(line+i,column+j);
		}
	}
		
//	writei(0xd5);//�ָ�����COLOR MODEΪ4K��ģʽ����LC[7��6]=10B��RRRR-GGGG-BBBB
}

/***********************************************************************************
*��������ReverseShowLine
*���ܣ�����һ�к���
*��ں�����line:���Ե��У�0--9��
		  
*���ں�������
************************************************************************************/
void ReverseShowLine(uint8 line)
{
	uint8 i;

	for(i=0;i<18;i++)
	  ReverseShowChar(line,i*3);

}

/***********************************************************************************
*��������ReverseShowLine_len
*���ܣ�����һ��ָ������
*��ں�����startline:���Ե��У�0--9��
		   startcolumn:���Կ�ʼ�У�0--53��
		   columnnum:���Ե�������0--53��
*���ں�������
************************************************************************************/
void ReverseShowLine_len(uint8 startline,uint8 startcolumn,uint8 columnnum)
{
	uint8 i,j;
	
	startline = startline % 10;
	startcolumn = startcolumn % 54;
	startline = startline << 4;
	
//	writei(0xd6);//����COLOR MODEΪ64K��ģʽ����LC[7��6]=10B��RRRRR-GGGGGG-BBBBB
	
	for(i=0;i<16;i++)
	{
		for(j=0;j<columnnum;j++)
		{
			ReverseShow31(startline+i,startcolumn+j);
		}
	}
	
	//ReverseShowChar(startline,(startcolumn+i*3));	
}

/***********************************************************************************
*��������ShowPicture
*���ܣ���ʾͼƬ--��3*1Ϊ��λ
*��ں�����startline:ͼƬ��ʼ�У�0--160��
		   startcolumn��ͼƬ��ʼ�У�0--53��
		   pictureline��ͼƬ�е��� ��1--160��
		   picturecolumn��ͼƬ�е��� ��1--160��
		   pictureaddr��ͼƬ��ַ
*���ں�������
************************************************************************************/
void ShowPicture(uint8 startline,uint8 startcolumn,uint8 pictureline,uint8 picturecolumn,uint8* pictureaddr)
{
	uint8 i,j;
	uint8 lastbits;//columnbyte
	uint8 convertdata[162];
	
	startline = startline % 160;
	startcolumn = startcolumn % 54;
	//pictureline = pictureline/8;
	//pictureline = pictureline % 160;
	//picturecolumn = picturecolumn % 160;
	
	lastbits = picturecolumn % 8;
	picturecolumn = picturecolumn >> 3;//����Ϊ8�ĵı��������ж��ٸ��ֽ�
	
	if(lastbits != 0) //����ɨ�費������������
	{
		picturecolumn += 1;
	}
	

	for(i = 0;i<pictureline;i++)
	{
		SetLine1(startline + i);
		SetColumn(startcolumn);
#ifdef UPDOWN_DIS
		len = UpDown_Convert(&convertdata[0],&pictureaddr[i*picturecolumn],picturecolumn);
		for(j=0;j<len/2;j++)
		{
			writed((convertdata[2*j]<<4)|(convertdata[2*j+1]>>4));
		}
#else			
		Convert(&convertdata[0],&pictureaddr[i*picturecolumn],picturecolumn);
		for(j=0;j<(picturecolumn*4);j++)
		{
		   //Show98(startline+i,startcolumn+j,pictureaddr+(i*picturecolumn+j)*8);
		   writed(convertdata[j]);  
		}
		writed(0x00);//д�������
#endif		
	//	SetLine1(startline + i + 1);
	//	SetColumn(startcolumn);					
	}	
}

/**********************************************************************************
*��������ShowRectangle
*���ܣ���ʾһ��ָ����ʼ�С��к;��γ�����ľ���
*��ں�����startline:��ʼ�У�0--160��
		   startcolumn����ʼ�У�0--53��
		   rectangleline�������е��������ȣ� ��1--160��
		   rectanglecolumn�������е�������ȣ� ��1--160�����Ϊ3�ı���
*���ز�������
***********************************************************************************/
void ShowRectangle(uint8 startline,uint8 startcolumn,uint8 rectangleline,uint8 rectanglecolumn,uint8 flag)
{
	uint8 endline,endcolumn;
	
	startline = startline % 160;
	startcolumn = startcolumn % 54;
	endline = startline + rectangleline;
	endcolumn = startcolumn*3 + rectanglecolumn;//0--160
	if(flag == TRUE)
	{
		CleanScreen(startline,startcolumn,rectangleline,rectanglecolumn/3);//�������������������
	}
	//��ʾ���ε���������
	ShowColumn(startline,endcolumn,rectangleline);//��
	ShowColumn(startline,startcolumn*3,rectangleline);//��		
	
	//��ʾ���ε���������
	ShowLine(startline,startcolumn,rectanglecolumn);//��
	ShowLine(endline-1,startcolumn,rectanglecolumn);//��
}


/**********************************************************************************
*��������ShowGPRSSignal
*���ܣ���ʾһ��16*16���ź�ǿ��
*��ڲ�����line:��ʼ����(0--10)
		   column:��ʼ����(0--54)
*���ز�������
***********************************************************************************/
void ShowGPRSSignal(uint8 line,uint8 column)
{
	uint8 GPRSSignal;
	uint8 lu8Csq = 0;
	GPRS_RUN_STA luGPRSSta;
	uint8 lu8data1 = 0;
	uint8 lu8data2 = 0;

	ReadGprsRunSta(&luGPRSSta);
	lu8Csq = luGPRSSta.Csq;

	if((lu8Csq == 99) || (lu8Csq < 4))//���ź�ǿ�ȷֵȼ���99��ʾ���������
		GPRSSignal = 0;
	else if(lu8Csq < 8)
		GPRSSignal = 1;
	else if(lu8Csq < 14)
		GPRSSignal = 2;
	else if(lu8Csq < 19)  //
		GPRSSignal = 3;
	else
		GPRSSignal = 4;
	
	line = line << 4;
	//GPRSSignal = 2;//GetGprsCSQ();//�˴��Ժ��滻Ϊ�õ�GRPS�ź�ǿ�ȵĺ���
	ShowPicture(line,column,0x10,0x10,&StateSignalPic[GPRSSignal][0]);

	lu8data1 = lu8Csq / 10;
	lu8data2 = lu8Csq % 10;
	ShowChar(line,column+6,lu8data1+0x30);
	ShowChar(line,column+9,lu8data2+0x30);
	
}

/**********************************************************************************
*��������ShowGC
*���ܣ���ʾ�ź����� GPRS����CDMA 16*16
*��ڲ�����line:��ʼ����(0--10)
		   column:��ʼ����(0--54)
*���ز�������
***********************************************************************************/
void ShowGC(uint8 line,uint8 column)
{
	uint8 SignalType;
	/*
	if(gGprsRunSta.Connect)
	{
		SignalType = GetChannelType() + 1;//�õ��������͵ĺ���������ǰͨ��ͨ������
	}
	else
	{
		SignalType = 0;//�հ�
	}
	*/
//	SignalType = GetChannelType() + 1;//�õ��������͵ĺ���������ǰͨ��ͨ������
	SignalType =  1;//�õ��������͵ĺ���������ǰͨ��ͨ������
	line = line << 4;
	ShowPicture(line,column,0x10,0x10,&GCPic[SignalType][0]);
	 
}

/**********************************************************************************
*��������ShowAbnormal
*���ܣ���ʾ�쳣���ͼ�� 16*16
*��ڲ�����line:��ʼ����(0--10)
		   column:��ʼ����(0--54)
		   abnormaltype:�쳣����  1-MBUS��·��2-SD���쳣��
		   
*���ز�������
***********************************************************************************/
void ShowAbnormal(uint8 abnormaltype,uint8 line,uint8 column)
{
	uint8 i = 0;
	uint8 lu8MBusErrFlag = 0;
	uint8 lu8SDErrFlag = 0;
	
	switch(abnormaltype){
		case 1:{
			for(i=0;i<METER_CHANNEL_NUM;i++){
				if(gu8MBusShortFlag[i] == 1){
					ShowHZ( line, column, 0x01ac );  //��ʾMS��MBUS Short.
					column += 5;
					ShowChar(line,column,i+1+0x30);  //��ʾ��·ͨ���š�

					lu8MBusErrFlag = 1;

					break;    //����ж�ͨ����·����ֻ��ʾ���Ȳ鵽�ģ�ֻ��ʾһ����
				}
			}
		}
		break;
		
		case 2:{
			if(gu8SDStatus != 0){
				ShowHZ( line, column, 0x01ab );  //��ʾSDE.
				column += 5;
				ShowChar(line,column,gu8SDStatus+0x30);  //��ʾgu8SDStatus

				lu8SDErrFlag = 1;
			}
			
		}
		break;

		default:
			break;

	}
	
		if((lu8SDErrFlag==1) || (lu8MBusErrFlag==1)){
			gu8ErrFlag = 1;
		}
		else{
			gu8ErrFlag = 0;
			gu16ErrOverTime = 0;

		}
}

/**********************************************************************************
*��������ShowAbnormalNum
*���ܣ���ʾ�쳣�澯�� 18*16 (2 ascii�ַ�)
*��ڲ�����line:��ʼ����(0--10)
		   column:��ʼ����(0--54)
*���ز�������
***********************************************************************************/
void ShowAbnormalNum(uint8 line, uint8 column)
{
	uint8 AbnormalNum[] = {0x30,0x30};
	//�˴���Ӹ澯��ת���͸�ֵ��AbnormalNum
	//AbnormalNum[0] =HexToAscii(GlobalVariable.ErrCode / 10);
	//AbnormalNum[1] =HexToAscii(GlobalVariable.ErrCode % 10);
	ShowLineChar(line,column,&AbnormalNum[0],2);
}

/**********************************************************************************
*��������ShowSysDate
*���ܣ���ʾ���������ڣ������ָ�ʽ��ʾ
*��ڲ�����line:��ʼ����(0--10)
		   column:��ʼ����(0--54)
		   datetype����ʾ��ʽ���ͣ�0ΪYY-MM-DD��1ΪYY��MM��DD��
*���ز�������
***********************************************************************************/
void ShowSysDate(uint8 line,uint8 column,uint8 datetype)
{
	//uint8 i;
	uint8 datebuf[10];
	uint8 data1,data2;
	
	line = line % 10;
	
	//��ʱ���Լ�д��Ĺ̶�ֵ(08��09��3��)���Ժ�������ĴӼ�������ʱ�亯��
	//datebuf[3] = 0x03;//��
	//datebuf[4] = 0x09;//��
	//datebuf[5] = 0x08;//��
	
	//UserReadDateTime(&datebuf[0]);//���ʱ������
	ReadDateTime(&datebuf[0]);
	datebuf[4] = datebuf[4]&0x1F;//ȥ������
	
		//��
	//====ZXL 08.09.27������ǰ�����ֽ�20
	ShowChar(line,column,0x32);
	ShowChar(line,column+3,0x30);
	column = column + 6;//Ϊ�˲�Ӱ��������ĸĶ����˴�columnֵ�䶯
	//====================================
	data1 = datebuf[5]/0x10;
	data2 = datebuf[5]%0x10;
	ShowChar(line,column,data1+0x30);
	ShowChar(line,column+3,data2+0x30);
	if(datetype == DATETYPE_CH)
	{
		ShowChar(line,column+6,0x2d);//"-"
		column = column + 9;
	}
	else
	{
		ShowHZ(line,column+6,0xfb);//"��"
		column = column + 12;
	}
		//��
	data1 = datebuf[4]/0x10;
	data2 = datebuf[4]%0x10;
	ShowChar(line,column,data1+0x30);
	ShowChar(line,column+3,data2+0x30);
	if(datetype == DATETYPE_CH)
	{
		ShowChar(line,column+6,0x2d);//"-"
		column = column + 9;
	}
	else
	{
		ShowHZ(line,column+6,0xfc);//"��"
		column = column + 12;
	}
	
		//��			
	data1 = datebuf[3]/0x10;
	data2 = datebuf[3]%0x10;
	ShowChar(line,column,data1+0x30);
	ShowChar(line,column+3,data2+0x30);
	
	if(datetype == DATETYPE_HZ)
	{
		ShowHZ(line,column+6,0xf3);//"��"		
	}		
	return;
}

/**********************************************************************************
*��������ShowSysTime
*���ܣ���ʾ������ʱ�䣬�����ָ�ʽ��ʾ
*��ڲ�����line:��ʼ����(0--10)
		   column:��ʼ����(0--54)
		   timetype����ʾ��ʽ���ͣ�0ΪHH:MM��1ΪHH:MM:SS
*���ز�������
***********************************************************************************/
void ShowSysTime(uint8 line,uint8 column,uint8 timetype)
{
//	uint8 i;
	uint8 datebuf[10];
	uint8 data1,data2;
	
	line = line % 10;
	
	//��ʱ���Լ�д��Ĺ̶�ֵ(23��44��10��)���Ժ�������ĴӼ�������ʱ�亯��
	//datebuf[0] = 0x10;
	//datebuf[1] = 0x44;
	//datebuf[2] = 0x23;
	
	//UserReadDateTime(&datebuf[0]);//���ʱ������
	ReadDateTime(&datebuf[0]);
	datebuf[4] = datebuf[4]&0x1F;//ȥ������

//�ж�ʱ���Ƿ��������������������ʾTimeErr.
	data1 = BCD_TO_HEX(datebuf[5]);
	if(data1 < 15){  //������жϣ�С��15������Ϊ�쳣��û�е�ػ�û��Уʱ��
		ShowLineChar_HZ(line,column-6,ZCTimeErr,7);
	}
	else{	//������ʾʱ�䡣
	
		//ʱ
		data1 = datebuf[2]/0x10;
		data2 = datebuf[2]%0x10;
		ShowChar(line,column,data1+0x30);
		ShowChar(line,column+3,data2+0x30);
		
		ShowChar(line,column+6,0x3a);//ð��
		
		//��
		data1 = datebuf[1]/0x10;
		data2 = datebuf[1]%0x10;
		ShowChar(line,column+9,data1+0x30);
		ShowChar(line,column+12,data2+0x30);
	
		
		//��			
		if(timetype == 1)
		{
			ShowChar(line,column+15,0x3a);//ð��
			data1 = datebuf[0]/0x10;
			data2 = datebuf[0]%0x10;
			ShowChar(line,column+18,data1+0x30);
			ShowChar(line,column+21,data2+0x30);
		
		}
	}

}
/**********************************************************************************
*��������ShowDate
*���ܣ���ʾ���ڣ������ָ�ʽ��ʾ
*��ڲ�����line:��ʼ����(0--10)
		   datebuf:�� �� ��
		   column:��ʼ����(0--54)
		   datetype����ʾ��ʽ���ͣ�0ΪYY-MM-DD��1ΪYY��MM��DD��
*���ز�������
***********************************************************************************/
void ShowDate(uint8 line,uint8 column,uint8* datebuf,uint8 datetype)
{
	//uint8 i;
	//uint8 datebuf[10];
	uint8 data1,data2;
	
	line = line % 10;
	
	datebuf[1] = datebuf[1]&0x1F;//ȥ������
	
		//��
	//ShowChar(line,column,0x32);
	//ShowChar(line,column+3,0x30);
	//column = column + 6;//Ϊ�˲�Ӱ��������ĸĶ����˴�columnֵ�䶯
	//====================================
	data1 = datebuf[2]/0x10;
	data2 = datebuf[2]%0x10;
	ShowChar(line,column,data1+0x30);
	ShowChar(line,column+3,data2+0x30);
	if(datetype == DATETYPE_CH)
	{
		ShowChar(line,column+6,0x2d);//"-"
		column = column + 9;
	}
	else
	{
		ShowHZ(line,column+6,0xfb);//"��"
		column = column + 12;
	}
		//��
	data1 = datebuf[1]/0x10;
	data2 = datebuf[1]%0x10;
	ShowChar(line,column,data1+0x30);
	ShowChar(line,column+3,data2+0x30);
	if(datetype == DATETYPE_CH)
	{
		ShowChar(line,column+6,0x2d);//"-"
		column = column + 9;
	}
	else
	{
		ShowHZ(line,column+6,0xfc);//"��"
		column = column + 12;
	}
	
		//��			
	data1 = datebuf[0]/0x10;
	data2 = datebuf[0]%0x10;
	ShowChar(line,column,data1+0x30);
	ShowChar(line,column+3,data2+0x30);
	
	if(datetype == DATETYPE_HZ)
	{
		ShowHZ(line,column+6,0xf3);//"��"		
	}		
	return;
}

/**********************************************************************************
*��������ShowTime
*���ܣ���ʾ����ʱ�䣬�����ָ�ʽ��ʾ
*��ڲ�����line:��ʼ����(0--10)
		   column:��ʼ����(0--54)
		   datebuf:�� �� ʱ
		   timetype����ʾ��ʽ���ͣ�0ΪHH:MM��1ΪHH:MM:SS
*���ز�������
***********************************************************************************/
void ShowTime(uint8 line,uint8 column,uint8* datebuf,uint8 timetype)
{
//	uint8 i;
	//uint8 datebuf[10];
	uint8 data1,data2;
	
	line = line % 10;
	
		//ʱ
		data1 = datebuf[2]/0x10;
		data2 = datebuf[2]%0x10;
		ShowChar(line,column,data1+0x30);
		ShowChar(line,column+3,data2+0x30);
		
		ShowChar(line,column+6,0x3a);//ð��
		
		//��
		data1 = datebuf[1]/0x10;
		data2 = datebuf[1]%0x10;
		ShowChar(line,column+9,data1+0x30);
		ShowChar(line,column+12,data2+0x30);
	
		
		//��			
		if(timetype == 1)
		{
			ShowChar(line,column+15,0x3a);//ð��
			data1 = datebuf[0]/0x10;
			data2 = datebuf[0]%0x10;
			ShowChar(line,column+18,data1+0x30);
			ShowChar(line,column+21,data2+0x30);
		
		}

}

/**********************************************************************************
*��������ShowIniDate
*���ܣ���ʾ��ʼ������
*��ڲ�����line:��ʼ����(0--9)
		   column:��ʼ����(0--54)
		   IniDate:��ʼ�����������ַ
*���ز�������
***********************************************************************************/
void ShowIniDate(uint8 line,uint8 column,uint8* IniDate)
{
	ShowChar(line,column,IniDate[0]);
	ShowChar(line,column+3,IniDate[1]);
	ShowHZ(line,column+6,0xfb);//��
	ShowChar(line,column+11,IniDate[2]);
	ShowChar(line,column+14,IniDate[3]);
	ShowHZ(line,column+17,0xfc);//��
	ShowChar(line,column+22,IniDate[4]);
	ShowChar(line,column+25,IniDate[5]);
	ShowHZ(line,column+28,0xf3);//��
}

/**********************************************************************************
*��������ShowIniTime
*���ܣ���ʾ��ʼ��ʱ��
*��ڲ�����line:��ʼ����(0--9)
		   column:��ʼ����(0--54)
		   IniDate:��ʼ��ʱ�������ַ
*���ز�������
***********************************************************************************/
void ShowIniTime(uint8 line,uint8 column,uint8* IniTime)
{

	ShowChar(line,column,IniTime[0]);
	ShowChar(line,column+3,IniTime[1]);
	ShowHZ(line,column+6,0xd1);//ʱ
	ShowChar(line,column+11,IniTime[2]);
	ShowChar(line,column+14,IniTime[3]);
	ShowHZ(line,column+17,0xfd);//��
	ShowChar(line,column+22,IniTime[4]);
	ShowChar(line,column+25,IniTime[5]);
	ShowHZ(line,column+28,0xfe);//��
}
void ShowTimeMS(uint8 line,uint8 column,uint8* Time)
{

	uint8 data1,data2;
	
	line = line % 10;
	
		//ʱ
		data1 = Time[1]/0x10;
		data2 = Time[1]%0x10;
		ShowChar(line,column,data1+0x30);
		ShowChar(line,column+3,data2+0x30);
		
		ShowHZ(line,column+6,0xd1);//ʱ
		
		//��
		data1 = Time[0]/0x10;
		data2 = Time[0]%0x10;
		ShowChar(line,column+11,data1+0x30);
		ShowChar(line,column+14,data2+0x30);
		ShowHZ(line,column+18,0xfd);//��
		

}
/**********************************************************************************
*��������ShowLine
*���ܣ���ʾ���ߣ�������columnnum�涨
*��ڲ�����line:��ʼ����(0--160)
		   column:��ʼ����(0--54)
		   columnnum:���ߵĳ���(0-160)
*���ز�������
***********************************************************************************/
void ShowLine(uint8 line,uint8 column,uint8 columnnum)
{
	uint8 i;
	uint8 lastbit;
	uint8 lastnum;
	
	line = line % 160;
	
	lastbit = columnnum % 3;
	columnnum = columnnum / 3;
/*	
	if(lastbit != 0)
	{
		columnnum += 1;
	}
*/	
	lastnum = columnnum % 2;
	columnnum = columnnum/2;
	SetLine1(line);
	SetColumn(column);
	for(i = 0;i < columnnum;i++)//��6�����ص�Ϊ��λ����
	{
		writed(0xff);
		writed(0xff);
		writed(0xff);
	}
#ifdef UPDOWN_DIS
	if(lastnum != 0)
	{
		writed(0xff);
		if(lastbit == 1)
		{
			writed(0xf0);
			writed(0x0f);
		}
		else if(lastbit == 2)
		{
			writed(0xf0);
			writed(0xff);
		}
		else
		{
			writed(0xf0);
		}		
	}
	else
	{
		if(lastbit == 1)
		{
			writed(0x00);
			writed(0xf0);
		}
		else if(lastbit == 2)
		{
			writed(0x0f);
			writed(0xf0);
		}		
	}

#else	
	if(lastnum != 0)//������6�����ص�����
	{
		writed(0xff);
		
		if(lastbit == 1)
		{
			writed(0xff);
			writed(0x00);
		}
		else if(lastbit == 2)
		{
			writed(0xff);
			writed(0xf0);
		}
		else
		{
			writed(0xf0);
		}

	}
	else
	{
		if(lastbit == 1)
		{
			writed(0xf0);
			writed(0x00);
		}
		else if(lastbit == 2)
		{
			writed(0xff);
			writed(0x00);
		}

	}
#endif
}

/**********************************************************************************
*��������ShowColumn
*���ܣ���ʾ���ߣ�������linenum�涨
*��ڲ�����line:��ʼ����(0--160)
		   column:��ʼ����(0--160)
		   linenum:���ߵĳ���(0-160)
*���ز�������
***********************************************************************************/
void ShowColumn(uint8 line,uint8 column,uint8 linenum)
{
	uint8 i;
	uint8 lastbit;
	//uint16 seg1,seg2,seg3;
	
	line = line % 160;
	column = column % 160;
	
	lastbit = column % 3;
	column = column / 3;
	
#ifdef UPDOWN_DIS
	if(lastbit == 0)
	{
		//seg1 = 0xf0;
		//seg2 = 0x00;
		//seg3 = 0x0f;
		for(i = 0; i < linenum; i++)
		{
			SetLine1(line + i);
			SetColumn(column);
			writed(0x00);
			writed(0xf0);
			//writed(0x00);
		}	
	}
	else if(lastbit == 1)
		{
			//seg1 = 0x0f;
			//seg2 = 0xf0;
			//seg3 = 0x00;	
			for(i = 0; i < linenum; i++)
			{
				SetLine1(line + i);
				SetColumn(column+1);
				writed(0xf0);
				writed(0x00);
				//writed(0x00);					
			}	
		}
		else
		{
			//seg1 = 0x00;
			//seg2 = 0x0f;
			//seg3 = 0xf0;
			for(i = 0; i < linenum; i++)
			{
				SetLine1(line + i);
				SetColumn(column+1);
				writed(0x0f);
				writed(0x00);
				//writed(0x0f);
			}			
		}
#else	
	if(lastbit == 0)
	{
		//seg1 = 0xf0;
		//seg2 = 0x00;
		//seg3 = 0x0f;
		for(i = 0; i < linenum; i++)
		{
			SetLine1(line + i);
			SetColumn(column);
			writed(0xf0);
			writed(0x00);
			//writed(0x00);
		}	
	}
	else if(lastbit == 1)
		{
			//seg1 = 0x0f;
			//seg2 = 0xf0;
			//seg3 = 0x00;	
			for(i = 0; i < linenum; i++)
			{
				SetLine1(line + i);
				SetColumn(column);
				writed(0x0f);
				writed(0x00);
				//writed(0x00);					
			}	
		}
		else
		{
			//seg1 = 0x00;
			//seg2 = 0x0f;
			//seg3 = 0xf0;
			for(i = 0; i < linenum; i++)
			{
				SetLine1(line + i);
				SetColumn(column);
				writed(0x00);
				writed(0xf0);
				//writed(0x0f);
			}			
		}
#endif
/*		
	for(i = 0; i < linenum; i++)
	{
		SetLine1(line + i);
		SetColumn(column);
		writed(seg1);
		writed(seg2);
		writed(seg3);		
	}	
*/
}

/**********************************************************************************
*��������CleanScreen
*���ܣ�����Ļ��
*��ڲ�����startline:��ʼ����(0--160)
		   startcolumn:��ʼ����(0--54)
		   linenum�������������0--160��
		   columnnum:���������(0-54)
*���ز�������
***********************************************************************************/
void CleanScreen(uint8 startline,uint8 startcolumn,uint8 linenum,uint8 columnnum)
{
	uint8 i,j;
	startline = startline % 160;
	startcolumn = startcolumn % 54;
	
	if(columnnum%2)
	{
		columnnum = columnnum/2 + 1;
	}
	else
	{
		columnnum = columnnum/2;
	}
	
	for(i=0;i<linenum;i++)
	{
		SetLine1(startline+i);
		SetColumn(startcolumn);
		for(j=0;j<columnnum;j++)
		{
			writed(0x00);
			writed(0x00);
			writed(0x00);
		}
		
	}
}
/**********************************************************************************
*��������BlackScreen
*���ܣ�����Ļ��ڣ�
*��ڲ�����startline:��ʼ����(0--160)
		   startcolumn:��ʼ����(0--54)
		   linenum��д�ڵ�������0--160��
		   columnnum:д�ڵ�����(0-54)
*���ز�������
***********************************************************************************/
void BlackScreen(uint8 startline,uint8 startcolumn,uint8 linenum,uint8 columnnum)
{
	uint8 i,j;
	startline = startline % 160;
	startcolumn = startcolumn % 54;
/*	
	if(columnnum%2)
	{
		columnnum = columnnum/2 + 1;
	}
	else
	{
		columnnum = columnnum/2;
	}
	
	for(i=0;i<linenum;i++)
	{
		SetLine1(startline+i);
		SetColumn(startcolumn);
		for(j=0;j<columnnum;j++)
		{
			writed(0xff);
			writed(0xff);
			writed(0xff);
		}
		
	}
*/

	for(i=0;i<linenum;i++)
	{
		SetLine1(startline+i);
		SetColumn(startcolumn);
		for(j=0;j<(columnnum/2);j++)
		{
			writed(0xff);
			writed(0xff);
			writed(0xff);
		}
		for(j=0;j<(columnnum%2);j++)
		{
			writed(0xff);
			writed(0xff);
		}
		
	}
}
/*=====================================
����Ϊ�˵��������������ⴴ���ļ�
=======================================*/
void ShowMenuTop(void)
{
	CleanScreen(0,0,16,54);
	ShowGPRSSignal(0,1);	//��ʾ�ź����ꡣ
 	//ShowGC(0,13);	   //��ʾ�ڵ�Z�ꡣ
 	//ShowAbnormalNum(0,18);	   //��ʾ00.
 	ShowAbnormal(1,0,16);	   //��ʾMBUS��·�쳣ָʾ��
 	ShowAbnormal(2,0,25);	   //��ʾSD���쳣��
 	//ShowRotatePoint(0,26);
 	
 	ShowSysTime(0,38,FALSE);  //��ʾʱ�䡣
 	ShowLine(15,0,160);
}

void ShowMenuBottom(uint8 Tip)
{
	//Tip = 1;
	ShowPicture(144,0,16,160,&MenuBottomPic[Tip][0]);
	ShowCommunication();//��ʾ "�ѵ�¼"��"δ��¼"
	ShowLine(145,0,160);
}

void ShowCommunication(void)
{
	GPRS_RUN_STA luGPRSSta;
	
	ReadGprsRunSta(&luGPRSSta);

   if(luGPRSSta.Connect)
   ShowPicture(145,41,14,36,&CommuniReg[0][0]);
   else
   ShowPicture(145,41,14,36,&CommuniReg[1][0]);

}

void ClearCommunication(void)
{
   ShowPicture(145,41,14,36,&CommuniReg[2][0]);
}


//��ʾ��������
void ShowStartPic(void)
{
	display_white();
#if SC_EXPROCAL_EN == 1
	ShowPicture(28,9,85,112,&RisesunLogo_Only[0]);//ͼƬ
	ShowPicture(115,11,16,96,&SystemStart[0]);//����
#else
	ShowPicture(28,9,104,104,&StartPic[0]);	//��̩�����ĵ�ͼ��
#endif
}

void zx_test(void)
{
	//uint32 i;
//	display_black();
//   	delay(0x005ffFFF);
// for(i=0;i<10000;i++)  	
// {   	
 	display_white();
//  	ReverseShowChar(1,0);

//   	delay(0x005ffFFF);
 
	ShowMenuTop();
	ShowMenuBottom(1);

 
/* 
	//ShowChar(0,0,0x30);
	//ShowChar(0,3,0x31);
	ShowChar(0,0,0x33);
	//ReverseShow38(0,6);
	ReverseShowChar(0,0);
	//ShowChar(0,9,0x34);
	ShowChar(0,12,0x35);
	ShowChar(0,15,0x36);
    ShowChar(0,18,0x37);
	ShowChar(0,21,0x38);
	ShowChar(0,24,0x39);
	ShowChar(0,27,0x41);
	ShowChar(0,30,0x42);
	ShowChar(0,33,0x61);
	ShowChar(0,36,0x62);

	
	ShowHZ(2,0,0x90);
	ShowHZ(2,5,0x92);
	ShowHZ(2,10,0x93);
	ShowHZ(2,15,0x94);
	ShowHZ(2,20,0x95);
	ShowHZ(2,25,0x96);
	 
*/		
	//ShowLineHZ(8,0,10,LineHZ1,sizeof(LineHZ1));
//	ShowLineChar(8,0,LineChar1,sizeof(LineChar1));
/*		
	for(i=0;i<10;i++)
	{
		ReverseShowLine(i);
	}
*/	
/*	ReverseShowChar(1,0);
	ReverseShowHZ(7,0);
	ReverseShowHZ(2,5);
	ReverseShowLine(8);
*/
//	ShowPicture(16,0,64,160,&MenuPic1[0]);
		delay(0x005ffFFF);
			delay(0x005ffFFF);
//	ShowPicture(16,0,112,160,&MenuPic1_2[0]);	
		delay(0x005ffFFF);
			delay(0x005ffFFF);
//	ShowPicture(16,0,128,160,&MenuPic1_2_11[0]);
		delay(0x005ffFFF);
	CleanScreen(16,0,128,54);
//	ShowPicture(16,0,80,160,&MenuPic1_2_12[0]);	
	delay(0x005ffFFF);
		delay(0x005ffFFF);
		
//	SetLine(6);
//	SetColumn(0);
	
//	writei(0xa5); //Set All-Pixel-ON ȫ��Ϊ��ɫ
//	writei(0xa7);  //Set Inverse Display //ȫ��Ļȡ��ɫ
//	writei(0xc8);  // Set N-Line Inversion
//	writei(0x1A);
	
//	writei(0xc8);  // Set N-Line Inversion
//	writei(0x1d);	
//	writei(0xa7);  //Set Inverse Display //ȫ��Ļȡ��ɫ
	
	delay(0x005ffFFF);
	delay(0x005ffFFF);
//  }	
/*	
    delay(0x005ffFFF);
   	delay(0x005ffFFF);
   	delay(0x005ffFFF);
   	delay(0x005ffFFF);
   	delay(0x005ffFFF);
   	
   	vertical();
    delay(0x005fFFFF);

	vertical1();
    delay(0x005fFFFF);
      
      
	horizontal2();
    delay(0x005fFFFF);
      

    snow();
    delay(0x005fFFFF);

    network();
    delay(0x005fFFFF);

    frame();
    delay(0x005fFFFF);
 */   
 
 
}

uint8 CheckDateAndTime(uint8* DTBcd)
{
	uint8 month1;
	uint8 day1;
	uint8 days[13];
	uint8 i;
	
	month1 = DTBcd[3]&0x1F;
	day1 = DTBcd[2];
	
	month1 = BcdToHex(month1);
	day1 = BcdToHex(day1);
	
	days[0] = 0;
	days[1] = 31;
	days[2] = 28;
	days[3] = 31;
	days[4] = 30;
	days[5] = 31;
	days[6] = 30;
	days[7] = 31;
	days[8] = 31;
	days[9] = 30;
	days[10] = 31;
	days[11] = 30;
	days[12] = 31;
	
	for(i=0;i<4;i++){
		if(BcdCheck(DTBcd[i])==FALSE){
			return (1);
		}
	}
	
	if(BcdToHex(DTBcd[0])>60){
		return 3;
	}
	if(BcdToHex(DTBcd[1])>23){
		return 4;
	}
	
	if( (month1 == 0) || (month1>12) )
	{
		return 6;
	}
	
	if( month1!= 2)
	{
		if ( (day1 > days[month1]) || (day1 ==0) )
		return 5;
	}
	
	if(month1 == 2)
	{
		if(day1 > 29)
		{
			return 6;
		}
	}
	
	return (0);//���������ֽ�ȫΪ�Ϸ�BCD�룬���ںϷ�ȡֵ��Χ��
}
