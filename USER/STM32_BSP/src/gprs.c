

#include <includes.h>

#define    HEAD_MAX_LEN    64
#define	   SOCKET_TCP	   0


//����˵����Ats_: AT��������,Ata_: ATӦ������
char Ats_CGMM[] = "AT+CGMM\r";
/*begin:yangfei modified 2013-01-15*/
char Ata_CGMM_xmz_R3[] = "\r\nMC52iR3\r\n\r\nOK\r\n";
char Ata_CGMM_xmz[] = "\r\nMC52i\r\n\r\nOK\r\n";
/*end:yangfei modified 2013-01-15*/
char Ats_SMSO[] = "AT^SMSO\r";
char Ata_SMSO[] = "\r\n^SMSO: MS OFF\r\n";

//����connecting profile:
//at^sics=0,conType,GPRS0
//at^sics=0,apn,cmnet
char Ats_SICS_conType[] = "at^sics=0,conType,GPRS0\r";
//char Ats_SICS_apn[256]="at^sics=0,apn,cmnet\r";    //���ڹ�����
char Ats_SICS_apn[3][256] =
{
    {"at^sics=0,apn,cmnet\r"},   		 //���ڹ�����
    {"at^sics=0,apn,UNIM2M.NJM2MAPN\r"},   //������ͨM2M����������
    {"at^sics=0,apn,whrd.wh.sd\r"}    //���������ȵ�VPN��.
};




///����tcp service profile
//at^siss=1,srvType,socket
char Ats_SISS_srvType[] = "at^siss=1,srvType,socket\r";

//at^siss=1,conId,0
char Ats_SISS_conId[] = "at^siss=1,conId,0\r";

//at^siss=1,address,"socktcp://117.22.67.183:5100"
char Ats_SISS_address_h[] = "at^siss=1,address,\"socktcp://"; //117.22.67.183:5100\r\"";
char Ats_SISS_address[96];
char IpAddrXmz[64] = "122.5.18.174"; //117.22.0.121

char IpPort11[16] = "1141";
char colon[] = ":";

//��TCP����
//AT^SISO=1
char Ats_SISO[] = "AT^SISO=1\r"; //AT^SISO=1
char Ats_SISI[] = "AT^SISI=1\r"; //AT^SISI=1
char Ata_SISI[] = "\r\n^SISI: 1,4,0,0,0,0\r\n\r\nOK\r\n";
char Ata_SISI_1[] = "\r\n^SISI: 1,4,"; //"\r\n^SISI: 1,4,0,0,0,0\r\n\r\nOK\r\n";

char Ats_SISW_h[] = "AT^SISW=1,"; //at^sisw=1,30\r
char Ats_SISW[64];
char Ata_SISW_m[] = "\r\n^SISW: 1,"; //^SISW: 1, 20, 20

char Ata_URC_SISR[] = "\r\n^SISR: 1, 1\r\n"; //^SISW: 1, 1\r\n
char Ats_SISR[] = "AT^SISR=1,1500\r";
char Ata_SISR_m[] = "\r\n^SISR: 1, "; //


char Ats_AT[] = "AT\r";
char Ata_OK[] = "\r\nOK\r\n";
char Ata_OK123[] = "\r\n\r\nOK\r\n";
//char Ata_OK125[]="\r\r\n\r\nOK\r\n";
char Ats_SSYNC[] = "AT^SSYNC=1\r";

char Ats_SSYNC_c[] = "AT^SSYNC?\r";
char Ata_SSYNC_c[] = "\r\n^SSYNC: 1\r\n\r\nOK\r\n";

//char Ats_IPR[]="AT+IPR=9600\r";
//char Ats_IPR[]="AT+IPR=57600\r";
char Ats_IPR[] = "AT+IPR=115200\r";
char Ats_IPR_0[] = "AT+IPR=0\r";

char Ats_IPR_c[] = "AT+IPR?\r";
//char Ata_IPR_c[]="\r\n+IPR: 9600\r\n\r\nOK\r\n";
//char Ata_IPR_c[]="\r\n+IPR: 57600\r\n\r\nOK\r\n";
char Ata_IPR_c[] = "\r\n+IPR: 115200\r\n\r\nOK\r\n";

char Ats_CPIN[] = "AT+CPIN\?\r"; ////��� SIM �����Ƿ�����
char Ata_CPIN[] = "\r\n+CPIN: READY\r\n\r\nOK\r\n";

char Ats_CREG[] = "AT+CREG\?\r"; ////��� GSM ����ע�����
char Ata_CREG[] = "\r\n+CREG: 0,1\r\n\r\nOK\r\n"; //����
char Ata_CREG_Or[] = "\r\n+CREG: 0,5\r\n\r\nOK\r\n"; //����

char Ats_CGREG_1[] = "AT+CGREG=1\r"; //


char Ats_CGATT_1[] = "AT+CGATT\?\r";
char Ata_CGATT_1[] = "+CGATT: 1\r\n\r\nOK\r\n";
char Ats_CGATT[] = "AT+CGATT=1\r";
//char Ats_CGAUTO[]="AT+CGAUTO=1\r";

char Ats_CGREG[] = "AT+CGREG\?\r"; //��� GPRS ����ע�����
char Ata_CGREG[] = "\r\n+CGREG: 0,1\r\n\r\nOK\r\n"; //����
char Ata_CGREG_Or[] = "\r\n+CGREG: 0,5\r\n\r\nOK\r\n"; //����

//char Ats_CGDCONT[256]="AT+CGDCONT=1,\"ip\",\"cmnet\"\r";  //���� APN���߽���� ,cmnet,���ڹ�����
char Ats_CGDCONT[3][256] =
{
    {"AT+CGDCONT=1,\"ip\",\"cmnet\"\r"},	//���ڹ�����
    {"AT+CGDCONT=1,\"ip\",\"UNIM2M.NJM2MAPN\"\r"},  //������ͨM2M����������
    {"AT+CGDCONT=1,\"ip\",\"whrd.wh.sd\"\r"}  //���������ȵ�VPN����
};


//Ata_OK

//char Ats_ETCPIP[]="AT%ETCPIP=\"csdytq\",\"csdytq\"\r";   //���� ppp ����
char Ats_ETCPIP[] = "AT%ETCPIP=\"\",\"\"\r"; //���� ppp ����
//Ata_OK
//============��������
//char At_IPOPEN[]="AT%IPOPEN=\"UDP\",";//AT%IPOPEN="TCP","222.91.64.151",5100
char At_IPOPEN[] = "AT%IPOPEN=\"TCP\","; //AT%IPOPEN="TCP","222.91.64.151",5100
char IpAddr[64] = "\"122.5.18.174\""; //117.22.0.121
char comma[] = ",";
char IpPort[16] = "1141";
char LcIpPort[16];
char over123[] = "\r";
char over124[] = "\"\r";
char Ats_IPOPEN[128];
char Ata_IPOPEN[] = "\r\nCONNECT\r\n";
//=============

char Ats_ATE0[] = "ATE0\r";
//Ata_OK
char Ats_CSQ[] = "AT+CSQ\r"; //��鵱�ص������ź�ǿ��
//char Ata_CSQ[]="\r\n+CSQ: 27,99 \r\n\r\nOK\r\n";
char Ats_IPCLOSE[] = "AT%IPCLOSE?\r";
char Ata_IPCLOSE[] = "\r\n%IPCLOSE: 1,0,0\r\n\r\nOK\r\n";
//========================
char AtaHead_CSQ[] = "\r\n+CSQ:"; //"\r\n+CSQ: 27,99 \r\n\r\nOK\r\n";
char AtaHead_IPSEND[] = "\r\n%IPSEND:"; //"\r\n%IPSEND:15\r\n\r\nOK\r\n" //900c: \r\n%IPSEND:1,15 \r\nOK\r\n
char AtaHead_IPDATA[] = "%IPDATA:";
//%IPDATA:8,"3132333435363636"
//%IPDATA:29,"3234323334323334323334333234323334333234323334323334323334"
//%IPDATA:144,"333234353334353334353334353334353335333435333435333435343335333435333435343335343335333435333435333435333435333435333435333435333435333435333435333234353334353334353334353334353335333435333435333435343335333435333435343335343335333435333435333435333435333435333435333435333435333435333435"
//%IPDATA:25,"68450045006841FFFFFFFFFF84F10000801E3117070006A416"\r\n
//========================
char Ats_IPSEND[] = "AT%IPSEND=\"";
char over_isnd[] = "\"\r";
///r/nERROR: 9 //��ǰû�� TCP�� UDP ���Ӵ���(��� IPSEND)
///r/nERROR: 20//TCP���ͻ�������
static uint32 gtmpnum;
char  TmpBuf[HEAD_MAX_LEN];//���Թ۲���
GPRS_RUN_STA gGprsRunSta;
GlobalPara   GVar;
char  gIpSendData[MAX_IPSEND_GTM900C_BYTENUM * 2 + 100]; //900Cһ�η���(AT%IPSEND)���ΪMAX_IPSEND_GTM900C_BYTENUM==1024�ֽ�(��������2048�������ַ�,���Գ˶�),�ټ������ŵȾͶ������һЩ

