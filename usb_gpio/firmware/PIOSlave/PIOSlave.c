/***********************************************************
*  汎用デバイスクラスを使ったリモートI/O
*  　デジタル入出力とアナログ入力
************************************************************/

/** ファイルインクルード **************************/
#include "USB/usb.h"
#include "USB/usb_function_generic.h"
#include "HardwareProfile.h"
#include "GenericTypeDefs.h"
#include "usb_config.h"
#include <delays.h>
#include <timers.h>
/** コンフィギュレーション　***********************/
#pragma config CPUDIV = NOCLKDIV ,USBDIV = OFF, PCLKEN = ON 
#pragma config FOSC = HS, PLLEN = ON, HFOFST = OFF
#pragma config PWRTEN = ON, BOREN = OFF, MCLRE = ON, BORV = 30
#pragma config WDTEN = OFF, LVP = OFF, FCMEN = OFF, IESO = OFF
#pragma config CP0 = OFF, XINST = OFF
#define	TIMER_15MS	0
#define	TIMER_xMS	1
#define	SERVO_LEFT	0
#define	SERVO_RIGHT	1

#define	POUT_ON		1
#define	POUT_OFF	0
#define	PIN_ON		1
#define	PIN_OFF		0

unsigned char tmrStatus = TIMER_15MS;
unsigned char servoPosition = SERVO_LEFT;
unsigned char OneSecCnt = 0;


unsigned char pout1_sts = POUT_OFF	;
unsigned char pout2_sts = POUT_OFF	;
unsigned char pout3_sts = POUT_OFF	;
unsigned char pout4_sts = POUT_OFF	;
unsigned char pout5_sts = POUT_OFF	;
unsigned char pout6_sts = POUT_OFF	;
unsigned char pout7_sts = POUT_OFF	;
unsigned char pout8_sts = POUT_OFF	;

unsigned char pin1_sts = PIN_OFF	;
unsigned char pin2_sts = PIN_OFF	;
unsigned char pin3_sts = PIN_OFF	;
unsigned char pin4_sts = PIN_OFF	;
unsigned char pin5_sts = PIN_OFF	;
unsigned char pin6_sts = PIN_OFF	;
unsigned char pin7_sts = PIN_OFF	;
unsigned char pin8_sts = PIN_OFF	;


#define	RC0_High		LATCbits.LATC0 = 1
#define	RC0_Low			LATCbits.LATC0 = 0

#define	RC1_High		LATCbits.LATC1 = 1
#define	RC1_Low			LATCbits.LATC1 = 0

#define	RC2_High		LATCbits.LATC2 = 1
#define	RC2_Low			LATCbits.LATC2 = 0

#define	RC3_High		LATCbits.LATC3 = 1
#define	RC3_Low			LATCbits.LATC3 = 0

/*** コマンド定数定義 ***/
typedef enum
{
    CHECK	= 0x30,
	POUT	= 0x31,
	PIN     = 0x32,
	// ALL		= 0x34,
    DISCONNECT= 0x7F
}TYPE_CMD;
/******** USB関連バッファ、変数定義 ****/
#pragma udata usbram2
BYTE INPacket[USBGEN_EP_SIZE];	// USB送信バッファ
BYTE OUTPacket[USBGEN_EP_SIZE];	// USB受信バッファ
#pragma udata
USB_HANDLE USBGenericOutHandle;
USB_HANDLE USBGenericInHandle;
/**** 変数定義　***/
BYTE  counter;
/** 関数プロトタイピング ****************************/
void USBDeviceTasks(void);
void YourHighPriorityISRCode(void);
void YourLowPriorityISRCode(void);
void USBCBSendResume(void);
void ProcessIO(void);



/** VECTOR REMAPPING ***********************************************/
void HighIntCode();
void LowIntCode();
extern void _startup(void);

//Vector Remapping for Standalone Operating
#pragma code HIGH_INTERRUPT_VECTOR = 0x08
void High_ISR(void) { _asm goto 0x1008 _endasm }

#pragma code LOW_INTERRUPT_VECTOR = 0x18
void Low_ISR(void) { _asm goto 0x1018 _endasm }

