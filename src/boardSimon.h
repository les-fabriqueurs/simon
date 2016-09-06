#ifndef _BOARDSIMON_H
#define _BOARDSIMON_H

#include "Wire.h"
#include "UISimon.h"

// ECRAN LCD  + BUZZER
#define PINBUZZER    A0
#define PINLCDRS     2
#define PINLCDRW     3
#define PINLCDENABLE 4
#define PINLCDD0     5
#define PINLCDD1     6
#define PINLCDD2     7
#define PINLCDD3     8
#define PINLCDD4     9
// avant: proto breadboard: 13 au lieu de 10
#define PINLCDD5     10
#define PINLCDD6     11
#define PINLCDD7     12

// initialisation de l'�cran LCD
LiquidCrystal lcd(PINLCDRS,PINLCDRW, PINLCDENABLE,PINLCDD0,PINLCDD1,PINLCDD2,PINLCDD3,PINLCDD4,PINLCDD5,PINLCDD6,PINLCDD7);

// touches
#define TOUCHE_BLEUE_1 4
#define TOUCHE_JAUNE_2 1
#define TOUCHE_VERTE_3 2
#define TOUCHE_ROUGE_4 8

//LEDs
#define LED_BLEUE_1  2
#define LED_JAUNE_2  8
#define LED_VERTE_3  4
#define LED_ROUGE_4  1

// Etat tampon des boutons
byte etatBoutons = 0;

// Etat des LEDs
byte etatLEDs = 0;

// intensit� du r�tro-�clairage
byte retroLcd = 0;

// permet de lire l'�tat des boutons et de le stocker en variable globale 
void litBoutons() {
   Wire.beginTransmission(0x20);
   Wire.write(0x12); // set MCP23017 memory pointer to GPIOA address
   Wire.endTransmission();
   Wire.requestFrom(0x20, 1); // request one byte of data from MCP20317
   etatBoutons = Wire.read(); // store the incoming byte into boutons 
   etatBoutons = (~etatBoutons)&0xFF;
 
}

char toucheBleue1() { return (etatBoutons & TOUCHE_BLEUE_1); }
char toucheJaune2() { return (etatBoutons & TOUCHE_JAUNE_2); }
char toucheVerte3() { return (etatBoutons & TOUCHE_VERTE_3); }
char toucheRouge4() { return (etatBoutons & TOUCHE_ROUGE_4); }

// est-ce que les touches sont appuy�es 1: oui 0: non
//char toucheHaut()   { return (etatBoutons & TOUCHE_HAUT); }
//char toucheBas()    { return (etatBoutons & TOUCHE_BAS); }
//char toucheGauche() { return (etatBoutons & TOUCHE_GAUCHE); } 
//char toucheDroite() { return (etatBoutons & TOUCHE_DROITE); } 
//char toucheStart()  { return (etatBoutons & TOUCHE_START); } 
//char toucheSelect() { return (etatBoutons & TOUCHE_SELECT); } 
//char toucheA()      { return (etatBoutons & TOUCHE_A); } 
//char toucheB()      { return (etatBoutons & TOUCHE_B); } 


// met � jour les 4 LEDs par le MCP23017
void setLEDs(byte leds) {
  Wire.beginTransmission(0x20);
  Wire.write(0x13); // address port A
  Wire.write(leds);  // value to send
  Wire.endTransmission();
  etatLEDs = leds;

}

// met � jour une LED
void setLED(byte ledValue, byte state) { 
  if (state) { 
    setLEDs(etatLEDs | ledValue);
  } else {
    setLEDs(etatLEDs & (~ledValue));  
  }
}

void setLEDBleue1(byte state) { setLED(LED_BLEUE_1,state);}
void setLEDJaune2(byte state) { setLED(LED_JAUNE_2,state);}
void setLEDVerte3(byte state) { setLED(LED_VERTE_3,state);}
void setLEDRouge4(byte state) { setLED(LED_ROUGE_4,state);}



// toute l'inistialisation d�pendant du mat�riel
void initBoard() {

  // le buzzer
  pinMode(PINBUZZER,OUTPUT);

  // LCD 2 lignes de 16 caract�res
  lcd.begin(16, 2);

  // configuration du mpc23017
  Serial.begin(9600);
  Wire.begin(); // wake up I2C bus
  Wire.beginTransmission(0x20);
  Wire.write(0x01); // IODIR B
  Wire.write(0x00); // set all of port B to outputs
  Wire.endTransmission(); 

  Wire.beginTransmission(0x20);
  Wire.write(0x0C); // GPPUA
  Wire.write(0xFF); // activer les pullups du registre A
  Wire.endTransmission(); 
  
  
}

void screenPrint(byte x, byte y, String txt) {
  lcd.setCursor(x, y);
  lcd.print(txt); 
}

void clearScreen() {
   lcd.clear();
}


#endif
