

#define ftdummy 0x00
#define ftrrxstatus 0xE7
#define ftrtxstatus 0xF7
#define ftrfreqcode 0x03
#define ftoncode 0x0F
#define ftoffcode 0x8F

typedef struct { byte data[4]; byte ftcom; } ftcomtype;
typedef struct { byte mode; unsigned long freq; } ftdatatype;

ftcomtype FTcom;
  
void FTsendcom(byte ftcom)
  {
   for (byte i=0;i<4;i++) Serial2.write(FTcom.data[i]); 
   Serial2.write(ftcom); 
  }

void FTclearpar()  {  for (byte i=0;i<4;i++) FTcom.data[i]=0x00; }

void FTon()
{
   FTclearpar(); FTsendcom(ftdummy);
   delay(100);
   FTclearpar(); FTsendcom(ftoncode);
}

void FToff() { FTclearpar(); FTsendcom(ftoffcode); }

unsigned long FTreadfreq()
{
  Serial.println("readfreq");
  FTclearpar(); 
  FTsendcom(ftrfreqcode);
  
  long tini=millis();
  Serial.println("RX");
  unsigned long auxt=millis();
  while ((!Serial2.available()) && ((millis()-auxt) < 500))
    {
    yield();
    }
  Serial.println();
  while (Serial2.available()) 
    {
    yield();
    Serial.print('X');  
    byte data[1];
    Serial2.readBytes(data,1);
    Serial.print(data[0],16);
    }
  Serial.println("RX2");
}

void initFT817()
{
  Serial2.begin(38400, SERIAL_8N2, RXD2, TXD2);
}

