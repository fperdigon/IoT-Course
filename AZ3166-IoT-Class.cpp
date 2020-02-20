#include <Arduino.h>

// Timer related imports and variables
//pio lib install SimpleTimer
#include <SimpleTimer.h> 
SimpleTimer acq_timer;
SimpleTimer tb_timer;
SimpleTimer scr_timer;

// Temperature & humedity sensor related imports and variables
#include "HTS221Sensor.h"
DevI2C *i2c;
HTS221Sensor *sensor;
float humidity = 0;
float temperature = 0;
unsigned char id;


// Magnetic sensor related imports and variables
#include "LIS2MDLSensor.h"
static LIS2MDLSensor *lis2mdl;
// Data from magnetometer sensor
static int axes[3];
static int base_x;
static int base_y;
static int base_z;
// Indicate whether the magnetometer sensor has been initialized
static bool initialized = false;

// The open / close status of the door
static bool preOpened = false;
static bool door_flag = false;
int door_count = 0;


// Wifi connection related imports and variables
#include <AZ3166WiFi.h>
char ssid[] = "Chris_network";    // your network SSID (name)
char password[] = "wifi-poly"; // your network password
int status = WL_IDLE_STATUS;  // the Wifi radio's status

// ThingsBoard IoT platform related imports and variables
#include "http_client.h"
String server = "https://demo.thingsboard.io";
String access_token = "5bzRIpc2sta2p1agEjl2";

void ThingsBoard_Send(String server, String access_token, String key, String value)
{
/*
This function allow to send data to ThingsBoard servers
server: hhtp domine or IP f the server e.g. : "https://demo.thingsboard.io"
access_token: is the token string for the device
key: is the name of the variable e.g "teperature", "humedity", "presion"
velue: is the numerical value for the variable we are mesuring
*/

// This a valid SSL certificate to be able to make HTTPS request 
const char SSL_CA_PEM[] = "-----BEGIN CERTIFICATE-----\n"
                          "MIIEkjCCA3qgAwIBAgIQCgFBQgAAAVOFc2oLheynCDANBgkqhkiG9w0BAQsFADA/\n"
                          "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n"
                          "DkRTVCBSb290IENBIFgzMB4XDTE2MDMxNzE2NDA0NloXDTIxMDMxNzE2NDA0Nlow\n"
                          "SjELMAkGA1UEBhMCVVMxFjAUBgNVBAoTDUxldCdzIEVuY3J5cHQxIzAhBgNVBAMT\n"
                          "GkxldCdzIEVuY3J5cHQgQXV0aG9yaXR5IFgzMIIBIjANBgkqhkiG9w0BAQEFAAOC\n"
                          "AQ8AMIIBCgKCAQEAnNMM8FrlLke3cl03g7NoYzDq1zUmGSXhvb418XCSL7e4S0EF\n"
                          "q6meNQhY7LEqxGiHC6PjdeTm86dicbp5gWAf15Gan/PQeGdxyGkOlZHP/uaZ6WA8\n"
                          "SMx+yk13EiSdRxta67nsHjcAHJyse6cF6s5K671B5TaYucv9bTyWaN8jKkKQDIZ0\n"
                          "Z8h/pZq4UmEUEz9l6YKHy9v6Dlb2honzhT+Xhq+w3Brvaw2VFn3EK6BlspkENnWA\n"
                          "a6xK8xuQSXgvopZPKiAlKQTGdMDQMc2PMTiVFrqoM7hD8bEfwzB/onkxEz0tNvjj\n"
                          "/PIzark5McWvxI0NHWQWM6r6hCm21AvA2H3DkwIDAQABo4IBfTCCAXkwEgYDVR0T\n"
                          "AQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8EBAMCAYYwfwYIKwYBBQUHAQEEczBxMDIG\n"
                          "CCsGAQUFBzABhiZodHRwOi8vaXNyZy50cnVzdGlkLm9jc3AuaWRlbnRydXN0LmNv\n"
                          "bTA7BggrBgEFBQcwAoYvaHR0cDovL2FwcHMuaWRlbnRydXN0LmNvbS9yb290cy9k\n"
                          "c3Ryb290Y2F4My5wN2MwHwYDVR0jBBgwFoAUxKexpHsscfrb4UuQdf/EFWCFiRAw\n"
                          "VAYDVR0gBE0wSzAIBgZngQwBAgEwPwYLKwYBBAGC3xMBAQEwMDAuBggrBgEFBQcC\n"
                          "ARYiaHR0cDovL2Nwcy5yb290LXgxLmxldHNlbmNyeXB0Lm9yZzA8BgNVHR8ENTAz\n"
                          "MDGgL6AthitodHRwOi8vY3JsLmlkZW50cnVzdC5jb20vRFNUUk9PVENBWDNDUkwu\n"
                          "Y3JsMB0GA1UdDgQWBBSoSmpjBH3duubRObemRWXv86jsoTANBgkqhkiG9w0BAQsF\n"
                          "AAOCAQEA3TPXEfNjWDjdGBX7CVW+dla5cEilaUcne8IkCJLxWh9KEik3JHRRHGJo\n"
                          "uM2VcGfl96S8TihRzZvoroed6ti6WqEBmtzw3Wodatg+VyOeph4EYpr/1wXKtx8/\n"
                          "wApIvJSwtmVi4MFU5aMqrSDE6ea73Mj2tcMyo5jMd6jmeWUHK8so/joWUoHOUgwu\n"
                          "X4Po1QYz+3dszkDqMp4fklxBwXRsW10KXzPMTZ+sOPAveyxindmjkW8lGy+QsRlG\n"
                          "PfZ+G6Z6h7mjem0Y+iWlkYcV4PIWL1iwBi8saCbGS5jN2p8M+X+Q7UNKEkROb3N6\n"
                          "KOqkqm57TH2H3eDJAkSnh6/DNFu0Qg==\n"
                          "-----END CERTIFICATE-----\n";

  
  // Example line: "https://demo.thingsboard.io/api/v1/5bzRIpc2sta2p1agEjl2/telemetry"
  String connection_str = server + "/api/v1/" + access_token + "/telemetry";

  // Example line: "{\"temperature\": 7}"
  String json_str = "{\"" + key + "\": " + value + "}";
    
  HTTPClient *httpClient = new HTTPClient(SSL_CA_PEM, HTTP_POST, connection_str.c_str());
  
  httpClient->set_header("Content-Type", "application/json");
  const Http_Response* result = httpClient->send(json_str.c_str(), json_str.length());

  if (result == NULL)
  {    
    Serial.print("Error Code: ");
    Serial.println(httpClient->get_error());
  }
  else
  {    
    Serial.println("Body");
    Serial.println(result->body);
  }

  delete httpClient;
}
void screen_upd_func(){      
    Screen.print(0, "Variables");
    
    String disp = "Temp: " + String(temperature);
    Screen.print(1, disp.c_str());

    disp = "Humd: " + String(humidity);
    Screen.print(2, disp.c_str());

    if (preOpened == false)
      Screen.print(3, "Door closed"); 
    else
      Screen.print(3, "Door opened"); 
      
      
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


    // Magnetometer acquisition  
    lis2mdl->getMAxes(axes);  
    int delta = 500;
    bool curOpened = false;
    Serial.printf("Axes: x - %d, y - %d, z - %d\n", axes[0], axes[1], axes[2]);
    if (abs(base_x - axes[0]) < delta && abs(base_y - axes[1]) < delta && abs(base_z - axes[2]) < delta)
    {           
      curOpened = false;
    }
    else
    {      
      curOpened = true;         
    }
    

    // if the door was not opened before activate the door flag and increment the counter
    if (curOpened == true && preOpened == false){
      door_count = door_count + 1;
      if (door_flag == false)
        door_flag = true;
    }

    if (curOpened == true)
    {
      if (door_flag == false)
        door_flag = true;
    }

    // send message when status change
    if (curOpened != preOpened)
    {      
      preOpened = curOpened;  
      
    }

}

