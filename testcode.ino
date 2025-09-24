#include <ELECHOUSE_CC1101_SRC_DRV.h>

#define SERIAL_BAUD 115200

byte buffer[16] = {};
bool inJammingMode = true;

void start_jammer() {
  ELECHOUSE_cc1101.setDRate(4.8);   // kbps
  ELECHOUSE_cc1101.setRxBW(58);     // kHz

  // Put into TX mode
  ELECHOUSE_cc1101.SetTx();
  ELECHOUSE_cc1101.setPA(-10);

  // Set infinite packet mode (streaming)
  ELECHOUSE_cc1101.SpiWriteReg(0x08, 0x02);  
  inJammingMode = true;

  Serial.println("Start Jamming Mode");
}

void start_transmitter() {
  ELECHOUSE_cc1101.setCCMode(1);       // set config for internal transmission mode.
  ELECHOUSE_cc1101.setDRate(100.0);
  ELECHOUSE_cc1101.setSyncMode(2);  // Combined sync-word qualifier mode. 0 = No preamble/sync. 1 = 16 sync word bits detected. 2 = 16/16 sync word bits detected. 3 = 30/32 sync word bits detected. 4 = No preamble/sync, carrier-sense above threshold. 5 = 15/16 + carrier-sense above threshold. 6 = 16/16 + carrier-sense above threshold. 7 = 30/32 + carrier-sense above threshold.
  ELECHOUSE_cc1101.setPA(10);      // set TxPower. The following settings are possible depending on the frequency band.  (-30  -20  -15  -10  -6    0    5    7    10   11   12) Default is max!
  ELECHOUSE_cc1101.setCrc(0);     // 1 = CRC calculation in TX and CRC check in RX enabled. 0 = CRC disabled for TX and RX.


  inJammingMode = false;
  Serial.println("Start Transmit Mode");
}

bool isHexChar(char c) {
  return (c >= '0' && c <= '9') ||
         (c >= 'A' && c <= 'F') ||
         (c >= 'a' && c <= 'f');
}

bool write_hex_to_buffer(String line) {
  for (int i = 0; i < 32; i++) {
    if (!isHexChar(line.charAt(i))) { 
      return false; // invalid character
    }
  }

  for (int i = 0; i < 16; i++) {
    String hexByte = line.substring(i * 2, i * 2 + 2);
    buffer[i] = (byte) strtoul(hexByte.c_str(), NULL, 16);
  }
  return true;
}

void setup() {
    Serial.begin(115200);
    
    if (ELECHOUSE_cc1101.getCC1101()){         // Check the CC1101 Spi connection.
      Serial.println("Connection OK");
    } else {
      Serial.println("Connection Error");
    }

    ELECHOUSE_cc1101.Init();
    ELECHOUSE_cc1101.setMHZ(433.27);   // Here you can set your basic frequency. The lib calculates the frequency automatically (default = 433.92).The cc1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ. Read More info from datasheet.
    ELECHOUSE_cc1101.setModulation(0);  // set modulation mode. 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK.
    start_jammer();
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();

    if (line.equalsIgnoreCase("stop")) {
      // Start the jammer
      start_jammer();
    } else if (line.length() == 32) {

      // 32 byte hex string
      if (write_hex_to_buffer(line)) {
        start_transmitter();
      } else {
        Serial.println("Not a valid hex string.");
      }
    } else {
      Serial.println("need a 32 byte hex string to transmit!");
      Serial.println("    EX: e8d555554d1d7171671f45ee828096bf");
    }
  }

  if (inJammingMode) {
    if (ELECHOUSE_cc1101.SpiReadStatus(0x3F) < 60) {
      ELECHOUSE_cc1101.SpiWriteReg(0x3F, 0xFF); 
    }
  } else {
    for (int i = 0; i < 30; i++) {
      ELECHOUSE_cc1101.SendData(buffer, 16, 500);
    }
    Serial.println("Packet sent");
    delay(5000);
  }
}
