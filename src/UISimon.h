#ifndef _UISIMON_H
#define _UISIMON_H

// toute l'initialisation dépendant du matériel
void initBoard();

// Pour ajuster l'éclairage à pourcent(age) 
void setLcdLight(unsigned char pourcent);

// Lecture du cache de l'état des boutons
void litBoutons();

// Est-ce que les touches sont appuyées 1: oui 0: non
char toucheBleue1() ;
char toucheJaune2() ;
char toucheVerte3() ;
char toucheRouge4() ;

// passe les LEDs à HIGH ou LOW
void setLEDBleue1(byte state);
void setLEDJaune2(byte state);
void setLEDVerte3(byte state);
void setLEDRouge4(byte state);

#endif
