/*******************************************************************************
 Projektname:       PRO-BOT128_Lib_V2.cprj
 Benötigte Libs´s:  IntFunc_lib.cc
 Routinen:          PRO-BOT128_Lib_V2.cc
 Autor:             Ulli Sommer / F. Rhyner
 Datum:             24.03.2009 / 24.11.2014 / 25.4.2015
 Funktion:          PRO-BOT128 Function Lib (Experimente mit den PRO-BOT128)
                    fuer C-Control Pro IDE ab Version 2.31
 ******************************************************************************/

//Steuerzeichen
#define LF 0x0A
#define CR 0x0D
#define SPACE 0x20

//I2C-Bus
#define SCL 24
#define SDA 25

//Zustände
#define ON 0
#define OFF 1
#define OUT 1
#define IN 0

//Status LEDs
#define FLL 19
#define FLR 18
#define BLL 17
#define BLR 16

//Liniensensor LED
#define Line_LED 20

//Taster SW2 (Boot)
#define Button 36

//Motor Enable Leitung
#define Motor_Enable 15

//Radencoder IR-LED
#define Encoder_IR 8

//ACS (Anit-Collisions-System)
#define PWM_IR 35
#define IR_left 27
#define IR_right 29
#define TSOP 26

//ADC Auflösung: 5V/1023 Steps
#define Ref 0.00488

//Negative Beschleunigung bei Zielnähe GO(xxx,xxx)
#define Deathzone 50

//Tonausgae über Noten
//1. Oktave
#define Tone_C1 440     //261Hz
#define Tone_Cis1 413   //277Hz
#define Tone_D1 392     //293Hz
#define Tone_Dis1 371   //311Hz
#define Tone_E1 350     //329Hz
#define Tone_F1 330     //349Hz
#define Tone_Fis1 312   //369Hz
#define Tone_G1 294     //392Hz
#define Tone_Gis1 277   //415Hz
#define Tone_A1 262     //440Hz
#define Tone_Ais1 247   //466Hz
#define Tone_H1 234     //493Hz

//2. Oktave
#define Tone_C2 220     //523Hz
#define Tone_Cis2 208   //554Hz
#define Tone_D2 196     //587Hz
#define Tone_Dis2 185   //622Hz
#define Tone_E2 175     //659Hz
#define Tone_F2 165     //698Hz
#define Tone_Fis2 156   //739Hz
#define Tone_G2 147     //784Hz
#define Tone_Gis2 138   //830Hz
#define Tone_A2 131     //880Hz
#define Tone_Ais2 125   //932Hz
#define Tone_H2 117     //987Hz

//UART-Schnittstelle
byte Uart_Buffer[80];
char Debug_Text[80];
byte Daten_Buffer[80];
int Buffer_Index;

//UART-Datenauswertung
byte Addr;
byte Command1;
byte Command2;
byte Command3;
byte CRC;
byte CRC_Sum;
byte Robo_Addr;
byte Command_Speed;

//Odometrie & Encoder
#define UNIT_TICK 3
#define SPEED_INTEGRATOR 10
#define DEG_TICK 10
int ENCODER_RIGHT;
int ENCODER_RIGHT_CNT;
int ENCODER_LEFT;
int ENCODER_LEFT_CNT;
int ODO_LEFT;
int ODO_RIGHT;

//Geschwindigkeitsregelung
byte left_dir;
byte right_dir;
int diff_reg;
byte speed_left;
byte speed_right;
int Direction;
int Turn;
int Speed;
byte fwd, bwd, turn_left, turn_right;

//Taster Variable für SW2 (Boot)
byte SW2;

//System Uhr
int rtc_cnt;
int Sekunde;
int Minute;
int Stunde;

