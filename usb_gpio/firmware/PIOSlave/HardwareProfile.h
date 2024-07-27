/***********************************************
*  汎用USB I/Oボード　ハードウェア定義
************************************************/
#define self_power          1
#define USB_BUS_SENSE       1

/******** クロック周波数 ******************/
#define CLOCK_FREQ 48000000

/****** 汎用入出力ピン定義 ****************/
#define mPO_1				LATCbits.LATC5
#define mPO_2				LATCbits.LATC4
#define mPO_3				LATCbits.LATC3
#define mPO_4				LATCbits.LATC6
#define mPO_5				LATCbits.LATC7

#define mPO_1_On()			mPO_1 = 1;
#define mPO_2_On()			mPO_2 = 1;
#define mPO_3_On()			mPO_3 = 1;
#define mPO_4_On()			mPO_4 = 1;				
#define mPO_5_On()			mPO_5 = 1;

#define mPO_1_Off()			mPO_1 = 0;
#define mPO_2_Off()			mPO_2 = 0;
#define mPO_3_Off()			mPO_3 = 0;
#define mPO_4_Off()			mPO_4 = 0;
#define mPO_5_Off()			mPO_5 = 0;

#define mPIO_1				PORTCbits.RC5
#define mPIO_2				PORTCbits.RC4
#define mPIO_3				PORTCbits.RC3
#define mPIO_4				PORTCbits.RC6
#define mPIO_5				PORTCbits.RC7

#define mPI_1				PORTBbits.RB4
#define mPI_2				PORTBbits.RB5
#define mPI_3				PORTBbits.RB6
//#define mPI_4				PORTBbits.RB7


#define mdebugLED			LATBbits.LATB7
#define debugLED_On()		mdebugLED = 1;
#define debugLED_Off()		mdebugLED = 0;
#define debugLED			PORTBbits.RB7




#define PROGRAMMABLE_WITH_USB_HID_BOOTLOADER	