//==========================
// Speed and RPM Generator
//==========================
// Outputs speed and RPM as square wave frequency
//
// Arduino Pro Mini
//
// SH1106 OLED display
//
// Using microseconds for timing
// because milliseconds did not
// have enough resolution
//
// Using a hardware timer could not
// output different frequencies on
// different pins
//
// In calculations the period is divided by 2 because
// we need to complete a full cycle of the
// output pins to count as the correct
// frequency, not just one change from low to high
// or high to low


//RPM Speed tester
#define Version "V1.6"


// OLED stuff here
// I2C and text only
// Jaycar XC3728 OLED set to I2C
// I2C device found at address 0x3C !
// 16 chars by 8 lines
// Coordinates 0,0 to 15,7 for x,y
#include <U8x8lib.h>
#include <Wire.h>
U8X8_SH1106_128X64_NONAME_HW_I2C u8x8(/* reset=*/U8X8_PIN_NONE);



//========================== Set These Manually ==========================

// ************ Speed related settings ************
float diff_r   = 3.70;     // diff ratio
float tyre_dia = 634.0;    // tyre diameter in mm
float vss_rev  = 4.0;      // vss pulses per tailshaft revolution

// If using a sensor directly on a wheel
// set diff_r = 1.0 and vss_rev to the
// number of pulses per wheel revolution


// ************ RPM related settings ************
// Set this to the minimum and maximum RPM required
// and number of Cylinders
const int Min_RPM   = 500;
const int Max_RPM   = 9000;
const int Cylinders = 4;

//========================================================================



//========================================================================

// Set to true for serial output for debugging
// normally leave this false, for no serial output
bool Debug_Mode = false;

// microseconds to output and display each RPM and Speed
// value for this period of time
uint32_t Wait_Time = 1000000;

// Kludge Fatcor to improve timing based on actual frequency measurements
// to allow for different crystals or somthing similar
float Kludge_Factor = 0.9996;

//========================================================================



// Pin definitions
// for outputs
#define RPM_Pin   9
#define Speed_Pin 10

// RPM variables
int      RPM_loop_counter;
float    RPM_Hz;
uint32_t RPM_period, RPM_start_time;
int      RPM = Min_RPM - 100;    // subtracting 100 because 100 gets added in the first loop

// Speed variables
int      Speed_loop_counter;
float    Speed_Hz, vss, distance_per_VSS_pulse, pulses_per_km;
uint32_t Speed_period, Speed_start_time;
int      vspeed = 0;    // starting at 0 because 5 gets added in the first loop

// Used for timing in the main loop
uint32_t Last_loop_time;



// ############################################################################################################



void setup()
    {

    // Initialize the output pins
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    pinMode(Speed_Pin, OUTPUT);
    digitalWrite(Speed_Pin, HIGH);
    pinMode(RPM_Pin, OUTPUT);
    digitalWrite(RPM_Pin, HIGH);

    if (Debug_Mode)
        {
        Serial.begin(115200);
        Serial.println("\nSpeedo and Tacho Tester");
        // double the Wait_Time of steps for Debug_Mode and readability
        Wait_Time = Wait_Time * 2;
        }


    // =======================================================
    // Calculate the distance travelled per VSS pulse
    // based on tyre size, diff ratio and VSS pulses per tailshaft revolution, in millimeters
    // This will be used to calc the frequency and period

    distance_per_VSS_pulse = tyre_dia * PI / diff_r / vss_rev;    // millimeters
    pulses_per_km          = 1000000.0 / distance_per_VSS_pulse;

    // =======================================================


    // Start the OLED display
    u8x8.begin();
    u8x8.setPowerSave(0);
    u8x8.setFlipMode(1);

    // set a small font for the static text
    u8x8.setFont(u8x8_font_chroma48medium8_r);

    // display startup text
    u8x8.drawString(3, 2, "Speedo and");
    u8x8.drawString(2, 4, "Tacho tester");
    u8x8.drawString(6, 6, Version);

    delay(2000);

    // display static text
    u8x8.clear();
    u8x8.drawString(11, 2, "km/hr");
    u8x8.drawString(11, 7, "RPM");

    // now set a bigger font for printing the digits
    u8x8.setFont(u8x8_font_px437wyse700b_2x2_r);


    // set current micros
    Last_loop_time = micros();


    }    // End void setup



// ############################################################################################################



void loop()
    {


    // Every "Wait_Time" set new RPM and Speed values
    // =======================================================
    if (micros() > Last_loop_time + Wait_Time)
        {
        Last_loop_time = micros();

        // toggle the LED pin to indicate activity
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));


        // Calc new Speed value

        if (vspeed < 60)
            vspeed = vspeed + 5;
        else
            vspeed = vspeed + 10;
        if (vspeed > 200)
            vspeed = 5;

        Speed_Hz     = pulses_per_km * (float)vspeed / 3600.0 * Kludge_Factor;    //pulses per km per second * speed
        Speed_period = 1000000 / Speed_Hz / 2;

        if (Debug_Mode)
            {
            Serial.println("Speed");
            Serial.print(vspeed);
            Serial.println(" km/hr");
            Serial.print(Speed_Hz);
            Serial.println(" Hz");
            Serial.print(Speed_period);
            Serial.println(" us");
            }


        // Display the new speed value on the OLED

        u8x8.setCursor(1, 1);
        u8x8.print(vspeed);
        u8x8.print("  ");


        // Calc new RPM value

        if (RPM <= Max_RPM)
            {
            if (RPM < 3000)
                RPM = RPM + 100;
            else
                RPM = RPM + 200;
            }
        if (RPM > Max_RPM)
            RPM = Min_RPM;

        RPM_Hz     = (float)RPM * (float)Cylinders / 120.0 * Kludge_Factor;
        RPM_period = int(1000000.0 / RPM_Hz / 2.0);

        if (Debug_Mode)
            {
            Serial.println("RPM");
            Serial.print(RPM);
            Serial.println(" RPM");
            Serial.print(RPM_Hz);
            Serial.println(" Hz");
            Serial.print(RPM_period);
            Serial.println(" us");
            Serial.println(" ");
            }


        // Display the new RPM Value on the OLED

        u8x8.setCursor(1, 6);
        u8x8.print(RPM);
        if (RPM < 1000)
            u8x8.print(" ");


        // Reset the counters and timers
        // for the next set of outputs

        RPM_loop_counter   = 1;
        Speed_loop_counter = 1;
        RPM_start_time     = micros();
        Speed_start_time   = micros();
        }
    // End if for the "once every Wait_Time" section
    // =======================================================


    // Toggle the Speed output pin

    if (micros() > Speed_start_time + (Speed_period * Speed_loop_counter))
        {
        digitalWrite(Speed_Pin, !digitalRead(Speed_Pin));
        Speed_loop_counter++;
        }


    // Toggle the RPM output pin

    if (micros() > RPM_start_time + (RPM_period * RPM_loop_counter))
        {
        digitalWrite(RPM_Pin, !digitalRead(RPM_Pin));
        RPM_loop_counter++;
        }


    }    // End void loop



// ############################################################################################################