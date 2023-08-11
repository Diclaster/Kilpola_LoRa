#include <SPI.h>              // include libraries
#include <LoRa.h>

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 6;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin

byte msgCount = 0;            // count of outgoing messages
long lastSendTime = 0;        // last send time
int interval = 5000;          // interval between sends
String Text;
String incoming;
byte proverka = 1;
byte ID = 0;
byte senderme = 0xCC;
byte recipientme = 0xDD;
String sender_name = "";
byte sender;
byte recipient;

String get_name(sender) {
  switch (sender) {
    case 0xAA:
      sender_name = "Медведь";
      break;
    case 0xBB:
      sender_name = "Краб";
      break;
    case 0xCC:
      sender_name = "Камчатка";
      break;
    case 0xDD:
      sender_name = "Ретранслятор";
      break;
    case 0xEE:
      sender_name = "Base";
      break;
    default:
      sender_name = "Некто";
  }
  return sender_name;
}


void setup() {
  Serial.begin(9600);
  Serial.println("LoRa Duplex");
  LoRa.setPins(csPin, resetPin, irqPin);
  Serial.println("LoRa init succeeded.");
  Serial.println("Я " + get_name(senderme) + ", a это наш чат.");

  Serial.setTimeout(5);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);
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

  if (proverka == 1) {
    if (millis() - lastSendTime > interval) {
      Text = "Test";
      sent(false);
      Serial.println(get_name(localAddress) + ": sent " + message + " ID: " + msgCount);
      lastSendTime = millis();
    }
  }
  priem(LoRa.parsePacket());
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

  if (recipient != senderme) {
    Serial.println("This message is not for me.");
    return;
  }

  if (soobsh == false) {
    proverka = LoRa.read();
    proverka();
  }
  else {
    while (LoRa.available()) {
      incoming += (char)LoRa.read();
    }
    Serial.println(sender_name + ": MsgID = " + ID + " RSSI = " + packetRSSI + " SNR = " + snr + " : " + "(" + incoming + ")");
    sent(true);
  }
}

void proverka() {
  if (ID != msgCount) {
    Serial.println("staroe");
    return;
  }
  if (proverka == 0) Serial.println(sender_name + ": RSSI = " + packetRSSI + " SNR = " + snr + " proverkaID: " + ID + " доставлено");
  else  msgCount++;
}

void sent(boolean isproverka) {
  LoRa.beginPacket();
  LoRa.write(destination);
  LoRa.write(localAddress);
  LoRa.write(msgCount);

  if (isproverka == true) {
    LoRa.write(1);
    LoRa.endPacket();
  }
  else {
    LoRa.write(Text.length());
    LoRa.print(Text);
    LoRa.endPacket;
  }
}
