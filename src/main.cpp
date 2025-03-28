// BU KODUN ÇALIŞMASI ŞU AN ŞÜPHELİ. ŞİFRELEME ÇALIŞMALARI DEVAM EDİYOR.

// SON EDIT 10.02.2025
// MUSTAFA ALPER KAYA TARAFINDAN OLUŞTURULDU
// nrf24 modüllerini kullanan trimli 8 kanallı RC kumanda alıcı kodu
// TRİM SERVONUN MENZİLİNİ AZALTACAĞI İÇİN SERVOLARI ADAM GİBİ TAKIN TRİMİ DE ADAM GİBİ YAPIN

/*
####################################
YAPILACAKLAR/YAPIMI DEVAM EDENLER (önem sırasına göre)
V-Kuyruk kuyruk tipine göre servo sinyal karıştırması
ChaCha20-Poly1305 Şifreleme Koruması
####################################
*/

/*
#####################################################         KANAL SIRALAMASI         #####################################################
  Keyfinize göre açın, kapatın, değiştirin.
  1 = aileron             düz
  2 = elevator            düz
  3 = gaz                 düz
  4 = rudder              düz
  5 = SWA(SPDT)           düz
  6 = SWB(SPDT)           düz
  7 = SWC(SPDT_MSM)       düz
  8 = SWD(SPDT)           düz
######################         DOĞRU ÇALIŞMASI İÇİN LÜTFEN HEM VERİCİDE HEM DE ALICIDA AYNI SIRALAMAYI YAPIN         #######################
*/

#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <Servo.h>


void failSafe(); // Verici ile iletişim koptuğunda Fail-Safe güvenlik önlemini gerçekleştiren fonksiyon.
int veriSayisi = 8;
Servo servoAileron;
Servo servoElevator;
Servo servoGaz;
Servo servoRudder;
Servo servoAuxA;
Servo servoAuxB;
Servo servoAuxC;
Servo servoAuxD;

#define aileronPinRX 2
#define elevatorPinRX 3
#define gazPinRX 4
#define rudderPinRX 5 
#define auxA 6 
#define auxB 7
#define auxC 8
#define auxD 9

unsigned long basarili = 0;
unsigned int failsafeAralik = 1000; // fail-safe devreye girmesi için gereken süre. 

const byte nrf24kod[5] = {'s','i','f','r','e'};
RF24 radio(A0,10);
int kanal[8]; // kanal sayısı köşeli parantezin içindeki sayıya bağlı. (İNT TİPİ İLE MAX 16)
void setup() {
  
  servoAileron.attach(aileronPinRX);
  servoElevator.attach(elevatorPinRX);
  servoGaz.attach(gazPinRX);
  servoRudder.attach(rudderPinRX);               
  servoAuxA.attach(auxA);
  servoAuxB.attach(auxB);
  servoAuxC.attach(auxC);
  servoAuxD.attach(auxD);
  
  radio.begin();
  radio.openReadingPipe(1,nrf24kod);
  radio.setChannel(76);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW); // güç çıkışı şu an düşük durumda, mesafeyi artırmak için ...(RF24_PA_HIGH/MAX) yapılmalı.
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    basarili = millis();
    radio.read(&kanal,sizeof(kanal));
    servoAileron.writeMicroseconds  (kanal[0]);
    servoElevator.writeMicroseconds (kanal[1]);
    servoGaz.writeMicroseconds      (kanal[2]);
    servoRudder.writeMicroseconds   (kanal[3]);
    servoAuxA.writeMicroseconds     (kanal[4]);
    servoAuxB.writeMicroseconds     (kanal[5]);
    servoAuxC.writeMicroseconds     (kanal[6]);
    servoAuxD.writeMicroseconds     (kanal[7]);
  }
  if (millis() - basarili >= failsafeAralik) {
    //failSafe();
  }
}

void failSafe() {
  kanal[0] = 2000;
  kanal[1] = 2000;
  kanal[2] = 1000;
  kanal[3] = 2000;
  kanal[4] = 1500;
  kanal[5] = 1500;
  kanal[6] = 1500;
  kanal[7] = 1500;

  servoAileron.writeMicroseconds(kanal[0]);
  servoElevator.writeMicroseconds(kanal[1]);
  servoGaz.writeMicroseconds(kanal[2]);
  servoRudder.writeMicroseconds(kanal[3]);
  servoAuxA.writeMicroseconds(kanal[4]);
  servoAuxB.writeMicroseconds(kanal[5]);
  servoAuxC.writeMicroseconds(kanal[6]);
  servoAuxD.writeMicroseconds(kanal[7]);

  while (millis() - basarili > failsafeAralik) {
    if (radio.available()) {
      return;
    }  
  }
}
  