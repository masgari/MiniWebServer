Arduino MiniWebServer

A small web server for Arduino.

Check out this video demonstrating what you can do with it:

http://www.youtube.com/watch?v=qZXKk6nCYuM

You can read up on MiniWebServer's history here:

http://www.webweavertech.com/ovidiu/weblog/archives/000484.html

http://www.webweavertech.com/ovidiu/weblog/archives/000477.html

The library is licensed under the terms of LGPL 2.1. Which means
you're free to use it in your projects (including commercial ones) as
long as you're sending back the changes you make to the library.

External dependencies
====================

MiniWebServer depends on the external library Flash version 4.0, which
is found here:

http://arduiniana.org/libraries/flash/

Make sure you dowload the Flash library and install it in your
Arduino's `libraries` directory, as described in this document:

http://arduino.cc/en/Guide/Libraries

Basic web server
================

To make use of the MiniWebServer library, you need to include the
following your sketch:

    #include <Ethernet.h>
    #include <Flash.h>
    #include <MiniWebServer.h>

TWS is implemented by the MiniWebServer class. The constructor method
takes two arguments. The first one is a list of handlers, functions to
be invoked when a particular URL is requested by an HTTP client. The
second one is a list of HTTP header names that are needed by the
implementation of your handlers. More on these later.

An HTTP handler is a simple function that takes as argument a
reference to the MiniWebServer object. When you create the
MiniWebServer class, you need to pass in the handlers for the various
URLs. Here is a simple example of a web server with a single handler.

    static uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

    //pin connected to RSTn on WIZ550io
    const byte WIZ_RST = 8;

    boolean index_handler(MiniWebServer& web_server) {
      web_server.send_error_code(200);
      web_server << F("<html><body><h1>Hello World!</h1></body></html>\n");
      return true;
    }

    MiniWebServer::PathHandler handlers[] = {
      // Register the index_handler for GET requests on /
      {"/", MiniWebServer::GET, &index_handler },
      {NULL}, // The array has to be NULL terminated this way
    };

    // Create an instance of the web server. No HTTP headers are requested
    // by the HTTP request handlers.
    MiniWebServer web = MiniWebServer(handlers, NULL);

    void setup() {
      Serial.begin(115200);
      
      // restarting wiznet
      pinMode(WIZ_RST, OUTPUT);
      //initialise RSTn
      digitalWrite(WIZ_RST, LOW);
      delay(1);
      digitalWrite(WIZ_RST, HIGH);
      delay(500);
      
      EthernetDHCP.begin(mac);
      web.begin();
    }

    void loop() {
      EthernetDHCP.maintain();
      web.process();
    }

In the loop() function we need the call to the process() to make sure
HTTP requests are serviced. If there is no new request, the method
returns immediately. Otherwise the process() method blocks until the
request is handled.

For a complete working example look in
MiniWebServer/example/SimpleWebServer.

Advanced topic: persistent HTTP connections
===========================================

Sometimes it's useful to have an HTTP client start a request. For
example, I need to be able to enter an IR learning process. This means
that I cannot afford MiniWebServer's process() to block while serving
my /learn request that initiated the IR learning process. Instead I
want the handler of the /learn request to set a variable in the code
that indicates that IR learning is active, and then return
immediately.

If you noticed the HTTP handlers return a boolean. If the returned
value is true, as it was the case in our examples above, the
connection to the HTTP client is closed immediately. If the returned
value is false the connection is left open. Your handler should save
the Client object handling the HTTP connection with the original
request. Your code becomes responsible with closing it when it's no
longer needed.

To obtain the Client object, use the get_client() method while in the
HTTP handler. You can write asynchronously to the client, to update it
with the state of the web server.

In my remotely controlled projection screen application, I have
another handler on /cancel that closes the /learn client
forcibly. Otherwise the /learn's Client connection is closed at the
end of the IR learning procedure. Since the Ethernet shield only
allows for 4 maximum HTTP clients open at the same time (because of 4
maximum client sockets), in my application I allow only one /learn
handler to be active at any given time.

