#include <IRremote.h> //Jag inkluderar IRremote biblioteket

//Här under definerar jag alla pinnar och variabler.
IRrecv irrecv(RECV_PIN);
decode_results results;
unsigned long key_value = 0;

const int RECV_PIN = 7;
const int motorPin1 = 5;
const int motorPin2 = 6;
const int motorPin3 = 10;
const int motorPin4 = 9;
const int ledPin = 13;

const int trigPin = 2;  
const int echoPin = 3; 
float distance; 
float duration;

bool selfdrive = false;
int selfDriveCount = 0;


void setup(){ //Här definierar jag vilka pinnar som ska vara output och vilka som ska vara input.
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600); //Initierar serial monitorn
  irrecv.enableIRIn(); //Initierar IRremoten
  digitalWrite(ledPin, LOW); //Sätter så att LEDlampan alltid börjar på LOW
} 


void loop(){//Här under initierar jag sensorn så att den söker av
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

//Här under gör jag så att "Ingen signal mottagen" blir utskrivet i serial monitorn om sensorn inte känner av ett avstånd
  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) {
    Serial.println("Ingen signal mottagen.");
    return;
  }

  distance = (duration*.0343)/2; //Definerar variablen distance så att den är i cm. Jag delar med två för att få avtåndet till ett objekt. Då vågorna behöver studsa på väggen och tillbaka.

//Här under printar jag ut avståndet i serial monitorn.
  Serial.print("Distance: ");
  Serial.println(distance);
  

// Startar en if-loop som känner av HEX-koderna från IRremoten. Håller man in en knapp på fjärrkontrollen så blir det först ett HEX värde, sen 0xFFFFFFF. Därför lagrar jag värdena i key.value. På detta sätt så blir 0xFFFFFFF till seanast tryckte HEX-värde. 
  if (irrecv.decode(&results)){
    if (results.value == 0xFFFFFFFF){
      results.value = key_value;
      // Serial.println(results.value, HEX); //Felsökning HEX-koder
    }

//Initierar en switch-sats som känner av en HEX-kod som styr selfDrive.
    switch(results.value){
      case 0xFF02FD: //selfdrive
      selfDriveCount += 1;
      Serial.println("SD:");
      Serial.println(selfDriveCount); 
      break;
    }
//Om selfDrive är avstängt så kan man styra bilen manuellt med fjärrkontrollen. Pga H-bryggan så finns det fyra motorPin till två motorer. Är pin1 hög och pin 2 låg åker motor 1 framåt.
    if (selfdrive == false){
      switch(results.value){
        case 0xFF629D: //Framåt
          digitalWrite(motorPin1, HIGH);  
          digitalWrite(motorPin2, LOW); 
          digitalWrite(motorPin3, LOW); 
          digitalWrite(motorPin4, HIGH); 
          break; 
        case 0xFFA857: //Bakåt
          digitalWrite(motorPin1, LOW);  
          digitalWrite(motorPin2, HIGH); 
          digitalWrite(motorPin3, HIGH); 
          digitalWrite(motorPin4, LOW); 
          break; 
        case 0xFFC23D: //Höger
          digitalWrite(motorPin1, LOW);  
          digitalWrite(motorPin2, HIGH); 
          digitalWrite(motorPin3, LOW); 
          digitalWrite(motorPin4, HIGH); 
          break;
        case 0xFF22DD: //Vänster
          digitalWrite(motorPin1, HIGH);  
          digitalWrite(motorPin2, LOW); 
          digitalWrite(motorPin3, HIGH); 
          digitalWrite(motorPin4, LOW);
          break; 
      }
    }

    key_value = results.value;
    irrecv.resume(); 
  }
  else{
   // Om inga HEX-koder tas emot så ska alla motorer vara av.
   digitalWrite(motorPin1, LOW);  
   digitalWrite(motorPin2, LOW); 
   digitalWrite(motorPin3, LOW); 
   digitalWrite(motorPin4, LOW); 
  } 

  delay(250);

  //If-loop som sätter på selfDrive om selfDrivecount är större eller lika med 3 och selfdrive är false. Tämnder även en lampa så det ska vara tydligare när selfdrive är på.
  if (selfDriveCount >= 3 && selfdrive == false){
      selfdrive = true;
      selfDriveCount = 0;
      digitalWrite(ledPin, HIGH);
  }
  //Samma princip som if-loopen över, fast den stänger av selfdrive. I denna behöver selfDriveCount bara nå 2, då signalerna blockerades av delayerna som finns i selfDrive algoritmen.
  else if(selfDriveCount >= 2 && selfdrive == true){
      selfdrive = false;
      selfDriveCount = 0;
      digitalWrite(ledPin, LOW);
  }
  
  //Självkörning. Använder avståndsvariablen för att känna av om vägen är fri.
  if (selfdrive == true){
    if (distance >= 50){
      digitalWrite(motorPin2, LOW);  //Framåt
      digitalWrite(motorPin3, LOW); 
      digitalWrite(motorPin4, HIGH); 
      digitalWrite(motorPin1, HIGH); 
      delay(1000);
    }
    else{
       digitalWrite(motorPin1, LOW);  //Bakåt
       digitalWrite(motorPin2, HIGH); 
       digitalWrite(motorPin3, HIGH); 
       digitalWrite(motorPin4, LOW); 
       delay(1000);
       digitalWrite(motorPin1, LOW);  //Höger
       digitalWrite(motorPin2, HIGH); 
       digitalWrite(motorPin3, LOW); 
       digitalWrite(motorPin4, HIGH);
       delay(500);
    }
  }
}


  
