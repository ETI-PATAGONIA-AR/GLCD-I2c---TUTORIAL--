///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
////////// TACOMETRO OPTICO CON FUNCION VOLTIMETRO E INDICADOR DE APERTURA DE VALVULAS ANALOGICAS /////////
//////////      prof.martintorres@educ.ar - ETI Patagonia               /////////   
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//#include <PCD8544.h>
#include <PCF8574_PCD8544.h>

//configuracion display 5110 I2C
PCF8574_PCD8544 display = PCF8574_PCD8544(0x20, 7, 6, 5, 4, 2);

// Variables de uso interno TACOMETRO//
volatile byte REV;                      //  TIPO DE DATO VOLÁTIL PARA ALMACENAR REVOLUCIONES
unsigned long int rpm, maxRPM , frec;  //  DEFINIR RPM Y RPM MÁXIMAS
unsigned long time;                    //  DEFINE EL TIEMPO NECESARIO PARA CUBRIR UNA REVOLUCIÓN
int led = 0,RPMlen , prevRPM;          // VARIABLES DEL TIPO ENTEROS PARA ALMACENAR EL VALOR DEL LED Y LAS RPM ACTUALES Y LAS RPM ANTERIORES
int flag = 0;                          // VARIABLE BANDERA PARA DETERMINAR SI LA LCD NECESITA SER LIMPIADA O NO
long prevtime = 0;                     //  ALMACENA EL TIEMPO DE INACTIVIDAD PARA CAMBIAR EL MENÚ

//Variables de uso interno VOLTIMETRO//
int analogInput = A0;                   //Definimos el pin analógico A0 para la lectura del voltaje
float vout = 0.0;                      //Definimos la variable Vout
float vin = 0.0;                      //Definimos la variable Vin
float R1 = 100000.0;                  //  R1 (100K) Valor de la resistencia R1 del divisor de tensión
float R2 = 10000.0;                   //  R2 (10K) Valor de la resistencia R2 del divisor de tención
float escala;
int value = 0;                        //Definimos la variable value

