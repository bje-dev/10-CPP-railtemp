#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char* ssid = "SSID";//nombre del wi-fi
const char* password = "password";//contraseña de wi-fi
const char* host = "172.17.33.90";//poner el Ip del servidor

IPAddress staticIP(172, 17, 35, 100); // Dirección IP fija deseada
IPAddress gateway(172, 17, 35, 1);    // Dirección IP de tu router
IPAddress subnet(255, 255, 0, 0);   // Máscara de subred


/*** Variables para Humedad y Temperatura ****/
float temperatura = 0;
float progresiva = 20.15;
String url;

#define DS18B20 5 //DS18B20 esta conectado al pin GPIO D5 del NodeMCU


WiFiClient client;
AsyncWebServer server(80);
OneWire ourWire(DS18B20);// Se declara un objeto para la libreria
DallasTemperature sensor(&ourWire);// Se declara un objeto para la otra libreria

void setup()
{
Serial.begin(9600);

sensor.begin();

server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Dispositivo sensor: Boulogne");
  });

server.on("/api", HTTP_GET, [](AsyncWebServerRequest *request){
    char bufer[50];
    sprintf(bufer, "{\"t\":%.2f,\"p\":%.2f}", temperatura, progresiva);
    request->send(200, "application/json", bufer);
  });

server.begin();

Serial.println();
Serial.printf("Connecting to %s ", ssid);

WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED)
{
delay(500);
Serial.print(".");
}
Serial.println("connected.");
WiFi.config(staticIP, gateway, subnet);
Serial.println("Static IP address configured:");
Serial.println(WiFi.localIP());
}


void loop()
{

Serial.printf("\n[Connecting to %s ... ", host);
if (client.connect(host, 80))
{
Serial.println("connected]");
sensor.requestTemperatures();    // Le pide el valor de temperatura al sensor
float nuevaTemperatura =sensor.getTempCByIndex(0);// Lee el valor de temperatura del sensor
temperatura = nuevaTemperatura;
Serial.println(temperatura);
url = "/railtemp/registrar.php?temperatura=" + String(temperatura) + "&progresiva=" + String(progresiva);

Serial.println("[Sending a request]");
delay(1000);  
client.print(String("GET /") + url +" HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n" +
                 "\r\n"
                );
  
Serial.println("[Response:]");
while (client.connected())
{
if (client.available())
{
 String line = client.readStringUntil('\n');
        Serial.println(line);
}
}
client.stop();
Serial.println("\n[Disconnected]");
}
else
{
Serial.println("connection failed!]");
client.stop();
}
delay(59000);
}
