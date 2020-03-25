/********************************************************************************************************************************************************
Made by Akhilesh Chandorkar
Board: ESP32 DOIT DEVKIT V1
Screen:###
All the Serial.println in comments were for debugging. Left them in if you have to change code.
This work is lisc....### 
 *******************************************************************************************************************************************************/


#include "ArduinoJson.h"
#include <Adafruit_GFX.h>    
#include <SPI.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <TimeLib.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

//Replace -1 with no. of your reset button (-1 if you don't have a button on your OLED)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//Replace SSID with your SSID(Name of Network) and password with your password
const char *ssid = "vRuchakhiLand_EXT";
const char *password = "Qs7ssbtv7sth";
const char *server = "corona.lmao.ninja";
//HTTPClient http;
StaticJsonDocument<900> parsed; //Memory pool
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
WiFiClientSecure client;

const char *root_ca =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n"
    "RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n"
    "VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n"
    "DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n"
    "ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n"
    "VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n"
    "mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n"
    "IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n"
    "mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n"
    "XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n"
    "dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n"
    "jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n"
    "BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n"
    "DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n"
    "9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n"
    "jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n"
    "Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n"
    "ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n"
    "R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n"
    "-----END CERTIFICATE-----\n";

void setup()
{
  Serial.begin(115200);
  btStop(); // turn off bluetooth

  //If this doesn't work, use the program to find out the port of your OLED and replace 0x3C
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000); // Pause for 2 seconds
 
  // Clear the buffer.
  display.clearDisplay();

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.begin(ssid, password);
  delay(100);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 15)
  {
    delay(500);
    Serial.print("Connecting to WiFi.. attempt: ");
    Serial.println(attempts);
    attempts = attempts + 1;
  }
  if ((WiFi.status() == WL_CONNECTED))
  {
    Serial.println("Connected to the WiFi network");
    timeClient.begin();
    // timeClient.setTimeOffset(3600); // uncomment for timezone fix, 3600 is GMT+1
    timeClient.update();

    // Start server connection
    client.setCACert(root_ca);
    Serial.println("\nStarting secure connection to server...");
    if (!client.connect(server, 443))
    {
      Serial.println("Connection failed!");
    }
    else
    {
      Serial.println("Connected to server!");
    }
  }
}

void loop()
{
  if ((WiFi.status() == WL_CONNECTED))
  { //Check the current connection status
    // Send the request to API
    // Update the timeclient
    timeClient.update(); //
    client.flush();
    if (client.connected())
    {
      client.println("GET https://corona.lmao.ninja/all HTTP/1.0");
      client.println("Host: corona.lmao.ninja");
      // Keep the connection alive so we dont have to keep opening a new one
      // which can take a couple seconds on an ESP32
      client.println("Connection: Keep-Alive");
      client.println();
      Serial.println("Sent request");

      String res = client.readString();
      Serial.println("res : ");
      Serial.println(res);
      // Gets the start of the payload string by searching for the start of the json object
      // This is not the best way to get the payload string
      String payload = res.substring(res.indexOf("{"), res.length());
      client.flush();
      Serial.println("Payload:");
      Serial.println(payload);

      // Deserialize the response
      auto error = deserializeJson(parsed, payload);
      //Serial.println("error done");
      
      if (error)
      { //Check for errors in parsing
        Serial.println("Parsing failed");
        delay(5000);
        return;
      }

      //Get the time in UCT form unedited from server
      String timeUpd = payload.substring(payload.lastIndexOf(":") + 1, payload.lastIndexOf("}")-3);
      int timeUpdated = timeUpd.toInt();
      //Was old method, which was unreliable as if figurse went up, you would have to change code:
      //int timeUpdated2 = payload.substring(60, 70).toInt();
      Serial.println("TimeUPD = ");
      Serial.println(timeUpd);
      Serial.println("TimeUpdated = ");
      Serial.println(timeUpdated);
      //Serial.println("TimeUpdated2 = ");
      //Serial.println(timeUpdated2);
      // Update the display
      updateDisplay(parsed["cases"], parsed["deaths"], parsed["recovered"], timeUpdated);

      delay(300000); // wait 2 minutes to refresh
    }
  }
  else
  {
    displayConnectionError();
  }
}

void updateDisplay(int cases, int deaths, int recoveries, int updated)
{
  // Clear the display
  display.clearDisplay();

  // Draw top line
  display.setTextColor(WHITE);
  display.drawLine(display.getCursorX(), display.getCursorY(), display.getCursorX() + display.width(), display.getCursorY(), WHITE);
  //For bigger screens, change add following code for bigger line
  //display.drawLine(display.getCursorX(), display.getCursorY() + 1, display.getCursorX() + display.width(), display.getCursorY() + 1, WHITE);
  display.display();

  // Draw Headline
  //For bigger screens, change change textSize to 2 for bigger font size
  display.setTextSize(1);
  display.setCursor(0, display.getCursorY() + 2);
  display.println("Covid-19");
  display.display();

  // Draw bottom line
  display.drawLine(display.getCursorX(), display.getCursorY() + 1, display.getCursorX() + display.width(), display.getCursorY() + 1, WHITE);
  //For bigger screens, change add following code for bigger line
  //display.drawLine(display.getCursorX(), display.getCursorY() + 2, display.getCursorX() + display.width(), display.getCursorY() + 2, WHITE);
  display.display();

  // Draw confirmed cases
  display.setCursor(0, display.getCursorY() + 3);
  display.setTextColor(WHITE);
  display.print("Confirmed:");
  display.println(cases);
  display.display();
  //Serial.println("Cases = ");
  //Serial.println(cases);

  // Draw recoveries
  display.setCursor(0, display.getCursorY() + 2);
  display.setTextColor(WHITE);
  display.print("Recovered:");
  display.println(recoveries);
  display.display();
  //Serial.println("Recoveries = ");
  //Serial.println(recoveries);

  // Draw deaths
  display.setCursor(0, display.getCursorY() + 2);
  display.setTextColor(WHITE);
  display.print("Deaths:");
  display.println(deaths);
  display.display();
  //Serial.println("Deaths = ");
  //Serial.println(deaths);

  // Draw last update time and checked time
  // Update time: last time new data was published to the api
  // Checked time: last time the esp32 checked for new data
  // N.B using String in general is not a good idea
  String time = EpochToTimeString(updated);
  //Serial.println("time = ");
  //Serial.println(time);
  display.setTextSize(1);
  display.setCursor(0, display.height() - 18);
  display.print("Updated: ");
  //Serial.println(time);
  display.println(time);
  display.print("Checked: ");
  display.println(timeClient.getFormattedTime());
  display.display();
  //Serial.println("time end");
}


// If the received time is 12:05:56
// It shows it like 12:5:56
String EpochToTimeString(int epoch)
{
  //Serial.println("inside EpochToTimeString");
  TimeElements te;
  breakTime(epoch, te);

  // N.B. Using String in general is not a good idea
  String h = String(te.Hour);
  String m = String(te.Minute);
  String s = String(te.Second);

  return h + ":" + m + ":" + s;
  //Serial.println("value returned");

}

void displayConnectionError()
{
  // Display connection error
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("Could not connect to WiFi network with SSID: ");
  display.println(ssid);
  display.display();
  delay(2000);
}
