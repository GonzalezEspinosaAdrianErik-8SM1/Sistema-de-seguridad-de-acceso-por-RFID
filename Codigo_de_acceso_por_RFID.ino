#include <SPI.h>                  // Se incluye esta librería para utilizar el protocolo de comunicación SPI.
#include <MFRC522.h>              // Se incluye esta librería para utilizar las funciones del lector RFID.
#include <Wire.h>                 // Se incluye esta librería para utiilizar el protocolo de comunicación I2C.
#include <LiquidCrystal_I2C.h>    // Se incluye la librería para utilizar las funciones de la LCD con comunicación I2C.
#include "Arduino.h"              // Se incluye esta librería para usar las funciones de la IDE de Arduino.
#include "SoftwareSerial.h"       // Esta librería permite comunicar al Arduino con el modulo MP3.
#include "DFRobotDFPlayerMini.h"  // Se icnluye esta librería para utilizar las funciones del módulo MP3.

constexpr uint8_t RST_PIN = 5;    // Se instancia el pin 5 del Arduino para utilizarlo como RESET del lector RFID.
constexpr uint8_t SS_PIN1 = 53;   // Se instancia el pin 53 del Arduino para utilizarlo como SS_DATA para la comunicación SPI.
const int advert_lights[2] = {6, 7}, buzzer = 8, seguro = 9, push = 4; // Se declaran las variables necesarias para el sistema.

// Se declaran las claves de identificación de cada tarjeta utilizada en la práctica.
byte access_code[4] = {0xD1, 0x9D, 0x04, 0x24}, access_code2[4] = {0xE3, 0x93, 0x68, 0x2E}, access_code3[4] = {0x23, 0x95, 0x69, 0xA9};

bool state = true;                // Se crea una variable booleana llamada state.
int chances = 0, desact = 0;      // Se crean dos variables de tipo entero que serviran como contadores dentro del programa.

void bienvenida();                // Se crea un prototipo de función llamado "bienvenida".
void denegado();                  // Se crea un prototipo de función llamado "denegado".
void bloqueo();                   // Se crea un prototipo de función llamado "bloqueado".

LiquidCrystal_I2C lcd(0x27, 16, 2); /*Se declara un objeto al cual se le lamará como "lcd" para poder 
                                      utilizar las funciones de librería incluida previamente.*/

MFRC522 mfrc522(SS_PIN1, RST_PIN);  // Se crea una instancia para el modulo MFRC522.
SoftwareSerial mySoftwareSerial(10, 11); /* Se crea una instancia para el modulo MP3 que permite establecer una 
                                            comunicación tipo USART entre este y el Arduino.*/

DFRobotDFPlayerMini myDFPlayer;             // Se instancia un objeto para identificar el modulo MP3 y utilizar sus funciones.

void setup() {
	Serial.begin(9600);	 // Se inicializa la comunicación serial con el PC.
	SPI.begin();			   // Se inicia la comunicación SPI.
	mfrc522.PCD_Init();	 // Se inicializa el módulo MFRC522.
  mySoftwareSerial.begin(9600); // Se inicializa la comunicación entre el Arduino y el modulo MP3.
  myDFPlayer.begin(mySoftwareSerial); // Se inician los pines RX y TX para la comunicación USART.
  myDFPlayer.volume(30);              // Se establece el volumen del módulo MP3 en 30.
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);  // Se configura el ecualizador del módulo MP3.
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD); // Se configura la micro SD para reproducir los audios.
  lcd.init();          // Se inicializa la LCD.
  lcd.backlight();     // Se enciende la luz de fondo de la LCD.
	pinMode(advert_lights[0], OUTPUT);  // Se declaran la variable "advert_lights" como SALIDA.
  pinMode(advert_lights[1], OUTPUT);  // Se declaran la variable "advert_lights" como SALIDA.
  pinMode(buzzer, OUTPUT);            // Se declaran la variable "buzzer" como SALIDA.
  pinMode(seguro, OUTPUT);            // Se declaran la variable "seguro" como SALIDA.
  pinMode(push, INPUT);               // Se declaran la variable "push" como ENTRADA.
}

