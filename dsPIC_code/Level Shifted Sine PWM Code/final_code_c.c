//FCY = 29491200 or 33.908ns
#include <libpic30.h>
#include <p30f2020.h>
#pragma config POSCMD = HS                      // Primary Oscillator Source (HS Oscillator Mode)
#pragma config OSCIOFNC = OSC2_CLKO             // OSCI/OSCO Pin Function (OSCO pin has clock out function)
#pragma config FRANGE = FRC_HI_RANGE            // Frequency Range Select (High Range)
#pragma config FCKSM = CSW_FSCM_OFF             // Clock Switching and Monitor (Sw Disabled, Mon Disabled)
#pragma config FNOSC = PRIOSC_PLL               // Oscillator Mode (Primary Oscillator (HS, EC) w/ PLL)
_FWDT(FWDTEN_OFF);                              //Turn off WatchDog Timer
_FGS(CODE_PROT_OFF);                            //Turn off code protect
_FPOR( PWRT_OFF );                              //Turn off power up timer 


unsigned int i = 0;
unsigned int middle_value = 23620;
unsigned int sine_lookup[200]={8,352,705,1057,1409,1761,2112,2463,2813,3162,3511,3859,4206,4552,4896,5240,5582,5923,6262,6600,6936,7271,7603,
7934,8263,8590,8914,9237,9557,9875,10190,10503,10814,11121,11426,11728,12027,12323,12617,12907,13193,13477,13757,14034,14307,14577,14844,15106,
15365,15620,15871,16119,16362,16601,16836,17067,17294,17517,17735,17949,18158,18363,18563,18759,18950,19137,19318,19495,19668,19835,19997,20155,
20307,20455,20597,20735,20867,20994,21116,21232,21344,21450,21551,21646,21736,21821,21901,21975,22043,22107,22164,22216,22263,22304,22340,22370,
22395,22414,22428,22436,22436,22427,22413,22393,22368,22337,22301,22260,22212,22160,22102,22038,21969,21895,21815,21729,21639,21543,21442,21335,
21223,21106,20984,20856,20724,20586,20443,20295,20142,19984,19822,19654,19481,19304,19122,18935,18744,18547,18347,18141,17932,17717,17499,17276,
17049,16818,16582,16342,16099,15851,15600,15344,15085,14822,14556,14286,14012,13735,13454,13170,12883,12593,12300,12003,11704,11402,11097,10789,
10478,10165,9850,9531,9211,8888,8564,8237,7908,7577,7244,6909,6573,6235,5896,5555,5212,4869,4524,4178,3831,3483,3134,2785,2435,2084,1732,1381,1029,676,324,8};

int main()
{
    TRISB = 0;
    LATB = 0;                       //All port B pins are set to LOW
    TRISE = 0;
    LATE  = 0;                      //All PWM pins are set to LOW

    Init_Fcn_Pulse_PwrSply(); 		//555 ON
   
    Init_Fcn_PWM4();				//BOOST ON


	 
     IOCON1 = 0b1100000000000000;
     IOCON2 = 0b1100010000000000;
     IOCON3 = 0b1100010000000000;

    while (1) 
    {
        __asm__(" PWRSAV #1");
    }
}



        
Init_Fcn_PWM4()                         // PWM4 is used for BOOST converter switching pulse
{
    /*-------PWM4 Control Register settings-------*/
    PTCON = 0b0000000000000000;         //PWM is Disabled
    IOCON4 = 0b1100010000000000;
    DTR4 = 0b0000000000000000;
    PHASE4 = 0b0000000000000000;
    PWMCON4 = 0b0000010010000000;


	IOCON1 = 0b0000000000000000;
    DTR1 = 0b0000000000111111;          //DeadTime around 150ns
    ALTDTR1 = 0b0000000000111111;       //DeadTime around 150ns
    PHASE1 = 0b0000000000000000;
    PWMCON1 = 0b0000010001000000;
    
    
    IOCON2 = 0b0000010000000000;
    DTR2 = 0b0000000000000000;
    PHASE2 = 0b0000000000000000;
    PWMCON2 = 0b0000010010000000;
    
    IOCON3 = 0b0000010000000000;
    DTR3 = 0b0000000000000000;
    PHASE3 = 0b0000000000000000;
    PWMCON3 = 0b0000010010000000;
    
        /*-------PWM1 Interrupt settings-------*/
    IFS1bits.PWM1IF = 0;                // Clear the PWM Interrupt Flag
    IEC1bits.PWM1IE = 1;                // Enable the PWM Interrupt
    IPC4bits.PWM1IP = 6;                // PWM interrupt priority 6(7 is highest)
    TRIG1 = 100;                        // Interrupt occurs at every TMR=100

    PTPER = 23620;                      // PWM Time period 20kHz 
    PDC4 = 10000;
    PDC1 = 100;
	PDC2 = 100;
	PDC3 = 100;

    LATBbits.LATB4 = 0;                 //Initial status of S3 
    LATBbits.LATB5 = 1;                 //Initial status of S4
    PTCON = 0b1000000000000000;         // Enable PWM module
}


Init_Fcn_Pulse_PwrSply()                //555 Timer Replacement
{
    OC1CONbits.OCM = 0b110;             //PWM mode selected
    OC1R = 0;                           //OC Set time
    OC1RS = 737;                        //OC Reset Time
    PR2 = 1475;
    T2CONbits.TON = 1;
}
/* ------------------------------- PWM1 ISR --------------------------------- */
void __attribute__((__interrupt__, __auto_psv__)) _PWM1Interrupt(void)
{
    IFS1bits.PWM1IF = 0;                                // Clear the PWM Interrupt Flag
    i = i+1;
	if(i>33 && i<167)
	{
		
		IOCON3bits.POLH = 1;                            // Polarity of PWM3H Active LOW    1 & 3
		IOCON3bits.POLL = 1;                            // Polarity of PWM3L Active LOW    1 & 3		
		PDC1 = sine_lookup[i]*2;							// S1 & S2		
	    __asm__("BCLR.W  0x041E, #0x8");                // PWM2L OVERRIDE Disabled      2
        __asm__("BCLR.W  0x041E, #0x9");                // PWM2H OVERRIDE Disabled      2
		PDC2 = sine_lookup[i]*2-middle_value;             // 2
        PDC3 = sine_lookup[i]*2-middle_value;             // 1 & 3
	}
	else
	{
		IOCON3bits.POLH = 0;                            // Polarity of PWM3H Active HIGH   1 & 3
		IOCON3bits.POLL = 0;                            // Polarity of PWM3H Active HIGH   1 & 3		
		PDC1 = sine_lookup[i]*2;                          // S1 & S2
        __asm__("BSET.W  0x041E, #0x8");                // PWM2L OVERRIDES      2
        __asm__("BSET.W  0x041E, #0x9");                // PWM2H OVERRIDES      2
		PDC3 = sine_lookup[i]*2;                          // 1 & 3
	}
		
	if(i>199)
	{
		__asm__("BTG.W  0x040A, #0xD"); 				//PWM1H Polarity Toggle
		__asm__("BTG.W  0x040A, #0xC"); 				//PWM1L Polarity Toggle
		__asm__("BTG.W  0x02CA, #0x4");					//Toggle RB4; 		// S3
		__asm__("BTG.W  0x02CA, #0x5");					//Toggle RB5; 		// S4
		i = 0;
	}
	
}

                    
    

  