//Vector Remapping for MCHIP Generic Bootloader
#pragma code GEN_RESET_VECTOR=0x800
void GEN_Reset(void) { _asm goto _startup _endasm }

#pragma code GEN_HIGH_INTERRUPT_VECTOR=0x808
void GEN_HighInt(void) { _asm goto HighIntCode _endasm }

#pragma code GEN_LOW_INTERRUPT_VECTOR=0x818
void GEN_LowInt(void) { _asm goto LowIntCode _endasm }

//Vector Remapping for MCHIP HID Bootloader
#pragma code HID_RESET_VECTOR=0x1000
void HID_Reset(void) { _asm goto _startup _endasm }

#pragma code HID_HIGH_INTERRUPT_VECTOR=0x1008
void HID_HighInt(void) { _asm goto HighIntCode _endasm }

#pragma code HID_LOW_INTERRUPT_VECTOR=0x1018
void HID_LowInt(void) { _asm goto LowIntCode _endasm }

#pragma code
	
#pragma interrupt HighIntCode
void HighIntCode() {
		//Check which interrupt flag caused the interrupt.
		//Service the interrupt, Clear the interrupt flag, Etc.
        //#if defined(USB_INTERRUPT)
	    //USBDeviceTasks();
        //#endif

        USBDeviceTasks();
		if (INTCONbits.TMR0IF && INTCONbits.TMR0IE){
			INTCONbits.TMR0IF=0;
			TMR0H = 0x49;	TMR0L = 0x00;
			
			// if(LATCbits.LATC3 == 0){
				// RC3_High;
			// }else{
				// RC3_Low;
			// }
			
			if(OneSecCnt != 4){	// 5 second (5-1)
				OneSecCnt++;
			}else{
				OneSecCnt = 0;
				if(LATCbits.LATC3 == 0){
					RC3_High;
					pin3_sts = PIN_ON;
					pin4_sts = PIN_OFF;
					pin5_sts = PIN_ON;
					pin6_sts = PIN_OFF;
					pin7_sts = PIN_ON;
					pin8_sts = PIN_OFF;
				}else{
					RC3_Low;
					pin3_sts = PIN_OFF;
					pin4_sts = PIN_ON;
					pin5_sts = PIN_OFF;
					pin6_sts = PIN_ON;
					pin7_sts = PIN_OFF;
					pin8_sts = PIN_ON;
				}
			}
			
			// if(tmrStatus == TIMER_15MS){
					// // output high
				// mPO_3_On();
				// if(servoPosition == SERVO_LEFT){
					// TMR0H = 0xFF;	TMR0L = 0x98;
				// }else{
					// TMR0H = 0xFF;	TMR0L = 0xE0;
				// }
				// tmrStatus = TIMER_xMS;
			// }else{
					// // output low
				// mPO_3_Off();
				// TMR0H = 0xFD;	TMR0L = 0x40;
				// tmrStatus = TIMER_15MS;
			// }
		}

}	//This return will be a "retfie fast", since this is in a #pragma interrupt section 

#pragma interruptlow LowIntCode
void LowIntCode() {
		//Check which interrupt flag caused the interrupt.
		//Service the interrupt, Clear the interrupt flag, Etc.
	
}	//This return will be a "retfie", since this is in a #pragma interruptlow section 



/*
//割り込みベクタ定義
#pragma code HID_RESET_VECTOR=0x1000

#pragma code REMAPPED_HIGH_INTERRUPT_VECTOR = 0x08
void Remapped_High_ISR (void)
{
     _asm goto YourHighPriorityISRCode _endasm
}
#pragma code REMAPPED_LOW_INTERRUPT_VECTOR = 0x18
void Remapped_Low_ISR (void)
{
     _asm goto YourLowPriorityISRCode _endasm
}
// 割り込み処理関数 
#pragma code
#pragma interrupt YourHighPriorityISRCode
void YourHighPriorityISRCode()
{
        USBDeviceTasks();
		if (INTCONbits.TMR0IF && INTCONbits.TMR0IE){
			INTCONbits.TMR0IF=0;
			if(tmrStatus == TIMER_15MS){
				//	output high
				mPO_3_On();
				if(servoPosition == SERVO_LEFT){
					TMR0H = 0xFF;	TMR0L = 0x98;
				}else{
					TMR0H = 0xFF;	TMR0L = 0xE0;
				}
				tmrStatus = TIMER_xMS;
			}else{
				//	output low
				mPO_3_Off();
				TMR0H = 0xFD;	TMR0L = 0x40;
				tmrStatus = TIMER_15MS;
			}
		}
}
#pragma interruptlow YourLowPriorityISRCode
void YourLowPriorityISRCode()
{	}

*/

