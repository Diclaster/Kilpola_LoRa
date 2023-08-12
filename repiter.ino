#include <SPI.h>              // include libraries
#include <LoRa.h>

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 6;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin

String incoming;              // outgoing message
byte localAddress = 0xCC;     // address of this device
int interval = 2000;
int proverka = 0;
byte sender;
byte recipient;

String get_name(sender) {
  switch (sender) {
    case 0xAA:
      sender_name = "Камчатка";
      break;
    case 0xBB:
      sender_name = "Base";
      break;
    case 0xCC:
      sender_name = "Ретранслятор1";
      break;
    case 0xEE:
      sender_name = "Ретранслятор2";
      break;
    case 0xDD:
      sender_name = "Ретранслятор3";
      break;
    default:
      sender_name = "Некто";
  }
  return sender_name;
}


void setup() {
  Serial.begin(9600);                   // initialize serial

  Serial.println("LoRa Duplex");

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin



  Serial.println("LoRa init succeeded.");
  Serial.println("Я " + get_name(localAddress) + ", a это наш чат.");

  Serial.setTimeout(5);

  if (!LoRa.begin(433E6)) {             // initialize ratio at 433 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  // LoRa settings
  LoRa.setFrequency(433E6);
  LoRa.setSpreadingFactor(9); // 6-12
  LoRa.setSignalBandwidth(7.8E3); // 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, 250E3, and 500E3
  LoRa.setCodingRate4(8); // 5-8
  LoRa.setTxPower(20); // 0-20
  LoRa.setGain(0);// 0-AGC 1-6-LNA gain
  LoRa.setPreambleLength(10);
  LoRa.enableCrc();
}

void loop() {
  priem(LoRa.parsePacket());

  if (proverka == 1) {
    if (millis() - lastSendTime > interval) {
      Text = incoming;
      sent(false);
      Serial.println("от " + get_name(sender) + " ретранслировано сообщение с ID= " + ID + " :" + incoming);
      lastSendTime = millis()
    }
  }
}
void priem(int packetSize) {
  if (packetSize == 0) return;

  boolean soobsh = LoRa.read();
  recipient = LoRa.read();
  sender = LoRa.read();
  ID = LoRa.read();
  sender_name = get_name(sender);
  int packetRSSI = LoRa.packetRssi();
  float snr = LoRa.packetSnr();

  if (soobsh == false) {
    proverka = LoRa.read();
    proverka();
  }
  else {
    while (LoRa.available()) {
      incoming += (char)LoRa.read();
    }
    if (incomingLength != incoming.length()) {
      Serial.println("error: message length does not match length");
      return;
    }
    Serial.println(sender_name + ": MsgID = " + ID + " RSSI = " + packetRSSI + " SNR = " + snr + " : " + "(" + incoming + ")");
    delay(100);
    proverka=1;
    sent(true);
  }
}

void proverka() {
  if (ID != msgCount) {
    Serial.println("staroe");
    return;
  }
  Serial.println(sender_name + ": RSSI = " + packetRSSI + " SNR = " + snr + " proverkaID: " + ID + " доставлено");
  proverka=0;
}

void sent(boolean isproverka) {
  recepient = sender;
  LoRa.beginPacket();
  LoRa.write(recepient);
  LoRa.write(senderme);
  LoRa.write(ID);

  if (isproverka == true) {
    LoRa.write(1);
    LoRa.endPacket();
  }
  else {
    LoRa.write(Text.length());
    LoRa.print(Text);
    LoRa.write(Hopes++)
    LoRa.endPacket;
  }
}
