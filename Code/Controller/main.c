
#include "main.h"

u8 g_SendBuff[60] = {0x00};
bool b_485 = 0;
u16 test_io = 0;
u16 count_1S = 0;

uint8_t fpgaBuf[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
uint8_t fpga_rxbuffer[100];

int main(void)
{   
    uint16_t len;
    //Disable the global interrupt
    //__set_PRIMASK(1);
    //Set interrupt group
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    //Initialize the delay func
    Delay_Init();
    // Initialize main timer(1000Hz)
    Exti_Init(EXIT_FPGA, ISR_Main_Int);
    //Timer_Init(ETimerType_Timer2, 10000/MAIN_FREQUENCY, ISR_Main_Int);
    // Initialize peripheral
    Peripheral_Init();
    //Enable the global interrupt
    __set_PRIMASK(0);
    //Uart_SendMsg(UART_TXPORT_COMPLEX_8, 0, 10, fpgaBuf);
    while(1)
    {
//        ISR_Main_Int();
//        len = Uart_RecvMsg(UART_RXPORT_COMPLEX_8, 50, fpga_rxbuffer);
//        if(len)
//          Uart_SendMsg(UART_TXPORT_COMPLEX_8, 0, len, fpga_rxbuffer);
    }
}


void ISR_Main_Int()
{
    u16 len = 0;
    u16 usTxState = 0;
    g_counter_main ++;
    
//    len = Uart_RecvMsg(UART_RXPORT_COMPLEX_8, 60, g_SendBuff);

//    if(len)
//    {
//        RS485_Ctrl((BitAction)1);
//        Uart_SendMsg(UART_TXPORT_COMPLEX_8, 0, len, g_SendBuff);
//    }
    
    usTxState = FMC_ReadWord(SPACE_COM, 0x250 >> 1);

    if((usTxState & 0x0002) != 0)
        RS485_Ctrl((BitAction)0);



    len = Uart_RecvMsg(UART_RXPORT_RS232_1, 60, g_SendBuff);

    if(len > 0)
    {
        Uart_SendMsg(UART_TXPORT_RS232_1, 0, len, g_SendBuff);
    }

    len = Uart_RecvMsg(UART_RXPORT_COMPLEX_8, 60, g_SendBuff);

    if(len)
    {
        RS485_Ctrl((BitAction)1);
        Uart_SendMsg(UART_TXPORT_COMPLEX_8, 0, len, g_SendBuff);
    }


    if(g_counter_main % (MAIN_FREQUENCY / 2) == 1)
    {
        memcpy(g_SendBuff, &g_counter_main, 4);
        Led_Blink(LED_1);

        if(count_1S <= 30)
        {
            count_1S++;

            switch(count_1S)
            {
                case 4:   //enable IF
                    test_io = FMC_ReadWord(SPACE_COM, 0x244 >> 1) | 0x0001;
                    FMC_WriteWord(SPACE_COM, 0x244 >> 1, test_io);
                    break;

                case 6:  //enable acc
                    test_io = FMC_ReadWord(SPACE_COM, 0x244 >> 1) | 0x0002;
                    FMC_WriteWord(SPACE_COM, 0x244 >> 1, test_io);
                    break;

                case 12: //enable gyro_x
                    test_io = FMC_ReadWord(SPACE_COM, 0x244 >> 1) | 0x0004;
                    FMC_WriteWord(SPACE_COM, 0x244 >> 1, test_io);
                    break;

                case 16: //enable gyro_y
                    test_io = FMC_ReadWord(SPACE_COM, 0x244 >> 1) | 0x0008;
                    FMC_WriteWord(SPACE_COM, 0x244 >> 1, test_io);
                    break;

                case 20: //enable gyro_z
                    test_io = FMC_ReadWord(SPACE_COM, 0x244 >> 1) | 0x0010;
                    FMC_WriteWord(SPACE_COM, 0x244 >> 1, test_io);
                    break;

                default:
                    break;
            }
        }
        else count_1S = count_1S;
    }
}


void Peripheral_Init(void)
{
    //Initialize LED
    //Led_Init();
    // Initialize can
    //Can_Init();
    // Initialize UDP
    //Udp_Init();
    //Initialize FMC(FPGA)
    FMC_Init();
    // Uart
    Uart_TxInit(UART_TXPORT_RS232_1, UART_BAUDRATE_115200BPS, UART_PARITY_NONE, UART_STOPBIT_ONE, UART_DEFAULT, UART_ENABLE);
    Uart_RxInit(UART_RXPORT_RS232_1, UART_BAUDRATE_115200BPS, UART_PARITY_NONE, UART_STOPBIT_ONE, UART_DEFAULT, UART_ENABLE);

    Uart_TxInit(UART_TXPORT_COMPLEX_8, UART_BAUDRATE_115200BPS, UART_PARITY_NONE, UART_STOPBIT_ONE, UART_RS422, UART_ENABLE);
    Uart_RxInit(UART_RXPORT_COMPLEX_8, UART_BAUDRATE_115200BPS, UART_PARITY_NONE, UART_STOPBIT_ONE, UART_RS422, UART_ENABLE);
}
