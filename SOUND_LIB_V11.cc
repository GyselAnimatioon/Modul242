/*
******************************************************************************
 Datei:       SOUND_LIB.cc
 Version:     1.1
 Autor:       Moritz Habegger & Micha Schena & F.Rhyner
			  Übernommen und verbessert von (Munsei)
              Auszüge des Quelltextes sind aus beep von Ulli Sommer

 Datum:       13.5.2015
 Funktion:    Es werden Konstanten, Variablen und Funktionen zur Erzeugung
              von Tönen auf dem Lautsprecher des Pro-Bot128
              zur Verfügung gestellt
******************************************************************************
*/

//----- Konstanten definieren -------------------------------------------------

//1. Oktave
#define ToneC1 440     //261Hz
#define ToneCis1 413   //277Hz
#define ToneD1 393     //293Hz
#define ToneDis1 371   //311Hz
#define ToneE1 350     //329Hz
#define ToneF1 330     //349Hz
#define ToneFis1 312   //369Hz
#define ToneG1 294     //392Hz
#define ToneGis1 277   //415Hz
#define ToneA1 262     //440Hz
#define ToneAis1 247   //466Hz
#define ToneH1 234     //493Hz

//2. Oktave
#define ToneC2 220     //523Hz
#define ToneCis2 208   //554Hz
#define ToneD2 196     //587Hz
#define ToneDis2 185   //622Hz
#define ToneE2 175     //659Hz
#define ToneF2 165     //698Hz
#define ToneFis2 156   //739Hz
#define ToneG2 147     //784Hz
#define ToneGis2 138   //830Hz
#define ToneA2 131     //880Hz
#define ToneAis2 125   //932Hz
#define ToneH2 117     //987Hz

//3. Oktave
#define ToneC3 110     //1046Hz
#define ToneCis3 104   //1109Hz
#define ToneD3 98     	//1174Hz
#define ToneDis3 92   	//1244Hz
#define ToneE3 87     	//1318Hz
#define ToneF3 82     	//1397Hz
#define ToneFis3 78   	//1480Hz
#define ToneG3 73     	//1568Hz
#define ToneGis3 69   	//1661Hz
#define ToneA3 65     	//1760Hz
#define ToneAis3 61   	//1864Hz
#define ToneH3 58     	//1975Hz

//4. Oktave
#define ToneC4 55		//2093Hz

//----- Globale Variablen definieren ------------------------------------------

float BPM, NF, NHD, NH, NQD, NQ, NQT, NED, NE, NET, NS;

//BPM: Beats per Minute / Takt
//NF:  full note / Ganze Note
//NHD: half note dotted / Halbe Note punktiert
//NH:  half note / Halbe Note
//NQD: quater note dotted / Viertelnote punktiert
//NQ:  quater note / Viertelnote
//NE:  eigth note dotted / Achtelnote punktiert
//NED: eigth note / Achtelnote
//NS:  sixteenth note / Sechzehntelnote

//----- Geschwindigkeit setzen und Notenlaengen berechnen ---------------------
void SetTempo(float tempo)
{
    BPM = tempo;
    NF = (60 / BPM) * 1000 * 4;     //full note            ganze Note
    NHD = (60 / BPM) * 1000 * 3;    //half note dotted     punktierte halbe Note
    NH = (60 / BPM) * 1000 * 2;     //half note            halbe Note
    NQD = (60 / BPM) * 1000 * 1.5;  //quater note dotted   punktierte Viertelnote
    NQ = (60 / BPM) * 1000;         //quater note          Viertelnote
    NQT = (60 / BPM) *1000 * 0.66;  //quater triole note   Vierteltriole
    NED = (60 / BPM) * 1000 * 0.75; //eigth note dotted    punktierte Achtelnote
    NE = (60 / BPM) * 1000 * 0.5;   //eigth note           Achtelnote
    NET = (60 / BPM) * 1000 * 0.33; //eigth triole note    Achteltriole
    NS = (60 / BPM) * 1000 * 0.25;  //sixteenth note       Sechszehntelnote
}

//----- Ton in gewünschter Länge und Höhe spielen -----------------------------
void PlayNote(word tonePitch, float noteLength)
{
    Timer_T0FRQ(tonePitch,PS_64); 	//Frequenz mittels des 8 Timers T0 erzeugen
    DELAY_MS(CfToInt(noteLength));  //Länge abwarten
    Timer_T0Stop();                	//Frequenz stoppen
    Timer_T0FRQ(440, 1);
}

//----- Eine Pause "spielen" --------------------------------------------------
void PlayPause(float noteLength)
{
    DELAY_MS(CfToInt(noteLength));
}

//----- Datentyp float in integer konvertieren -------------------------------
int CfToInt(float floatValue) {
  int intValue;

  intValue = floatValue;
  return intValue;
}