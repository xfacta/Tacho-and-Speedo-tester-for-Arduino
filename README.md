# Tacho and Speedo tester for Arduino
- Outputs speed and RPM as square wave frequency
- for testing a tacho or speedo for automotive use
- Two independant outputs -> one for each of speed and RPM
- Using microseconds for timing
- because milliseconds did not have enough resolution
- and hardware timers wouldnt output two independant frequencies
 
 
 ## Uses Arduino Pro Mini and SH1106 OLED display
- I2C and text only
- Jaycar XC3728 OLED set to I2C

Outputs would need some interface circuitry
for driving low-impedance loads or non-digital inputs

## Speed related settings
```
diff_r = 3.90;                // diff ratio
tyre_dia = 576.0;             // tyre diameter in mm
vss_rev = 4.0;                // vss pulses per tailshaft revolution
```
If simulating a sensor directly on a wheel then `set diff_r = 1.0` and `vss_rev` to the number of pulses per wheel revolution

## RPM related settings
```
Min_RPM = 500                // minimum RPM required
Max_RPM = 8000               // maximum RPM required
Cylinders = 4                // number of cylinders
```
It is assumed 4 stroke, for 2 stroke double the number of cylinders
