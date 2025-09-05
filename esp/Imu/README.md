Postup kalibrace imu esp32

1. Zkompilovat a nahrát testovací fw do esp32 (#define CALIBRATE)
2. Přípojit k terminálu (115200 bd)
3. Postavit na vodorovnou podložku a nepohybovat deskou (robotem)
4. Resetovat esp (druhé tlačítko od usb konektoru)
5. Cca po 3 sekundách se objeví kalibrační hodnoty akcelerometru a gyra
6. Zapsat do config.h (pro daného robota)
   např.:
    #define ACC_BIAS    {-0.60, -58.37, -13.21}
    #define GYRO_BIAS   {-2.15, 0.25, -0.16}
7. Spustit aplikaci MotionCal
    https://learn.adafruit.com/adafruit-sensorlab-magnetometer-calibration/magnetic-calibration-with-motioncal
8. Připojit COMx a Clear
9. Otáčet robotem tak, aby se pokryla celá koule (viz. popis výše)
10. Zapsat do config.h
    např.:
    #define MAG_BIAS    {3.99, 9.87, 6.42}
    #define MAG_CORECT  {{0.999, 0.008, 0.002}, {0.008, 1.027, 0.009}, {0.002, 0.009, 0.974}}
11. Zkompilovat a nahrát testovací fw do esp32 bez kalibrace(// #define CALIBRATE)
12. Připojit k terminálu a otestovat
     