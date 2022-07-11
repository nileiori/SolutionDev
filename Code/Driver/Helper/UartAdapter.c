
#include "UartAdapter.h"
#include "AUartAdapter.h"
#include "Description.h"
#include "FPGADefine.h"
#include "string.h"

#include "AUartAdapter.h"

// Tx-Uart
#define UART_TBASE                  (FPGA_BASE_ADDR + 0x00)
#define UART_THR(com)               (volatile unsigned short*)(UART_TBASE + 0x00 + com * 0x0010) // Tx-Uart data send macro
#define UART_TBAUD(com)             (volatile unsigned short*)(UART_TBASE + 0x02 + com * 0x0010) // Tx-Uart baudrate set macro
#define UART_TCONFIG(com)           (volatile unsigned short*)(UART_TBASE + 0x04 + com * 0x0010) // Tx-Uart config(databits, stopbits, parity) set macro
// Rx-Uart
#define UART_RBASE                  (FPGA_BASE_ADDR + 0x100)
#define UART_RHR(com)               (volatile unsigned short*)(UART_RBASE + 0x00 + com * 0x0010) // Rx-Uart data send macro
#define UART_RBAUD(com)             (volatile unsigned short*)(UART_RBASE + 0x02 + com * 0x0010) // Tx-Uart baudrate set macro
#define UART_RCONFIG(com)           (volatile unsigned short*)(UART_RBASE + 0x04 + com * 0x0010) // Rx-Uart config(databits, stopbits, parity) set macro
#define UART_RFIFO_STATE(com)       (volatile unsigned short*)(UART_RBASE + 0x0c + com * 0x0010) // Rx-Uart FOFI state macro
// Mode choose
#define UART_MODE_SEL               (volatile unsigned short*)(UART_RBASE + 0x232) // R232/422 Choose

/********************************************************************************\
����: ���մ��ڳ�ʼ��
����: rxPort - ���ں�
     baudRate - ������
     parityBits - У��λ
     stopBits - ֹͣλ
     mode - RS232/422/485ѡ��
     enable - ����ʹ��
����: ��
\********************************************************************************/
void Uart_RxInit(EUartRxPort rxPort, EUartBaudrate baudRate, EUartParitybits parityBits, EUartStopbits stopBits, EUartMode mode, EUartEnable enable)
{
    UCHAR sel = 0x00;
#ifndef WIN32
    if (UART_RXPORT_NULL == rxPort) return;
    if (UART_RXPORT_RS232_1 == rxPort)
    {
        FUART_RxInit(rxPort, baudRate, parityBits, stopBits);
        return;
    }

    if (rxPort >= UART_RXPORT_COMPLEX_8)
    {
        sel = (rxPort - UART_RXPORT_COMPLEX_8) / 2;
        if (mode == UART_RS422)
        {
            *(UART_MODE_SEL) |= (1 << sel); // 422ģʽ            
            *(UART_MODE_SEL) &= ~(1 << (sel + 4)); // ȫ˫��ģʽ
        }
        else if (mode == UART_RS232)
        {
            *(UART_MODE_SEL) &= ~(1 << sel); // 232ģʽ
            *(UART_MODE_SEL) &= ~(1 << (sel + 4)); // ȫ˫��ģʽ
        }
        else if (mode == UART_RS485)
        {
            *(UART_MODE_SEL) |= (1 << sel); // 422ģʽ
            *(UART_MODE_SEL) |= (1 << (sel + 4)); // ��˫��ģʽ
        }
        else {}
    }
    // ������ջ�����
    *(UART_RHR(rxPort)) = 0x0001;
    // ���ò�����
    *(UART_RBAUD(rxPort)) = baudRate;
    // �������ã�У��λ/ֹͣλ/ʹ�ܣ�
    *(UART_RCONFIG(rxPort)) = parityBits | stopBits | enable;
#endif
}