void loop() {                         // Se inicia el bucle infinito.
     
    state = true;                              // La variable STATE mantiene su estado en TRUE.
    digitalWrite(advert_lights[1], 1);         // Se encienden las luces de advertencia.
    digitalWrite(seguro, 1);                   // Se acciona el seguro.
    lcd.setCursor(0,0);                        // Se situa el cursor en la posición 0 del renglón 1.
    lcd.print("Vehiculo cerrado");             // Se muestra el mensaje "Vehiculo cerrado" en la LCD.
    lcd.setCursor(1,1);                        // Se situa el cursor en la posición 1 del renglón 2.
    lcd.print("Presente su ID");               // Se muestra el mensaje "Presente su ID" en la LCD.
  
  if ( ! mfrc522.PICC_IsNewCardPresent()) {    // El lector RFID se habilita y comienza a leer si hay una tarjeta presente.
		return;
	}

	if ( ! mfrc522.PICC_ReadCardSerial()) {      // Si hay una tarjeta presente, leera el codigo serial de la tarjeta.
		return;
	}

// Esta función nos permite comparar si el codigo de la tarjeta leida coincide con alguna de las claves de acceso registradas previamente.
	if(mfrc522.uid.uidByte[0] == access_code[0] && mfrc522.uid.uidByte[1] == access_code[1] && mfrc522.uid.uidByte[2] == access_code[2] && mfrc522.uid.uidByte[3] == access_code[3])
  
  //Si el codigo de la tarjeta presentada coincide con el codigo de acceso No.1 (access_code), entonces:
  {
    if(state)                                    // Si la variable "state" es verdadera.
    {
     bienvenida();                               // Se manda a llmar la función bienvenida.
     lcd.clear();                                // Se limpia la LCD.
     lcd.setCursor(3,0);                         // Se situa el cursor en la posición 3 del renglón 1.
     lcd.print("Bienvenido");                    // Se imprime el mensaje "Bienvenido".
     lcd.setCursor(4,1);                         // Se situa el cursor en la posición 4 del renglón 2.
     lcd.print("Sr. Erik");                      // Se imprime el nombre del usuario al cual esta ligada la tarjeta.
     myDFPlayer.play(2);                         // Se reproduce en el módulo MP3 un mensaje de voz.
     delay(1000);                                // Se da un delay de 1 segundo.

     for(int shift = 0; shift < 17; shift++)     // Se hace un ciclo "for" para hacer un barrido de caracteres en la LCD.
     {
     lcd.setCursor(shift+3,0);                   // Se situa el cursor en la posición marcada por el ciclo "for" en el renglón 1.
     lcd.print("Bienvenido");                    // Se imprime el mensaje "Bienvenido".
     lcd.setCursor(shift+4,1);                   // Se situa el cursor en la posición marcada por el ciclo "for" en el renglón 2.
     lcd.print("Sr. Erik");                      // Se imprime el nombre del usuario al cual esta ligada la tarjeta.
     delay(150);                                 // Se da un delay de 150 milisegundos entre cada aumento del ciclo for.
     lcd.clear();                                // Se limpia la LCD.
     }                                           // Termina el ciclo "for".

     lcd.clear();                                // Se limpia la LCD.
     lcd.setCursor(3,0);                         // Se situa el cursor en la posición 3 del renglón 1.
     lcd.print("Maneje con");                    // Se imprime el mensaje "Maneje con".
     lcd.setCursor(5,1);                         // Se situa el cursor en la posición 5 del renglón 2.
     lcd.print("cuidado");                       // Se imprime el mensaje "cuidado".
     delay(1000);                                // Se de un delay de 1 segundo.

     for(int shift = 3; shift < 17; shift++)     // Se hace un ciclo "for" para hacer un barrido de caracteres en la LCD.
     {
     lcd.setCursor(shift,0);                     // Se situa el cursor en la posición marcada por el ciclo "for" en el renglón 1.
     lcd.print("Maneje con");                    // Se imprime el mensaje "Maneje con".
     lcd.setCursor(shift+2,1);                   // Se situa el cursor en la posición marcada por el ciclo "for" en el renglón 2.
     lcd.print("cuidado");                       // Se imprime el mensaje "cuidado".
     delay(150);                                 // Se da un delay de 150 milisegundos entre cada aumento del ciclo for.
     lcd.clear();                                // Se limpia la LCD.
     }                                           // Termina el ciclo "for".
     
     lcd.clear();                                // Se limpia la LCD.
     state = false;                              // El estado de la variable "state" cambia a false.
    }
    
    chances = 0;                                 // El contador "chances" se iguala a 0.
    delay(2000);                                 // Se da un delay de 2 segundos.
  }

// Esta función nos permite comparar si el codigo de la tarjeta leida coincide con alguna de las claves de acceso registradas previamente.
  	else if(mfrc522.uid.uidByte[0] == access_code2[0] && mfrc522.uid.uidByte[1] == access_code2[1] && mfrc522.uid.uidByte[2] == access_code2[2] && mfrc522.uid.uidByte[3] == access_code2[3])
  
   //Si el codigo de la tarjeta presentada coincide con el codigo de acceso No.2 (access_code2), entonces:
  {
    if(state)                                    // Si la variable "state" es verdadera.
    {
     bienvenida();                               // Se manda a llamar la función bienvenida.
     lcd.clear();                                // Se limpia la LCD.
     lcd.setCursor(3,0);                         // Se situa el cursor en la posición 3 del renglón 1.
     lcd.print("Bienvenido");                    // Se imprime el mensaje "Bienvenido".
     lcd.setCursor(2,1);                         // Se situa el cursor en la posición 2 del renglón 2.
     lcd.print("Papa disenador");                // Se imprime el mensaje "Papa disenador".
     myDFPlayer.play(1);                         // Se reproduce un mensaje de voz en el módulo MP3.
     delay(1000);                                // Se da un delay de 1 segundo.

     for(int shift = 0; shift < 17; shift++)     // Se hace un ciclo "for" para hacer un barrido de caracteres en la LCD.
     {                     
     lcd.setCursor(shift+3,0);                   // Se situa el cursor en la posición marcada por el ciclo "for" en el renglón 1.
     lcd.print("Bienvenido");                    // Se imprime el mensaje "Bienvenido".
     lcd.setCursor(shift,1);                     // Se situa el cursor en la posición marcada por el ciclo "for" en el renglón 2.
     lcd.print("Papa Disenador");                // Se imprime el nombre del usuario al cual esta ligada la tarjeta.
     delay(150);                                 // Se da un delay de 150 milisegundos entre cada aumento del ciclo for.
     lcd.clear();                                // Se limpia la LCD.
     }                                           // Se termina el barrido de carácteres. 

     lcd.clear();                                // Se limpia la LCD.
     lcd.setCursor(3,0);                         // Se situa el cursor en la posición 3 del renglón 1.
     lcd.print("Maneje con");                    // Se imprime el mensaje "Maneje con".
     lcd.setCursor(5,1);                         // Se situa el cursor en la posición 5 del renglón 2.
     lcd.print("cuidado");                       // Se imprime el mensaje "cuidado".
     delay(1000);                                // Se da un delay de 1 segundo.

     for(int shift = 3; shift < 17; shift++)     // Se hace un ciclo "for" para hacer un barrido de caracteres en la LCD.
     {
     lcd.setCursor(shift,0);                     // Se situa el cursor en la posición marcada por el ciclo "for" en el renglón 1.
     lcd.print("Maneje con");                    // Se imprime el mensaje "Maneje con".
     lcd.setCursor(shift+2,1);                   // Se situa el cursor en la posición marcada por el ciclo "for" en el renglón 2.
     lcd.print("cuidado");                       // Se imprime el mensaje "cuidado".
     delay(150);                                 // Se da un delay de 150 milisegundos entre cada aumento del ciclo for.
     lcd.clear();                                // Se limpia la LCD.
     }                                           // Se termina el barrido de carácteres.
     
     lcd.clear();                                // Se limpia la LCD.
     state = false;                              // El estado de la variable "state" cambia a false.
    }
    
    chances = 0;                                 // El contador "chances" se iguala a 0.
    delay(2000);                                 // Se da un delay de 2 segundos.
  }

// Esta función nos permite comparar si el codigo de la tarjeta leida, coincide con alguna de las claves de acceso registradas previamente.
  else if(mfrc522.uid.uidByte[0] == access_code3[0] && mfrc522.uid.uidByte[1] == access_code3[1] && mfrc522.uid.uidByte[2] == access_code3[2] && mfrc522.uid.uidByte[3] == access_code3[3])
  
  //Si el codigo de la tarjeta presentada coincide con el codigo de acceso No.3 (access_code3), entonces:
  {
    if(state)                                    // Si la variable "state" es verdadera.
    { 
     bienvenida();                               // Se manda a llamar la función bienvenida.
     lcd.clear();                                // Se limpia la LCD.
     lcd.setCursor(3,0);                         // Se situa el cursor en la posición 3 del renglón 1.
     lcd.print("Bienvenido");                    // Se imprime el mensaje "Bienvenido".
     lcd.setCursor(2,1);                         // Se situa el cursor en la posición 2 del renglón 2.
     lcd.print("Sr. Santiago");                  // Se imprime el nombre del usuario al cual esta ligada la tarjeta.
     myDFPlayer.play(3);                         // Se reproduce un mensaje de voz en el módulo MP3.
     delay(1000);                                // Se da un delay de 1 segundo.

     for(int shift = 0; shift < 17; shift++)     // Se hace un ciclo "for" para hacer un barrido de caracteres en la LCD.
     {
     lcd.setCursor(shift+3,0);                   // Se situa el cursor en la posición marcada por el ciclo "for" en el renglón 1.
     lcd.print("Bienvenido");                    // Se imprime el mensaje "Bienvenido".
     lcd.setCursor(shift+2,1);                   // Se situa el cursor en la posición marcada por el ciclo "for" en el renglón 1.
     lcd.print("Sr. Santiago");                  // Se imprime el nombre del usuario al cual esta ligada la tarjeta.
     delay(150);                                 // Se da un delay de 150 milisegundos entre cada aumento del ciclo for.
     lcd.clear();                                // Se limpia la LCD.
     }                                           // Se termina el barrido de carácteres.
 
     lcd.clear();                                // Se limpia la LCD.
     lcd.setCursor(3,0);                         // Se situa el cursor en la posición 3 del renglón 1.
     lcd.print("Tenga un");                      // Se imprime el mensaje "Tenga un".
     lcd.setCursor(3,1);                         // Se situa el cursor en la posición 3 del renglón 1.
     lcd.print("buen viaje");                    // Se imprime el mensaje "un buen viaje".
     delay(1000);                                // Se da un delay de 1 segundo.

     for(int shift = 3; shift < 17; shift++)     // Se hace un ciclo "for" para hacer un barrido de caracteres en la LCD.
     {
     lcd.setCursor(shift,0);                     // Se situa el cursor en la posición marcada por el ciclo "for" en el renglón 1.
     lcd.print("Tenga un");                      // Se imprime el mensaje "Tenga un".
     lcd.setCursor(shift+1,1);                   // Se situa el cursor en la posición marcada por el ciclo "for" en el renglón 2.
     lcd.print("buen viaje");                    // Se imprime el mensaje "buen viaje".
     delay(150);                                 // Se da un delay de 150 milisegundos entre cada aumento del ciclo for.
     lcd.clear();                                // Se limpia la LCD.
     }                                           // Se termina el barrido de carácteres.
     
     lcd.clear();                                // Se limpia la LCD.
     state = false;                              // El estado de la variable "state" cambia a false.
    }
    
    chances = 0;                                 // El contador "chances" se iguala a 0.
    delay(2000);                                 // Se da un delay de 2 segundos.
  }                                              

  else                                           // Si no se cumple ninguna de las condiciones, entonces:
  {
     chances++;                                  // El contador "chances" aumenta en 1 unidad.

/* Esta condición solo se cumplirá cuando se presente 3 veces la tarjeta (RFID) equivocado, el lector RFID se bloqueará 
   y solo podrá ser desbloqueado con una clave manual. */

     while(chances == 3)                         // Mientras que el contador "chances" sea igual a 3.
     {
       lcd.clear();                              // Se limpia la LCD.
       lcd.setCursor(3,0);                       // Se situa el cursor en la posición 3 del renglón 1.
       lcd.print("Lector ID");                   // Se imprime el mensaje "Lector ID".
       lcd.setCursor(3,1);                       // Se situa el cursor en la posición 3 del renglón 2.
       lcd.print("bloqueado");                   // Se imprime el mensaje "bloqueado".
       bloqueo();                                // Se manda a llamar a la función bloqueo.
       myDFPlayer.play(9);                       // Se reproduce un mensaje de voz en el módulo MP3.
       delay(2000);                              // Se da un delay de 2 segundos.
       lcd.clear();                              // Se limpia la LCD.
       lcd.setCursor(3,0);                       // Se situa el cursor en la posición 3 del renglón 1.
       lcd.print("Desbloque");                   // Se imprime el mensaje "Desbloque".
       lcd.setCursor(2,1);                       // Se situa el cursor en la posición 2 del renglón 2.
       lcd.print("con su clave");                // Se imprime el mensaje "con su llave".
       delay(2000);                              // Se da un delay de 2 segundos.
       lcd.clear();                              // Se limpia la LCD.

/* La clave manual consta de 4 pulsos con espera de medio segundo entre cada pulso, al presionar el botón 4 veces seguidas, se
   entra en la siguente condición: */ 

       while(desact < 4)                         // Si "desact" es menor a cuatro, entonces:
       {                         
         lcd.setCursor(3,0);                     // Se situa el cursor en la posición 3 del renglón 1.
         lcd.print("Introduzca");                // Se imprime el mensaje "Introduza".
         lcd.setCursor(4,1);                     // Se situa el cursor en la posición 3 del renglón 1.
         lcd.print("su clave");                  // Se imprime el mensaje "su clave: ".
         desact += digitalRead(push);            // Cada que se presione el botón "desact" incrementa en una unidad.
         delay(500);                             // Se da un delay de 500 milisegundos.
       }

// Se utiliza este ciclo "for" como indicador, acciona un led y un buzzer que indica que se desbloqueo el lector ID.       
       for(int i = 0; i < 2; i++)                
      {
        digitalWrite(advert_lights[0], 1);       // Se enciende el indicador "enable" (led verde).
        digitalWrite(advert_lights[1], 0);       // Se apaga el indicador "disable" (led rojo).
        digitalWrite(buzzer, 1);                 // Se enciende el buzzer.
        delay(100);                              // Se da un delay de 100 milisegundos.
        digitalWrite(advert_lights[0], 0);       // Se apaga el indicador "enable" (led verde).
        digitalWrite(advert_lights[1], 0);       // Continua apagado el indicador "disable" (led rojo).
        digitalWrite(buzzer, 0);                 // Se apaga el buzzer.
        delay(100);                              // Se un delay de 100 milisegundos.
      }
       lcd.clear();                              // Se limpia la LCD.
       lcd.setCursor(4,0);                       // Se situa el cursor en la posición 4 del renglón 1.
       lcd.print("Clave");                       // Se imprime el mensaje "Clave".
       lcd.setCursor(3,1);                       // Se situa el cursor en la posición 3 del renglón 1.
       lcd.print("correcta");                    // Se imprime el mensaje "correcta".
       myDFPlayer.play(7);                       // Se reproduce en el módulo MP3 un mensaje de voz.
       delay(2000);                              // Se da un delay de 2 segundos.
       lcd.clear();                              // Se limpia la LCD.
       lcd.setCursor(3,0);                       // Se situa el cursor en la posición 3 del renglón 1.
       lcd.print("Lector ID");                   // Se imprime el mensaje "Lector ID".
       lcd.setCursor(2,1);                       // Se situa el cursor en la posición 2 del renglón 2.
       lcd.print("desbloqueado");                // Se imprime el mensaje "desbloqueado".
       delay(2000);                              // Se da un delay de 2 segundos.
       lcd.clear();                              // Se limpia la LCD.

       desact = 0;                               // La variable "desact", se iguala a 0 (se reinician los pulsos).
       chances = 0;                              // La variable "chances", se iguala a 0 (se restablecen los chances).
       state = false;                            // La variable "state" cambia su estado a FALSE.
  }

// Si el usuario presenta un ID no valido, se ejecuta la siguiente condición:
    if(state)                                    // Si la variable "state" es VERDADERA, entonces:
    {
    myDFPlayer.play(8);                          // Se reproduce en el módulo MP3 un mensaje de voz.
    lcd.clear();                                 // Se limpia la LCD.
    lcd.setCursor(3,0);                          // Se situa el cursor en la posición 3 del renglón 1.
    lcd.print("ID invalido");                    // Se imprime el mensaje "ID invalido".
    lcd.setCursor(1,1);                          // Se situa el cursor en la posición 1 del renglón 2.
    lcd.print("Presente su ID");                 // Se imprime el mensaje "Presente su ID".
    denegado();                                  // Se manda a llamar la función "denegado".
    delay(3000);                                 // Se da un delay de 3 segundos.
    lcd.clear();                                 // Se limpia la LCD.
    }
  }

} // Termina el programa principal.

