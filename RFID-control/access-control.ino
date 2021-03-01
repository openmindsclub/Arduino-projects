
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define RST_PIN 9
#define SS_PIN  10

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

Servo serv;

int pos;
byte len, block = 1;
byte buffer[18];
byte passphrase[] = {"Open_Minds_Club!"};

int toner = 6;
int ledRed = A1;
int ledGreen = A2;


void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  serv.attach(5);

  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);

  //Initialize key
  for(int i=0; i<6; i++){
    key.keyByte[i] = 0xff;
  }
}


void loop() {  
  
  //Turn off leds
  digitalWrite(ledGreen, LOW);
  digitalWrite(ledRed, LOW);

  //Initialize buffer
  for (byte i=0; i<18; i++)
  {
    buffer[i] = 0;
  }
  
  //Clear the previous connected tag
  mfrc522.PCD_StopCrypto1();

  //Select a tag
  if( !mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial() ){
    return;
  }

  //Authenticate with the selected tag 
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));

  //check if the authentification with the selected tag was successful
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Authentication failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //Read block 1 from the selected card
  len = 18;
  status = mfrc522.MIFARE_Read(block, buffer, &len);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //Print the UID of the selected tag
  Serial.print("Card UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  
  //Print the content of block 1 of the selected tag
  Serial.print("  Content of block 1: ");
  for (int i=0; i<16; i++)
  {
    Serial.write(buffer[i]);
  }
  Serial.println();

  //Check if the content of block 1 of the selected tag match the passphrase
  if(!memcmp(buffer, passphrase, 16))
  {
    Serial.println("Authentification successful");

    digitalWrite(ledGreen, HIGH);
    for (pos = 90; pos >= 0; pos -= 1) { 
    serv.write(pos);
    delay(15);
  }
  delay(1000);
  for (pos = 0; pos <= 90; pos += 1) {
    serv.write(pos);
    delay(15);
    }
    digitalWrite(ledGreen, LOW);
  }
  else
  {
    Serial.println("Authentification failed");

    tone(toner, 300, 200);
    digitalWrite(ledRed, HIGH);
    delay(1000);
    digitalWrite(ledRed, LOW);
    noTone(toner);
    
  }
}