/********************************************************************************\
����: ���ʹ��ڳ�ʼ��
����: txPort - ���ں�
     baudRate - ������
     parityBits - У��λ
     stopBits - ֹͣλ
     mode - RS232/422/485ѡ��
     enable - ����ʹ��
����: ��
\********************************************************************************/
void Uart_TxInit(EUartTxPort txPort, EUartBaudrate baudRate, EUartParitybits parityBits, EUartStopbits stopBits, EUartMode mode, EUartEnable enable)
{
    UCHAR sel = 0x00;
#ifndef WIN32
    if (UART_TXPORT_NULL == txPort) return;
    if (UART_TXPORT_RS232_1 == txPort)
    {
        FUART_TxInit(txPort, baudRate, parityBits, stopBits);
        return;
    }

    if (txPort >= UART_RXPORT_COMPLEX_8)
    {
        sel = (txPort - UART_RXPORT_COMPLEX_8) / 2;
        if (mode == UART_RS422)
        {
            *(UART_MODE_SEL) |= (1 << sel); // 422ģʽ            
            *(UART_MODE_SEL) &= ~(1 << (sel + 4)); // ȫ˫��ģʽ
        }
        else if (mode == UART_RS232)
        {
            *(UART_MODE_SEL) &= ~(1 << sel); // 232ģʽ
            *(UART_MODE_SEL) &= ~(1 << (sel + 4)); // ȫ˫��ģʽ
        }
        else if (mode == UART_RS485)
        {
            *(UART_MODE_SEL) |= (1 << sel); // 422ģʽ
            *(UART_MODE_SEL) |= (1 << (sel + 4)); // ��˫��ģʽ
        }
        else {}
    }
    // ���ò�����
    *(UART_TBAUD(txPort)) = baudRate;
    // �������ã�У��λ/ֹͣλ/ʹ�ܣ�
    *(UART_TCONFIG(txPort)) = parityBits | stopBits | enable;
#endif
}

/********************************************************************************\
����: ���ʹ�������
����: txPort - ���ں�
     buffer - ������Ϣ
     start - ֡ʼ
     len - ֡��
����: ��
\********************************************************************************/
void Uart_SendMsg(EUartTxPort txPort, USHORT start, USHORT len, UCHAR* buffer)
{
#ifndef WIN32
    USHORT i = 0;

    if (UART_TXPORT_NULL == txPort) return;
    if (UART_TXPORT_RS232_1 == txPort)
    {
        FUART_SendData(txPort, buffer + start, len);
        return;
    }

    for (i = start; i < len; i++)
    {
        *(UART_THR(txPort)) = *(buffer + i);
    }
#endif
}


/********************************************************************************\
����: ���մ�������
����: rxPort - ���ں�
     buffer - ������Ϣ
     len - ��Ҫ���յĳ���
����: ʵ�ʶ�ȡ����
\********************************************************************************/
USHORT Uart_RecvMsg(EUartRxPort rxPort, USHORT len, UCHAR* buffer)
{
#ifndef WIN32
    USHORT revLen = 0;
    USHORT i = 0;

    if (UART_RXPORT_NULL == rxPort) return 0;
    if (UART_RXPORT_RS232_1 == rxPort)
    {
        return FUART_RecvData(rxPort, buffer, len);
    }

    revLen = *(UART_RFIFO_STATE(rxPort));

    if (len < revLen) revLen = len;

    for (i = 0; i < revLen; i++)
    {
        buffer[i] = (UCHAR)*(UART_RHR(rxPort));
    }

    return revLen;
#else
    return 0;
#endif
}

/********************************************************************************\
����: ��ս��ջ�����
����: rxPort - ���ں�
����: ��
\********************************************************************************/
void Uart_ClearRecvBuffer(EUartRxPort rxPort)
{
#ifndef WIN32
    if (UART_RXPORT_NULL == rxPort) return;
    *(UART_RHR(rxPort)) = 0x0001;
#endif
}