// A continuación se instancian las funciones declaradas al inicio del programa.
void bienvenida()                                // Se instancia la función "bienvenida".
{
   digitalWrite(seguro, 0);                      // Se abre el seguro.
   lcd.clear();                                  // Se limpia la LCD.
   lcd.setCursor(4,0);                           // Se situa el cursor en la posición 4 del renglón 1.
   lcd.print("ID Valido");                       // Se imprime el mensaje "ID Valido".
   lcd.setCursor(0,1);                           // Se situa el cursor en la posición 0 del renglón 2.
   lcd.print("Vehiculo abierto");                // Se imprime el mensaje "Vehículo abierto".

// Se hace un ciclo "for" para accionar las luces de advertencia y el buzzer.
   for(int i = 0; i < 2; i++)                    
   {
   digitalWrite(advert_lights[0], 1);            // Se enciende el indicador "enable" (led verde).
   digitalWrite(advert_lights[1], 0);            // Se apaga el indicador "disable" (led rojo).
   digitalWrite(buzzer, 1);                      // Se enciende el buzzer.
   delay(100);                                   // Se da un delay de 100 milisegundos.
   digitalWrite(advert_lights[0], 0);            // Se apaga el indicador "disable" (led verde).
   digitalWrite(advert_lights[1], 0);            // Se apaga el indicador "enable" (led rojo).
   digitalWrite(buzzer, 0);                      // Se apaga el buzzer.
   delay(100);                                   // Se da un delay de 100 milisegundos.
   }

   delay(2000);                                  // Se da un delay de 2 segundos.
} // Termina la función.