//---[ System Initialisierung ]-------------------------------------------------
void PRO_BOT128_INIT(void)
{

    //SPI-Schnittstelle ausschalten
    SPI_Disable();

    //IR-LED Encoder Pin als Ausgang
    Port_DataDirBit(Encoder_IR,1);

    //Line-LED Pin als Ausgang
    Port_DataDirBit(Line_LED,PORT_OUT);

    //Status LEDs Pins als Ausgang
    Port_DataDirBit(FLL,PORT_OUT);
    Port_DataDirBit(FLR,PORT_OUT);
    Port_DataDirBit(BLL,PORT_OUT);
    Port_DataDirBit(BLR,PORT_OUT);

    //Taster SW2 als Eingang + Pullup ein
    Port_DataDirBit(Button,0);
    Port_WriteBit(Button,1);

    //ACS-System
    Port_DataDirBit(IR_left,PORT_OUT);
    Port_DataDirBit(IR_right,PORT_OUT);
    Port_DataDirBit(TSOP,PORT_IN);

    //Radencoder Interrupts
    Irq_SetVect(INT_6,ENC_RIGHT);
    Irq_SetVect(INT_7,ENC_LEFT);

    //Interrupt bei jeden Flankenwechsel
    Ext_IntEnable(6,1);
    Ext_IntEnable(7,1);

    //Taster SW2 Interrupt
    Irq_SetVect(INT_4,BUTTON);
    Ext_IntEnable(4,2);

    //Timer 2 Interrupt
    Irq_SetVect(INT_TIM2COMP,SYSTEM_CNT);

    //I2C-Bus Initialisierung
    I2C_Init(I2C_100kHz);

    //Antrieb Initialisierung
    Port_DataDirBit(Motor_Enable,PORT_OUT);  //Enable Pin als Ausgang
    Timer_T1PWMX(255,1,1,PS_1);              //PWM Initialisierung
    Timer_T1PWA(128);                        //PWM Kanal A
    Timer_T1PWB(128);                        //PWM Kanal B

    //Initialisierung auf Startzustand des Roboters
    FLL_OFF();
    FLR_OFF();
    BLL_OFF();
    BLR_OFF();
    LINE_LED_OFF();
    ENC_LED_OFF();
    DRIVE_OFF();
    ODO_RESET();

    //SW2 Variable
    SW2 = 0;

    //Motorregler Startwerte
    diff_reg = 0;
    Direction = 255;
    Turn = 255;
    Speed = 0;
    fwd = 0;
    bwd = 0;
    turn_left = 0;
    turn_right = 0;

    //PC-BOT Interface (PRO-BOT128 Adresse)
    Robo_Addr = 42;

    //RTC Startzeit festlegen z.B. 00:00:00
    Sekunde=0;
    Minute=0;
    Stunde=0;

}

//---[ Taster SW2 über IRQ ]----------------------------------------------------
byte BUTTON(void)
{
    SW2 = 0;
    SW2 = Irq_GetCount(INT_4);
    return SW2;
}

//---[ System Counter 10ms ]----------------------------------------------------
void SYSTEM_CNT(void)
{
    //RTC "Real Time Clock"
    rtc_cnt++;
    if(rtc_cnt==100)
    {
       Sekunde++;

       if(Sekunde==60)
       {
          Sekunde=0;
          Minute++;

          if(Minute==60)
          {
             Minute=0;
             Stunde++;

             if(Stunde==24)
             {
                Stunde = 0;
             }
          }
       }
       rtc_cnt=0;
    }

   Irq_GetCount(INT_TIM2COMP);

}

//---[ Status LED "FRONT LED LEFT" einschalten ]--------------------------------
void FLL_ON(void)
{
    Port_WriteBit(FLL,PORT_OFF);
}

//---[ Status LED "FRONT LED LEFT" ausschalten ]--------------------------------
void FLL_OFF(void)
{
    Port_WriteBit(FLL,PORT_ON);
}

//---[ Status LED "FRONT LED RIGHT" einschalten ]-------------------------------
void FLR_ON(void)
{
    Port_WriteBit(FLR,PORT_OFF);
}

//---[ Status LED "FRONT LED RIGHT" ausschalten ]-------------------------------
void FLR_OFF(void)
{
    Port_WriteBit(FLR,PORT_ON);
}

//---[ Status LED "BACK LED LEFT" einschalten ]---------------------------------
void BLL_ON(void)
{
    Port_WriteBit(BLL,PORT_OFF);
}

//---[ Status LED "BACK LED LEFT" ausschalten ]---------------------------------
void BLL_OFF(void)
{
    Port_WriteBit(BLL,PORT_ON);
}

//---[ Status LED "BACK LED RIGHT" einschalten ]--------------------------------
void BLR_ON(void)
{
    Port_WriteBit(BLR,PORT_OFF);
}

//---[ Status LED "BACK LED RIGHT" ausschalten ]--------------------------------
void BLR_OFF(void)
{
    Port_WriteBit(BLR,PORT_ON);
}