extern UpCommParaSaveType	gPARA_UpPara;		//GPRS����ͨѶ����

uint16 gFirRand16;
uint8 gStartTime[6];

uint8 test_buff[512];
OS_EVENT *GprsXmzSem;
extern uint8 gGprsFirst;

void GprsOff_xmz(void)
{

    GPRS_POW_ON();
    OSTimeDly(OS_TICKS_PER_SEC / 2);
    GPRS_POW_OFF();

}

void GprsOn_xmz(void)
{
    GPRS_POW_OFF();
    OSTimeDly(OS_TICKS_PER_SEC / 2);
    GPRS_POW_ON();

    GPRS_RESET_DISABLE();

    OSTimeDly(OS_TICKS_PER_SEC);
    GPRS_RESET_ENABLE();
    OSTimeDly(OS_TICKS_PER_SEC);
    GPRS_RESET_DISABLE();
    OSTimeDly(OS_TICKS_PER_SEC);
}

uint16  RandFirRand16(void)
{
    uint32 seed, randu32;
    CPU_SR	cpu_sr;

    OS_ENTER_CRITICAL();
    seed = *((uint32 *)gStartTime); //���ʱ�����������
    srand(seed);
    randu32 = rand();
    OS_EXIT_CRITICAL();

    gFirRand16 = randu32 % LC_PORT_RANGE;
    return gFirRand16;

}

uint16  RandLcPort(void)
{
    uint16 lcport;
    uint32 randu32;
    CPU_SR	cpu_sr;

    OS_ENTER_CRITICAL();
    srand(OSIdleCtr);
    randu32 = rand();
    OS_EXIT_CRITICAL();
    randu32 = randu32 % LC_PORT_RANGE;
    lcport = LC_PORT_MIN + (gFirRand16 + randu32) % LC_PORT_RANGE;
    return lcport;

}

void UGprsWriteStr(char *Str)
{
    UpDevSend(UP_COMMU_DEV_GPRS, (uint8 *)Str, strlen(Str));
    //UP_COMMU_DEV_232   UP_COMMU_DEV_GPRS
}

uint8 GetGprsRunSta_ModuId(void)
{
    uint8 val;
    CPU_SR	cpu_sr;

    OS_ENTER_CRITICAL();
    val = gGprsRunSta.ModuId;
    OS_EXIT_CRITICAL();
    return val;
}

//������Ascii�ַ���ʾ��16������ת����1��uint8��HEX����ԭ���ݸ�λ�ַ���ǰ����λ�ַ��ں�
uint8 nAsciiToHex(char *chars)
{
    uint8 i;
    uint8 val[2];
    for(i = 0; i < 2; i++)
    {
        if(chars[i] >= '0' && chars[i] <= '9')
        {
            val[i] = chars[i] - '0';
        }
        else if(chars[i] >= 'A' && chars[i] <= 'F')
        {
            val[i] = chars[i] - 'A' + 0x0A;
        }
        //else{}//�����Ƿ��ַ��ݴ����ⲿ��֤
    }
    return (val[0] * 16 + val[1]);

}

//����AsciiToHex�ķ�����,���ص�ASCII����ĸ����Ϊ��д
void nHexToAscii(uint8 data, char *chars)
{
    uint8 i;
    uint8 val[2];
    val[0] = data / 16;
    val[1] = data % 16;
    for(i = 0; i < 2; i++)
    {
        if(val[i] <= 9)
        {
            chars[i] = val[i] + '0';
        }
        else if(val[i] >= 0x0A && val[i] <= 0x0F)
        {
            chars[i] = val[i] - 0x0A + 'A';
        }
    }
}

//�����4��Ascii��ʾ��10������ת���� uint16��HEX������"256"ת��Ϊ0x100
//�������Ϊ�ַ���
uint8 AsciiDec(char *str, uint16 *result)
{
    uint8 i, len;
    uint16 val, pow;
    len = strlen(str);
    if(len > 4)
    {
        return 0xff;//���ȳ���
    }
    val = 0;
    pow = 1;
    for(i = 0; i < len; i++)
    {
        if(str[len - 1 - i] < 0x30 || str[len - 1 - i] > 0x39)
        {
            return 0xfe;//�ַ��Ƿ�
        }
        val = val + (str[len - 1 - i] - 0x30) * pow;
        pow *= 10;
    }
    *result = val;
    return 0;
}

uint8 ReadCurSType(void)
{
    uint8 val;
    CPU_SR	cpu_sr;

    OS_ENTER_CRITICAL();
    val = gGprsRunSta.SocketType;
    OS_EXIT_CRITICAL();
    return val;
}
/*
��������:�Ƚ��ַ�
yangfei added note
*/
uint8 IfByte(uint8 data, char *HeadStr, char *OrHeadStr, uint8 p)
{
    if(data == (uint8)(HeadStr[p]))
    {
        return 0;//ƥ����ѡ��Ӧ�ɹ�
    }
    else if(OrHeadStr != NULL) //ƥ�䱸ѡ��Ӧ
    {
        if(data == (uint8)(OrHeadStr[p]))
        {
            return 1;//ƥ�䱸ѡ��Ӧ�ɹ�
        }
        else
        {
            return 2;//ƥ��ʧ��
        }
    }
    else
    {
        return 2;//ƥ��ʧ��
    }

}
//Find: ���շ�ʽ, TRUE: һֱ������ȷ��HeadStr,ֱ����ʱ������ַ�������ĳ���ܴ��ֵ;
//FALSE: ��OutTime �յ���һ���ַ�����������HeadStr�ĳ��Ⱥ󼴷��سɹ���ʧ��

//���������м��ֽ�ʱ�ĳ�ʱ��ע����Ҫ��������һ��������յ��ĵ�һ���ֽڲ�ƥ��ʱ��C-D������������IPCLOSEʱ���յ�IPDATA����
//��ô�ں����ƥ������ж�IPCLOSE�ĵ�һ���ַ���ƥ�䣬�ͱ���˶��м��ַ���ƥ�䣬���������յ�������IPCLOSE ����ƥ��
//�м��ַ�����ƥ��'P'����������ġ�����ȴ�ʱ��Ӧ�ÿ���GPRSģ���ڷ���IPDATA����ӦIPCLOSEʱ���ܴ��ڵ��������м��ַ�
//֮���ʱ������ʱ��������˴�ֵҪ�ʵ����ô�һ��

