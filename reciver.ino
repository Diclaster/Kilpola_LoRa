#include <SPI.h>              // include libraries
#include <LoRa.h>

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 6;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin

String outgoing;              // outgoing message

byte localAddress = 0xEE;     // address of this device
byte destination = 0xCC;      // destination to send to

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

   if (!LoRa.begin(433E6)) {             // initialize ratio at 433 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  // LoRa settings
  LoRa.setFrequency(433E6);
  LoRa.setSpreadingFactor(7); // 6-12
  LoRa.setSignalBandwidth(125E3); // 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, 250E3, and 500E3
  LoRa.setCodingRate4(8); // 5-8
  LoRa.setTxPower(20); // 0-20
  LoRa.setGain(0);// 0-AGC 1-6-LNA gain
  LoRa.setPreambleLength(10);
  LoRa.enableCrc();
}

void loop() {
  onReceive(LoRa.parsePacket());
}


void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length
  int packetRSSI = LoRa.packetRssi();
  float snr = LoRa.packetSnr();

  String incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }
  String sender_name = get_name(sender);
  if (incomingLength != incoming.length()) {   // check length for error
    Serial.println("error: message length does not match length");
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
    if (recipient != localAddress) {
      Serial.println("This message is not for me.");
      return;                           // skip rest of function
    }

  // if message is for this device, or broadcast, print details:

  Serial.println(sender_name + ": RSSI=" + packetRSSI + " SNR=" + snr + " MsgID=" + incomingMsgId + " : " + "(" + incoming + ")" );

  delay(500);
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender  address
  LoRa.write(incomingMsgId);                             
  LoRa.endPacket();                     // finish packet and send it
  Serial.println("proverka");
}
