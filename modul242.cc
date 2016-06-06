void main(void) {
  PRO_BOT128_INIT();
  LCD_INIT();
  ACS_INIT(2);

  ENC_LED_ON();
  DRIVE_ON();

  int speed;
  speed = 180;

  while (1) {
    MOTOR_POWER(speed,speed);
    if(ACS_LEFT() && ACS_RIGHT()) {
      MOTOR_DIR(1,1);
      FLL_ON();
      BLL_ON();
    } else {
      MOTOR_DIR(0,1);
      FLL_OFF();
      BLL_OFF();
    }
  }
}
