// Incluimos las librerias necesarias para trabajar con el Ethernet Shield
// Si estas usando el Wifi Shield debes incluir <WiFi.h> en lugar de <Ethernet.h>
// Mas información en http://arduino.cc/en/Guide/ArduinoWiFiShield
#include <Ethernet.h>
#include <SPI.h>

#define LED_RED 7 // Definimos el pin del LED rojo
#define LED_GREEN 8 // Definimos el pin del LED verde
#define TEMP_SENSOR 1 // Definimos el pin analogo del sensor
#define RINGING 100
#define INUSE 101
#define NOTINUSE 102


// Definimos la MAC Address de nuestro Ethernet Shield
byte mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Definimos la Dirección IP del Arduino
byte ip[] = {0, 0, 0, 0};

// Definimos la Dirección IP del Servidor Node.js
byte serverNode[] = {0, 0, 0, 0};

//Se crea un server escuchando en el puerto 80
EthernetServer server(80);  

// Instanciamos el cliente Ethernet con el que nos conectaremos al servidor interno
EthernetClient client;

//Variable para almacenar los request HTTP
String http_req;

// Instanciamos el cliente Ethernet con el que nos conectaremos al servidor externo
EthernetClient clientSender;
// Definimos la variable donde almacenaremos la respuesta del servidor.
String response = String(10);
// Definimos el separador del mensaje
char separator = '|';
// Utilizaremos esta variable para verificar si la lectura de la respuesta ha comenzado
boolean started = false;

boolean alarmInProgress = false;

boolean alarmSent = false;



void setup()
{
  // Iniciamos el Ethernet Shield
  Ethernet.begin(mac, ip); 
  // Utilizaremos el puerto Serial como debug
  Serial.begin(9600); 
  // Definimos el pin del LED rojo como salida
  pinMode(LED_RED, OUTPUT); 
}

void loop()
{
  int voltage = analogRead(TEMP_SENSOR);
  
  
  send_alert_if_exists(voltage);
  
  //Se busca si hay algun cliente conectado
  EthernetClient client = server.available();
  
  //Si hay un cliente
  if (client)
  {  // got client?
    boolean currentLineIsBlank = true;
    while (client.connected())
    {
      // client data available to read
      if (client.available())
      {
        // read 1 byte (character) from client
        char c = client.read();
        // save the HTTP request 1 char at a time 
        http_req += c;
        // last line of client request is blank and ends with \n
        // respond to client only after last line received
        if (c == '\n' && currentLineIsBlank)
        {
          //ProcessCheckbox(client);
          
          if (http_req.indexOf("led=inuse") > -1)
          {
            notification(INUSE);
          }
          
          if (http_req.indexOf("led=notinuse") > -1)
          {
            notification(NOTINUSE);
          }
          
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();
          
          // send web page
          client.println("<!DOCTYPE html>");
          client.println("<html>");
          client.println("<head>");
          client.println("<title>Arduino Call Notifier</title>");
          client.println("</head>");
          client.println("<body>");
          client.println("<h1>HI</h1>");
          client.println("</body>");
          client.println("</html>");
          
          Serial.print(http_req);
          
          // finished with request, empty string
          http_req = "";    
          break;
        }
        
        // every line of text received from the client ends with \r\n
        if (c == '\n')
        {
          // last character on line of received text
          // starting new line with next character read
          currentLineIsBlank = true;
        }
        else if (c != '\r')
        {
          // a text character was received from client
          currentLineIsBlank = false;
        }
      } // end if (client.available())
    } // end while (client.connected())
    
    // give the web browser time to receive the data
    delay(1);
    
    // close the connection
    client.stop();
    
    // end if (client)
  } 
  
}

void notification(int device_state)
{
  switch(device_state)
  {
    case RINGING:
      digitalWrite(2, HIGH);
    break;
    
    case INUSE:
      digitalWrite(LED_RED, HIGH);
    break;
    
    case NOTINUSE:
      digitalWrite(LED_RED, LOW);
    break;
    
    default:
      digitalWrite(LED_RED, LOW);
    break;
  }
}

void send_alert_if_exists(int voltage)
{
  if(voltage < 1020)
  {
    alarmInProgress = true;
  }
  else
  {
    alarmInProgress = false;
    alarmSent = false;
  }
  
    if(alarmInProgress == true && alarmSent == false)
    {
      alarmSent = true;
      Serial.print("Vol:");
      Serial.println(voltage);
      
      if (clientSender.connect(serverNode, 8080))
      {
        //alarmInProgress = true;
        Serial.println("Send get");
        // Enviamos la peticion GET utilizando el protocolo HTTP

        clientSender.println("GET /?status=alert HTTP/1.0");
        clientSender.println();
        clientSender.stop();
        delay(10);
      }
    }
  
}
