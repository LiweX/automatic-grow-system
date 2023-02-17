#include <SoftwareSerial.h>	// libreria que permite establecer pines digitales
				// para comunicacion serie

SoftwareSerial ESP01(10, 11); 	// pin 10 como RX, pin 11 como TX

int encendido_hora;
int encendido_min;
int apagado_hora;
int apagado_min;
int encender;
String hora;
String min;

void setup(){
  Serial.begin(9600);		// inicializa monitor serie a 9600 bps
  ESP01.begin(9600);		// inicializa ESP01 a 9600 bps
  ESP01.println("AT+CWMODE=1");		// establece modo cliente (station)
  delay(200);
  ESP01.println("AT+CIPMUX=1");		// establece multiples conexiones en simultaneo
  delay(200);
  ESP01.println("AT+CIPSERVER=1,80");	// inicia servidor web en puerto 80
  delay(200);
  
}

void loop()
{
  checkTime();
}

void checkTime(){
  if(ESP01.available()){			// si hay datos disponibles desde el modulo
    delay(100);
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
        min = ESP01.readStringUntil('x');
        pagina += "<h1>Min = " + min + "</h1></body></html>";	// agrega a la cadena pagina el texto y HTML
        }else{
          pagina += "<h1>ERROR</h1></body></html>";
        }
        if(encender>0){
          encendido_hora=hora.toInt();
          encendido_min=min.toInt();
        }else{
          apagado_hora=hora.toInt();
          apagado_min=min.toInt();
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
