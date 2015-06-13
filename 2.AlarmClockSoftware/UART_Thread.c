#include <stdio.h>
#include "stdint.h"
#include <string.h>
#include "Driver_USART.h"
#include <cmsis_os.h>
#include "LightState.h"

/**
 ******************************************************************************
 * @file    UART_Thread.c
 * @author  Jeroen Vennegoor op Nijhuis
 * @version V1.0
 * @brief   Uart thread: takes care of all serial communication with XBee module
 ******************************************************************************
 */

//define variables
void UART_Thread(void const *argument);                       // thread function
osThreadId tid_UART_Thread;                                         // thread id
osThreadDef(UART_Thread, osPriorityHigh, 1, 0);                 // thread object
ARM_USART_STATUS status;
int sendmessage = 0;
int receiveready = 0;
int waitingForReply = 0;
uint8_t txBuffer[10];   //buffer for Tx data
uint32_t txLength = 1;
uint8_t rxBuffer[20];  //buffer for Rx data
uint8_t rxByte;
uint32_t rxLength = 0;
struct LightState state;
int getLightStateReceived = 0;
osMutexId Mutex1_id;                                     // mutex id
osMutexDef(Mutex1);
osStatus mutexStatus;

/* extern */
extern ARM_DRIVER_USART Driver_USART4;
extern void SetClockTime(int hours, int minutes, int seconds);

void myUSART_callback(uint32_t event) {
	switch (event) {
	case ARM_USART_EVENT_RECEIVE_COMPLETE:
		rxBuffer[rxLength] = rxByte;
		rxLength++;
		osSignalSet(tid_UART_Thread, 0x01);
		break;
	case ARM_USART_EVENT_TRANSFER_COMPLETE:
	case ARM_USART_EVENT_SEND_COMPLETE:
	case ARM_USART_EVENT_TX_COMPLETE:
		/* Success: Wakeup Thread */
		osSignalSet(tid_UART_Thread, 0x02);
		break;
	case ARM_USART_EVENT_RX_TIMEOUT:
		__breakpoint(0); /* Error: Call debugger or replace with custom error handling */
		break;
	case ARM_USART_EVENT_RX_OVERFLOW:
		__breakpoint(0); /* Error: Call debugger or replace with custom error handling */
		break;
	case ARM_USART_EVENT_TX_UNDERFLOW:
		__breakpoint(0); /* Error: Call debugger or replace with custom error handling */
		break;
	}
}

int Init_UART_Thread(void) {
	Mutex1_id = osMutexCreate(osMutex(Mutex1));
	tid_UART_Thread = osThreadCreate(osThread(UART_Thread), NULL);
	if (!tid_UART_Thread)
		return (-1);

	return (0);
}

/**
 * @brief  ParseData: parses RX data from UART
 * @param  uint8_t * pData, uint16_t length
 * @retval None
 */

void ParseData(uint8_t * pData, uint16_t length) {
	if (pData[0] == 0x04) {
		uint32_t seconds = 0;
		uint32_t minutes = 0;
		uint32_t hours = 0;
		uint8_t* p;
		p = pData + 2;
		memcpy(&seconds, p, 4);
		p = p + 4;
		memcpy(&minutes, p, 4);
		p = p + 4;
		memcpy(&hours, p, 4);
		SetClockTime(hours, minutes, seconds);
	}

	if (pData[0] == 0x06) {
		uint8_t on = 0;
		uint8_t bri = 0;
		uint8_t sat = 0;
		uint16_t hue = 0;
		uint8_t* p;
		p = pData + 2;
		memcpy(&on, p, 1);
		p = p + 1;
		memcpy(&bri, p, 1);
		p = p + 1;
		memcpy(&sat, p, 1);
		p = p + 1;
		memcpy(&hue, p, 2);
		state.on = on;
		state.bri = bri;
		state.sat = sat;
		state.hue = hue;
		getLightStateReceived = 1;
	}
	rxLength = 0;
}

/**
 * @brief  Main loop for UART Thread
 * @param  void const *argument
 * @retval None
 */

void UART_Thread(void const *argument) {
	static ARM_DRIVER_USART * USARTdrv = &Driver_USART4;

	ARM_USART_STATUS usart_status;

	int i = 0;
	for (i = 0; i < 20; i++) {
		rxBuffer[i] = 0x00;
	}

	USARTdrv->Initialize(myUSART_callback);
	USARTdrv->PowerControl(ARM_POWER_FULL);
	/*Configure the USART to 9600 Bits/sec */
	USARTdrv->Control(
			ARM_USART_MODE_ASYNCHRONOUS | ARM_USART_DATA_BITS_8
					| ARM_USART_PARITY_NONE | ARM_USART_STOP_BITS_1
					| ARM_USART_FLOW_CONTROL_NONE, 9600);

	/* Enable Receiver and Transmitter lines */
	USARTdrv->Control(ARM_USART_CONTROL_TX, 1);
	USARTdrv->Control(ARM_USART_CONTROL_RX, 1);

	while (1) {
		//do we need to send a message?
		if (sendmessage == 1) {
			usart_status = USARTdrv->GetStatus();
			USARTdrv->Control(ARM_USART_CONTROL_TX, 1);
			USARTdrv->Send((uint8_t*) txBuffer, txLength);
			osSignalWait(0x02, osWaitForever);
			sendmessage = 0;
			waitingForReply = 1;

		}
		//are we waiting for reply ?
		if (waitingForReply == 1) {
			int MessageLength = 0;
			usart_status = USARTdrv->GetStatus();
			USARTdrv->Control(ARM_USART_CONTROL_RX, 1);
			USARTdrv->Receive(&rxByte, 1);
			osSignalWait(0x01, 2000);
			USARTdrv->Receive(&rxByte, 1);
			osSignalWait(0x01, 2000);
			MessageLength = rxBuffer[1];
			for (i = 2; i < MessageLength; i++) {
				USARTdrv->Receive(&rxByte, 1);
				osSignalWait(0x01, 2000);
			}
			ParseData(rxBuffer, rxLength);
			waitingForReply = 0;
		}
		osDelay(100);
		osThreadYield();
	}
}

/**
 * @brief  RequestTime
 * @param  None
 * @retval None
 */
void RequestTime() {
	uint8_t Command[3] = { 0x03, 0x03, 0xff };
	mutexStatus = osMutexWait(Mutex1_id, NULL);
	memcpy(txBuffer, Command, 3);
	txLength = 3;
	sendmessage = 1;
	osMutexRelease(Mutex1_id);

}

/**
 * @brief  SetLightState
 * @param  struct LightState
 * @retval None
 */
void SetLightState(struct LightState state) {
	uint8_t Command[8] = { 0x07, 0x09, 0, 0, 0, 0, 0, 0xff };
	char hue1 = (state.hue >> (8 * 0));
	char hue2 = (state.hue >> (8 * 1));
	Command[2] = state.on;
	Command[3] = state.sat;
	Command[4] = state.bri;
	Command[5] = hue1;
	Command[6] = hue2;
	memcpy(txBuffer, Command, 8);
	txLength = 8;
	sendmessage = 1;
}

/**
 * @brief  GetLightState
 * @param  None
 * @retval None
 */
void GetLightState(void) {
	uint8_t Command[3] = { 0x05, 0x03, 0xff };
	getLightStateReceived = 0;
	memcpy(txBuffer, Command, 3);
	txLength = 3;
	sendmessage = 1;
	while (getLightStateReceived == 0) {
		osDelay(10);
	}
}

