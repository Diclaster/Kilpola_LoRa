#include <SPI.h>              // include libraries
#include <LoRa.h>

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 6;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin

String outgoing;              // outgoing message

int msgCount = 0;             // count of outgoing messages
byte localAddress = 0xEE;    // address of this device
byte destination = 0xCC;      // destination to send to
long lastSendTime = 0;        // last send time
int interval = 0;             // interval between sends
int proverka = 0;
int incomingMsgId = 0;
byte repit = 0x00;
String Data;
String incoming;

String get_name(byte sender) {
  String sender_name = "";
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
      sender_name = "Hello";
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
  Serial.begin(9600);                   // initialize serial

  Serial.println("LoRa Duplex");

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin

  Serial.println("LoRa init succeeded.");
  Serial.println("Я " + get_name(localAddress) + ", a это наш чат.");

  Serial.setTimeout(5);

  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  // LoRa settings
  LoRa.setFrequency(433E6);
  LoRa.setSpreadingFactor(9); // 6-12
  LoRa.setSignalBandwidth(20.8E3); // 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, 250E3, and 500E3
  LoRa.setCodingRate4(8); // 5-8
  LoRa.setTxPower(20); // 0-20
  LoRa.setGain(0);// 0-AGC 1-6-LNA gain
  LoRa.setPreambleLength(10);
  LoRa.enableCrc();
}

void loop() {

  if (Serial.available()) {
    if (msgCount > incomingMsgId) {
      Serial.println("Подождите, есть неотправленные сообщения");
      Serial.readString();
    }
    else {
      msgCount++;
      Data = Serial.readString();
      Data[Data.length() - 1] = ' ';
      interval = 0;
    }
  }
  if (millis() - lastSendTime > interval) {
    if (Data != "ping " && msgCount > incomingMsgId) {
      sendMessage(Data);
      Serial.println(get_name(localAddress) + " послал: '" + Data + "' с ID= " + msgCount);
    }

    if (Data == "ping ") {
      sendMessage("ping");
      Serial.println("ping, ID: " + String(msgCount));
    }
    interval = 10000;
    lastSendTime = millis();
  }
  onReceive(LoRa.parsePacket());
}


void onReceive(int packetSize) {
  if (packetSize == 0) return;

  repit = LoRa.read();
  int recipient = LoRa.read();
  byte sender = LoRa.read();
  int packetRSSI = LoRa.packetRssi();
  float snr = LoRa.packetSnr();
  String sender_name = get_name(sender);

  if (repit != 0x2A) {
    Serial.println("Direct message: " + sender_name + ": RSSI=" + packetRSSI + " SNR=" + snr);
    return;
  }
  if (recipient != localAddress) {
    Serial.println("This is my message.");
    return;
  }
  incomingMsgId = LoRa.read();
  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }
  if (incoming != "") {
    Serial.println(sender_name + ": RSSI=" + packetRSSI + " SNR=" + snr + " ID= " + incomingMsgId + " Text: " + incoming);
    incoming = "";
    delay(100);
    sendMessage("");
  }
  else {
    interval = 5000;
    Serial.println();
  }
  if (Data == "ping ") {
    msgCount++;
  }
}
void sendMessage(String outgoing) {
  LoRa.beginPacket();
  LoRa.write(0x00);
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
}