static const byte sensorPin = 0;
static const byte ledPin = 7;
    
 void setup()
 {
  attachInterrupt(0, RPMCount, RISING);     //  CONFIGURACION DE INTERRUPCIÓN PARA CUANDO EL SENSOR CONMUTA DE ESTADO BAJO A ALTO
  Serial.begin(9600);                          // OBTENER VALORES UTILIZANDO MONITOR SERIE
  display.begin();       
  display.setContrast(60);
  display.display(); //
  display.clearDisplay();   //
 
  pinMode(ledPin, OUTPUT);
     REV = 0;                                  //  EMPEZAR TODAS LAS VARIABLES DESDE 0
     rpm = 0;
     frec = 0;
     time = 0;
     pinMode(analogInput, INPUT);             //Activamos el input del pin analógico A0
     pinMode(ledPin, OUTPUT);
     pinMode(3, OUTPUT);           
     pinMode(4, OUTPUT);
     digitalWrite(3, HIGH);                  //  PIN VCC DEL SENSOR IR
     digitalWrite(4, LOW);                   // PIN GND DEL SENSOR

  display.clearDisplay();   //   
  display.setTextSize(1);
  display.setTextColor(BLACK); 
  display.setCursor(0, 0);
  display.println("--------------");
  display.setCursor(0, 7);
  display.println("OPTICTACOMETRO");
  display.setCursor(0, 15);
  display.println("Desing by:    ");
  display.setCursor(0, 25);
  display.println("   Torres A.M.");
  display.setCursor(0, 35);
  display.println("ETI  Patagonia");
  display.setCursor(0, 44);
  display.println("--------------");
  display.display();
  delay(2000);
  display.clearDisplay();
  delay(10);
  display.display();   
 }
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
 {
  long currtime = millis();                   // OBTENER TIEMPO ACTUAL
  long idletime = currtime - prevtime;        //  CALCULAR EL TIEMPO DE INACTIVIDAD
    if(REV >= 5 )                            //  SE ACTUALIZARÁ DESPUÉS DE CADA 5 LECTURAS
   {
     if(flag==0)                             //  BORRAR LA LCD PARA EVITAR CUALQUIER TEXTO BASURA
     {
  display.setCursor(0, 0);
  display.println("--------------");
  display.setCursor(0, 44);
  display.println("--------------");
       display.display();
       flag=1;                               //   DESPUÉS DE BANDERA = 1, EL BUCLE NO SE EJECUTARÁ DE NUEVO
     }
     rpm = 30*1000/(millis() - time)*REV;    //  CALCULAR RPM UTILIZANDO REVOLUCIONES Y TIEMPO TRANSCURRIDO
     if(rpm > maxRPM)
     maxRPM = rpm;                          //  OBTENGA LAS RPM MÁXIMAS EN EL TIEMPO QUE ESTUVO DETECTANDO EL SENSOR
     time = millis();                           
     REV = 0;
     int x= rpm;                            //  CALCULAR NUMERO DE DIGITOS EN RPM
     while(x!=0)
     {
       x = x/10;
       RPMlen++;
     }       
     if(RPMlen!=prevRPM)                    // SI LAS RPM CAEN A UN NÚMERO MÁS BAJO CON MENOS DÍGITOS, LA LCD SE BORRARÁ
       {
       display.clearDisplay();
       display.setTextSize(1);
       display.setTextColor(BLACK);
       prevRPM = RPMlen;
       flag=0;
  display.setCursor(0, 0);
  display.println("------PP------");
  display.setCursor(0, 44);
  display.println("--------------");
  display.display();
       }
  display.clearDisplay();
  display.display();
  display.setCursor(0, 0);
  display.println("--------------");
  display.setCursor(0, 7);
  display.println("RPM:");
  display.setCursor(25, 7);
  display.println(rpm,DEC);                     //  IMPRIMIR RPM EN SISTEMA DECIMAL
      flag=1;                                 //   DESPUÉS DE LA BANDERA = 1, EL BUCLE NO VOLVERÁ A EJECUTARSE
  ////FRECUENCIMETRO
  display.setCursor(0, 15);
  display.println("Frec:"); 
     frec = rpm / 60 ;     
     display.setCursor(27, 15);
     display.println(frec,DEC);
  ////VOLTIMETRO
 value = analogRead(analogInput);        //Leemos el pin analógico A0
    vout = (value * 5.0) / 1024.0;          // Cálculo para obtener el Vout
    vin = vout / (R2/(R1+R2));              // Cálculo para obtener Vin del divisor de tensión
    escala=vin;
    if (vin<0.09) {                         //Si la lectura de Vin es menor que 0.09V le decimos que nos muestre 0
    vin=0.0;
    }
    escala = map(escala,0,24,0,100);
      
  display.setCursor(0, 25);
  display.println("T.EV:");
  display.setCursor(27, 25);
  display.println(vin);
  display.setCursor(0, 35);
  display.println("% EV:");
  display.setCursor(27, 35);
  display.println(escala);
  display.setCursor(0, 44);
  display.println("--------------");   
  display.display();
  delay(500);
    prevtime = currtime;                        // RESET TIEMPO INACTIVO
   }
   if(idletime > 5000 )                      //  SI NO HAY LECTURAS DURANTE 5 SEGUNDOS, LA PANTALLA MOSTRARÁ RPM MÁXIMAS
   {
    if(flag==1)                            // LIMPIA LA PANTALLA DEL DISPLAY LCD
       {
       display.clearDisplay();
       delay(500);
  display.display();
       flag=0;
       }
  display.clearDisplay();   //   
  display.setTextSize(1);
  display.setTextColor(BLACK); 
  display.setCursor(0, 0);
  display.println("--------------");
  display.setCursor(0, 7);
  display.println("MAX RPM MEDIDA");
  display.setCursor(5, 15);
  display.println("-> ");
  display.setCursor(25, 15);
  display.println(maxRPM,DEC);                     // IMPRIME EN EL DISPLAY LAS RPM MAX
   /////
value = analogRead(analogInput);        //Leemos el pin analógico A0
    vout = (value * 5.0) / 1024.0;          // Cálculo para obtener el Vout
    vin = vout / (R2/(R1+R2));              // Cálculo para obtener Vin del divisor de tensión
    escala=vin;
    if (vin<0.09) {                         //Si la lectura de Vin es menor que 0.09V le decimos que nos muestre 0
    vin=0.0;
    }
    escala = map(escala,0,24,0,100);
  
  display.setCursor(0, 25);
  display.println("T.EV:");
  display.setCursor(27, 25); 
  display.println(vin);
  display.setCursor(0, 35);
  display.println("% EV:");
  display.setCursor(27, 35);
  display.println(escala);
  display.setCursor(0, 44);
  display.println("--------------");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
    

  display.setTextSize(1);
  display.setTextColor(BLACK); 
  display.setCursor(0, 0);
  display.println("--------------");
  display.setCursor(0, 7);
  display.println("estadoINACTIVO");
  display.setCursor(0, 15);
  display.println("LISTOparaMEDIR");
/////
   value = analogRead(analogInput);            //Leemos el pin analógico A0
    vout = (value * 5.0) / 1024.0;              // Cálculo para obtener el Vout
    vin = vout / (R2/(R1+R2));                  // Cálculo para obtener Vin del divisor de tensión
    escala=vin;
    if (vin<0.09) {                             //Si la lectura de Vin es menor que 0.09V le decimos que nos muestre 0
    vin=0.0;
    }
    escala = map(escala,0,24,0,100);
 
  display.setCursor(0, 25);
  display.println("T.EV:"); 
  display.setCursor(27, 25);
  display.println(vin);
  display.setCursor(0, 35);
  display.println("% EV:");
  display.setCursor(27, 35);
  display.println(escala);
  display.setCursor(0, 44);
  display.println("--------------");
  display.display();   
delay(2000);
     prevtime = currtime;
   }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////
 void RPMCount()                                // CADA VEZ QUE EL SENSOR CONMUTA DE ESTADO BAJO A ALTO, ESTA FUNCIÓN SERÁ INVOCADA
 {
   REV++;                                         // AUMENTAR LOS DISPAROS
   if (led == LOW)
     {
      led = HIGH;                                 // CAMBIAR ESTADO DEL LED
     }
   else
     {
     led = LOW;
     }
   digitalWrite(ledPin, led);
 }
//////////////////////////////////////////////////////////////  FIN DEL PROGRAMA  ///////////////////////////////////////////////////////////////////////