void tb_update_func(){
  ThingsBoard_Send(server, access_token, "temperature", String(temperature));
  ThingsBoard_Send(server, access_token, "humedity", String(humidity));
  ThingsBoard_Send(server, access_token, "door_count", String(door_count));  
  if (door_flag == true){
    ThingsBoard_Send(server, access_token, "door_status", String(50)); // To see the peak
    door_flag = false;
  }
  else{
    ThingsBoard_Send(server, access_token, "door_status", String(0));
  }

  
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


    // Magnetic sensor related setup
    lis2mdl = new LIS2MDLSensor(*i2c);
    lis2mdl->init(NULL);
  
    lis2mdl->getMAxes(axes);
    base_x = axes[0];
    base_y = axes[1];
    base_z = axes[2];

    int count = 0;
    int delta = 10;
    char buffer[20];
    while (true)
    {
      delay(1000);
      lis2mdl->getMAxes(axes);
      
      // Waiting for the data from sensor to become stable
      if (abs(base_x - axes[0]) < delta && abs(base_y - axes[1]) < delta && abs(base_z - axes[2]) < delta)
      {
        count++;
        if (count >= 5)
        {
          // Done          
          break;
        }
      }
      else
      {
        count = 0;
        base_x = axes[0];
        base_y = axes[1];
        base_z = axes[2];
      }    
    }

    Screen.print(1, "Magnetic sensor");
    Screen.print(2, "started ...");
    delay(4000);
     

    // Wifi connection related setup
    Screen.print(1, "Conecting Wifi ...");
    Screen.print(2, " ");
    
    if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield not present");
        // Don't continue:
        while (true);
    }
    
    const char* fv = WiFi.firmwareVersion();
    Serial.printf("Wi-Fi firmware: %s\r\n", fv);
    
    // attempt to connect to Wifi network:
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network:
        status = WiFi.begin(ssid, password);
        // Wait 10 seconds for connection:
        delay(10000);
    }
    
    Serial.print("You're connected to the network");

    Screen.print(1, "Wifi connected");
    Screen.print(2, " ");
    delay(4000);
    
    Screen.print(0, "Init sucessfull");
    Screen.print(1, " ");
    Screen.print(2, " ");
    delay(4000);


    // Timers related setup
    acq_timer.setInterval(1000, acquisition_func);
    scr_timer.setInterval(1000, screen_upd_func);
    tb_timer.setInterval(5000, tb_update_func);

}


void loop() {
    acq_timer.run();
    scr_timer.run();
    tb_timer.run();   
}