//ע��: �˺�����HeadStr��OrHeadStr���ȳ�ʱ����,�����޸�
uint8 GprsGetHead(uint8 device, char *HeadStr, char *OrHeadStr, uint16 OutTime, uint8 Find) //������
{
    uint8 err, flag;
    uint8 data, headlen;
    uint16 p;
    uint32 n;
    uint32 i = 0, count = 0;

    headlen = strlen(HeadStr);
    if(headlen == 0 || headlen > HEAD_MAX_LEN) //headlen �������0 ����HeadStr����ǿ�;HeadStr�������HEAD_MAX_LEN���ַ�
    {
        return 0xff;
    }
    //if(Find==FALSE){
    //OutTime=OS_TICKS_PER_SEC/5;
    //}
    err = UpGetch(device, &data, OutTime);
    if(err) //��ʱ
    {
        gtmpnum++;//Ϊ���ڴ˴������öϵ�
        return err;
    }
    /*begin:yangfei added 2013-02-26 for test */
    test_buff[i++] = data;
    /*end:yangfei added 2013-02-26 for test */
    n = 5000; ///��������������յ��ַ���
    flag = 0;
    p = 0;
    while(n > 1)
    {
        n--;
        //if(data!=(uint8)(HeadStr[p])){
        if(IfByte(data, HeadStr, OrHeadStr, p) >= 2)
        {
            if(Find == TRUE)
            {
                if(p > 0) //����ƥ��HeadStr�ĵ�1���ֽ��Ժ���ֽڣ���ƥ��p���ֽ� ��ƥ���p+1���ֽڳ���
                {
                    p = 0;   //
                    continue;//���¿�ʼƥ��HeadStr�ĵ�1���ֽ�
                }
            }
            else
            {
                flag = 0xfe; //��ʾʧ�ܣ�������Ҫ������headlen���ַ������Բ�������
                TmpBuf[p] = data; //���Թ۲���
                TmpBuf[p + 1] = '\0'; //���ڵ��Թ۲�
                p++;

            }
        }
        else
        {
            TmpBuf[p] = data; //���Թ۲���
            TmpBuf[p + 1] = '\0'; //���ڵ��Թ۲�
            p++;

        }

        if(p == headlen) //ƥ�����
        {
            gtmpnum++;//Ϊ���ڴ˴������öϵ�
            break;
        }
        err = UpGetch(device, &data, OutTime); //OS_TICKS_PER_SEC/5
        if(err) //
        {
            return 0xfe;	//�����м��ַ�ʱ��ʱ  //ncq080819 ����Ҳ����������ƥ���1���ַ����������泬ʱʱ��Ӧ��ô�������ð죬���ڻ�������
        }
        /*begin:yangfei added 2013-02-26 for test */
        test_buff[i++] = data;
        if(i >= 100)
        {
            //debug("\r\n%s\r\n",test_buff);
#if  0
            OSMutexPend (FlashMutex, 0, &Err);
            SDSaveData("test.txt", test_buff, 512, 512 * count);
            OSMutexPost (FlashMutex);
#endif
            i = 0;
            count++;
        }
        /*end:yangfei added 2013-02-26 for test */
    }
    if(n == 1) //���Ҵ��������ƣ�Ӧ����ʧ��
    {
        flag = 0xfd;
    }
    return flag;
}

uint8 CMD_AT_1(char *AtComd, char *AtRightAnsw, char *OrAtRightAnsw, uint16 OutTime)
{
    uint8 err;

    UGprsWriteStr(AtComd);//ע�⺯�����غ�Ӳ�����Ͳ�û�����
    err = GprsGetHead(UP_COMMU_DEV_AT, AtRightAnsw, OrAtRightAnsw, OutTime, TRUE); //���շ��ؽ����OutTime����������ϲ����ã��еĻ�ϳ����罨����������
    return err;

}

uint8 CMD_AT_RP_1(char *AtComd, char *AtRightAnsw, char *OrAtRightAnsw, uint16 OutTime, uint32 rp)
{
    uint32 i;
    uint8 err;
    for(i = 0; i < 1 + rp; i++) //rp:���Դ�����0 Ϊ�����ԣ�ֻ��һ��
    {
        FeedTaskDog();
        err = CMD_AT_1(AtComd, AtRightAnsw, OrAtRightAnsw, OutTime);
        if(err == 0) //�ɹ�
        {
            break;
        }
        OSTimeDly(2 * OS_TICKS_PER_SEC); //ʧ������ǰ����ʱ
        //UpQueueFlush(UP_COMMU_DEV_AT);
    }
    return err;


}

//ʹ���豸UP_COMMU_DEV_AT���ʲ������룬����ʹ���豸UP_COMMU_DEV_AT�ĺ����������䲢����
//ATEMode:�Ƿ�����ԣ�TRUE�������ԣ� FLASE����������
uint8 CMD_AT(char *AtComd, char *AtRightAnsw, char *OrAtRightAnsw, uint16 OutTime, uint8 ATEMode)
{
    uint8 err;
    //	while(1)
    //	{
    UGprsWriteStr(AtComd);//ע�⺯�����غ�Ӳ�����Ͳ�û�����
    //	}
    OSTimeDly(OS_TICKS_PER_SEC / 8);

    if(ATEMode == TRUE)
    {
        err = GprsGetHead(UP_COMMU_DEV_AT, AtComd, NULL, OS_TICKS_PER_SEC / 8, TRUE); //���ջ��ԣ���ʱ���ý�С
        //����������ʱ��OS_TICKS_PER_SEC/20�Ļ����յ�һ�������ֽھͳ�ʱ������Ϊ���͵��ֽڶ࣬����ռȥ�˺ܶ�ʱ��
        //����ע������ĳ�ʱ����Ӳ�����͵�ʱ�����ڡ����⣬�м��ַ��ĳ�ʱҪע��������GprsGetHead�����˵��
        if(err)
        {
            debug_err(gDebugModule[GPRS_MODULE], "AtComd err=%d ", err);
            return 0xff;
        }
    }
    err = GprsGetHead(UP_COMMU_DEV_AT, AtRightAnsw, OrAtRightAnsw, OutTime, FALSE); //���շ��ؽ����OutTime����������ϲ����ã��еĻ�ϳ����罨����������
    if(err)
    {
        debug_err(gDebugModule[GPRS_MODULE], "AtRightAnsw err=%d ", err);
    }
    return err;

}

//ʹ���豸UP_COMMU_DEV_AT���ʲ������룬����ʹ���豸UP_COMMU_DEV_AT�ĺ����������䲢����
//ATEMode:�Ƿ�Ĵ����ԣ�TRUE�������ԣ� FLASE����������
uint8 CMD_AT_RP(char *AtComd, char *AtRightAnsw, char *OrAtRightAnsw, uint16 OutTime, uint32 rp, uint8 ATEMode)
{
    uint32 i;
    uint8 err;
    for(i = 0; i < 1 + rp; i++) //rp:���Դ�����0 Ϊ�����ԣ�ֻ��һ��
    {
        FeedTaskDog();
        err = CMD_AT(AtComd, AtRightAnsw, OrAtRightAnsw, OutTime, ATEMode);
        if(err == 0) //�ɹ�
        {
            break;
        }
        OSTimeDly(2 * OS_TICKS_PER_SEC); //ʧ������ǰ����ʱ
        //UpQueueFlush(UP_COMMU_DEV_AT);
    }
    return err;
}

//============================================================================
//���ڽ����м䲿�ֳ��Ȳ��̶���AT����Ӧ��Ĳ��̶����Ȳ��֣�ƥ��β�ַ����˳�����������С�Ŀ��ܳ��Ⱥ����Ŀ��ܳ��ȷ���err
//�����������ڽ����м䲿�֣������ַ��ĳ�ʱʱ��̶�ΪOS_TICKS_PER_SEC/5
uint16 GprsGetViaTail(uint8 device, char *data, char Tail, char MinLen, uint16 MaxLen, uint8 *err)
{
    uint16 n;
    uint8 tmp;
    n = 0;
    do
    {
        if(n >= MaxLen)
        {
            *err = 0xff;
            return n;
        }
        *err = UpGetch(device, &tmp, OS_TICKS_PER_SEC / 5); //�����ַ��ĳ�ʱʱ��̶�Ϊ
        if(*err != OS_ERR_NONE) //��ʱ
        {
            return n;
        }
        data[n++] = tmp;
        data[n] = '\0'; //���ڵ��Թ۲�

    }
    while(tmp != Tail);
    if(n < MinLen)
    {
        *err = 0xfe;
    }
    return n;
}

