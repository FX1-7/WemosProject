#include <StringSplitter.h>
#include <ArduinoJson.h>
#include <TM1638plus.h> //include the library
#include <Streaming.h>
#include <DS3231.h>
#include <Wire.h>
#include <String>
#include <stdio.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>



#define OLED_RESET -1
#define OLED_SCREEN_I2C_ADDRESS 0x3C
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define STROBE_TM D5 // strobe = GPIO connected to strobe line of module
#define CLOCK_TM D6 // clock = GPIO connected to clock line of module
#define DIO_TM D7 // data = GPIO connected to data line of module

DS3231 rtc;

bool high_freq = false; //default false, If using a high freq CPU > ~100 MHZ set to true.
bool h12Flag;
bool pmFlag;
byte Hour;
byte Minute;
byte Second;

// Constructor object (GPIO STB , GPIO CLOCK , GPIO DIO, use high freq MCU)
TM1638plus tm(STROBE_TM, CLOCK_TM, DIO_TM, high_freq);
byte buttons;

void setDateAndTime() {
    rtc.setClockMode(false); // false = 24hr clock mode
    rtc.setYear(21);
    rtc.setMonth(12);
    rtc.setDate(8);
    rtc.setHour(15);
    rtc.setMinute(51);
    rtc.setSecond(00);
}


const char* ssid = "No."; // Set it to something unique
const char* password = "liqd7253"; // MUST be at least 8 chars

const char* host = "192.168.186.180"; // host that provides inspirational quotes.

void setup() {
    Serial.begin(115200); // Open up serial so we can check that button pushes are going through as expected.
    Wire.begin();

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());


    tm.displayBegin();
    tm.reset(); // Wipe the display so that nothing is showing
   // set up a pattern on the LEDs
    tm.setLEDs(0x0100); //The MSB of this 16 bit value controls which LEDS light up

     // -- OLED --------------
    display.begin(SSD1306_SWITCHCAPVCC, OLED_SCREEN_I2C_ADDRESS);
    display.display();
    delay(2000);
    display.clearDisplay();
    display.setTextSize(1); // - a line is 21 chars in this size
    display.setTextColor(WHITE);

}

void loop() {

    display.clearDisplay();
    delay(5000);
    Serial.println();
    Serial.print("Generating quote from: ");
    Serial.println(host);

    WiFiClient client; // Use WiFiClient class to create TCP connections
    const int httpPort = 4000;

    if (!client.connect(host, httpPort)) { // Attempt to connect to host
        Serial.println("connection failed");
        return; // No luck! Break out of
        // loop()... which will be
        // immediately called again
    }

    // OK - If we've reached here
    // then we're connected
    // We now create a URI for
    //the request
    String url = "/random";

    // This will send the request
    // to the web server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Content-Type: application/xml" + "\r\n" + "Connection: keep-alive\r\n\r\n");
    String line = client.readString();
    StringSplitter* splitter = new StringSplitter(line, '\"content\":', 20);
    String item = splitter->getItemAtIndex(13);
    item.replace(",\"author\"", "");
    item.replace("\"", "");
    for (int i = 0; i < item.length(); i += 20) {
        int m = i + 5;
        display.setCursor(0, 0);
        display << "test" << endl;

    }
    display.display();
    Serial.print(item);
    delay(5000);
    // loop and wait 5 seconds for
    // a reply from the server.
    client.stop();
    return; // Nothing heard back from
    // server - Give up and try
    // again.
    Serial.println();
    Serial.println("closing connection");
    //setDateAndTime();
    delay(5000);

    buttons = tm.readButtons(); // read which buttons are pressed
    Hour = rtc.getHour(h12Flag, pmFlag);
    Minute = rtc.getMinute();
    Second = rtc.getSecond();

    int H = Hour;
    int M = Minute;
    int S = Second;
    char buffer[9];
    sprintf(buffer, "%02d:%02d:%02d", H, M, S);

    tm.displayText(buffer);
    //tm.displayHex(0, H1);
    //tm.displayHex(1, H2);
    //tm.displayHex(2, '.');
    //tm.displayHex(3, M1);
    //tm.displayHex(4, M2);
    //tm.displayHex(5, '.');
    //tm.displayHex(6, S1);
    //tm.displayHex(7, S2);
    delay(1000);

    if (M == 30 || M == 00 && S == 00) {
        Serial << "u gay" << endl;
    }

    if (buttons != 0) {
        Serial << "The button pushed was: " << buttons << endl;
        Serial << "The time is as follows: " << endl;
        Serial.print(buffer);
        Serial << endl << rtc.getHour(h12Flag, pmFlag) << ":" << rtc.getMinute() << ":" << rtc.getSecond() << endl;
    }
}