//---[ IRQ Routine für den rechten Radencoder ]---------------------------------
void ENC_RIGHT(void)
{
    ENCODER_RIGHT_CNT++;
    ODO_RIGHT++;
    Irq_GetCount(INT_6);
}

//---[ IRQ Routine für den linken Radencoder ]----------------------------------
void ENC_LEFT(void)
{
    ENCODER_LEFT_CNT++;
    ODO_LEFT++;
    Irq_GetCount(INT_7);
}

//---[ Odometer zurücksetzen ]--------------------------------------------------
void ODO_RESET(void)
{
    ODO_LEFT=0;
    ODO_RIGHT=0;
}

//---[ IR Encoder LED einschalten ]---------------------------------------------
void ENC_LED_ON(void)
{
    Port_WriteBit(Encoder_IR,PORT_ON);
}

//---[ IR Encoder LED ausschalten ]---------------------------------------------
void ENC_LED_OFF(void)
{
    Port_WriteBit(Encoder_IR,PORT_OFF);
}

//---[ Tonausgabe ]-------------------------------------------------------------
void BEEP(word Tone, word Periode)
{
    Timer_T0FRQ(Tone,PS_64);
    DELAY_MS(Periode);
    Timer_T0Stop();
    Timer_T0FRQ(440,1);
}

//---[ Geräuschpegel messen ]---------------------------------------------------
word SOUND_LEVEL(void)
{
    return GET_ADC(3);
}

//---[ Lichtsensor links auslesen ]---------------------------------------------
word LDR_LEFT(void)
{
    return GET_ADC(4);
}

//---[ Lichtsensor rechts auslesen ]--------------------------------------------
word LDR_RIGHT(void)
{
    return GET_ADC(5);
}

//---[ Wert des linken Fototransistor des Liniensensors auslesen ]--------------
word READ_LINE_LEFT(void)
{
    return GET_ADC(2);
}

//---[ Wert des rechten Fototransistor des Liniensensors auslesen ]-------------
word READ_LINE_RIGHT(void)
{
    return GET_ADC(1);
}

//---[ Liniensensor LED einschalten ]-------------------------------------------
void LINE_LED_ON(void)
{
    Port_WriteBit(Line_LED,PORT_OFF);
}

//---[ Liniensensor LED ausschalten ]-------------------------------------------
void LINE_LED_OFF(void)
{
    Port_WriteBit(Line_LED,PORT_ON);
}

//---[ Betriebsspannung messen ]------------------------------------------------
float AKKU_SPG(void)
{
    return (GET_ADC(0) * Ref) * 1.08;  //Betriebsspannung berechnen
}

//---[ Beliebigen ADC-Kanal einlesen ]------------------------------------------
word GET_ADC(byte Channel)
{
    //Uref 2,56V internal ref.
    ADC_Set(ADC_VREF_BG,Channel);
    return ADC_Read();
}

//---[ Antrieb einschalten ]----------------------------------------------------
void DRIVE_ON(void)
{
    Port_WriteBit(Motor_Enable,1);
}

//---[ Antrieb ausschalten ]----------------------------------------------------
void DRIVE_OFF(void)
{
    Port_WriteBit(Motor_Enable,0);
}

//---[ Geschwindigkeit und Richtung über PWM-Werte einstellen ]-----------------
void DRIVE(byte left, byte right)
{

    /*********************************
     ---> Direkte PWM Ansteuerung
          128 = stopp
          128 bis 255 = vorwärts
          1 bis 128 = rückwärts
     *********************************/

    //Begrenzung
    if(left>255)left=255;
    if(left<1)left=1;
    if(right>255)right=255;
    if(right<1)right=1;

    Timer_T1PWA(left);
    Timer_T1PWB(right);

}

//---[ Motor-Drehrichtung 1=FWD ; 0=RWD ; 255=STOP ]----------------------------
void MOTOR_DIR(byte left, byte right)
{
    //FWD = vorwärts
    //RWD = rückwärts
    left_dir=left;
    right_dir=right;
}