/***********  メイン関数 ***************************/
#pragma code
void main(void)
{   
	/* IOピン初期設定　*/
	//ANSEL = 0x70;					// AN4,5,6以外デジタルに設定
	//ANSELH =0x00;	
	//TRISA = 0xFF;					// RA0-5入力
	//TRISB = 0xFF;					// RBすべて入力
	//LATC = 0;						// 出力初期化
	//TRISC = 0x07;					// RC0,1,2アナログ入力、RC3-7出力
	/* ADC初期化 */
	//ADCON0 = 0;						// 停止
	//ADCON1 = 0;						// VDD-Vss
	//ADCON2 = 0xBE;					// 右詰め,20Tad,Fosc/64
	
	//hong ->
	TRISA = 0xFF;		//	set to all input
	TRISB = 0xFF;		//	set to all input
	TRISC = 0xFF;		//	set to all input
	/*debugLED_On();
	TRISCbits.TRISC3 = 0;
	mPO_3_Off();
	debugLED_On();
	*/
		// I/O
	RC0_Low;
	RC1_Low;
	RC2_Low;
	RC3_Low;
	
	TRISCbits.TRISC0 = 0;		//	set to output
	TRISCbits.TRISC1 = 0;		//	set to output
	TRISCbits.TRISC2 = 0;		//	set to output
	TRISCbits.TRISC3 = 0;		//	set to output
	
	// timer setting
	// Enable interrupt priority 
    RCONbits.IPEN = 1;
    // Make receive interrupt high priority 
    IPR1bits.RCIP = 1; // priority interrupt enbabled
    INTCONbits.PEIE = 1;// peripherial interrupt enabled
    INTCON2bits.TMR0IP = 1;// timer0 interrupt prirority setup
    INTCONbits.GIEL = 1;// global interrupt enabled
    INTCONbits.GIE = 1;// global interrupt enabled
	/*
	tmrStatus = TIMER_15MS;
	servoPosition = SERVO_LEFT;
	TMR0H = 0xFD;	TMR0L = 0x40;
    OpenTimer0( TIMER_INT_ON &
     T0_16BIT &
     T0_SOURCE_INT &
	 T0_PS_1_256 );*/
	/*
	tmrStatus = TIMER_15MS;
	TMR0H = 0xFD;	TMR0L = 0x40;
	T0CONbits.T08BIT = 1;	//	Timer0 is configured as a 8-bit timer/counter 
	T0CONbits.T0CS = 0;		//	Internal instruction cycle clock (CLKOUT) 
	T0CONbits.T0SE = 0;		//	Increment on low-to-high transition on T0CKI pin (rising edge mode)
	T0CONbits.PSA = 1;		//	TImer0 prescaler is NOT assigned. Timer0 clock input bypasses prescaler.
	T0CONbits.T0PS2 = 1;	T0CONbits.T0PS1 = 1;	T0CONbits.T0PS0 = 1;	//	1:256 prescale value
	T0CONbits.TMR0ON = 1;	//	timer on
	INTCONbits.TMR0IE = 1;
	*/
	//	hong <-
	
	
	//	1 second timer setting
	//	FOSC/4 = 12MHz
	//	prescale 1:256 -> 12M/256 = 46.875kHz
	//	65535-46875 = 18688 -> 0x4900 
	TMR0H = 0x49;	TMR0L = 0x00;
	T0CONbits.T08BIT = 0;	//	Timer0 is configured as a 16-bit timer/counter 
	T0CONbits.T0CS = 0;		//	Internal instruction cycle clock (CLKOUT) 
	T0CONbits.T0SE = 0;		//	Increment on low-to-high transition on T0CKI pin (rising edge mode)
	T0CONbits.PSA = 0;		//	Timer0 prescaler is assigned. Timer0 clock input comes from prescaler output.
	T0CONbits.T0PS2 = 1;	T0CONbits.T0PS1 = 1;	T0CONbits.T0PS0 = 1;	//	1:256 prescale value
	T0CONbits.TMR0ON = 1;	//	timer on
	INTCONbits.TMR0IE = 1;
	
	

	/* USB関連 **/
	USBDeviceInit();				// USB初期化	
	USBGenericInHandle = 0;	
	USBGenericOutHandle = 0;
	/** USBアタッチ許可と割り込み許可 */
	USBDeviceAttach();
	
	/*********** メインループ ***************/
    while(1) {
		/*** USB接続中ならユーザー関数実行 ***/
		if((USBDeviceState >= CONFIGURED_STATE)&&(USBSuspendControl!=1))
			ProcessIO();			// ユーザーアプリ実行
	}
}
/***************************************************
 * ユーザーアプリ、入出力処理関数
 * USBからのコマンドにより機能実行
 ***************************************************/
