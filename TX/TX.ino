/* 1 ch NRF 24 TRANSMITTER example.

  Module // Arduino UNO //        ESP32
    GND    ->   GND           ->  GND
    Vcc    ->   3.3V(External)->  3.3v
    CE     ->   D9            ->  D4
    CSN    ->   D10           ->  D5
    CLK    ->   D13           ->  D18
    MOSI   ->   D11           ->  D23
    MISO   ->   D12           ->  D19
*/

/* First we include the libraries. Download it from
   my webpage if you donw have the NRF24 library */
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>

/*//////////////////////////////////////////////////////*/

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__) ||defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)
#define CSN 10
#define CE  9
#elif defined(ESP32)
#define CSN 5
#define CE  4
#else
#error "Make Config you're self"
#endif

#define Debug_mode false


/*Create a unique pipe out. The receiver has to
  wear the same unique code*/
const uint64_t pipeOut = 0x662266; //IMPORTANT: The same as in the receiver!!!
/*//////////////////////////////////////////////////////*/

/*Create the data struct we will send
  The sizeof this struct should not exceed 32 bytes
  This gives us up to 32 8 bits channals */

RF24 radio(CE, CSN); // select  CSN and CE  pins


/*//////////////////////////////////////////////////////*/
//Create a struct to send over NRF24
struct MyData {
  byte test;
};
MyData data;
byte count = 0;
/*//////////////////////////////////////////////////////*/



//This function will only set the value to  0 if the connection is lost...
void resetData()
{
  data.test = 0;
}

void setup()
{
  Serial.begin(9600);
  if (Debug_mode)
    printf_begin();
  radio.begin();
  radio.setDataRate(RF24_250KBPS); //speed  RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radio.openWritingPipe(pipeOut); //Open a pipe for writing
  radio.openReadingPipe(0, pipeOut); //Open a pipe for reading
  radio.setChannel(108);// Set RF communication channel.
  radio.setPALevel(RF24_PA_MAX); //translate to: RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
  radio.enableDynamicPayloads(); //This way you don't always have to send large packets just to send them once in a while. This enables dynamic payloads on ALL pipes.
  //radio.disableDynamicPayloads();//This disables dynamic payloads on ALL pipes. Since Ack Payloads requires Dynamic Payloads, Ack Payloads are also disabled. If dynamic payloads are later re-enabled and ack payloads are desired then enableAckPayload() must be called again as well.
  radio.setCRCLength(RF24_CRC_16); // Use 8-bit or 16bit CRC for performance. CRC cannot be disabled if auto-ack is enabled. Mode :RF24_CRC_DISABLED  ,RF24_CRC_8 ,RF24_CRC_16
  radio.setRetries(10, 15);//Set the number of retry attempts and delay between retry attempts when transmitting a payload. The radio is waiting for an acknowledgement (ACK) packet during the delay between retry attempts.Mode: 0-15,0-15
  radio.setAutoAck(true); // Ensure autoACK is enabled
  radio.stopListening();//Stop listening for incoming messages, and switch to transmit mode.
  resetData();

}

/**************************************************/

void loop()
{
  count++;
  data.test = count;//fill data to MyData
  Serial.print("Send");
  Serial.println(count);
  if (count == 254) {
    count = 0;
  }
  NRF24_Transmit();//Transmit MyData
  delay(200);
}

void NRF24_Transmit() {
  radio.writeFast(&data, sizeof(MyData));//Transmit Data. use one of this two: write() or writeFast()
  if (Debug_mode)
    radio.printDetails();//Show debug data
  bool OK = radio.txStandBy();//Returns 0 if failed. 1 if success.
  delayMicroseconds(50);
  radio.flush_tx();//Empty all 3 of the TX (transmit) FIFO buffers
}