//---[ Alternative Geschwindigkeits Einstellung ]-------------------------------
void MOTOR_POWER(byte left, byte right)
{

    //Drehrichtung muss zuvor mit MOTOR_DIR(xxx,xxx) festgelegt werden!

    //Begrenzung
    left=left/2;
    right=right/2;

    //linker Motor 0 bis 255
    //vorwärts
    if(left_dir==1)Timer_T1PWA(128+left);

    //linker Motor 0 bis 255
    //rückwärts
    if(left_dir==0)Timer_T1PWA(128-left);

    //Motor links stopp
    if(left_dir==255)Timer_T1PWA(128);

    //rechter Motor 0 bis 255
    //vorwärts
    if(right_dir==1)Timer_T1PWB(128+right);

    //rechter Motor 0 bis 255
    //rückwärts
    if(right_dir==0)Timer_T1PWB(128-right);

    //Motor rechts stopp
    if(right_dir==255)Timer_T1PWB(128);

}

//---[ Antrieb anhalten ]-------------------------------------------------------
void MOTOR_STOP(void)
{
    Direction=255;
    Turn=255;
    Speed=0;
    MOTOR_DIR(255,255);
    MOTOR_POWER(128,128);
    diff_reg=0;
    fwd=0;bwd=0;turn_left=0;turn_right=0;
}

//---[ Geregelte vorwärts fahren ]----------------------------------------------
void DRIVE_FWD(byte speed, int time)
{
    //speed Werte von 1 bis 200
    //time gibt an wie lange vorwärts gefahren werden soll (100ms Einheiten)
    //wird bei time eine 0 eingetragen, wird nach verlassen der Routine
    //der Antrieb nicht von selber wieder angehalten!!!
    int x;
    for(x=0;x<=time;x++) //100ms steps
    {
        Turn=255;
        Direction=1;
        bwd=0;turn_left=0;turn_right=0;
        if(fwd==0)
        {
          fwd=1;
          speed_left=speed;
          speed_right=speed;
          Speed=speed;
        }
        REGULATOR();
    }
    if(time>=1)MOTOR_STOP();
}

//---[ Geregelte rückwärts fahren ]---------------------------------------------
void DRIVE_BWD(byte speed, int time)
{
    //speed Werte von 1 bis 200
    //time gibt an wie lange rückwärts gefahren werden soll (100ms Einheiten)
    //wird bei time eine 0 eingetragen, wird nach verlassen der Routine
    //der Antrieb nicht von selber wieder angehalten!!!
    int x;
    for(x=0;x<=time;x++)
    {
        Turn=255;
        Direction= -1;
        fwd=0;turn_left=0;turn_right=0;
        if(bwd==0)
        {
          bwd=1;
          speed_left=speed;
          speed_right=speed;
          Speed=speed;
        }
        REGULATOR();
    }
    if(time>=1)MOTOR_STOP();
}

//---[ Geregelte links drehen ]-------------------------------------------------
void TURN_LEFT(byte speed, int time)
{
    //speed Werte von 1 bis 200
    //time gibt an wie lange gedreht werden soll (100ms Einheiten)
    //wird bei time eine 0 eingetragen, wird nach verlassen der Routine
    //der Antrieb nicht von selber wieder angehalten!!!
    int x;
    for(x=0;x<=time;x++)
    {
        Direction=255;
        Turn= -1;
        fwd=0;bwd=0;turn_right=0;
        if(turn_left==0)
        {
          turn_left=1;
          speed_left=speed;
          speed_right=speed;
          Speed=speed;
        }
        REGULATOR();
    }
    if(time>=1)MOTOR_STOP();
}

//---[ Geregelte rechts drehen ]------------------------------------------------
void TURN_RIGHT(byte speed, int time)
{
    //speed Werte von 1 bis 200
    //time gibt an wie lange gedreht werden soll (100ms Einheiten)
    //wird bei time eine 0 eingetragen, wird nach verlassen der Routine
    //der Antrieb nicht von selber wieder angehalten!!!
    int x;
    for(x=0;x<=time;x++)
    {
        Direction=255;
        Turn=1;
        fwd=0;bwd=0;turn_left=0;
        if(turn_right==0)
        {
          turn_right=1;
          speed_left=speed;
          speed_right=speed;
          Speed=speed;
        }
        REGULATOR();
    }
    if(time>=1)MOTOR_STOP();
}

