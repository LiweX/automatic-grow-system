#include <SoftwareSerial.h>
#include <ThreeWire.h>  
#include <RtcDS1302.h>

SoftwareSerial ESP01(10, 11); 	// pin 10 como RX, pin 11 como TX
ThreeWire myWire(4,5,2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

int encendido_hora;
int encendido_min;
int apagado_hora;
int apagado_min;
int encender;
bool encendido;
String hora;
String minuto;

void setup (){
    pinMode(12,OUTPUT);
    WIFI_init();
    RTC_init();
}

void loop(){
    set_time();
    check_time();
}

void WIFI_init(){
  Serial.begin(9600);		// inicializa monitor serie a 9600 bps
  ESP01.begin(9600);		// inicializa ESP01 a 9600 bps
  ESP01.println("AT+CWMODE=1");		// establece modo cliente (station)
  delay(1000);
  ESP01.println("AT+CWJAP=\"306-2.4\",\"ComplejosConjugados\"");		// unirse a la red local
  delay(5000);
  ESP01.println("AT+CIPMUX=1");		// establece multiples conexiones en simultaneo
  delay(1000);
  ESP01.println("AT+CIPSERVER=1,80");	// inicia servidor web en puerto 80
  delay(1000);
  
}

void set_time(){
  //if(Serial.available()) ESP01.println(Serial.readString());
  if(ESP01.available()){			// si hay datos disponibles desde el modulo
    delay(100);
    //Serial.print(ESP01.readString());//leer laa respuesta del modulo esp
    if(ESP01.find("+IPD,")){			// busca por cadena +IPD,
      int conexion = ESP01.read()-48;		// lee el byte, resta 48 y almacena en variable conexion
      String pagina = "<!doctype html><html><head></head><body>";// crea cadena pagina con etiquetas HTML
      if(ESP01.find("encender=")){
        encender =  ESP01.read()-48;
      }
      if(ESP01.find("hora=")){			
        hora = ESP01.readStringUntil('x');
        pagina += "<h1>Hora = " + hora + "</h1>";	// agrega a la cadena pagina el texto y HTML
        if(ESP01.find("min=")){
        minuto = ESP01.readStringUntil('x');
        pagina += "<h1>Min = " + minuto + "</h1></body></html>";	// agrega a la cadena pagina el texto y HTML
        }else{
          pagina += "<h1>ERROR</h1></body></html>";
        }
        if(encender>0){
          encendido_hora=hora.toInt();
          encendido_min=minuto.toInt();
        }else{
          apagado_hora=hora.toInt();
          apagado_min=minuto.toInt();
        }
        String enviar = "AT+CIPSEND=";		// crea cadena enviar con comando AT para enviar datos
        enviar+=conexion;			// agrega a la cadena el numero de conexion
        enviar+=",";				// agrega a la cadena una coma
        enviar+=pagina.length();		// agrega a la cadena la longitud de la cadena pagina
        ESP01.println(enviar);			// envia cadena enviar por conexion serie al modulo
        delay(100);
        ESP01.println(pagina);			// envia cadena pagina por conexion serie al modulo
        delay(100); 
        String cerrar = "AT+CIPCLOSE=";	// crea cadena cerrar con comando AT para cerrar conexion
        cerrar+=conexion;			// agrega a cadena cerrar el numero de conexion
        ESP01.println(cerrar);			// envia la cadena cerrar por conexion serie al modulo
        delay(300);
      }
    }
  }
}

// CONNECTIONS:
// DS1302 CLK/SCLK --> 5
// DS1302 DAT/IO --> 4
// DS1302 RST/CE --> 2
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND

void RTC_init () 
{
    Serial.begin(9600);

    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);

    Rtc.Begin();

    encendido = false;

    encendido_hora=0;
    encendido_min=0;
    apagado_hora=0;
    apagado_min=0;

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    printDateTime(compiled);
    Serial.println();

    if (!Rtc.IsDateTimeValid()) 
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }

    if (Rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled) 
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }
}

void check_time () 
{
    RtcDateTime now = Rtc.GetDateTime();
    //printDateTime(now);
    //Serial.println();

    if (!now.IsValid())
    {
        // Common Causes:
        //    1) the battery on the device is low or even missing and the power line was disconnected
        Serial.println("RTC lost confidence in the DateTime!");
    }

    if (now.Hour() == encendido_hora && now.Minute() == encendido_min && !encendido)
    {
      Serial.println("PRENDO CUESTION");
      encendido=true;
      digitalWrite(12,HIGH); //prendo foco
    }
    
    if (now.Hour() == apagado_hora && now.Minute() == apagado_min && encendido)
    {
      Serial.println("APAGO CUESTION");
      encendido=false;
      digitalWrite(12,LOW); //prendo foco
    }

    delay(1000);
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}

