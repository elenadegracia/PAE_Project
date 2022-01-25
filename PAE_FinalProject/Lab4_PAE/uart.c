/*
 * uart.c
 */
#include "msp.h"
#include <msp432p401r.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include"uart.h"
#include "lib_PAE.h"

void init_interrupciones()
{
    //Comentat per no saturar NVIC

    //Int. port 1 = 35 corresponde al bit 3 del segundo registro ISER1:
    //NVIC->ICPR[1] |= 1 << (PORT1_IRQn & 31); //Primero, me aseguro de que no quede ninguna interrupcion residual pendiente para este puerto,
    //NVIC->ISER[1] |= 1 << (PORT1_IRQn & 31); //y habilito las interrupciones del puerto
    //Timer A0
    //NVIC->ICPR[0] |= 1 << (TA0_0_IRQn & 31); //Primero, me aseguro de que no quede ninguna interrupcion residual pendiente para este puerto,
    //NVIC->ISER[0] |= 1 << (TA0_0_IRQn & 31); //y habilito las interrupciones del puerto
    //Timer A1
    //NVIC->ICPR[0] |= 1 << (TA1_0_IRQn & 31); //Primero, me aseguro de que no quede ninguna interrupcion residual pendiente para este puerto,
    //NVIC->ISER[0] |= 1 << (TA1_0_IRQn & 31); //y habilito las interrupciones del puerto

    NVIC->ICPR[0] |= 1 << (EUSCIA0_IRQn);
    NVIC->ISER[0] |= 1 << (EUSCIA0_IRQn);

    __enable_interrupt(); //Habilitamos las interrupciones a nivel global del micro.

}
void init_uart(void)
{
    UCA0CTLW0 |= UCSWRST;         //Fem un reset de la USCI, desactiva la USCI
    UCA0CTLW0 |= UCSSEL__SMCLK;   //UCSYNC=0 mode as�ncron
                                  //UCMODEx=0 seleccionem mode UART
                                  //UCSPB=0 nomes 1 stop bit
                                  //UC7BIT=0 8 bits de dades
                                  //UCMSB=0 bit de menys pes primer
                                  //UCPAR=x ja que no es fa servir bit de paritat
                                  //UCPEN=0 sense bit de paritat
                                  //Triem SMCLK (24MHz) com a font del clock BRCLK
    UCA0MCTLW = UCOS16;       // Necessitem sobre-mostreig => bit 0 = UCOS16 = 1
    UCA0BRW = 13;          //Prescaler de BRCLK fixat a 13. Com SMCLK va a24MHz,
                           //volem un baud rate de 115200kb/s i fem sobre-mostreig de 16
                           //el rellotge de la UART ha de ser de ~1.85MHz (24MHz/13).
    UCA0MCTLW |= (0x25 << 8);    //UCBRSx, part fractional del baud rate

    //Configurem els pins de la UART
    P1SEL0 |= BIT2 | BIT3;          //I/O funci�: P3.3 = UART2TX, P3.2 = UART2RX
    P1SEL1 &= ~(BIT2 | BIT3 );

    UCA0CTLW0 &= ~UCSWRST;          //Reactivem la l�nia de comunicacions s�rie
    EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG; // Clear eUSCI RX interrupt flag
    EUSCI_A0->IE |= EUSCI_A_IE_RXIE; // Enable USCI_A0 RX interrupt, nomes quan tinguem la recepcio
}

#define TXD0_READY (UCA0IFG & UCTXIFG)

/* funcions per canviar el sentit de les comunicacions */



/* funci� TxUACx(byte): envia un byte de dades per la UART 0 */
void TxUAC0(uint8_t bTxdData)
{
    while (!TXD0_READY)
        ; // Espera a que estigui preparat el buffer de transmissi�
    UCA0TXBUF = bTxdData;
}
void Sentit_Dades_Rx(void)
{ //Configuraci� del Half Duplex dels motors: Recepci�
    P3OUT &= ~BIT0; //El pin P3.0 (DIRECTION_PORT) el posem a 0 (Rx)
}

void Sentit_Dades_Tx(void) //enviar
{ //Configuraci� del Half Duplex dels motors: Transmissi�
    P3OUT |= BIT0; //El pin P3.0 (DIRECTION_PORT) el posem a 1 (Tx)
}
/***********************************************************************
 *
 * TX PACKET
 *
 ***********************************************************************/