//---[ Regelwerte berechnen ]---------------------------------------------------
void REGULATOR(void)
{

       ENCODER_LEFT=ENCODER_LEFT_CNT;
       ENCODER_RIGHT=ENCODER_RIGHT_CNT;
       ENCODER_LEFT_CNT=0;
       ENCODER_RIGHT_CNT=0;

       //vorwärts
       if((Direction==1) && (Speed>0))MOTOR_DIR(1,1);

       //rückwärts
       if((Direction== -1) && (Speed>0))MOTOR_DIR(0,0);

       //rechts drehen
       if((Turn==1) && (Speed>0))MOTOR_DIR(1,0);

       //links drehen
       if((Turn== -1) && (Speed>0))MOTOR_DIR(0,1);

       //---> Differenz berechnen

       diff_reg=ENCODER_LEFT-ENCODER_RIGHT;

       //---> Drehzahl ausgleichen

       if(diff_reg>0)
       {
          //linkes Rad schneller als das rechte?
          if((speed_left>Speed) || (speed_right>250))
          {
              speed_left=speed_left-SPEED_INTEGRATOR;
          }
          else
          {
              speed_right=speed_right+SPEED_INTEGRATOR;
          }
       }

       if(diff_reg<0)
       {
         //rechtes Rad schneller als das linke?
         if((speed_right>Speed) || (speed_left>250))
         {
             speed_right=speed_right-SPEED_INTEGRATOR;
         }
         else
         {
             speed_left=speed_left+SPEED_INTEGRATOR;
         }
       }

       MOTOR_POWER(speed_left,speed_right);

       DELAY_MS(100);

}

//---[ GO(Distance As cm, Speed 1 To 255) ]-------------------------------------
void GO(int distance, byte speed, int return_wait)
{

    //Distance: vorwärts = +1 To +32767cm ; rückwärts = -1 To -32768cm
    //Speed   : Geschwindigkeit 128 To 220

    int enc_count, tot_count;
    enc_count=0;
    tot_count=0;

    //Antrieb sicherheitshalber mal anhalten
    MOTOR_STOP();

    //Impulse in cm umrechnen
    enc_count=ABS_INT(distance)*UNIT_TICK;

    //Odometer zurücksetzen
    ODO_RESET();

    //Go To... jetzt bewegen wir uns!
    do
    {
       //Neue Odometerwerte aufaddieren
       tot_count=tot_count+(ODO_LEFT+ODO_RIGHT/2);

       //Odometer zurücksetzen
       ODO_RESET();

       //Ziel fast erreicht? --> langsamer werden
       if(tot_count>(enc_count-Deathzone))speed=140;

       //Antrieb ansteuern
       if(distance < -0)
       {
          DRIVE_BWD(speed,0);
          BLL_ON();
          BLR_ON();
       }

       if(distance>0)
       {
          DRIVE_FWD(speed,0);
          FLL_ON();
          FLR_ON();
       }


    }while (tot_count<enc_count);

    //Antrieb anhalten und die vorgegebene Zeit warten bevor wir zurückspringen
    MOTOR_STOP();
    BLL_OFF();
    BLR_OFF();
    FLL_OFF();
    FLR_OFF();
    DELAY_MS(return_wait);

}


//---[TURN(Degrees As Grad, Speed 1 To 255) ]-----------------------------------
void TURN(int degrees, byte speed, int return_wait)
{

    //Degrees: Winkelangabe von 1 bis 360° = rechts drehen ; -1 To -360 = links drehen
    //Speed  : Drehgeschwindigkeit von 1 bis 200

    int enc_count, tot_count;

    //Variablen Initialisieren
    tot_count=0;
    enc_count=0;

    //Impulse in Grad umrechnen
    enc_count=ABS_INT(degrees)*DEG_TICK;
    enc_count=enc_count/36;

    //Odometer zurücksetzen
    ODO_RESET();

    //Jetzt drehen wir uns mal...
    do
    {

       tot_count=tot_count+(ODO_LEFT+ODO_RIGHT/2);

       //Odometer zurücksetzen
       ODO_RESET();

       //Antrieb ansteuern
       if(degrees < -1)
       {
          TURN_LEFT(speed,0);
          FLL_ON();
          BLL_ON();
       }

       if(degrees>0)
       {
          TURN_RIGHT(speed,0);
          FLR_ON();
          BLR_ON();
       }

    } while (tot_count<enc_count);

    //Antrieb anhalten und die vorgegebene Zeit warten bevor wir zurückspringen
    MOTOR_STOP();
    BLL_OFF();
    BLR_OFF();
    FLL_OFF();
    FLR_OFF();
    DELAY_MS(return_wait);

}

