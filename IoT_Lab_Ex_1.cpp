#include <Arduino.h>

// Timer related imports and variables
// pio lib install SimpleTimer
#include <SimpleTimer.h> 
SimpleTimer acq_timer;
SimpleTimer scr_timer;

// Temperature & humedity sensor related imports and variables
#include "HTS221Sensor.h"
DevI2C *i2c;
HTS221Sensor *sensor;
float humidity = 0;
float temperature = 0;
unsigned char id;

void screen_upd_func(){      
    Screen.print(0, "Variables");
    
    String disp = "Temp: " + String(temperature);
    Screen.print(1, disp.c_str());

    disp = "Humd: " + String(humidity);
    Screen.print(2, disp.c_str());    
      
}

void acquisition_func(){

    // Temperature and humedity adquisition
    // enable
    sensor -> enable();
    // read id
    sensor -> readId(&id);
    Serial.printf("ID: %d\r\n", id);
    // get humidity
    sensor -> getHumidity(&humidity);
    Serial.print("Humidity: ");
    Serial.println(humidity);
    // get temperature
    sensor -> getTemperature(&temperature);
    Serial.print("Temperature: ");
    Serial.println(temperature);
    // disable the sensor
    sensor -> disable();
    // reset
    sensor -> reset();    

}


void setup() {
    Screen.clean();
    Screen.print(0, "Init process ...");

    // Serial communication setup
    // Initialize serial and wait for port to open
    Serial.begin(115200);
    
    // Temperature & humedity sensor related setup
    i2c = new DevI2C(D14, D15);
    sensor = new HTS221Sensor(*i2c);
    // init the sensor
    sensor -> init(NULL);
    
    Screen.print(1, "Temp & Hume sensor");
    Screen.print(2, "started ...");
    delay(4000);
    
    
    Screen.print(0, "Init sucessfull");
    Screen.print(1, " ");
    Screen.print(2, " ");
    delay(4000);


    // Timers related setup
    acq_timer.setInterval(1000, acquisition_func);
    scr_timer.setInterval(1000, screen_upd_func);   
}


void loop() {
    acq_timer.run();
    scr_timer.run();      
}
