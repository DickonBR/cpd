
#include <ESP8266WiFi.h>
#include "DHT.h"
#include <BlynkSimpleEsp8266.h>
#define BLYNK_PRINT Serial // Comment this out to disable prints and save space
char auth[] = "5da47c84b5b34349b16ccaca4bbf563a";

#define DHTTYPE DHT11  

const char* ssid = "iot";
const char* password = "@lego1234";

WiFiServer server(80);

const int DHTPin = 4;
DHT dht(DHTPin, DHTTYPE);

static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];

void setup() {
 
  Serial.begin(115200);
  delay(10);
  Blynk.begin(auth, ssid, password);

  dht.begin();
  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  server.begin();
  Serial.println("Web server running. Waiting for the ESP IP...");
  delay(10000);
  
  Serial.println(WiFi.localIP());
}

void loop() {

  Blynk.run();
  Blynk.virtualWrite(10, celsiusTemp); // pino virtual  V10
  Blynk.virtualWrite(11, humidityTemp); // pino virtual  V11
 
 WiFiClient client = server.available();
 
  if (client) {
    Serial.println("New client");

    boolean blank_line = true;
    while (client.connected()) 

    
    
    {
      if (client.available()) {
        char c = client.read();
        
        if (c == '\n' && blank_line) {

            float h = dht.readHumidity();
            float t = dht.readTemperature();

            if (isnan(h) || isnan(t) ) {
              Serial.println("Failed to read from DHT sensor!");
              strcpy(celsiusTemp,"Failed");
              strcpy(humidityTemp, "Failed");         
            }
            else{
              
              float hic = dht.computeHeatIndex(t, h, false);       
              dtostrf(hic, 6, 2, celsiusTemp);                     
              dtostrf(h, 6, 2, humidityTemp);
              
            }
            
            client.println(celsiusTemp);
            client.println(humidityTemp);

            break;
        }
        if (c == '\n') {
          // when starts reading a new line
          blank_line = true;
        }
        else if (c != '\r') {
          // when finds a character on the current line
          blank_line = false;
        }
      }
    }  
    
    // closing the client connection
    
    delay(1);
    client.stop();
    Serial.println("Client disconnected.");
  }
}  