//---[ ACS System ]-------------------------------------------------------------
void ACS_INIT(byte sensity)
{
    /*
    PWM für das ACS-System einstellen
    Timer_T3PWM(Par1,Par2,PS);
    Period = Par1*PS/FOSC (51*8/14,7456MHz=27,66 µs)  = 36Khz  Wert=51
    Pulse  = Par2*PS/FOSC (25*8/14,7456MHz=13,56 µs) On Time
    Timer_T3PWM(Word period,Word PW0,Byte PS) '36Khz
    Mit Par1, Par2 kann die Entfernung eingestellt werden
    muss teilweise angepasst werden (Roboter spezifisch)
    */
    Port_WriteBit(IR_left,PORT_ON);
    Port_WriteBit(IR_right,PORT_ON);
    Timer_T3PWM(51,sensity,PS_8);
}

//---[ IR-UART-INIT ]-----------------------------------------------------------
void IR_UART_INIT(void)
{
    /*
    PWM für die IR-Datenübertragung einstellen
    Timer_T3PWM(Par1,Par2,PS);
    Period = Par1*PS/FOSC (51*8/14,7456MHz=27,66 µs)  = 36Khz
    Pulse  = Par2*PS/FOSC (25*8/14,7456MHz=13,56 µs) On Time
    Timer_T3PWM(Word period,Word PW0,Byte PS) '36Khz
    */
    Port_WriteBit(IR_left,PORT_ON);
    Port_WriteBit(IR_right,PORT_ON);
    Timer_T3PWM(51,25,PS_8);
}

//---[ "ACS" Hindernis rechts? ]------------------------------------------------
byte ACS_RIGHT(void)
{
    byte ACS_Right;
    Port_WriteBit(IR_left,PORT_ON);
    Port_WriteBit(IR_right,PORT_OFF);
    DELAY_MS(1);
    ACS_Right=Port_ReadBit(TSOP);
    Port_WriteBit(IR_right,PORT_ON);
    DELAY_MS(1);
    return ACS_Right;
}

//---[ "ACS" Hindernis links? ]-------------------------------------------------
byte ACS_LEFT(void)
{
    byte ACS_Left;
    Port_WriteBit(IR_right,PORT_ON);
    Port_WriteBit(IR_left,PORT_OFF);
    DELAY_MS(1);
    ACS_Left=Port_ReadBit(TSOP);
    Port_WriteBit(IR_left,PORT_ON);
    DELAY_MS(1);
    return ACS_Left;
}

//---[ Verzögerungs Routine ]---------------------------------------------------
void DELAY_MS(int time)
{
    //Der Interpreter wird mit dieser Routine nicht komplett angehalten
    //nicht sonderlich präzise, aber die bessere alternative zu AbsDelay!
    int x, y;
    for(x=0;x<=time;x++)
    {
       for(y=0;y<=60;y++)
       {
       }
    }
}

//---[ Absolte Integer ]--------------------------------------------------------
int ABS_INT(int Val)
{
    //Minuswerte werden Positiv gerechnet
    int X_Save;
    if(Val < -0)
    {
       X_Save=Val;
       Val=X_Save-Val;
       Val=Val-X_Save;
       return Val;
    }
    else
    {
       return Val;
    }
}

//---[ Aus einen Lowbyte und einen Highbyte eine Word Variable zusammenstellen]-
word MAKE_WORD(byte a, byte b)
{
    return (a << 8) + b;
}

//---[ UART 0 Initialisieren ]--------------------------------------------------
void UART_0_INIT(void)
{
    //Init RS232: 19200 bps, 8 Bit, 1 Stop, NP
    Serial_Init_IRQ(0,Uart_Buffer,40,70,SR_8BIT | SR_1STOP | SR_NO_PAR,SR_BD19200);
}

//---[ UART 1 Initialisieren ]--------------------------------------------------
void UART_1_INIT(void)
{
    //Init RS232: 2400 bps, 8 Bit, 1 Stop, NP
    Serial_Init_IRQ(1,Uart_Buffer,40,70,SR_8BIT | SR_1STOP | SR_NO_PAR,SR_BD2400);
}