void ProcessIO(void)
{
	int i;
	/***** データ受信処理 ******/
	if(!USBHandleBusy(USBGenericOutHandle))			// 受信完了か？
	{        
		/** 受信データ取り出し **/
		counter = 0;								// 送信バイト数リセット
		// clear all inpacket data
		for (i = 0; i < USBGEN_EP_SIZE; i++) {
			INPacket[i] = 0x00;
		}
		
		INPacket[0] = OUTPacket[0];					// エコーバック
		// INPacket[1] = OUTPacket[1];			
		/******** コマンドの処理 ********/
		switch(OUTPacket[0])						// コマンドコードチェック
		{
			/** 接続確認OK応答 **/
			case CHECK:
				INPacket[1] = 'O';
				INPacket[2] = 'K';
				counter=0x03;						// 送信バイト数4
				break;
			/** 出力ピン出力要求の場合 ***/
			case POUT:
				if(OUTPacket[1] == 0x01){
					pout1_sts = POUT_ON;	RC0_High;
				}else{
					pout1_sts = POUT_OFF;	RC0_Low;
				}
				
				if(OUTPacket[2] == 0x01){
					pout2_sts = POUT_ON;	RC1_High;
				}else{
					pout2_sts = POUT_OFF;	RC1_Low;
				}
				
				if(OUTPacket[3] == 0x01){
					pout3_sts = POUT_ON;	RC2_High;
				}else{
					pout3_sts = POUT_OFF;	RC2_Low;
				}
				
				if(OUTPacket[4] == 0x01){
					pout4_sts = POUT_ON;	RC3_High;
				}else{
					pout4_sts = POUT_OFF;	RC3_Low;
				}
				pout5_sts = OUTPacket[5] == 0x01 ? POUT_ON:POUT_OFF;
				pout6_sts = OUTPacket[6] == 0x01 ? POUT_ON:POUT_OFF;
				pout7_sts = OUTPacket[7] == 0x01 ? POUT_ON:POUT_OFF;
				pout8_sts = OUTPacket[8] == 0x01 ? POUT_ON:POUT_OFF;
				
				break;
			case PIN:
				
				INPacket[1] = pout1_sts == POUT_ON ? 0x01:0x00;
				INPacket[2] = pout2_sts == POUT_ON ? 0x01:0x00;
				INPacket[3] = pout3_sts == POUT_ON ? 0x01:0x00;
				INPacket[4] = pout4_sts == POUT_ON ? 0x01:0x00;
				INPacket[5] = pout5_sts == POUT_ON ? 0x01:0x00;
				INPacket[6] = pout6_sts == POUT_ON ? 0x01:0x00;
				INPacket[7] = pout7_sts == POUT_ON ? 0x01:0x00;
				INPacket[8] = pout8_sts == POUT_ON ? 0x01:0x00;
				
				pin1_sts = PORTCbits.RC4 == 0 ? PIN_ON:PIN_OFF;
				pin2_sts = PORTCbits.RC5 == 0 ? PIN_ON:PIN_OFF;
				
				INPacket[9] = pin1_sts == PIN_ON ? 0x01:0x00;
				INPacket[10] = pin2_sts == PIN_ON ? 0x01:0x00;
				INPacket[11] = pin3_sts == PIN_ON ? 0x01:0x00;
				INPacket[12] = pin4_sts == PIN_ON ? 0x01:0x00;
				INPacket[13] = pin5_sts == PIN_ON ? 0x01:0x00;
				INPacket[14] = pin6_sts == PIN_ON ? 0x01:0x00;
				INPacket[15] = pin7_sts == PIN_ON ? 0x01:0x00;
				INPacket[16] = pin8_sts == PIN_ON ? 0x01:0x00;
				
				counter=17;
				break;
			/**** 一括状態転送要求の場合 **************/
			// case ALL:
				
				// INPacket[2] = PORTCbits.RC4 == 0 ? 0x30:0x31;
				// INPacket[3] = PORTCbits.RC5 == 0 ? 0x30:0x31;
				
				// INPacket[10] = LATCbits.LATC0 == 0 ? 0x30:0x31;
				// INPacket[11] = LATCbits.LATC1 == 0 ? 0x30:0x31;
				// INPacket[12] = LATCbits.LATC2 == 0 ? 0x30:0x31;
				// INPacket[13] = LATCbits.LATC3 == 0 ? 0x30:0x31;
				
				
				// // DI状態セット 
				// //INPacket[2] = mPI_1==1 ? 0x30:0x31;
				// //INPacket[3] = mPI_2==1 ? 0x30:0x31;
				// //INPacket[4] = mPI_3==1 ? 0x30:0x31;
				// //INPacket[5] = mPI_4==1 ? 0x30:0x31;	
				// // DO状態セット 				
				// //INPacket[6] = mPIO_1==0 ? 0x30:0x31;
				// // INPacket[6]= debugLED==0 ? 0x30:0x31;							
				// // INPacket[7] = mPIO_2==0 ? 0x30:0x31;
				// // INPacket[8] = mPIO_3==0 ? 0x30:0x31;
				// // INPacket[9] = mPIO_4==0 ? 0x30:0x31;												
				// // INPacket[10]= mPIO_5==0 ? 0x30:0x31;							
			// /*
				// // 計測データセット 
				// ADCON0 = 0x11;					// AN4選択
				// ADCON0bits.GO = 1;              // A/D変換開始
				// while(ADCON0bits.NOT_DONE);     // 変換完了待ち
				// INPacket[11] = ADRESL;			// 送信バッファにセット
				// INPacket[12] = ADRESH;						
				// ADCON0 = 0x15;					// AN5選択
				// ADCON0bits.GO = 1;              // A/D変換開始
				// while(ADCON0bits.NOT_DONE);     // 変換完了待ち
				// INPacket[13] = ADRESL;			// 送信バッファにセット
				// INPacket[14] = ADRESH;
				// ADCON0 = 0x19;					// AN6選択
				// ADCON0bits.GO = 1;              // A/D変換開始
				// while(ADCON0bits.NOT_DONE);     // 変換完了待ち
				// INPacket[15] = ADRESL;			// 送信バッファにセット
				// INPacket[16] = ADRESH;
			// */
				// counter = 17;
				// break;																	
			// /** 切り離しコマンドの場合　***/
			case DISCONNECT:
				Reset();
				break;
			// case 0x55:
				// INPacket[2] = 0x32;
				// INPacket[3] = 0x33;
				// INPacket[4] = 0x34;
				// INPacket[5] = 0x35;
				// INPacket[6] = 0x36;
				// INPacket[7] = 0x37;
				// INPacket[8] = 0x38;
				// INPacket[9] = 0x39;
				// counter = 10;
				// break;
			// case 0xAA:
				// INPacket[2] = 0xA2;
				// INPacket[3] = 0xA3;
				// INPacket[4] = 0xA4;
				// INPacket[5] = 0xA5;
				// INPacket[6] = 0xA6;
				// INPacket[7] = 0xA7;
				// INPacket[8] = 0xA8;
				// INPacket[9] = 0xA9;
				// counter = 10;
				// break;
			default:
				break;
		}
		/**** USB送受信実行 常に64バイトで送信 *****/
		if(counter != 0){								// 送信データありか？
			while(USBHandleBusy(USBGenericInHandle));		// ビジーチェック
			/* 送信実行　*/
			USBGenericInHandle = USBGenWrite(USBGEN_EP_NUM,(BYTE*)&INPacket,64);
		}
		/* 次の受信実行 常に64バイト受信 */
		USBGenericOutHandle = USBGenRead(USBGEN_EP_NUM,(BYTE*)&OUTPacket,64);
	}
}


