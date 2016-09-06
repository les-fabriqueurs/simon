#include "Wire.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "boardSimon.h" //tout ce qui dépend de la configuration matérielle ;-)


// MPC23017 branché sur I2C (A4,A5) avec 4 LEDs GPIOA 0,1,2,3 et 4 boutons GPIOB 0,1,2,3

// Les états du jeu
#define ETAT_INIT_PARTIE         0
#define ETAT_SIMON_JOUE          1
#define ETAT_HUMAIN_JOUE         2
#define ETAT_ECOUTE_COUP_HUMAIN  3
#define ETAT_MENU_INITIAL        4
//#define ETAT_ENREGISTRE_HI_SCORE 5
#define ETAT_AFFICHE_HISCORE     6
#define ETAT_FREE_PLAY           7
#define ETAT_MENU_MULTIJOUEURS   8
#define ETAT_JOUEURN_JOUE        9
#define ETAT_JOUEURN_AJOUTE_NOTE 10

// l'etat quand on allume le jeu
#define ETAT_ON                  4


byte level         = 0; // nombre de notes à jouer 
byte etat          = ETAT_MENU_INITIAL;  
byte longueur      = 0; // longueur que le joueur à joué (nombre de notes)
byte sequenceSimon[200];
byte sequenceJoueur[200];
byte relache       = 1;  // si le joueur a relaché ou non
byte hiScore       = 0;  // le meilleur score
int addr           = 0;  // on écrit le meilleur score en eeprom addr=0

