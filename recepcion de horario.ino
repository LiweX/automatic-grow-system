#include <SoftwareSerial.h>	// libreria que permite establecer pines digitales
				// para comunicacion serie

SoftwareSerial D1(10, 11); 	// pin 10 como RX, pin 11 como TX

int encendido_hora;
int encendido_min;
int apagado_hora;
int apagado_min;
int encender;
String hora;
String min;
void setup(){
  Serial.begin(9600);		// inicializa monitor serie a 9600 bps
  D1.begin(9600);		// inicializa D1 a 9600 bps
  D1.println("AT+CWMODE=1");		// establece modo cliente (station)
  delay(200);
  D1.println("AT+CIPMUX=1");		// establece multiples conexiones en simultaneo
  delay(200);
  D1.println("AT+CIPSERVER=1,80");	// inicia servidor web en puerto 80
  delay(200);
  
}

void loop()
{
  checkTime();
}

void checkTime(){
if(D1.available()){			// si hay datos disponibles desde el modulo
  delay(100);
 if(D1.find("+IPD,")){			// busca por cadena +IPD,
  int conexion = D1.read()-48;		// lee el byte, resta 48 y almacena en variable conexion
  String pagina = "<!doctype html><html><head></head><body>";// crea cadena pagina con etiquetas HTML
  if(D1.find("encender=")){
    encender =  D1.read()-48;
  }
  if(D1.find("hora=")){			// busca por cadena led=
  hora = D1.readStringUntil('x');
  pagina += "<h1>Hora = "+hora+"</h1>";	// agrega a la cadena pagina el texto y HTML
  if(D1.find("min=")){
  min = D1.readStringUntil('x');
  pagina += "<h1>Min = "+min+"</h1></body></html>";	// agrega a la cadena pagina el texto y HTML
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
  D1.println(enviar);			// envia cadena enviar por conexion serie al modulo
  delay(100);
  D1.println(pagina);			// envia cadena pagina por conexion serie al modulo
  delay(100); 
  String cerrar = "AT+CIPCLOSE=";	// crea cadena cerrar con comando AT para cerrar conexion
  cerrar+=conexion;			// agrega a cadena cerrar el numero de conexion
  D1.println(cerrar);			// envia la cadena cerrar por conexion serie al modulo
  delay(300);
 }
}
}
}