/******************************************************************
************** USB Callback Functions *****************************
*******************************************************************/
/******************************************************************
 * Function:        void USBCBSuspend(void)
 ******************************************************************/
void USBCBSuspend(void)
{
}
/*******************************************************************
 * Function:        void USBCBWakeFromSuspend(void)
 *******************************************************************/
void USBCBWakeFromSuspend(void)
{
}
/********************************************************************
 * Function:        void USBCB_SOF_Handler(void)
 *******************************************************************/
void USBCB_SOF_Handler(void)
{
}
/*******************************************************************
 * Function:        void USBCBErrorHandler(void)
 *******************************************************************/
void USBCBErrorHandler(void)
{
}
/*******************************************************************
 * Function:        void USBCBCheckOtherReq(void)
 *******************************************************************/
void USBCBCheckOtherReq(void)
{
}//end
/*******************************************************************
 * Function:        void USBCBStdSetDscHandler(void)
 *******************************************************************/
void USBCBStdSetDscHandler(void)
{
}//end
/*******************************************************************
 * Function:        void USBCBInitEP(void)
 *******************************************************************/
void USBCBInitEP(void)
{
    USBEnableEndpoint(USBGEN_EP_NUM,USB_OUT_ENABLED|USB_IN_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
    USBGenericOutHandle = USBGenRead(USBGEN_EP_NUM,(BYTE*)&OUTPacket,USBGEN_EP_SIZE);
}
/*******************************************************************
 * Function:        void USBCBSendResume(void)
 ******************************************************************/
void USBCBSendResume(void)
{
    static WORD delay_count;

    if(USBGetRemoteWakeupStatus() == TRUE) 
    {
        //Verify that the USB bus is in fact suspended, before we send
        //remote wakeup signalling.
        if(USBIsBusSuspended() == TRUE)
        {
            USBMaskInterrupts();
            
            //Clock switch to settings consistent with normal USB operation.
            USBCBWakeFromSuspend();
            USBSuspendControl = 0; 
            USBBusIsSuspended = FALSE;  //So we don't execute this code again, 

            delay_count = 3600U;
            do
            {
                delay_count--;
            }while(delay_count);
            
            //Now drive the resume K-state signalling onto the USB bus.
            USBResumeControl = 1;       // Start RESUME signaling
            delay_count = 1800U;        // Set RESUME line for 1-13 ms
            do
            {
                delay_count--;
            }while(delay_count);
            USBResumeControl = 0;       //Finished driving resume signalling

            USBUnmaskInterrupts();
        }
    }
}
/*******************************************************************
 * Function:        BOOL USER_USB_CALLBACK_EVENT_HANDLER
 *******************************************************************/
BOOL USER_USB_CALLBACK_EVENT_HANDLER(USB_EVENT event, void *pdata, WORD size)
{
    switch(event)
    {
        case EVENT_TRANSFER:
            break;
        case EVENT_SOF:
            USBCB_SOF_Handler();
            break;
        case EVENT_SUSPEND:
            USBCBSuspend();
            break;
        case EVENT_RESUME:
            USBCBWakeFromSuspend();
            break;
        case EVENT_CONFIGURED: 
            USBCBInitEP();
            break;
        case EVENT_SET_DESCRIPTOR:
            USBCBStdSetDscHandler();
            break;
        case EVENT_EP0_REQUEST:
            USBCBCheckOtherReq();
            break;
        case EVENT_BUS_ERROR:
            USBCBErrorHandler();
            break;
        case EVENT_TRANSFER_TERMINATED:

            break;
        default:
            break;
    }      
    return TRUE; 
}