void denegado()                                  // Se instancia la función "denegado".
{
  digitalWrite(seguro, 1);                       // Se abre el seguro.

// Se hace un ciclo "for" para accionar las luces de advertencia y el buzzer.
  for(int alarm = 0; alarm < 4; alarm++)
  {
  digitalWrite(advert_lights[1], 1);             // Se enciende el indicador "disable" (led rojo).
  digitalWrite(buzzer, 1);                       // Se enciende el buzzer.
  delay(50);                                     // Se da un delay de 50 milisegundos.
  digitalWrite(advert_lights[1], 0);             // Se apaga el indicador "disable" (led verde).
  digitalWrite(buzzer, 0);                       // Se apaga el buzzer.
  delay(50);                                     // Se da un delay de 50 milisegundos.
  }
} // Termina la función 

void bloqueo()                                   // Se instancia la función "bloqueo".
{
  digitalWrite(seguro, 1);                       // El seguro se mantiene cerrado.

// Se hace un ciclo "for" para accionar las luces de advertencia y el buzzer.  
  for(int alarm = 0; alarm < 10; alarm++)        
  {
  digitalWrite(advert_lights[1], 1);             // Se enciende el indicador "disable" (led rojo).
  digitalWrite(buzzer, 1);                       // Se enciende el buzzer.
  delay(100);                                    // Se da un delay de 100 milisegundos.
  digitalWrite(advert_lights[1], 0);             // Se apaga el indicador "disable" (led rojo).
  digitalWrite(buzzer, 0);                       // Se apaga el buzzer.
  delay(100);                                    // Se da un delay de 100 milisegundos.
  }
} // Se termina la función.

