#include <Key.h>
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
//posibil sa fi declarat in plus variabile/constante pentru unele componente,le-am folosit pentru testing.
SoftwareSerial mySerial(30, 31); // RX, TX , am declarat un serial pentru partea de bluetooth care nu a mai functionat
//global am declarat pinii pe care ii folosesc pentru toate componentele
const int trig_pin = 9;
const int echo_pin = 10;
const int buzzer_pin = 8;
const int led_pin1=7;
const int led_pin2=6;
String message;
//am setat initial cele doua leduri sa fie stinse(LOW)
int ledstate=LOW;
long duration;
int distance, initialDistance, currentDistance, i;//acestea sunt niste variabile declarate pentru senzorul de distanta(ultrasonic),distanta este distanta calculata,am inca o distanta initiala si o distanta curenta
//initial si current distance le am puse in cazul in care in momentul in care conectez placuta la sursa de curent poate in fata senzorului la distanta x se afla un obiect,este preventiv ca senzorul sa nu porneasca alarma instant
int screenOffMsg = 0;//am declarat aceasta variabila ca sa nu se suprapuna datele transmise pe display,de exemplu ecranul initial cu cel in care introduc parola
String password = "3651";//parola prealeasa
String tempPassword;//variabila pentru stocarea parolei introduse dupa ce pornesc programul(cand vreau sa o schimb sau cand se introduce)
char bluetoothVal;//pentru bluetooth
boolean activated = false;//daca alarma este activa in momentul acela sau nu