//---[ Ein CR+LF aus den UART schreiben ]---------------------------------------
void NEWLINE(byte Com)
{
    Serial_Write(Com,CR);
    Serial_Write(Com,LF);
    DELAY_MS(50);
}

//---[ Einen Text aus den UART schreiben ]--------------------------------------
void PRINT(byte Com, char serial_text[])
{
    Serial_WriteText(Com,serial_text);
    DELAY_MS(50);
}

//---[ Einen Text + CR/LF aus den UART schreiben ]------------------------------
void PRINTLN(byte Com, char serial_text[])
{
    Serial_WriteText(Com,serial_text);
    NEWLINE(Com);
    DELAY_MS(50);
}

//---[ Eine Integer Variable aus den UART schreiben ]---------------------------
void PRINT_INT(byte Com, int Val)
{
    char text[10];
    Str_WriteInt(Val,text,0);
    PRINT(Com,text);
}

//---[ Eine Word Variable aus den UART schreiben ]------------------------------
void PRINT_WORD(byte Com, int Val, byte len)
{
    char text[6];
    Str_WriteWord(Val,10,text,0,len);
    PRINT(Com,text);
}

//---[ Eine Float Variable aus den UART schreiben ]-----------------------------
void PRINT_FLOAT(byte Com, float Val, byte Len)
{
    char text[22];
    Str_WriteFloat(Val,Len,text,0);
    PRINT(Com,text);
}

//---[ SRF02 der Fa. Devantech auslesen (Ultraschall Sensor) ]------------------
int READ_SRF02(byte Slaveid)
{
    byte LSB, MSB, Temp, Slaveid_read;
    word tmp;
    Slaveid_read=Slaveid+1;

    //Messvorgang in cm starten
    I2C_Start();
    I2C_Write(Slaveid);
    I2C_Write(0);
    I2C_Write(81);
    I2C_Stop();

    DELAY_MS(100);

    I2C_Start();
    I2C_Write(Slaveid);
    I2C_Write(2);
    I2C_Stop();

    I2C_Start();
    I2C_Write(Slaveid_read);
    MSB=I2C_Read_ACK();
    LSB=I2C_Read_NACK();
    I2C_Stop();
    return (MSB<<8)+LSB;
}

//---[ Kompass CMPS03 von Devantech auslesen ]----------------------------------
word READ_CMPS03(void)
{
    word tmp;
    byte LSB, MSB;

    //Register auswählen
    I2C_Start();
    I2C_Write(0xC0);
    I2C_Write(2);
    I2C_Stop();

    //Werte holen
    I2C_Start();
    I2C_Write(0xC1);
    MSB=I2C_Read_ACK();
    LSB=I2C_Read_NACK();
    I2C_Stop();
    tmp=((MSB<<8)+LSB)/10; //Ausgabe in Grad
    return tmp;
}

//---[ CMPS03 Kalibrieren ]-----------------------------------------------------
void CAL_CMPS03(void)
{
    I2C_Start();
    I2C_Write(0xC0);
    I2C_Write(15);
    I2C_Write(255);
    I2C_Stop();
}

void SAVE_CAL_CMPS03(void)
{
    I2C_Start();
    I2C_Write(0xC0);
    I2C_Write(15);
    I2C_Write(0);
    I2C_Stop();
}

//---[ PCF8574 lesen ]----------------------------------------------------------
byte PCF8574_R(byte addr)
{
    byte Port;
    I2C_Start();
    I2C_Write(addr);
    Port=I2C_Read_NACK();
    I2C_Stop();
    return Port;
}

//---[ PCF8574 schreiben ]------------------------------------------------------
void PCF8574_W(byte addr, byte command)
{
    I2C_Start();
    I2C_Write(addr);
    I2C_Write(command);
    I2C_Stop();
}

//---[ I2C-Temperatursensor LM75 auslesen ]-------------------------------------
float READ_LM75(byte Addr)
{
    byte MSB, LSB;
    float Grad;

    I2C_Start();
    I2C_Write(Addr);
    MSB=I2C_Read_ACK();
    LSB=I2C_Read_NACK();
    I2C_Stop();

    if(MSB<0x80)
    {
       Grad=((MSB*10)+(((LSB&0x80)>>7)*5));
    }
    else
    {
       Grad=((MSB*10)+(((LSB&0x80)>>7)*5));
       Grad=-(2555.0-Grad);
    }

    return Grad/10.0;
}

