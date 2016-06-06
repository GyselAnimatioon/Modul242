void main(void) {
    PRO_BOT128_INIT();
    LCD_INIT();
    // Richtwerte Hinderniserkennung: 8:15cm 6:11cm 5:9cm 4:6cm 3:4cm
    ACS_INIT(6);

    ENC_LED_ON();
    DRIVE_ON();

    int speedL;
    speedL = 180;
    int speedR;
    speedR = 180;

    while (1) {
        MOTOR_POWER(speedL,speedR);
        if(ACS_LEFT() && ACS_RIGHT()) {
            speedL = 180;
            speedR = 180;
            MOTOR_DIR(1,1);
            setText("Fährt");
        } else if(!ACS_LEFT() && !ACS_RIGHT()) {
            speedL = 250;
            speedR = 250;
            MOTOR_DIR(0,1);
            setText("Rueckwerts");
        } else if(!ACS_LEFT()) {
            speedL = 180;
            speedR = 180;
            MOTOR_DIR(1,0);
            setText("Links");
        } else if(!ACS_RIGHT()) {
            speedL = 180;
            speedR = 180;
            MOTOR_DIR(0,1);
            setText("Rechts");
        } else {
            setText("ELSE");
        }
    }
}

/* Zeigt den Text an Welchen man übergibt.
   Für die Anzahl an Milisekunden die man angibt. */
void showText(char text[], int dauer) {
    LCD_CLEAN_SCREEN();
    LCD_WRITE_TEXT(text);
    DELAY_MS(dauer);
}

/* Setzt den Text auf dem Display */
void setText(char text[]) {
    LCD_CLEAN_SCREEN();
    LCD_WRITE_TEXT(text);
}