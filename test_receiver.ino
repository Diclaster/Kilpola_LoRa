#include <SPI.h>              // include libraries
#include <LoRa.h>

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 6;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin

String outgoing;              // outgoing message

byte msgCount = 0;            // count of outgoing messages
byte localAddress = 0xFF;     // address of this device
byte destination = 0xFF;      // destination to send to
long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends

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

  // LoRa settings
  LoRa.setFrequency(433E6);
  LoRa.setSpreadingFactor(12); // 6-12
//  LoRa.setSignalBandwidth(7.8E3); // 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, 250E3, and 500E3
//  LoRa.setCodingRate4(8); // 5-8
//  LoRa.setTxPower(20); // 0-20
//  LoRa.setGain(0);// 0-AGC 1-6-LNA gain
//  LoRa.enableCrc();

  // LoRa.onReceive(onReceive);

  if (!LoRa.begin(433E6)) {             // initialize ratio at 433 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  // LoRa.receive();
}

void loop() {
  /*
    if (millis() - lastSendTime > interval) {
    String message = "Test message" + String(millis() / 1000); //Serial.readString(); // send a message
    if (message != 0) {
      //interval = random(300);// 2-3 seconds
      sendMessage(message);
      //Serial.println("Me: " + message);
      Serial.println();
      lastSendTime = millis();            // timestamp the message
    }
    }
  */

  // Serial.println("Awaiting...");

  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());

  //delay(500);
}

void sendMessage(String outgoing) {
  //  LoRa.beginPacket();                   // start packet'
  //  LoRa.write(username, 15);
  //  LoRa.write(chat, 25);
  //  LoRa.write(latitude, 10);
  //  LoRa.write(longitude, 10);
  //  LoRa.write(timestamp, 4);
  //  LoRa.write(lifetime,  1);
  //  LoRa.write(text_message.length());        // add text message length
  //  LoRa.print(text_message);                 // add text_message
  //  LoRa.endPacket();                     // finish packet and send it
}
void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return
  // read packet header bytes:
  //LoRa.write(username, 15);


  //  LoRa.write(chat, 25);
  //  LoRa.write(latitude, 10);
  //  LoRa.write(longitude, 10);
  //  LoRa.write(timestamp, 4);
  //  LoRa.write(lifetime,  1);
  //  LoRa.write(text_message.length());        // add text message length
  //  LoRa.print(text_message);                 // add text_message



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

  if (incomingLength != incoming.length()) {   // check length for error
    //Serial.println("error: message length does not match length");
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  //  if (recipient != localAddress && recipient != 0xAA) {
  //    Serial.println("This message is not for me.");
  //    return;                             // skip rest of function
  //  }

  // if message is for this device, or broadcast, print details:
  String sender_name = get_name(sender);

  Serial.println(sender_name + ": RSSI=" + packetRSSI + " SNR=" + snr + " : " + incoming);
  //  Serial.println("Sent to: 0x" + String(recipient, HEX));
  //  Serial.println("Message ID: " + String(incomingMsgId));
  //  Serial.println("Message length: " + String(incomingLength));
  //  Serial.println("Message: " + incoming);
  //  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  //  Serial.println("Snr: " + String(LoRa.packetSnr()));
  //Serial.println();
}
