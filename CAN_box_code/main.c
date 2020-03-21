
#include "datatypes.h"
#include "can.h"
#include "can_user.h"
#include "hardware.h"
#include "crc_data.h"


// identifier is needed by PCANFlash.exe -> do not delete
const b8_t Ident[] __attribute__ ((used)) = { "PCAN-Router"};


// info data for PCANFlash.exe
const crc_array_t  C2F_Array __attribute__((section(".C2F_Info"), used)) = {

	.Str = CRC_IDENT_STRING,
	.Version = 0x21,
	.Day = 5,
	.Month = 5,
	.Year = 6,
	.Mode = 1,

	// crc infos are patched during link time by flash.ld
	// crc value is patched by PCANFlash.exe
};


// variables for LED toggle
static u8_t LED_toggleCAN1;
static u8_t LED_toggleCAN2;



// main_greeting()
// transmitt a message at module start
static void  main_greeting ( void)
{
	CANMsg_t  Msg;
	

	Msg.Id   = 0x123;
	Msg.Len  = 8;
	Msg.Type = CAN_MSG_STANDARD;
	
	Msg.Data32[0] = 0x67452301;
	Msg.Data32[1] = 0xEFCDAB89;
	
	// Send Msg
	CAN_UserWrite ( CAN_BUS1, &Msg);
}




// main()
// entry point from crt0.S
int  main ( void)
{

	// init hardware
	HW_Init();
	
	
	// init CAN
	CAN_UserInit();
	
	
	// Set green LEDs for CAN1 and CAN2
	HW_SetLED ( HW_LED_CAN1, HW_LED_GREEN);
	HW_SetLED ( HW_LED_CAN2, HW_LED_GREEN);
	
	
	// send the greeting message
	//main_greeting();
	
	
	// main loop
	while ( 1)
	{
		CANMsg_t  RxMsg, TxMsg;
		

		// process messages from CAN1  
		if ( CAN_UserRead ( CAN_BUS1, &RxMsg) != 0)
		{
			if ( RxMsg.Id != 0x2E4  &&  RxMsg.Type == CAN_MSG_STANDARD)
			{
				// message received from CAN1
				LED_toggleCAN1 ^= 1;

				if ( LED_toggleCAN1)
				{
					HW_SetLED ( HW_LED_CAN1, HW_LED_ORANGE);
				}
			
				else
				{
					HW_SetLED ( HW_LED_CAN1, HW_LED_GREEN);
				}
			
			
				// copy message
				TxMsg.Id   = RxMsg.Id;
				TxMsg.Type = RxMsg.Type;
				TxMsg.Len  = RxMsg.Len;
			
				TxMsg.Data32[0]	= RxMsg.Data32[0];
				TxMsg.Data32[1]	= RxMsg.Data32[1];
				
				// send
				CAN_UserWrite ( CAN_BUS2, &TxMsg);
			}
		}



		// process messages from CAN2
		if ( CAN_UserRead ( CAN_BUS2, &RxMsg) != 0)
		{
			if ( RxMsg.Id != 0x2E4  &&  RxMsg.Type == CAN_MSG_STANDARD)
			{
			// message received from CAN2
			LED_toggleCAN2 ^= 1;

			if ( LED_toggleCAN2)
			{
				HW_SetLED ( HW_LED_CAN2, HW_LED_ORANGE);
			}
			
			else
			{
				HW_SetLED ( HW_LED_CAN2, HW_LED_GREEN);
			}
			
			
			// copy message
			TxMsg.Id   = RxMsg.Id;
			TxMsg.Type = RxMsg.Type;
			TxMsg.Len  = RxMsg.Len;
			
			TxMsg.Data32[0]	= RxMsg.Data32[0];
			TxMsg.Data32[1]	= RxMsg.Data32[1];
			
			// send
			CAN_UserWrite ( CAN_BUS1, &TxMsg);
			}
		}
	}
}
