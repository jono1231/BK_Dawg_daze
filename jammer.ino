#include <ELECHOUSE_CC1101_SRC_DRV.h>

#define SERIAL_BAUD 115200

void setup() {
  Serial.begin(SERIAL_BAUD);

  // Check the CC1101 Spi connection.
  if (ELECHOUSE_cc1101.getCC1101()){         
    Serial.println("Connection OK");
  } else{
    Serial.println("Connection Error");
  }

  // Initialize CC1101
  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.setMHZ(433.27);
  ELECHOUSE_cc1101.setDRate(4.8);   // kbps
  ELECHOUSE_cc1101.setRxBW(58);     // kHz

  // Put into TX mode
  ELECHOUSE_cc1101.SetTx();
  ELECHOUSE_cc1101.setPA(-30);

  // Set infinite packet mode (streaming)
  ELECHOUSE_cc1101.SpiWriteReg(0x08, 0x02);  
  // 0x08 = PKTCTRL0, 0x02 = infinite length, no CRC
}

void loop() {
  // Keep stuffing FIFO with 0xFF
  while (ELECHOUSE_cc1101.SpiReadStatus(0x3F) < 60) { 
    ELECHOUSE_cc1101.SpiWriteReg(0x3F, 0xFF); 
    Serial.println("Jamming");
  }
}