boolean activateAlarm = false;
boolean alarmActivated = false;
boolean enteredPassword;//starea parolei introduse
boolean passChangeMode = false;
boolean passChanged = false;
const byte ROWS = 4;//numar de randuri
const byte COLS = 4;//numar de coloane
char keypressed;
//simbolurile pentru keypad de 4x4
char keyMap[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {14, 15, 16, 17};//randuri
byte colPins[COLS] = {18, 19, 20, 21};//coloane
Keypad myKeypad = Keypad( makeKeymap(keyMap), rowPins, colPins, ROWS, COLS);//aici imi declar keypadul si imi pun toate butoanele,randurile si coloanele folosite de pe el
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;//pentru display 16x2
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);//declar displayul
long getDistance(){//aceasta functie este folosita pentru senzorul de distanta ca sa aproximeze distanta buna
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);
  duration = pulseIn(echo_pin, HIGH);
  distance = duration*0.034/2;
  return distance;
}
void setup()//am declarat in/o
{
 Serial.begin(9600);
//  Serial2.begin(9600);
pinMode(31, OUTPUT);
mySerial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  pinMode(buzzer_pin, OUTPUT);
  pinMode(led_pin1,OUTPUT);
  pinMode(led_pin2,OUTPUT);
  
}
void enterPassword_blue() {//inca nu functioneaza
//  Serial.write('A');
mySerial.write("Suna alarma ma!");

  int k = 5;
  tempPassword = "";
  activated = true;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" *** ALARM *** ");
  lcd.setCursor(0, 1);
  lcd.print("Pass>");
  while (activated) {  
//    while(Serial.available())
//  {
//    bluetoothVal=Serial.read();
//  }

    if(mySerial.available())
  {
    bluetoothVal=mySerial.read();
  }
  Serial.println(bluetoothVal);

      if (bluetoothVal == '0' || bluetoothVal == '1' || bluetoothVal == '2' || bluetoothVal == '3' ||
          bluetoothVal == '4' || bluetoothVal == '5' || bluetoothVal == '6' || bluetoothVal == '7' ||
          bluetoothVal== '8' || bluetoothVal == '9' ) {
        tempPassword += bluetoothVal;
        lcd.setCursor(k, 1);
        lcd.print("*");
        k++;
      }
     if (k > 9 || bluetoothVal == '#') {
        tempPassword = "";
        k=5;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(" *** ALARM *** ");
        lcd.setCursor(0,1);
        lcd.print("Pass>");
      }
      if ( bluetoothVal == '*') {
        if ( tempPassword == password ) {
          activated = false;
          alarmActivated = false;
          noTone(buzzer_pin);
          screenOffMsg = 0; 
        }
        else if (tempPassword != password) {
          lcd.setCursor(0,1);
          lcd.print("Wrong! Try Again");
          delay(2000);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(" *** ALARM *** ");
          lcd.setCursor(0,1);
          lcd.print("Pass>");
        }
      }    
    }
}
void enterPassword() {//functie pentru parola
  int k = 5;
  tempPassword = "";
  activated = true;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" *** ALARM *** ");
  lcd.setCursor(0, 1);
  lcd.print("Pass>");
  while (activated) {//in momentul in care alarma se activeaza o sa imi citeasca parola introdusa de mine pe keypad si o va compara cu parola initiala sau cea schimbata ulterior
    keypressed = myKeypad.getKey();
    if (keypressed != NO_KEY) {
      if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' ||
          keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' ||
          keypressed == '8' || keypressed == '9' ) {
        tempPassword += keypressed;
        lcd.setCursor(k, 1);
        lcd.print("*");
        k++;
      }
    }
     if (k > 9 || keypressed == '#') {//sterge parola daca cumva am facut o greseala,prin apasarea taster '#'
        tempPassword = "";
        k=5;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(" *** ALARM *** ");
        lcd.setCursor(0,1);
        lcd.print("Pass>");
      }
      if ( keypressed == '*') {//este folosit ca un buton de ok
        if ( tempPassword == password ) {
          activated = false;
          alarmActivated = false;
          noTone(buzzer_pin);
          screenOffMsg = 0; 
        }
        else if (tempPassword != password) {//daca parola introdusa de mine nu corespunde va afisa un mesaj de eroare si o va sterge,avand posibilitatea de a scrie alta parola
          lcd.setCursor(0,1);
          lcd.print("Wrong! Try Again");
          delay(2000);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(" *** ALARM *** ");
          lcd.setCursor(0,1);
          lcd.print("Pass>");
        }
      }    
    }
}
void loop()
{
//  mySerial.write('a');
//  mySerial.write('A');
 if (!alarmActivated) {//daca alarma nu este pornita mi se afiseasza pe ecran otiunile
    if (screenOffMsg == 0 ){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("A - Activate");//activare alarma,acesta activeaza alarma dar nu o si porneste
      lcd.setCursor(0,1);
      lcd.print("B - Change Pass");//daca vrem sa schimbam parola
      screenOffMsg = 1;
    }
    keypressed = myKeypad.getKey();
     if (keypressed =='A'){        
      tone(buzzer_pin, 1000, 200);//feedback sonor pentru apasarea tastei A
      activateAlarm = true;            
    }
    else if (keypressed =='B') {//daca apasam B avem posibiliatea sa schimbam parola,parola initiala va fi inlocuita
      lcd.clear();
      int i=1;
      tone(buzzer_pin, 2000, 100);//feedback sonor pentru apasarea tastei B
      tempPassword = "";
      lcd.setCursor(0,0);
      lcd.print("Current Password");
      lcd.setCursor(0,1);
      lcd.print(">");
      passChangeMode = true;
      passChanged = true;   
      while(passChanged) {      
      keypressed = myKeypad.getKey();
      if (keypressed != NO_KEY){
        if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' ||
            keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' ||
            keypressed == '8' || keypressed == '9' ) {
         tempPassword += keypressed;
         lcd.setCursor(i,1);
         lcd.print("*");
         i++;
         tone(buzzer_pin, 2000, 100);
        }
      }
      if (i > 5 || keypressed == '#') {//folosim sa stergem ce am scris
        tempPassword = "";
        i=1;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Current Password");
        lcd.setCursor(0,1);
        lcd.print(">"); 
      }
      if ( keypressed == '*') {//dupa ce am introdus parola noastra initiala,prin apasarea '*' vom putea trece la pasul urmator unde vom alege o parola noua
        i=1;
        tone(buzzer_pin, 2000, 100);
        if (password == tempPassword) {
          tempPassword="";
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Set New Password");
          lcd.setCursor(0,1);
          lcd.print(">");
          while(passChangeMode) {
            keypressed = myKeypad.getKey();
            if (keypressed != NO_KEY){
              if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' ||
                  keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' ||
                  keypressed == '8' || keypressed == '9' ) {
                tempPassword += keypressed;
                lcd.setCursor(i,1);
                lcd.print("*");
                i++;
                tone(buzzer_pin, 2000, 100);
              }
            }
            if (i > 5 || keypressed == '#') {
              tempPassword = "";
              i=1;
              tone(buzzer_pin, 2000, 100);
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Set New Password");
              lcd.setCursor(0,1);
              lcd.print(">");
            }
            if ( keypressed == '*') {
              i=1;
              tone(buzzer_pin, 2000, 100);
              password = tempPassword;
              passChangeMode = false;
              passChanged = false;
              screenOffMsg = 0;
            }            
          }
        }
      }
    }
   }
 }
