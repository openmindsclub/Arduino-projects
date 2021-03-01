#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN  10

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

byte block = 1;
byte len = 16;
byte passphrase[] = {"Open_Minds_Club!"};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  //Initialize key
  for(int i=0; i<6; i++){
    key.keyByte[i] = 0xff;
  }
}

void loop() {
  //Clear the previous connected tag
  mfrc522.PCD_StopCrypto1();
  
  //Select a tag
  if( !mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial() ){
    return;
  }
  //Authenticate with the selected tag
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Authentication failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //Write the passphrase to the selected card
  status = mfrc522.MIFARE_Write(block, passphrase, len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Writing failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  Serial.println("Writing successful");
}