//---[ UART Empfangsroutine ]---------------------------------------------------
void UART_INPUT(byte Com)
{
    int sz, Buffer_Cnt;

    Buffer_Index=0;
    Addr=0;
    Command1=0;
    Command2=0;
    Command3=0;
    CRC=255;

    Buffer_Cnt=Serial_IRQ_Info(Com,RS232_FIFO_RECV); //wie viele Char sind im Buffer?

    if(Buffer_Cnt!=0)                                //Sind überhaupt Daten da?
    {
        do                                           //Bytes holen
        {
         Buffer_Index++;
         sz=Serial_ReadExt(Com);
         if(sz!=13)                                  //Wurde ein CR empfangen?
         {
             Daten_Buffer[Buffer_Index]=sz;          //Bytes ins Array schieben
             if(sz==0x100)break;                     //bei &H100 sind keine Daten mehr im Buffer
                                                     //Schleife verlassen
         }
         else
         {
                 Addr=Daten_Buffer[1];               //Array Daten zuordnen
                 Command1=Daten_Buffer[2];
                 Command2=Daten_Buffer[3];
                 Command3=Daten_Buffer[4];
                 CRC=Daten_Buffer[5];
         }

        }while(1);
    }


    //Prüfsumme CRC mit Xor berechnen
    CRC_Sum=Addr | Command1 | Command2 | Command3;

    //Daten richtig empfangen? Ja --> Daten interpretieren
    if(CRC_Sum==CRC)DATA_INTERPRET(Com);

}


//---[ Empfangene Daten interpretieren ]----------------------------------------
void DATA_INTERPRET(byte Com)
{

    //RC Mode über den PC
    //Addr = 255 : Command1 = 1 : Command2 = 1 To 4/255 : Command3 = speed
     if(Addr==Robo_Addr)
     {
       if(Command1==1)
       {
         if(CRC_Sum==CRC)
         {
            Command_Speed=Command3;

            switch(Command2)
            {
                   case 1:
                        DRIVE_FWD(Command_Speed,0);
                        break;
                   case 2:
                        DRIVE_BWD(Command_Speed,0);
                        break;
                   case 3:
                        TURN_RIGHT(Command_Speed,0);
                        break;
                   case 4:
                        TURN_LEFT(Command_Speed,0);
                        break;
                   case 255:
                        MOTOR_STOP();
                        break;

            }
         }
       }
     }


    //System-Status an den PC senden
    //Addr = Robo_Addr : Command1 = 4 : Command2 = 1 Or 2 : Command3 = 0
     if(Addr==Robo_Addr)
     {
       if(Command1==4)
       {
        if(Command3==0)
        {
         if(CRC_Sum==CRC)
         {
          BEEP(150,200);
          if(Command2==1)STATUS(Com);
         }
        }
       }
     }


    //Roboter reseten
    //Addr = Robo_Addr : Command1 = 255 : Command2 = 128 : Command3 = 0
     if(Addr==Robo_Addr)
     {
       if(Command1==255)
       {
        if(Command3==0)
        {
         if(CRC_Sum==CRC)
         {
            if(Command2==128)
            {
               Debug_Text="System reset...";
               PRINTLN(Com,Debug_Text);
               main();
            }
         }
        }
       }
     }

}

//---[ Satus an den PC senden ]-------------------------------------------------
void STATUS(byte Com)
{
    NEWLINE(Com);
    NEWLINE(Com);
    Debug_Text="*** PRO-BOT128 Status ***";
    PRINTLN(Com,Debug_Text);
    CLOCK(Com);
    AKKU_STATUS(Com);
}

void AKKU_STATUS(byte Com)
{
    Debug_Text="Akku : ";
    PRINT(Com,Debug_Text);
    PRINT_FLOAT(Com,AKKU_SPG(),2);
    Debug_Text=" Volt";
    PRINT(Com,Debug_Text);
    NEWLINE(Com);
}

void CLOCK(byte Com)
{
    char Sep[2];
    Debug_Text="Clock: ";
    Sep=":";
    PRINT(Com,Debug_Text);
    PRINT_WORD(Com,Stunde,2);
    PRINT(Com,Sep);
    PRINT_WORD(Com,Minute,2);
    PRINT(Com,Sep);
    PRINT_WORD(Com,Sekunde,2);
    NEWLINE(Com);
}