//char Ata_IPCLOSE[]="\r\n%IPCLOSE: 1,0,0\r\n\r\nOK\r\n";
/*uint8 GprsGetIPCLOSE(uint16 OutTime)
{
	uint8 err;
	err=GprsGetHead(UP_COMMU_DEV_AT,Ata_IPCLOSE,OutTime,TRUE);
	return err;

}*/
//ʹ���豸UP_COMMU_DEV_AT���ʲ������룬����ʹ���豸UP_COMMU_DEV_AT�ĺ����������䲢����
//�رջ��Է�ʽ��ʹ��
uint8 CMD_IpClose_hw(void)
{
    uint8 err;
    UGprsWriteStr(Ats_IPCLOSE);
    err = GprsGetHead(UP_COMMU_DEV_AT, Ata_IPCLOSE, NULL, OS_TICKS_PER_SEC, TRUE);
    if(err)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
uint8 CMD_IpClose_xmz(void)
{
    uint8 err;
    OSSemPend(GprsXmzSem, 0, &err);
    OSTimeDly(OS_TICKS_PER_SEC / 10);
    err = CMD_AT_RP(Ats_SISI, Ata_SISI_1, NULL, OS_TICKS_PER_SEC * 2, 0, TRUE);
    if(err)
    {
        OSSemPost(GprsXmzSem);
        return TRUE;
    }
    //else{
    //	return FALSE;
    //}
    err = GprsGetHead(UP_COMMU_DEV_AT, Ata_OK, NULL, OS_TICKS_PER_SEC / 2, TRUE);
    if(err)
    {
        OSSemPost(GprsXmzSem);
        return TRUE;
    }
    else
    {
        OSSemPost(GprsXmzSem);
        return FALSE;
    }

}
uint8 CMD_IpClose(void)
{
    if(GetGprsRunSta_ModuId() == MODU_ID_HW)
    {
        return CMD_IpClose_hw();
    }
    else
    {
        return CMD_IpClose_xmz();
    }
}

//char AtaHead_CSQ[]="\r\n+CSQ: ";
//char Ata_CSQ[]="\r\n+CSQ: 27,99 \r\n\r\nOK\r\n";
//ʹ���豸UP_COMMU_DEV_AT���ʲ������룬����ʹ���豸UP_COMMU_DEV_ATIPD�ĺ����������䲢����
//�رջ��Է�ʽ��ʹ��
uint8 GprsGetCSQ(uint16 OutTime, char *pCsq)
{
    uint8 err;
    char  CsqStr[12];
    uint16 len;
    err = GprsGetHead(UP_COMMU_DEV_AT, AtaHead_CSQ, NULL, OS_TICKS_PER_SEC, TRUE);
    if(err == 0)
    {
        len = GprsGetViaTail(UP_COMMU_DEV_AT, CsqStr, '\n', 7, 9, &err);
        if(err)
        {
            return err;
        }

        err = GprsGetHead(UP_COMMU_DEV_AT, Ata_OK, NULL, OS_TICKS_PER_SEC / 5, FALSE);
        if(err)
        {
            return err;
        }
        if(CsqStr[len - 2] != '\r')
        {
            return 0xff;
        }
        memcpy(pCsq, &CsqStr[1], 7);
        //'\r'����Ŀո�Ͳ��ж���
        //CsqStr[len-3]='\0';
        //strcpy(pCsq,&CsqStr[1]);
    }
    else
    {
        return err;
    }
    return 0;
}

//ʹ���豸UP_COMMU_DEV_AT���ʲ������룬����ʹ���豸UP_COMMU_DEV_ATIPD�ĺ����������䲢����
//�رջ��Է�ʽ��ʹ��
uint8 CMD_Csq(uint8 *csq)//������/��Ϊģ�鹲��,����GprsXmzSem�Ի�Ϊģ����˵�ǲ���Ҫ��,������������Ҫ
{
    uint8 err;
    uint16 val;
    char CsqStr[12];

    OSSemPend(GprsXmzSem, 0, &err);
    OSTimeDly(OS_TICKS_PER_SEC / 10);

    UGprsWriteStr(Ats_CSQ);
    err = GprsGetCSQ(OS_TICKS_PER_SEC, CsqStr);
    if(err == 0)
    {
        if(CsqStr[1] == ',')
        {
            CsqStr[1] = '\0';
        }
        else if(CsqStr[2] == ',')
        {
            CsqStr[2] = '\0';
        }
        else
        {
            OSSemPost(GprsXmzSem);
            return 0xf0;
        }
        err = AsciiDec(CsqStr, &val);
        *csq = (uint8)val;
    }
    OSSemPost(GprsXmzSem);
    return err;

}

uint8 CMD_smso(void)
{
    uint8 err;

    err = CMD_AT_RP(Ats_SMSO, Ata_SMSO, NULL, 3 * OS_TICKS_PER_SEC, 0, TRUE);
    if(err)
    {
        return err;
    }

    err = GprsGetHead(UP_COMMU_DEV_AT, Ata_OK, NULL, OS_TICKS_PER_SEC / 2, FALSE);
    return err;

}

//char AtaHead_IPDATA[]="%IPDATA:";
//%IPDATA:25,"68450045006841FFFFFFFFFF84F10000801E3117070006A416"\r\n
//��ռ�豸UP_COMMU_DEV_ATIPD
uint8 GprsGetIPDATA_xmz(char *ipdata, uint16 OutTime, uint16 *StrLen)
{
    uint8 err;

    ipdata = ipdata;
    StrLen = StrLen;
    /*begin:yangfei added 2013-02-28 needed deleted*/
#if 1
    err = GprsGetHead(UP_COMMU_DEV_ATIPD, Ata_URC_SISR, NULL, OutTime, TRUE);
#else
    err = GprsGetHead(UP_COMMU_DEV_AT, Ata_URC_SISR, NULL, OutTime, TRUE);
#endif
    /*end:yangfei added 2013-02-28 needed deleted*/

    return err;

}

/*
get data from USART3RecQueue_At UP_COMMU_DEV_AT

jh:����
*/
uint8 GprsGetIPDATA_jh(char *ipdata, uint16 OutTime, uint16 *StrLen)
{
    uint8 err;
    char  lenstr[8];
    uint16 i, Len;
    OutTime = OutTime;
    OSSemPend(GprsXmzSem, 0, &err);

    OSTimeDly(OS_TICKS_PER_SEC / 10);
    UGprsWriteStr(Ats_SISR);

    err = GprsGetHead(UP_COMMU_DEV_AT, Ats_SISR, NULL, OS_TICKS_PER_SEC, TRUE);
    if(err)
    {
        OSSemPost(GprsXmzSem);
        debug_err(gDebugModule[GPRS_MODULE], "Ats_SISR err=%d", err);
        return err;
    }

    err = GprsGetHead(UP_COMMU_DEV_AT, Ata_SISR_m, NULL, OS_TICKS_PER_SEC, FALSE);
    if(err)
    {
        OSSemPost(GprsXmzSem);
        debug_err(gDebugModule[GPRS_MODULE], "Ata_SISR_m err=%d", err);
        return err;
    }

    Len = GprsGetViaTail(UP_COMMU_DEV_AT, lenstr, '\n', 3, 8, &err);
    if(err)
    {
        OSSemPost(GprsXmzSem);
        debug_err(gDebugModule[GPRS_MODULE], "%s %d err=%d", __FUNCTION__, __LINE__, err);
        return err;
    }
    if(lenstr[Len - 2] != '\r')
    {
        OSSemPost(GprsXmzSem);
        debug_err(gDebugModule[GPRS_MODULE], "%s %d err=%d", __FUNCTION__, __LINE__, err);
        return 0xff;
    }
    lenstr[Len - 2] = '\0';


    err = AsciiDec(lenstr, &Len);
    if(err)
    {
        OSSemPost(GprsXmzSem);
        debug_err(gDebugModule[GPRS_MODULE], "%s %d err=%d", __FUNCTION__, __LINE__, err);
        return 0xfe;
    }

    if(Len > MAX_REC_MC52i_BYTENUM)
    {
        return 0xdd;
    }
    for(i = 0; i < Len; i++)
    {
        err = UpGetch(UP_COMMU_DEV_AT, (uint8 *)(&ipdata[i]), OS_TICKS_PER_SEC);
        if(err)
        {
            OSSemPost(GprsXmzSem);
            debug_err(gDebugModule[GPRS_MODULE], "%s len=%d err=%d", __FUNCTION__, Len, err);
            return 0xf0;
        }
    }

    *StrLen = Len;
    OSSemPost(GprsXmzSem);
    return 0;
}

uint8 GprsGetIPDATA(char *ipdata, uint16 OutTime, uint16 *StrLen)
{
    uint8 err;
    char  lenstr[8];
    uint16 Len;

    err = GprsGetHead(UP_COMMU_DEV_ATIPD, AtaHead_IPDATA, NULL, OutTime, TRUE);
    if(err)
    {
        return err;
    }
    Len = GprsGetViaTail(UP_COMMU_DEV_ATIPD, lenstr, '\"', 3, 6, &err);
    if(err)
    {
        return err;
    }
    if(lenstr[Len - 1] != '\"')
    {
        return 0xff;
    }
    ////����Ͳ��ж� ","��
    lenstr[Len - 2] = '\0';

    err = AsciiDec(lenstr, &Len);
    if(err)
    {
        return 0xfe;
    }

    Len *= 2;
    Len += 3; //'\"' "\r","\n"
    Len = GprsGetViaTail(UP_COMMU_DEV_ATIPD, ipdata, '\n', Len, Len, &err);
    if(err)
    {
        return err;
    }
    if(ipdata[Len - 3] != '\"')
    {
        return 0xfe;
    }

    //����Ͳ��ж� "\r"��
    ipdata[Len - 3] = '\0';
    *StrLen = Len - 3;
    return 0;
}

//char AtaHead_IPSEND[]="\r\n%IPSEND:";//"\r\n%IPSEND:15\r\n\r\nOK\r\n"

uint8 GprsGetIPSEND(uint16 OutTime, uint16 *pBufNum)
{
    uint8 err;
    char Str[8];
    uint16 len;
    uint8 CurSocketType;
    CurSocketType = ReadCurSType();

    if(CurSocketType == 0) //TCP
    {
        err = GprsGetHead(UP_COMMU_DEV_AT, AtaHead_IPSEND, NULL, OutTime, TRUE);
    }
    else //UDP
    {
        err = GprsGetHead(UP_COMMU_DEV_AT, Ata_OK, NULL, OutTime, TRUE);
    }

    if(err == 0)
    {
        if(CurSocketType != 0) //UDP
        {
            *pBufNum = 15;
            return 0;
        }

        len = GprsGetViaTail(UP_COMMU_DEV_AT, Str, ' ', 4, 5, &err);
        if(err)
        {
            return err;
        }

        err = GprsGetHead(UP_COMMU_DEV_AT, Ata_OK, NULL, OS_TICKS_PER_SEC / 5, FALSE);
        if(err)
        {
            return err;
        }

        //if(Str[len-2]!='\r'){
        //	return 0xff;
        //}

        Str[len - 1] = '\0';
        err = AsciiDec(&Str[2], pBufNum);
        if(err)
        {
            return 0xfe;
        }
    }
    else
    {
        return err;
    }

    return 0;
}

//extern uint32 gprs_sfai1,gprs_sfai2;
uint8 GprsIPSEND_xmz(uint8 *ipdata, uint16 len, uint16 *pBufNum)
{
    char buf[8];
    uint8 err;
    uint8 PreSmybol[2] = {0xFB, 0xFB};

    if(len > MAX_IPSEND_MC52i_BYTENUM)
    {
        return 0xdd;
    }
    /*begin:yangfei added 2013-02-28 for ������Ϊ0ʱ����û������*/
    if(len == 0)
    {
        debug_info(gDebugModule[GPRS_MODULE], "GprsIPSEND_xmz len=0");
        return 0xd1;
    }
    /*end:yangfei added 2013-02-28  */
    OSSemPend(GprsXmzSem, 0, &err);
    OSTimeDly(OS_TICKS_PER_SEC / 10);
    strcpy(Ats_SISW, Ats_SISW_h);

    gpHex16ToStr(len, buf);
    /*begin:yangfei added 2013-03-28 for send PreSmybol 0xFB 0xFB so len need add 2*/
    if(*pBufNum == 0xaa55)
    {
        gpHex16ToStr(len + 2, buf);
    }
    /*end:yangfei added 2013-03-28  */
    strcat(Ats_SISW, buf);
    strcat(Ats_SISW, over123);

    UGprsWriteStr(Ats_SISW);

    err = GprsGetHead(UP_COMMU_DEV_AT, Ats_SISW, NULL, OS_TICKS_PER_SEC, TRUE);
    if(err)
    {
        LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: <GprsIPSEND_xmz> Ats_SISW get head String is %s!", TmpBuf);
        LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: <GprsIPSEND_xmz> Ats_SISW get head Failure, The err is %d!", err);
        debug_err(gDebugModule[GPRS_MODULE], "Ats_SISW get head Failure, The err is %d!", err);
        OSSemPost(GprsXmzSem);
        return err;
    }

    err = GprsGetHead(UP_COMMU_DEV_AT, Ata_SISW_m, NULL, OS_TICKS_PER_SEC, FALSE);
    if(err)
    {
        LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: <GprsIPSEND_xmz> Ata_SISW_m get head String is %s!", TmpBuf);
        LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: <GprsIPSEND_xmz> Ata_SISW_m get head Failure, The err is %d!", err);
        debug_err(gDebugModule[GPRS_MODULE], "Ats_SISW_m get head Failure, The err is %d!", err);
        OSSemPost(GprsXmzSem);
        //gprs_sfai1++;
        return err;
    }
    /*begin:yangfei added 2013-03-28 for send PreSmybol 0xFB 0xFB*/
    if(*pBufNum == 0xaa55)
    {
        UpDevSend(UP_COMMU_DEV_GPRS, PreSmybol, sizeof(PreSmybol));
    }
    /*end:yangfei added 2013-03-28  */
    UpDevSend(UP_COMMU_DEV_GPRS, ipdata, len);

    err = GprsGetHead(UP_COMMU_DEV_AT, Ata_OK, NULL, 2 * OS_TICKS_PER_SEC, TRUE);
    if(err)
    {
        LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: <GprsIPSEND_xmz> Ata_OK get head String is %s!", TmpBuf);
        LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: <GprsIPSEND_xmz> Ata_OK get head Failure, The err is %d!", err);
        debug_err(gDebugModule[GPRS_MODULE], "Ata_OK get head Failure, The err is %d!", err);
        OSSemPost(GprsXmzSem);
        //gprs_sfai2++;
        return err;
    }
    *pBufNum = 15;
    OSSemPost(GprsXmzSem);
    return 0;
}

uint8 GprsIPSEND(uint8 *ipdata, uint16 len, uint16 *pBufNum)
{
    if(GetGprsRunSta_ModuId() == MODU_ID_XMZ)
    {
        return GprsIPSEND_xmz(ipdata, len, pBufNum);
    }
    return 1;
}

void InitGprsRunSta(void)
{
    CPU_SR	cpu_sr;

    OS_ENTER_CRITICAL();
    ///////gGprsRunSta.ModuId=0;
    gGprsRunSta.Ready = FALSE;
    gGprsRunSta.Connect = FALSE;
    gGprsRunSta.SocketType = 0; //0:TCP;1:UDP  //��ReadyΪTRUE���京�����Ч
    gGprsRunSta.IpCloseNum = 0;
    gGprsRunSta.IpSendFailNum = 0;
    gGprsRunSta.IpSendRemnBufNum = 0;
    gGprsRunSta.Csq = 0;
    //gGprsRunSta.FrmRecTimes=0;
    //gGprsRunSta.FrmSndTimes=0;
    gGprsRunSta.RecDog = 0;
    gGprsRunSta.SndDog = 0;
    OS_EXIT_CRITICAL();
}

//��ȡGPRS����״̬
void ReadGprsRunSta(GPRS_RUN_STA *Sta)
{
    CPU_SR	cpu_sr;

    OS_ENTER_CRITICAL();
    memcpy((uint8 *)Sta, (uint8 *)(&gGprsRunSta), sizeof(GPRS_RUN_STA));
    OS_EXIT_CRITICAL();
}
//����GPRS����״̬
void UpdGprsRunSta(GPRS_RUN_STA *Sta)
{
    CPU_SR	cpu_sr;

    OS_ENTER_CRITICAL();
    memcpy((uint8 *)(&gGprsRunSta), (uint8 *)Sta, sizeof(GPRS_RUN_STA));
    OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_ModuId(uint8 val)
{
    CPU_SR	cpu_sr;

    OS_ENTER_CRITICAL();
    gGprsRunSta.ModuId = val;
    OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_Stype(uint8 val)
{
    CPU_SR	cpu_sr;

    OS_ENTER_CRITICAL();
    gGprsRunSta.SocketType = val;
    OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_Cont(uint8 val)
{
    CPU_SR	cpu_sr;

    OS_ENTER_CRITICAL();
    gGprsRunSta.Connect = val;
    OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_Ready(uint8 val)
{
    CPU_SR	cpu_sr;

    OS_ENTER_CRITICAL();
    gGprsRunSta.Ready = val;
    OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_IpCloseNum(uint8 val)
{
    CPU_SR	cpu_sr;

    OS_ENTER_CRITICAL();
    gGprsRunSta.IpCloseNum = val;
    OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_IpSendRemnBufNum(uint8 val)
{
    CPU_SR	cpu_sr;

    OS_ENTER_CRITICAL();
    gGprsRunSta.IpSendRemnBufNum = val;
    OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_Csq(uint8 val)
{
    CPU_SR	cpu_sr;

    OS_ENTER_CRITICAL();
    gGprsRunSta.Csq = val;
    OS_EXIT_CRITICAL();
}

void UpdGprsRunSta_AddIpCloseNum(void)
{
    CPU_SR	cpu_sr;

    OS_ENTER_CRITICAL();
    gGprsRunSta.IpCloseNum++;
    OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_AddIpSendFailNum(void)
{
    CPU_SR	cpu_sr;

    OS_ENTER_CRITICAL();
    gGprsRunSta.IpSendFailNum++;
    OS_EXIT_CRITICAL();
}

void UpdGprsRunSta_FeedRecDog(void)
{
    CPU_SR	cpu_sr;

    OS_ENTER_CRITICAL();
    gGprsRunSta.RecDog = 0;
    OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_AddRecDog(void)
{
    CPU_SR	cpu_sr;

    OS_ENTER_CRITICAL();
    gGprsRunSta.RecDog++;
    OS_EXIT_CRITICAL();
}

void UpdGprsRunSta_FeedSndDog(void)
{
    CPU_SR	cpu_sr;

    OS_ENTER_CRITICAL();
    gGprsRunSta.SndDog = 0;
    OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_AddSndDog(void)
{
    CPU_SR	cpu_sr;

    OS_ENTER_CRITICAL();
    gGprsRunSta.SndDog++;
    OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_AddFrmRecTimes(uint32 n)
{
    CPU_SR	cpu_sr;

    OS_ENTER_CRITICAL();
    gGprsRunSta.FrmRecTimes++;
    gGprsRunSta.FrmRecBs += n;
    OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_AddFrmSndTimes(uint32 n)
{
    CPU_SR	cpu_sr;

    OS_ENTER_CRITICAL();
    gGprsRunSta.FrmSndTimes++;
    gGprsRunSta.FrmSndBs += n;
    OS_EXIT_CRITICAL();
}

void GetSocketType(void)
{
    UpdGprsRunSta_Stype(SOCKET_TCP);
}

void ProSocketType(void)
{
    uint8 SocketType;
    SocketType = ReadCurSType();

    if(SocketType == 0) //TCP
    {
        At_IPOPEN[11] = 'T';
        At_IPOPEN[12] = 'C';
        At_IPOPEN[13] = 'P';
    }
    else //UDP
    {
        At_IPOPEN[11] = 'U';
        At_IPOPEN[12] = 'D';
        At_IPOPEN[13] = 'P';
    }
}

//��ȡGPRS�ź�ǿ��  ����ֵ��ΧΪ0(С)--4(��)���ֱ��ʾGPRS��ǿ�ȴ�С
uint8 GetGprsCSQ(void)
{
    uint8  GprsSignal;
    GPRS_RUN_STA  Sta;

    ReadGprsRunSta(&Sta);
    if((Sta.Csq > 0) && (Sta.Csq < 32))
    {
        GprsSignal = (Sta.Csq / 8) + 1;
    }
    else
    {
        GprsSignal = 0;
    }
    return GprsSignal;
}

void ReadCsq(void)
{
    uint8 csq;
    if(CMD_Csq(&csq) == 0)
    {
        UpdGprsRunSta_Csq(csq);
    }
}

uint8 GprsTalk(uint32 rp)
{
    uint8 err;
    err = CMD_AT_RP(Ats_AT, Ata_OK, NULL, OS_TICKS_PER_SEC, rp, TRUE);

#if 1
    UGprsWriteStr(Ats_IPR_c);
    OSTimeDly(OS_TICKS_PER_SEC / 8);
#endif
    if(err)
    {
        debug_err(gDebugModule[GPRS_MODULE], "GprsTalk Ats_AT CMD Err!");
        USART3_BAUNDRATE_Init(57600) ;
        OSTimeDly(OS_TICKS_PER_SEC / 2);
        CMD_AT_RP(Ats_IPR, Ata_OK, NULL, OS_TICKS_PER_SEC * 2, 2, TRUE);
        CMD_AT_RP(Ats_IPR_c, Ata_IPR_c, NULL, OS_TICKS_PER_SEC, 2, TRUE);
        USART3_BAUNDRATE_Init(115200) ;
        OSTimeDly(OS_TICKS_PER_SEC / 2);
    }
    return err;
}

uint8 ModelTalkIn(void)
{
    uint8 err;
    uint8 n;

    OSTimeDly(5 * OS_TICKS_PER_SEC);
    FeedTaskDog();
    err = GprsTalk(5);
    if(err == 0)
    {
        return 0;
    }

    //�Ựʧ��,��������ģ����п����������»Ự
    n = 3;
    do
    {
        GprsOff_xmz();
        OSTimeDly(10 * OS_TICKS_PER_SEC);
        FeedTaskDog();
        GprsOn_xmz();
        OSTimeDly(5 * OS_TICKS_PER_SEC);
        err = GprsTalk(3);
        if(err == 0)
        {
            return 0;
        }
        if(n-- == 0)
        {
            break;
        }
    }
    while(err);

    return 0xff;
}

uint8 ModelIdentify(void)
{
    uint8 err;
    uint8 n;
    uint8 nn;
    //return MODU_ID_XMZ;

    n = 5;
gplp:
    nn = 10;
    while(ModelTalkIn())
    {
        nn--;
        if(nn == 0)
        {
            return MODU_ID_UNKNOWN;
        }

        FeedTaskDog();
        OSTimeDly(25 * OS_TICKS_PER_SEC);
        FeedTaskDog();
    }
    OSTimeDly(OS_TICKS_PER_SEC / 2);
    //�Ự�Ѿ��ɹ�,��ȡģ��ID====
    err = CMD_AT_RP(Ats_CGMM, Ata_CGMM_xmz, NULL, OS_TICKS_PER_SEC, 2, TRUE);
    if(err == 0)
    {
        //GprsCtrInit_Xmz();
        OSTimeDly(5 * OS_TICKS_PER_SEC);
        FeedTaskDog();
        return MODU_ID_XMZ;
    }
    FeedTaskDog();
    /*begin:yangfei added 2013-02-23 for GPRS Module  compatible*/
    err = CMD_AT_RP(Ats_CGMM, Ata_CGMM_xmz_R3, NULL, OS_TICKS_PER_SEC, 2, TRUE);
    if(err == 0)
    {
        //GprsCtrInit_Xmz();
        OSTimeDly(5 * OS_TICKS_PER_SEC);
        FeedTaskDog();
        return MODU_ID_XMZ;
    }
    /*end:yangfei added 2013-02-23*/


    //====�����ʧ��
    FeedTaskDog();
    //GprsCtrInit_Xmz();
    OSTimeDly(5 * OS_TICKS_PER_SEC);
    FeedTaskDog();
    n--;
    if(n == 0)
    {
        return MODU_ID_UNKNOWN;
    }
    goto gplp;

}

void GprsRestart_xmz(void)
{
    uint8 err;
    err = CMD_AT_RP(Ats_AT, Ata_OK, NULL, OS_TICKS_PER_SEC, 3, TRUE);
    LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <GprsRestart_xmz> GPRS Modul Shutdown!");
    debug_debug(gDebugModule[GPRS_MODULE], "WARNING: <GprsRestart_xmz> GPRS Modul Shutdown!");
    OSTimeDly(OS_TICKS_PER_SEC);
    if(err == 0) //��Ϊ����GPRSģ�鴦�ڿ���״̬
    {
        //err=CMD_smso();
        //if(err){
        GprsOff_xmz();//�����ػ�
        LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <GprsRestart_xmz> GPRS Modul Power Off!");
        //}
        OSTimeDly(15 * OS_TICKS_PER_SEC);
        FeedTaskDog();
    }

    LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <GprsRestart_xmz> GPRS Modul Power On, And ReStart!");
    debug_debug(gDebugModule[GPRS_MODULE], "INFO: <GprsRestart_xmz> GPRS Modul Power On, And ReStart!");
    GprsOn_xmz();
    OSTimeDly(5 * OS_TICKS_PER_SEC);
    FeedTaskDog();
}
/*#if TEST_SELF_CHECK > 0
uint8 SetXmzBsp9600(void)
{
	uint8 err;

	GprsCtrInit_Xmz();
	//�����ʼ�ĳ�ʼ���ǰ���Ϊģ��Ŀ��ػ��źŵļ������õģ��Լ����û�н���ģ��ʶ��
	//���������е�����������������ģ��Ĳ����ʣ����ԾͰ������ӵļ�������I/O����Ҫʹ���ĵÿ����ź�һֱ�����͵ģ�
	//����һֱ�����͵�ʵ���ϲ���Ӱ��ģ�鹤��

	ChangeUART1Bps(9600);//������104
	err=GprsTalk(1);
	if(err){
		ChangeUART1Bps(1200);
		err=GprsTalk(1);
	}
	if(err){
		GprsOff_xmz();//�����ػ�
		OSTimeDly(2*OS_TICKS_PER_SEC);
		GprsOn_xmz();
		OSTimeDly(8*OS_TICKS_PER_SEC);
	}

	err=CMD_AT_RP(Ats_IPR_c,Ata_IPR_c,NULL,OS_TICKS_PER_SEC,1,TRUE);
	OSTimeDly(OS_TICKS_PER_SEC/2);
	if(err){
		ChangeUART1Bps(1200);
		OSTimeDly(OS_TICKS_PER_SEC/4);

		err=CMD_AT_RP(Ats_IPR,Ata_OK,NULL,OS_TICKS_PER_SEC*2,2,TRUE);
		OSTimeDly(OS_TICKS_PER_SEC/2);

		ChangeUART1Bps(9600);
		OSTimeDly(OS_TICKS_PER_SEC/4);
	}

	return err;
}
uint8 SetXmzBspAuto(void)
{
	uint8 err;

	ChangeUART1Bps(9600);
	OSTimeDly(OS_TICKS_PER_SEC/4);

	err=CMD_AT_RP(Ats_IPR_0,Ata_OK,NULL,OS_TICKS_PER_SEC*2,2,TRUE);
	OSTimeDly(OS_TICKS_PER_SEC/2);

	return err;
}
void SendBpsFailInfo(void)
{
	char *p1;
	uint32 i,n;
	p1="Bps setting fail! Please restart concentrator!";

	n=strlen(p1);

	OS_ENTER_CRITICAL();
	for(i=0;i<n;i++){
		UART0Putch(p1[i]);
	}
	UART0Putch(0x0d);
	UART0Putch(0x0a);
	OS_EXIT_CRITICAL();

}
uint8 SetXmzBsp(void)
{
	uint8 err;
	if(IpPort[0]=='9'){//������
		/////err=SetXmzBspAuto();//������
	}
	else{
		err=SetXmzBsp9600();
	}

	if(err){
		SendBpsFailInfo();//����д����0˵��ʧ�ܣ���������������������
	}
	return err;
}

#endif*/

//ʹ���豸UP_COMMU_DEV_AT���ʲ������룬����ʹ���豸UP_COMMU_DEV_AT�ĺ����������䲢����
//ֱ����UART1 ���ͣ��˺����ɹ�ǰ��������������UART1 ���ͣ���ͨ��ȫ�ֽṹ��gGprsRunStaʵ��
uint8 GprsInit_xmz(void)
{
    uint8 err;
    uint8 len = 0;
    char temp[10] = {0x00};
    uint32 i, n;
    UpCommParaSaveType	UpPara;
    GprsAPNSelect ApnSet;
    CPU_SR	cpu_sr;

    OSMutexPend (FlashMutex, 0, &err);
    SDReadData("/GprsAPNSelect", &ApnSet, sizeof(GprsAPNSelect), 0);
    OSMutexPost (FlashMutex);

    n = 0;

#if(TEST_NO_IPSET_EN == 0)
    {
        OS_ENTER_CRITICAL();
        UpPara = gPARA_UpPara;
        OS_EXIT_CRITICAL();

        len = ConverHexToASCIP(UpPara.HostIPAddr, (uint8 *)IpAddrXmz);
        IpAddrXmz[len] = 0x00;

        len = Hex_BcdToAscii(UpPara.HostPort, (uint8 *)temp);
        for(i = 0; i < len; i++)
        {
            IpPort[i] = temp[len - 1 - i];
        }
        IpPort[len] = 0x00;

    }
#endif

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    while(1)
    {
        n++;
        if(n > 10)
        {
            for(i = 0; i < 12; i++)
            {
                FeedTaskDog();
                OSTimeDly(10 * OS_TICKS_PER_SEC);
                ReadCsq();
            }
            //n=1;			//for test
        }
        FeedTaskDog();

        if(gGprsFirst == FALSE)
        {
            GprsRestart_xmz();
        }
        //�������˵������������һ�β�������GPRSģ��ʱ�����ֺܶ�ʱ�����淢AT��ʧ��1�Σ���ϸ���Է��֣�
        //ģ�鿪���󣨷���^SYSSTART��5�����ң������AT����������Ӧ�����ٷ�AT�����1������Ӧ��
        //��ˣ�����ڶ��ν���AT����Ӧ�ɹ�ʵ��������һ�ε��ͺ��Ӧ�����ڶ��ε�AT����Ӧ�����������
        //����û�й�ϵ���ᱻ�����IPR����Ե������������û������ԵĴ���

        gGprsFirst = FALSE;

        UpQueueFlush(UP_COMMU_DEV_AT); //�豸UP_COMMU_DEV_AT�Ķ��У����

        FeedTaskDog();
        err = CMD_AT_RP(Ats_AT, Ata_OK, NULL, OS_TICKS_PER_SEC, 3, TRUE);
        OSTimeDly(OS_TICKS_PER_SEC);
        if(err)
        {
            LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <GprsInit_xmz> Ats_AT CMD Err!");
            debug_err(gDebugModule[GPRS_MODULE], "WARNING: <GprsInit_xmz> Ats_AT CMD Err!");
            continue;
        }
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <GprsInit_xmz> Ats_AT CMD OK!");
        /*needed to be modified*/
#if 0
        err = CMD_AT_RP(Ats_IPR_c, Ata_IPR_c, NULL, OS_TICKS_PER_SEC, 2, TRUE);
        OSTimeDly(OS_TICKS_PER_SEC / 2);
        if(err)
        {
            LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <GprsInit_xmz> Ats_IPR_c CMD Err!");
            debug_err(gDebugModule[GPRS_MODULE], "WARNING: <GprsInit_xmz> Ats_IPR_c CMD Err!");
            err = CMD_AT_RP(Ats_IPR, Ata_OK, NULL, OS_TICKS_PER_SEC * 2, 2, TRUE);
            LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <GprsInit_xmz> Try CMD Ats_IPR CMD!");
            OSTimeDly(OS_TICKS_PER_SEC / 2);
        }
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <GprsInit_xmz> Ats_IPR_c CMD OK!");
        debug_info(gDebugModule[GPRS_MODULE], "WARNING: <GprsInit_xmz> Ats_IPR_c CMD OK!");
#else
        err = CMD_AT_RP(Ats_IPR_0, Ata_OK, NULL, OS_TICKS_PER_SEC, 2, TRUE);
        OSTimeDly(OS_TICKS_PER_SEC / 2);
        if(err)
        {
            debug_err(gDebugModule[GPRS_MODULE], "WARNING: <GprsInit_xmz> Ats_IPR_0 CMD Err!");
            OSTimeDly(OS_TICKS_PER_SEC / 2);
        }
#endif
        err = CMD_AT_RP(Ats_SSYNC_c, Ata_SSYNC_c, NULL, OS_TICKS_PER_SEC, 2, TRUE);
        OSTimeDly(OS_TICKS_PER_SEC / 2);
        if(err)
        {
            LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <GprsInit_xmz> Ats_SSYNC_c CMD Err!");
            err = CMD_AT_RP(Ats_SSYNC, Ata_OK, NULL, OS_TICKS_PER_SEC * 2, 2, TRUE);
            LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <GprsInit_xmz> Try CMD Ats_SSYNC CMD!");
            OSTimeDly(OS_TICKS_PER_SEC / 2);
        }
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <GprsInit_xmz> Ats_SSYNC_c CMD OK!");
        FeedTaskDog();

        err = CMD_AT_RP(Ats_CPIN, Ata_CPIN, NULL, OS_TICKS_PER_SEC * 4, 20, TRUE);
        OSTimeDly(OS_TICKS_PER_SEC);
        if(err)
        {
            LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <GprsInit_xmz> Ats_CPIN CMD Err!");
            continue;
        }
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <GprsInit_xmz> Ats_CPIN CMD OK!");

        FeedTaskDog();
        ReadCsq();
        OSTimeDly(2 * OS_TICKS_PER_SEC);
        ReadCsq();
        OSTimeDly(2 * OS_TICKS_PER_SEC);
        ReadCsq();
        OSTimeDly(2 * OS_TICKS_PER_SEC);
        FeedTaskDog();

        err = CMD_AT_RP(Ats_CREG, Ata_CREG, Ata_CREG_Or, OS_TICKS_PER_SEC * 2, 30, TRUE);
        OSTimeDly(OS_TICKS_PER_SEC);
        if(err)
        {
            LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <GprsInit_xmz> Ats_CREG CMD Err!");
            continue;
        }
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <GprsInit_xmz> Ats_CREG CMD OK!");

        ReadCsq();
        OSTimeDly(OS_TICKS_PER_SEC);

        if(ApnSet.WriteFlag == 0xAA55)
        {
            if(ApnSet.GprsAPNSet >= SUPPORT_APN_NUM)
                ApnSet.GprsAPNSet = 0;
            err = CMD_AT_RP(Ats_CGDCONT[ApnSet.GprsAPNSet], Ata_OK, NULL, 2 * OS_TICKS_PER_SEC, 15, TRUE);
        }
        else
            err = CMD_AT_RP(Ats_CGDCONT[0], Ata_OK, NULL, 2 * OS_TICKS_PER_SEC, 15, TRUE);

        //err=CMD_AT_RP(Ats_CGDCONT,Ata_OK,NULL,2*OS_TICKS_PER_SEC,15,TRUE);
        OSTimeDly(OS_TICKS_PER_SEC);
        if(err)
        {
            LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <GprsInit_xmz> Ats_CGDCONT CMD Err!");
            continue;
        }
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <GprsInit_xmz> Ats_CGDCONT CMD OK!");

        err = CMD_AT_RP(Ats_CGATT, Ata_OK, NULL, 10 * OS_TICKS_PER_SEC, 3, TRUE);
        OSTimeDly(OS_TICKS_PER_SEC);

        err = CMD_AT_RP(Ats_CGREG, Ata_CGREG, Ata_CGREG_Or, OS_TICKS_PER_SEC * 2, 30, TRUE);
        OSTimeDly(OS_TICKS_PER_SEC);
        if(err)
        {
            LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <GprsInit_xmz> Ats_CGREG CMD Err!");
            continue;
        }
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <GprsInit_xmz> Ats_CGREG CMD OK!");
        /*begin:yangfei added 2013-02-26 for*/
        err = CMD_AT_RP(Ats_CGREG_1, Ata_OK, NULL, OS_TICKS_PER_SEC * 2, 3, TRUE);
        OSTimeDly(OS_TICKS_PER_SEC);
        /*end:yangfei added 2013-02-26*/
        ReadCsq();
        OSTimeDly(OS_TICKS_PER_SEC);




        //if(err){
        //	continue;
        //}
        /*char Ats_SICS_conType[]="at^sics=0,conType,GPRS0\r";
        char Ats_SICS_apn[64]="at^sics=0,apn,cmnet\r";
        ///����tcp service profile
        //at^siss=1,srvType,socket
        char Ats_SISS_srvType[]="at^siss=1,srvType,socket\r";
        //at^siss=1,conId,0
        char Ats_SISS_conId[]="at^siss=1,conId,0\r";
        //at^siss=1,address,"socktcp://117.22.67.183:5100"
        char Ats_SISS_address[]="at^siss=1,address,\"socktcp://117.22.67.183:5100\r\"";
        */
        GetSocketType();
        //ProSocketType();
        err = CMD_AT_RP(Ats_SICS_conType, Ata_OK123, NULL, OS_TICKS_PER_SEC, 3, TRUE);
        OSTimeDly(OS_TICKS_PER_SEC);
        if(err)
        {
            LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <GprsInit_xmz> Ats_SICS_conType CMD Err!");
            continue;
        }
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <GprsInit_xmz> Ats_SICS_conType CMD OK!");

        if(ApnSet.WriteFlag == 0xAA55)
        {
            if(ApnSet.GprsAPNSet >= SUPPORT_APN_NUM)
                ApnSet.GprsAPNSet = 0;
            err = CMD_AT_RP(Ats_SICS_apn[ApnSet.GprsAPNSet], Ata_OK123, NULL, OS_TICKS_PER_SEC, 3, TRUE);
        }
        else
            err = CMD_AT_RP(Ats_SICS_apn[0], Ata_OK123, NULL, OS_TICKS_PER_SEC, 3, TRUE);



        //err=CMD_AT_RP(Ats_SICS_apn,Ata_OK123,NULL,OS_TICKS_PER_SEC,3,TRUE);
        OSTimeDly(OS_TICKS_PER_SEC);
        if(err)
        {
            LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <GprsInit_xmz> Ats_SICS_apn CMD Err!");
            continue;
        }
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <GprsInit_xmz> Ats_SICS_apn CMD OK!");

        err = CMD_AT_RP(Ats_SISS_srvType, Ata_OK123, NULL, OS_TICKS_PER_SEC, 3, TRUE);
        OSTimeDly(OS_TICKS_PER_SEC);
        if(err)
        {
            LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <GprsInit_xmz> Ats_SISS_srvType CMD Err!");
            continue;
        }
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <GprsInit_xmz> Ats_SISS_srvType CMD OK!");

        err = CMD_AT_RP(Ats_SISS_conId, Ata_OK123, NULL, OS_TICKS_PER_SEC, 3, TRUE);
        OSTimeDly(OS_TICKS_PER_SEC);
        if(err)
        {
            LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <GprsInit_xmz> Ats_SISS_conId CMD Err!");
            continue;
        }
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <GprsInit_xmz> Ats_SISS_conId CMD OK!");


        strcpy(Ats_SISS_address, Ats_SISS_address_h);
        strcat(Ats_SISS_address, IpAddrXmz);
        strcat(Ats_SISS_address, colon);
        strcat(Ats_SISS_address, IpPort);
        strcat(Ats_SISS_address, over124);

        /*begin:yangfei modified 2013-02-26 for return timeout err*/
#if 0
        err = CMD_AT_RP(Ats_SISS_address, Ata_OK123, NULL, OS_TICKS_PER_SEC, 3, TRUE);
#else
        err = CMD_AT_RP(Ats_SISS_address, Ata_OK123, NULL, OS_TICKS_PER_SEC * 3, 3, TRUE);
#endif
        /*end:yangfei modified 2013-02-26 for */
        OSTimeDly(OS_TICKS_PER_SEC);
        if(err)
        {
            LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <GprsInit_xmz> Ats_SISS_address CMD Err!");
            debug_err(gDebugModule[GPRS_MODULE], "<GprsInit_xmz> Ats_SISS_address CMD Err!");
            continue;
        }
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <GprsInit_xmz> Ats_SISS_address CMD OK!");
        //debug_debug(gDebugModule[GPRS_MODULE],"INFO: <GprsInit_xmz> Ats_SISS_address CMD OK!");

        UpQueueFlush(UP_COMMU_DEV_ATIPD);
        //������

        err = CMD_AT_RP(Ats_SISO, Ata_OK123, NULL, OS_TICKS_PER_SEC * 20, 0, TRUE);
        OSTimeDly(OS_TICKS_PER_SEC);
        if(err)
        {
            LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <GprsInit_xmz> Ats_SISO CMD Err!");
            debug_err(gDebugModule[GPRS_MODULE], "<GprsInit_xmz> Ats_SISO CMD Err!");
            continue;
        }
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <GprsInit_xmz> Ats_SISO CMD OK!");
        debug_info(gDebugModule[GPRS_MODULE], "INFO: <GprsInit_xmz> Ats_SISO CMD OK!");
        //TaskGprsMana_FeedTaskDog();
        OSTimeDly(OS_TICKS_PER_SEC);


        //err=CMD_AT_RP(Ats_SISI,Ata_SISI,NULL,OS_TICKS_PER_SEC*5,4,TRUE);
        ////OSTimeDly(OS_TICKS_PER_SEC);

        OSTimeDly(OS_TICKS_PER_SEC);
        break;
    }
    FeedTaskDog();
    return 0;
}
