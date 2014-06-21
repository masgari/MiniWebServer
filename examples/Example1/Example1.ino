// -*- c++ -*-
//
// Copyright 2010 Ovidiu Predescu <ovidiu@gmail.com>
// Date: June 2010
// Updated: 08-JAN-2012 for Arduno IDE 1.0 by <Hardcore@hardcoreforensics.com>
//

#include <pins_arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Flash.h>
#include <MiniWebServer.h>

/****************VALUES YOU CHANGE*************/
// pin 10 is the SPI select pin for the Ethernet
const int ETHER_CS = 10;

//pin connected to RSTn on WIZ550io
const byte WIZ_RST = 8;

// Don't forget to modify the IP to an available one on your home network
byte ip[] = { 192, 168, 2, 73 };
/*********************************************/

static uint8_t mac[] = {
  0x00, 0x0B, 0xDC, 0x1D, 0x27, 0x90
};

boolean index_handler(MiniWebServer& web_server);

MiniWebServer::PathHandler handlers[] = {
  {"/", MiniWebServer::GET, &index_handler },
  {NULL},
};

boolean index_handler(MiniWebServer& web_server) {
  web_server.send_error_code(200);
  web_server.send_content_type("text/html");
  web_server.end_headers();
  web_server << F("<html><body><h1>Hello World!</h1></body></html>\n");
  //web_server.print("<html><body><h1>Hello World!</h1></body></html>\n");
  return true;
}

boolean has_ip_address = false;
MiniWebServer web = MiniWebServer(handlers, NULL);

const char* ip_to_str(const uint8_t* ipAddr)
{
  static char buf[16];
  sprintf(buf, "%d.%d.%d.%d\0", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
  return buf;
}

void setup() {
  Serial.begin(57600);
  // Ensure we are in a consistent state after power-up or a reset
  // button These pins are standard for the Arduino w5100 Rev 3
  // ethernet board They may need to be re-jigged for different boards
  pinMode(ETHER_CS, OUTPUT);	// Set the CS pin as an output
  digitalWrite(ETHER_CS, HIGH);	// Turn off the W5100 chip! (wait for

  // restarting wiznet
  pinMode(WIZ_RST, OUTPUT);
  //initialise RSTn
  digitalWrite(WIZ_RST, LOW);
  delay(1);
  digitalWrite(WIZ_RST, HIGH);
  delay(500);

  // Initialize the Ethernet.
  Serial << F("Setting up the Ethernet card...\n");
  Ethernet.begin(mac, ip);

  // Start the web server.
  Serial << F("Web server starting...\n");
  web.begin();

  Serial << F("Server IP: ");
  Serial.println(Ethernet.localIP());

  Serial << F("Ready to accept HTTP requests.\n\n");
}

void loop() {
  web.process();
}