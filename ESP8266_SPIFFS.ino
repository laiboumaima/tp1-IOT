#include <WebSocketsServer.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
float humidity, temperature;
const char* password = "Badra#1999";
const char* ssid = "Souhila";
const int wifi_channel=1;
const boolean wifi_hidden=false;

const int port_webserver= 80;
const int port_websocket= 81;

const int LED1=2;


const char LEDON[] = "LEDOn";
const char LEDOff[] = "LEDOff";

ESP8266WebServer server(80); //Server on port 80
WebSocketsServer webSocket = WebSocketsServer(81);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
      
        case WStype_DISCONNECTED:
            Serial.println("Client disconnnected !"+ num);
            break;
            
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                Serial.println("Websocket client connnected !");        
                // send message to client
                webSocket.sendTXT(num, "Connected");                
            }
            break;
            
        case WStype_TEXT:
          {
            if (strcmp(LEDON, (const char *)payload) == 0) {
              digitalWrite(LED1,1);
            }
            else if (strcmp(LEDOff, (const char *)payload) == 0) digitalWrite(LED1,0);
           
            // send message to client
             webSocket.sendTXT(num, payload);
             String data = "{\"Temperature\":\""+ String(temperature) +"\", \"Humidity\":\""+ String(humidity) +"\"}";
             webSocket.broadcastTXT(data);
             humidity +=1;
             temperature +=2;
          }
            break;
            
        case WStype_BIN:
            Serial.printf("[%u] get binary length: %u\n", num, length);
            hexdump(payload, length);
            // webSocket.sendBIN(num, payload, length);
            break;
    }

}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

void handleRoot(){
  server.sendHeader("Content-Security-Policy", "script-src 'unsafe-inline';");
  server.send(200,"text/html");
}

void setup() {
  pinMode(LED1, OUTPUT);
  
 
  
  Serial.begin(115200);
 WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

    
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

  delay(100);  

   if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  server.serveStatic("/css", SPIFFS, "/css");
  server.serveStatic("/js", SPIFFS, "/js");
  server.serveStatic("/", SPIFFS, "/index.html");
  
 

 
  Serial.println("Starting file System");
   server.begin();                  //Start server
  Serial.println("HTTP server started");
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
Serial.println("Websocket server started");
  //server.serveStatic("/", SPIFFS, "/index.html","Content-Security-Policy script-src;" );
  server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"  

}

void loop() {
  
  webSocket.loop();
  server.handleClient();
}