//TxPacket() 3 par�metres: ID del Dynamixel, Mida dels par�metres, Instruction byte. torna la mida del "Return packet"
byte TxPacket(byte bID, byte bParameterLength, byte bInstruction,
              byte Parametros[16])
{
    byte bCount, bCheckSum, bPacketLength;
    byte TxBuffer[32];
    Sentit_Dades_Tx(); //El pin P3.0 (DIRECTION_PORT) el posem a 1 (Transmetre)
    TxBuffer[0] = 0xff; //Primers 2 bytes que indiquen inici de trama FF, FF.
    TxBuffer[1] = 0xff;
    TxBuffer[2] = bID; //ID del m�dul al que volem enviar el missatge
    TxBuffer[3] = bParameterLength + 2; //Length(Parameter,Instruction,Checksum)
    TxBuffer[4] = bInstruction; //Instrucci� que enviem al M�dul

    for (bCount = 0; bCount < bParameterLength; bCount++) //Comencem a generar la trama que hem d�enviar
    {
        TxBuffer[bCount + 5] = Parametros[bCount];
    }
    bCheckSum = 0;
    bPacketLength = bParameterLength + 4 + 2; //longitud del paquet total
    for (bCount = 2; bCount < bPacketLength - 1; bCount++) //C�lcul del checksum
    {
        bCheckSum += TxBuffer[bCount];
    }

    TxBuffer[bCount] = ~bCheckSum; //Escriu el Checksum (complement a 1)
    for (bCount = 0; bCount < bPacketLength; bCount++) //Aquest bucle �s el que envia la trama al M�dul Robot
    {
        TxUAC0(TxBuffer[bCount]);
    }
    while ((UCA0STATW & UCBUSY)); //Espera fins que s�ha transm�s el �ltim byte
    Sentit_Dades_Rx(); //Posem la l�nia de dades en Rx perqu� el m�dul Dynamixel envia resposta
    return (bPacketLength);
}





byte received_data;
byte read_data_UART;
struct RxReturn RxPacket(void)
{
    struct RxReturn respuesta;
    byte bCount, bLenght, bChecksum;
    byte Rx_time_out = 0;

    //Ponemos la linea half duplex en Rx
    Sentit_Dades_Rx();
    //Activa_TimerA1_TimeOut();

    //es llegeixen els 4 primers parametres
    for (bCount = 0; bCount < 4; bCount++) //bRxPacketLength; bCount++)
    {
        //Reset_Timeout();
        received_data = 0; //No_se_ha_recibido_Byte();
        while (!received_data) //Se_ha_recibido_Byte())
        {
            //Rx_time_out = TimeOut(1000); // tiempo en decenas de microsegundos (ara 10ms)
            if (Rx_time_out)
                break; //sale del while
        }
        if (Rx_time_out){
            break; //sale del for si ha habido Timeout
        }

//Si no, es que todo ha ido bien, y leemos un dato:
        respuesta.StatusPacket[bCount] = read_data_UART; //Get_Byte_Leido_UART();
    } //fin del for
    if (!Rx_time_out){
        bLenght = respuesta.StatusPacket[3] + 4;
    // Continua llegint la resta de bytes del Status Packet
        bChecksum = 0;
        for(bCount = 4; bCount < bLenght; bCount++){
            received_data = 0; //No_se_ha_recibido_Byte();
            while (!received_data) //Se_ha_recibido_Byte())
            {
                //Rx_time_out = TimeOut(1000); // tiempo en decenas de microsegundos (ara 10ms)
                if (Rx_time_out)
                    break; //sale del while
            }
            if (Rx_time_out)
            {
                break; //sale del for si ha habido Timeout
            }
            respuesta.StatusPacket[bCount] = read_data_UART;
        }
        for(bCount = 0 ; bCount < bLenght-1 ; bCount++){
            bChecksum += respuesta.StatusPacket[bCount];
        }

        bChecksum = ~bChecksum; // Checksum a complement a 1

        // Comparem que el checkSum rebut amb el c�lcul
        if(respuesta.StatusPacket[bLenght-1] != bChecksum){
            respuesta.error = true;
        }

    }
    return respuesta;

}
void EUSCIA0_IRQHandler(void)
{ //interrupcion de recepcion en la UART A0
    UCA0IE &= ~UCRXIE;//Interrupciones desactivadas en RX
    read_data_UART = UCA0RXBUF;
    received_data = 1;
    UCA0IE |= UCRXIE;//Interrupciones reactivadas en RX
}
//no es fa servir
/*struct RxReturn RxTxPacket(byte bID, byte bParameterLength, byte bInstruction,const byte* Parametros) {
    struct RxReturn respuesta;
    TxPacket(bID, bParameterLength, bInstruction, Parametros);
    respuesta = RxPacket();
    return respuesta;
}
*/

