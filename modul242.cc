void main(void) {
    PRO_BOT128_INIT();
    LCD_INIT();
    ACS_INIT(6);

    ENC_LED_ON();
    DRIVE_ON();

    int speedL, speedR;
    int links, rechts;

    while (1) {
        MOTOR_POWER(speedL,speedR);
        speedL = 180;
        speedR = 180;
        if(ACS_LEFT() && ACS_RIGHT()) {
            MOTOR_DIR(1,1);
            setText("Fahren");
        } else if(!ACS_LEFT() && !ACS_RIGHT()) {
            speedL = 250;
            speedR = 250;
            MOTOR_DIR(0,1);
            setText("Schnelles Drehen");
        } else if(!ACS_LEFT()) {
            MOTOR_DIR(1,0);
            setText("Rechts Drehen");
        } else if(!ACS_RIGHT()) {
            MOTOR_DIR(0,1);
            setText("Links Drehen");
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