if (activateAlarm) {//dupa apasarea tastei A,alarma se activeaza,vom avea 10 secunde pana cand senzorul de distanta va incepe sa fie functional
  //am adaugat un feedback audio pentru cele 10 secunde
  //cat timp alarma este activata,pe lcd vom vedea un mesaj
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Alarm will be");
    lcd.setCursor(0,1);
    lcd.print("activated in");
   
    int countdown = 9;
    while (countdown != 0) {
      lcd.setCursor(13,1);
      lcd.print(countdown);
      countdown--;
      tone(buzzer_pin, 700, 100);
      delay(1000);
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Alarm Activated!");
    initialDistance = getDistance();
    activateAlarm = false;
    alarmActivated = true;
  }
  //daca senzorul de distanta detecteaza ceva,alarma va fi pornita
  //in if-ul de mai jos se prezinta actiunile care vor fi executate atunci cand alarma incepe sa sune
  //va fi un micuti delay in care niste leduri blinkuie iar mmai apoi va incepe un sunet de 1000khz care nu se va opri pana cand functia
  //enterPassword nu isi termina executia
if (alarmActivated == true){
      currentDistance = getDistance() + 10;
      if ( currentDistance < initialDistance) {
        digitalWrite(led_pin1,HIGH);
        delay(1);
        digitalWrite(led_pin2,HIGH);
        delay(100);
        digitalWrite(led_pin1,LOW);
        delay(1);
        digitalWrite(led_pin2,LOW);
        delay(100);
        digitalWrite(led_pin1,HIGH);
        delay(1);
        digitalWrite(led_pin2,HIGH);
        delay(100);
        digitalWrite(led_pin1,LOW);
        delay(1);
        digitalWrite(led_pin2,LOW);
        delay(100);
        digitalWrite(led_pin1,HIGH);
        delay(1);
        digitalWrite(led_pin2,HIGH);
        delay(100);
        digitalWrite(led_pin1,LOW);
        delay(1);
        digitalWrite(led_pin2,LOW);
        delay(100);
        digitalWrite(led_pin1,HIGH);
        delay(1);
        digitalWrite(led_pin2,HIGH);
        delay(100);
        digitalWrite(led_pin1,LOW);
        delay(1);
        digitalWrite(led_pin2,LOW);
        delay(100);
        digitalWrite(led_pin2,HIGH);
        digitalWrite(led_pin1,HIGH);
        tone(buzzer_pin, 1000);
        lcd.clear();
        enterPassword();
       // enterPassword_blue();
        digitalWrite(led_pin2,LOW);
        digitalWrite(led_pin1,LOW);
      }
    }
  }
    