byte joueur        = 1; // a qui de jouer (1,2,3,4...nbJoueurs?)
byte nbJoueurs     = 2; // nombre de joueurs multijoueur (2 à 16?);
byte joueursOK[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // Les joueurs encore en jeu

void setup() {

  initBoard();

  // optionel: pour mettre à jour le hiScore manuellement: EEPROM.write(addr,15);
  //EEPROM.write(addr,15);

  // seed pour le random
  setEtat(ETAT_ON);
}

//
// enregistre l'appui touche du joueur 
//
void enregistreNote() {

   litBoutons();
   if ( (relache == 1) &&
      ((etatBoutons == TOUCHE_BLEUE_1) || (etatBoutons == TOUCHE_JAUNE_2) || (etatBoutons == TOUCHE_VERTE_3) || (etatBoutons == TOUCHE_ROUGE_4)) ) {
     joueNote(etatBoutons);
     sequenceJoueur[longueur] = etatBoutons;
     longueur++;
   }

   if (etatBoutons == 0) {
     relache = 1; 
   } else { 
     relache = 0;
   }
}

//
// joue une note et allume la LED adequate
//
void joueNote(byte note) {

  // on joue la note 
  if (note == TOUCHE_BLEUE_1) {
    setLEDBleue1(HIGH);
    tone(PINBUZZER,1500,250);
  }
  if (note == TOUCHE_JAUNE_2) {
    setLEDJaune2(HIGH);
    tone(PINBUZZER,2500,250);
  }
  if (note == TOUCHE_VERTE_3) {
    setLEDVerte3(HIGH);
    tone(PINBUZZER,2000,250);
  }
  if (note == TOUCHE_ROUGE_4) {
    setLEDRouge4(HIGH);
    tone(PINBUZZER,1000,250);
  }
  delay(250);
  
  // on eteint la led
  setLEDs(0);
  noTone(PINBUZZER);
}

//
// joue la séquence de notes de SIMON
//
void joue() {

  delay(1000);

  byte i=0;
  for (i=0;i<level;i++) {
    joueNote(sequenceSimon[i]);
    delay(200);
  }
}

//
// ajoute une note à la séquence SIMON
void ajoutNote() {
  int rnd[4]={1,2,4,8};
  
  sequenceSimon[level]=rnd[random(0,4)];
  level++;
}

// 
// son et message à joueur quand le joueur a bon
//
void sonGagne() {
  
  //clearScreen();
  screenPrint(0,1,String("Correct!    "));
  delay(1000);
}

//
// son à jouer en cas d'erreur
//
void sonPerd() {
  clearScreen();
  screenPrint(0, 0,String("Perdu!"));
  screenPrint(0,1,String("Score=")+(level-1));
  delay(5000);
}

//
// introduction
//
int intro() {
    screenPrint(0, 0,String("Nouvelle partie"));
    joueNote(TOUCHE_ROUGE_4);
    joueNote(TOUCHE_BLEUE_1);
    joueNote(TOUCHE_VERTE_3);
    joueNote(TOUCHE_JAUNE_2);
    delay(1000);
    joueNote(TOUCHE_JAUNE_2);
    delay(250);
    joueNote(TOUCHE_JAUNE_2);
    delay(250);
    joueNote(TOUCHE_ROUGE_4);
    hiScore = EEPROM.read(addr);
    clearScreen();
    screenPrint(0, 0,String("Top score=")+hiScore);
}

int introMulti() {
    screenPrint(0, 0,String("Multi-joueurs!"));
    joueNote(TOUCHE_JAUNE_2);
    joueNote(TOUCHE_VERTE_3);
    joueNote(TOUCHE_BLEUE_1);
    joueNote(TOUCHE_ROUGE_4);
    delay(1000);
    joueNote(TOUCHE_ROUGE_4);
    delay(250);
    joueNote(TOUCHE_ROUGE_4);
    delay(250);
    joueNote(TOUCHE_JAUNE_2);
}

// changement de l'etat
void setEtat(char newEtat) {
    clearScreen();
    etat = newEtat;

    if (etat == ETAT_MENU_INITIAL) { 
      // Message de bienvenu
      screenPrint(0,0,String("Bienvenue sur"));
      screenPrint(0,1,String("Simon!"));
    }

    if (etat == ETAT_ECOUTE_COUP_HUMAIN) {
      screenPrint(0,0,String("Ton tour!"));
    }

    if (etat == ETAT_JOUEURN_JOUE) {
      longueur = 0;
      screenPrint(0,0,String("Joueur ")+(joueur+1)+String("   "));
      screenPrint(0,1,String(String("Etape:")+level));
      joue();
      screenPrint(0,1,String("Joue!      "));
    }

    if (etat == ETAT_JOUEURN_AJOUTE_NOTE) {
      screenPrint(0,0,String("Joueur ")+(joueur+1)+String("   "));
      screenPrint(0,1,String("Ta note!"));
    }

    if (etat == ETAT_MENU_MULTIJOUEURS) {
        screenPrint(0,0,String("Multi-joueurs!"));
        screenPrint(0,1,String("Nb joueurs:")+nbJoueurs+String("  "));
        delay(200);
    }
    
    if (etat == ETAT_SIMON_JOUE) {
      screenPrint(0,0,String("Ecoute! "));
      screenPrint(0,1,String(String("Etape:")+level));
    }
}


void etatInitPartie() {
  randomSeed(millis());
    intro();  
    delay(2000);
    level    = 0;
    longueur = 0;
    ajoutNote();
    setEtat(ETAT_SIMON_JOUE); 
}

void etatSimonJoue() {
    joue();
    setEtat(ETAT_HUMAIN_JOUE);
}

void etatHumainJoue() {
     clearScreen();
     screenPrint(0, 0,String("Ton tour! "));
     setEtat(ETAT_ECOUTE_COUP_HUMAIN);        
}

void etatEcouteCoupHumain() {
  byte i = 0;
  byte perdu = 0;
  
  enregistreNote();

  if (longueur == level) {
    delay(1000);
      
    // on vérifie si le joueur a gagné
    for (i=0;i<level;i++) {
      if (sequenceSimon[i] != sequenceJoueur[i]) {
        perdu = 1;
      }
    }
  
    if (perdu == 0) {
      sonGagne();
      ajoutNote();
      longueur = 0;
      setEtat(ETAT_SIMON_JOUE);
    } else {
      if ((level-1)>hiScore) {

        // Le meilleur score est battu!
        joueNote(TOUCHE_ROUGE_4);
        joueNote(TOUCHE_VERTE_3);
        joueNote(TOUCHE_JAUNE_2);
        joueNote(TOUCHE_BLEUE_1);
        joueNote(TOUCHE_JAUNE_2);
        joueNote(TOUCHE_VERTE_3);
        joueNote(TOUCHE_ROUGE_4);
        clearScreen(); 
        screenPrint(0,0,String("Record battu!!"));
        hiScore = level - 1;
        screenPrint(0, 1,String("Score:")+hiScore); 
        EEPROM.write(addr,hiScore);
      } else {
        sonPerd();
      }
      setEtat(ETAT_MENU_INITIAL);
      level = 0;
      longueur = 0;
    }
  } 
}

// Le menu initial
// Bouton 1: démarre une partie
// Bouton 2: affiche le meilleur score
// Bouton 3: lumière/contraste -
// Bouton 4: lumière/contraste +
void etatMenuInitial() {

    delay(200);
    litBoutons();
    
    // mode un joueur
    if (toucheRouge4()) {
      setEtat(ETAT_INIT_PARTIE);
    }

    // Free play
    if (toucheBleue1()) { 
      setEtat(ETAT_FREE_PLAY);
      screenPrint(0,0,String("Free play!"));
    }
    
    // affichage HI-SCORE
    if (toucheJaune2()) { 
      setEtat(ETAT_AFFICHE_HISCORE);
    }
    
    // mode deux joueurs
    if (toucheVerte3()) {
      setEtat(ETAT_MENU_MULTIJOUEURS);
    }    
}

void etatAfficheHiScore() {
  int hiScore = 0;
  hiScore = EEPROM.read(addr);
  clearScreen();
  screenPrint(0, 0,String("Top score:")+hiScore);
  delay(3000);
  setEtat(ETAT_MENU_INITIAL);
}

void etatFreePlay() {
    delay(50);
    litBoutons();
    if (etatBoutons == (TOUCHE_BLEUE_1 | TOUCHE_JAUNE_2 | TOUCHE_VERTE_3 | TOUCHE_ROUGE_4)) {
      setEtat(ETAT_MENU_INITIAL);
      return;
    }
   if ( (relache == 1) &&
      ((etatBoutons == TOUCHE_BLEUE_1) || (etatBoutons == TOUCHE_JAUNE_2) || (etatBoutons == TOUCHE_VERTE_3) || (etatBoutons == TOUCHE_ROUGE_4)) ) {
     joueNote(etatBoutons);
   }

   if (etatBoutons == 0) {
     relache = 1; 
   } else { 
     relache = 0;
   }
}

// Le menu multijoueurs
// Bouton 1: nombre de joueurs --
// Bouton 2: nombre de joueurs ++
// Bouton 3: Valide
// Bouton 4: retour au menu précédent
void etatMenuMultiJoueurs() {

  // Message de bienvenu
  screenPrint(0,0,String("Multi-joueurs!"));
  screenPrint(0,1,String("Nb joueurs:")+nbJoueurs+String("  "));
    
  delay(200);
  litBoutons();
    
  // Moins de joueurs
  if (toucheRouge4()) { 
    if (nbJoueurs>=3) {
      nbJoueurs--;
      screenPrint(0,1,String("Nb joueurs:")+nbJoueurs+String("  "));
      delay(200);
    }
  }
    
  // plus de joueurs
  if (toucheBleue1()) { 
    if (nbJoueurs<16) {
      nbJoueurs++;
      screenPrint(0,1,String("Nb joueurs:")+nbJoueurs+String("  "));
       delay(200);
   }
  }
    
  // valider
  if (toucheVerte3()) {
    introMulti();
    delay(2000);
    for (byte i=0;i<nbJoueurs;i++) {
      joueursOK[i] = 1;
    }
    level    = 0;
    longueur = 0;
    joueur   = 0;
    ajoutNote();
    clearScreen();
    setEtat(ETAT_JOUEURN_JOUE);
  }
    
  // retour au menu précédent
  if (toucheJaune2()) {
    setEtat(ETAT_MENU_INITIAL);
  }
}

// passage au joueur suivant
void auSuivant() {
  
  while (!joueursOK[(joueur+1)%nbJoueurs]) {
    joueur = (joueur+1) % nbJoueurs;
  }
  joueur = (joueur+1) % nbJoueurs;  
  longueur = 0;
  setEtat(ETAT_JOUEURN_JOUE);
}

char joueurGagnant() { // Est-ce qu'on a un gagnant
  char i;
  char gagnant = -1;
  for (i=0;i<nbJoueurs;i++) { 
    if (joueursOK[i]) {
      if (gagnant != -1) { // il ne doit en rester qu'un
        return -1;
      } else {
        gagnant = i+1;
      }
    }
  }
  return gagnant;
}

// le joueur N joue
void etatJoueurNJoue() {
  byte i = 0;
  byte perdu = 0;
  
  enregistreNote();

  if (longueur == level) {
//    delay(1000);
      
    // on vérifie si le joueur a gagné
    for (i=0;i<level;i++) {
      if (sequenceSimon[i] != sequenceJoueur[i]) {
        perdu = 1;
      }
    }
  
    if (perdu == 0) { // Le joueur a bien tape la sequence
      //sonGagne();
      setEtat(ETAT_JOUEURN_AJOUTE_NOTE);
      
    } else { // le joueur s'est trompe, il a perdu
      clearScreen(); 
      screenPrint(0,0,String("Joueur ")+(joueur+1)+String(" perdu!"));
      //screenPrint(0, 1,String("Score:")+(level-1));
      joueursOK[joueur] = 0;
      delay(3000);
      

      // on a un gagnant?
      char gagnant = joueurGagnant(); 
      if (gagnant != -1) { // on a un gagnant!!
        clearScreen();
        screenPrint(0,0,String("Bravo joueur ")+(byte)gagnant+String("!"));
        //hiScore = level - 1;
        screenPrint(0, 1,String("Score:")+(level-1));
        delay(3000);
        setEtat(ETAT_MENU_INITIAL); 
      } else { // pas de gagnant
        auSuivant();      
      }
    }
  } 
}

// Le joueur joue sa note supplémentaire
void etatJoueurNAjouteNote() {
  enregistreNote();
  if (longueur > level) { // le joueur a joue son coup
    sequenceSimon[level] = sequenceJoueur[level];
    level = longueur;
    auSuivant();    
  }
}

//
// boucle principale
//
void loop() {

  if (etat == ETAT_MENU_INITIAL) {
    etatMenuInitial();
  }

  if (etat == ETAT_INIT_PARTIE) { 
    etatInitPartie();
  }
  if (etat == ETAT_SIMON_JOUE) {
    etatSimonJoue();
  }
  if (etat == ETAT_HUMAIN_JOUE) {
    etatHumainJoue();
  }
  if (etat == ETAT_ECOUTE_COUP_HUMAIN) {
    etatEcouteCoupHumain();
  }
  if (etat == ETAT_AFFICHE_HISCORE) {
    etatAfficheHiScore();
  }

  if (etat == ETAT_FREE_PLAY) {
    etatFreePlay();
  }
 
  if (etat == ETAT_MENU_MULTIJOUEURS) {
    etatMenuMultiJoueurs();
  }
  
  if (etat == ETAT_JOUEURN_JOUE) {
    etatJoueurNJoue();
  }

  if (etat == ETAT_JOUEURN_AJOUTE_NOTE) {
    etatJoueurNAjouteNote();
  }
 
}
