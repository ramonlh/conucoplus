

#define INITFAB false    // si true, se resetea a fábrica, si false no se hace nada
#define versinst 2010    // 
#define debug true
#define debugwifi false

#ifdef ESP8266
extern "C" {
#include "user_interface.h"
}
#endif

#include "commontexts.h"              // include
#include <IoTtweet.h>                 // Local
#include <ModbusMaster.h>             // Local
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>                // Local
#include <ESP8266HTTPClient.h>        // Local
#include <ESP8266WebServer.h>         // Local
#include <ESP8266HTTPUpdateServer.h>  // Local
#include <ESP8266httpUpdate.h>        // Local
#include "ESP8266WiFiAP.h"            // Local
#include "Wire.h"
#include <Adafruit_BMP085.h>          // Local
#include <ACROBOTIC_SSD1306.h>
#include "Time.h"                     // Local
#include "TimeLib.h"                  // Local
#include "OneWire.h"                  // Local
#include "DallasTemperature.h"        // Local
#include "DHTesp.h"                   // Local
#include "defines.h"                  // include
#include "Base64.h"                   // include
#include "RCSwitch.h"                 // Local
#include "LiquidCrystal_I2C.h"        // Local
#include <FS.h>
#include <ESP8266FtpServer.h>
#include <PubSubClient.h>
#include "variables.h"                // include

ADC_MODE(ADC_TOUT);
ESP8266WebServer server(88);
OneWire owire(owPin);
DallasTemperature sensors1(&owire);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, urlNTPserverpool, 3600, 60000);
ESP8266HTTPUpdateServer httpUpdater;
ModbusMaster MBnode;
Adafruit_BMP085 bmp085;
RCSwitch mySwitch = RCSwitch();
LiquidCrystal_I2C lcd(0x27, 16, 2); // Display LCD 2x16 azul
DHTesp dht[2];
FtpServer ftpSrv;   
WiFiClient espClient;
PubSubClient PSclient(espClient);
IoTtweet myiot;

#include "basicfunctions.h"            /// include
//#include "ajaxcode.h"                  // include
#include "htmlFunctions.h"             // include
#include "conuco8266.h"               // include
#include "jsonfunctions.h"             // include
#include "main.h"                     // include

void initvars()
{
  memset(contaremote, 0, sizeof(contaremote));
  for (byte i = 0; i < maxdevrem; i++) {
    strcpy(auxdesc, vacio); savedescr(filemacdevrem, auxdesc, i, 14);
    strcpy(auxdesc, vacio); savedescr(fileidmyjsonrem, auxdesc, i, 10);
  }
}

void initPines()
{
  pinMode(bt2Pin, INPUT);
  pinMode(rx433, INPUT_PULLUP);
  pinMode (sdPin[0], OUTPUT);  pinMode (sdPin[1], OUTPUT);
  pinMode(ledSt, OUTPUT); digitalWrite(ledSt, 0);
  //  pinMode (rs485enpin, OUTPUT); digitalWrite(rs485enpin,0);
  memset(mbtemp, 0, sizeof(mbtemp));       // estado relés remotos modbus (1 bit cada uno);
  memset(mbcons, 0, sizeof(mbcons));       // estado relés remotos modbus (1 bit cada uno);
  memset(mbstatus, 0, sizeof(mbstatus));   // estado relés remotos modbus (1 bit cada uno);
  for (byte i = 0; i < 3; i++) MbRant[i] = MbR[i];
  MbC8ant[0] = conf.MbC8[0];
  memset(ListOri, 0, sizeof(ListOri));
  mact1 = 0; mact2 = 0; mact10 = 0; mact60 = 0; mact3600 = 0; mact86400 = 0;
  for (byte j = 0; j < 1; j++) for (byte i = 0; i < 8; i++) bevenENABLE[j][i] = 255; // todo unos 111111
  memset(timerem, 0, sizeof(timerem));
  for (byte i = 0; i < maxdevrem; i++) {
    actirem[i] = true;
    actisenal[i] = true;
  }
}

void initSerial() { Serial.begin (115200); delay(10); }
void initEEPROM() { EEPROM.begin(ROMsize); }

void initSPIFFS()
{
  Serial.println(); Serial.println(t(files));
  Serial.print(c(tspiffs)); Serial.print(b);
  if (SPIFFS.begin()) Serial.println(ok); else {
    Serial.println(c(terror)); ;
    return;
  }
}

void initFTP() { ftpSrv.begin(conf.userDev, conf.passDev); }

void initDS18B20()
{
  sensors1.begin();
  sensors1.setResolution(conf.prectemp);
  nTemp1 = sensors1.getDeviceCount();
  dPrint(t(sondastemp)); dPrint(dp); dPrintI(nTemp1); dPrint(b);
  dPrint(b); dPrint(t(Modo)); dPrint(dp);
  dPrint((sensors1.isParasitePowerMode()) ? c(tparasite) : c(tpower)); dPrint(crlf);
  for (byte i = 0; i < nTemp1; i++)
  {
    if (sensors1.getAddress(addr1Wire[i], i))
    {
      dPrint(b);
      for (uint8_t j = 0; j < 8; j++) {
        if (addr1Wire[i][j] < 16) dPrint(cero);
        Serial.print(addr1Wire[i][j], HEX);
      }
      dPrint(crlf);
    }
  }
}

void leerConf()
{
  if (readconf() < sizeof(conf)) saveconf();
  if ((conf.netseg == 0) || (conf.netseg == 255)) conf.netseg = 1; // por seguridad
  strcat(hostraiz, itoa(conf.netseg, buff, 10)); strcat(hostraiz, punto);
}

void initWiFi()
{
  if (conf.wifimode==0) WiFi.mode(WIFI_STA);
  else if (conf.wifimode==1) WiFi.mode(WIFI_AP);
  else if (conf.wifimode==2) WiFi.mode(WIFI_AP_STA);
  WiFi.setPhyMode(WIFI_PHY_MODE_11N);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.setAutoReconnect(true);
  dPrint(t(wifimodet)); dPrint(dp); dPrintI(conf.wifimode); dPrint(crlf);
  dPrint(c(MAC)); dPrint(dp); dPrint(WiFi.softAPmacAddress()); dPrint(crlf);
  for (byte i = 0; i<6; i++) {
    WiFi.softAPmacAddress().substring(i*3,i*3+2).toCharArray(conf.EEmac[i], 3);
    strcat(mac, conf.EEmac[i]);
    }
  if ((conf.wifimode==1) || (conf.wifimode==2)) // AP o AP+STA
    {
    WiFi.channel(conf.canalAP);
    WiFi.softAP(conf.ssidAP, conf.passAP, conf.canalAP, false);
    dPrint(t(canal)); dPrint(dp); dPrintI(WiFi.channel()); dPrint(crlf);
    dPrint(c(tIP)); dPrint(dp); Serial.print(WiFi.softAPIP()); dPrint(crlf);
    }
  if ((conf.wifimode==0) || (conf.wifimode==2)) // STA o AP+STA
    {
    dPrint(t(staticip));  dPrint(dp); dPrint(conf.staticIP ? t(SI) : t(NO)); dPrint(coma);
    if (conf.staticIP==1)
      {
      WiFi.config(conf.EEip, conf.EEgw, conf.EEmask, conf.EEdns, conf.EEdns2);
      }
    else
      {
      WiFi.begin(conf.ssidSTA,conf.passSTA);
      }
    dPrint(crlf);
    if (debugwifi) Serial.setDebugOutput(true);
    byte cont = 0;
    dPrint(t(conectando)); dPrint(b); dPrint(conf.ssidSTA); dPrint(barra); dPrint(conf.passSTA); dPrint(b);
    while ((!WiFi.isConnected()) && (cont++ < 20))  { delay(500); dPrint(punto);  }
    dPrint(b);
    dPrint((WiFi.status()==WL_CONNECTED)?ok:c(terror)); dPrint(crlf);
    dPrint(c(tIP)); dPrint(dp); Serial.print(WiFi.localIP()); dPrint(crlf);
    dPrint(c(tport)); dPrint(dp); Serial.print(88); dPrint(crlf);
    dPrint("GW"); dPrint(dp); Serial.print(WiFi.gatewayIP()); dPrint(crlf);
    dPrint("mask"); dPrint(dp); Serial.print(WiFi.subnetMask()); dPrint(crlf);
    dPrint(c(thost)); dPrint(WiFi.hostname()); dPrint(crlf);
  }
}

void initwebserver()
{
  const char * headerkeys[] = {"User-Agent", "Cookie"};
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  server.collectHeaders(headerkeys, headerkeyssize);
  initHTML();
  server.begin();
}

void initEntDig()
{
  if (conf.modo45 == 0) {       //Entradas digitales ON/OFF
    Serial.print(c(tinput)); Serial.print(dp); Serial.println(c(modet));
    pinMode(edPin[0], INPUT_PULLUP);
    pinMode(edPin[1], INPUT_PULLUP);
    for (byte i = 0; i < maxED; i++) if (conf.tipoED[i] == 2) dht[i].setup(edPin[i], DHTesp::DHT11);
  }
  else if (conf.modo45 == 1) {  // I2C
    Serial.print(i2c); Serial.print(b); Serial.println(c(modet));
    if (bmp085.begin()) {
      bmp085enabled = true;
      Serial.println(c(BMP085OK));
    } else {
      Serial.print(b);
      Serial.println(c(BMP085notfound));
    }
  }
  else if (conf.modo45 == 2) {  // modbus
    Serial.print(modbust); Serial.print(b); Serial.println(c(modet));
    //    SoftSerial.begin(modbusbaud);
    //    SoftSerial.setTransmitEnablePin(rs485enpin);
    //    MBnode.begin(1, SoftSerial);
    pinMode (edPin[0], INPUT_PULLUP);
    pinMode (edPin[1], OUTPUT);
  }
}

void initSalDig()
{
  for (byte i = 0; i < maxSD; i++)
  {
    pinMode(sdPin[i], OUTPUT);
    digitalWrite(sdPin[i], valorpin[conf.valinic[i] > 1 ? getbit8(conf.MbC8, sdPin[i] - 12) : conf.valinic[i]]);
  }
}

void initIFTTT()
{
  if (WiFi.isConnected()) {
    if (conf.iftttenable)
    {
      strcpy(auxdesc, itoa(WiFi.localIP()[0], buff, 10));
      for (byte i = 1; i <= 3; i++) {
        strcat(auxdesc, ".");
        strcat(auxdesc, itoa(WiFi.localIP()[i], buff, 10));
      }
      ifttttrigger(conucochar, conf.iftttkey, conf.aliasdevice, auxdesc, conf.myippub);
    }
    if (conf.modomyjson == 1) putmyjson();
    if (conf.mododweet == 1) postdweet(mac);
    if (conf.iottweetenable == 1) postIoTweet();
  }
}

void init433()
{
  mySwitch.enableReceive(rx433);  // Receiver on interrupt 0 => that is pin #2,
  //  mySwitch.enableTransmit(tx433);  // 15
}

void initLCD()
{
  lcd.init();  lcd.backlight();
  lcdshowstatus();
}

void initOled()
{
  oled.init();                      // Initialize SSD1306 OLED display
  oled.clearDisplay();
  oled.setTextXY(0, 0);  oled.putString("CONUCO Plus");
  oled.setTextXY(1, 0);  oled.putString("Dev.: "); oled.putNumber(conf.iddevice);
  oled.setTextXY(2, 0);  for (byte i = 0; i < 4; i++) {
    oled.putNumber(conf.EEip[i]);
    oled.putChar('.');
  }
  oled.setTextXY(3, 0);  oled.putString(conf.myippub);
  oled.setTextXY(5, 0);  oled.putString(readdescr(filedesclocal, 0, 20)); // oled.putFloat(MbR[0]*0.01);
  oled.putFloat(2345 * 0.01);
  oled.setTextXY(6, 0);  oled.putString(readdescr(filedesclocal, 1, 20)); // oled.putFloat(MbR[0]*0.01);
  oled.putFloat(4532 * 0.01);
}

void initTime()
{
  timeClient.setTimeOffset(7200);
  if (timeClient.update() == 1) {
    countfaulttime = 0;
    setTime(timeClient.getEpochTime());
  }
}

void checkRemotes()
{
  for (byte i = 0; i < maxdevrem; i++)
    if ((conf.idremote[i] >= 150) && (conf.idremote[i] <= 166))
    {
      dPrint(b); dPrintI(conf.idremote[i]); dPrint(b);
      int auxerr = ReqJsonConf(conf.idremote[i], 88);
      if (auxerr == HTTP_CODE_OK) {
        Serial.println(ok);
        extraevaloresTempConf(false);
        strcpy(auxdesc, aliasdevicetemp); savedescr(filedevrem, auxdesc, i, 20);
        for (byte j = 0; j < maxsalrem; j++)
          if (conf.idsalremote[j] == conf.idremote[i])
            if ((conf.senalrem[j] >= 0) && (conf.senalrem[j] <= 7)) {
              readdescr(filedesctemp, conf.senalrem[j], 20); savedescr(filesalrem, auxdesc, j, 20);
            }
        saveconf();
      }
      else
        Serial.println(c(terror));
      clearmsg();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void ICACHE_FLASH_ATTR setup(void) {
  initSerial();
  initSPIFFS();
  leerConf();
  initvars();
  initPines();
  initEEPROM();
  initEntDig();
  initSalDig();
  initDS18B20();
  initWiFi();
  initwebserver();
  initFTP();
  timeClient.begin();
  if (WiFi.isConnected()) {
    initTime();
    checkMyIP();
    actualizamasters();
    actualizaremotos();
    checkRemotes();
  }
  initIFTTT();
  initMqtt();
  init433();
  initLCD();
  initOled();
  leevaloresOW();

  dPrint(t(ippublica)); dPrint(dp); dPrint(conf.myippub); dPrint(crlf);
  dPrint(conf.userDev); dPrint(barra); dPrint(conf.passDev); dPrint(crlf);
  dPrint(t(dispositivo)); dPrint(dp); dPrintI(conf.iddevice); dPrint(crlf);
  dPrint(t(versiont)); dPrint(dp); dPrintI(versinst); dPrint(crlf);
  dPrint(c(runningt)); dPrint(crlf);
  printhora(); dPrint(crlf);

  //  Serial.println("Starting MQTT broker");
  //  myBroker.init();
  //  myBroker.subscribe("#");

}   // setup()

void testChange()
{
  if (statusChange) {
    lcdshowstatus();
    if (conf.modomyjson == 1) putmyjson();
    if (conf.mododweet == 1) postdweet(mac);
    if (conf.iottweetenable == 1) postIoTweet();
    actualizamasters();
  }
  if (iftttchange[0]>0)
  {
    if (getbit8(iftttchange,0)==1) // SD 0
    {
      if ((getbit8(conf.iftttpinSD,0)==1) && (getbit8(conf.MbC8,0)==1))
        ifttttrigger(conucochar, conf.iftttkey, conf.aliasdevice, readdescr(filedesclocal, 6, 20), textonoff(1));
      if ((getbit8(conf.iftttpinSD,8)==1) && (getbit8(conf.MbC8,0)==0))
        ifttttrigger(conucochar, conf.iftttkey, conf.aliasdevice, readdescr(filedesclocal, 6, 20), textonoff(0));
    }
    if (getbit8(iftttchange, 1) == 1) // SD 1
    {
      if ((getbit8(conf.iftttpinSD, 1) == 1) && (getbit8(conf.MbC8, 1) == 1)) // en ON
        ifttttrigger(conucochar, conf.iftttkey, conf.aliasdevice, readdescr(filedesclocal, 7, 20), textonoff(1));
      if ((getbit8(conf.iftttpinSD, 9) == 1) && (getbit8(conf.MbC8, 1) == 0)) // en OFF
        ifttttrigger(conucochar, conf.iftttkey, conf.aliasdevice, readdescr(filedesclocal, 7, 20), textonoff(0));
    }

    if (getbit8(iftttchange, 2) == 1)  // ED 0
    {
      if ((getbit8(conf.iftttpinED, 0) == 1) && (getbit8(conf.MbC8, 2) == 1)) // en ON
        ifttttrigger(conucochar, conf.iftttkey, conf.aliasdevice, readdescr(filedesclocal, 4, 20), textonoff(1));
      if ((getbit8(conf.iftttpinED, 8) == 1) && (getbit8(conf.MbC8, 2) == 0)) // en OFF
        ifttttrigger(conucochar, conf.iftttkey, conf.aliasdevice, readdescr(filedesclocal, 4, 20), textonoff(0));
    }

    if (getbit8(iftttchange, 3) == 1)   // ED 1
    {
      if ((getbit8(conf.iftttpinED, 1) == 1) && (getbit8(conf.MbC8, 3) == 1)) // en ON
        ifttttrigger(conucochar, conf.iftttkey, conf.aliasdevice, readdescr(filedesclocal, 5, 20), textonoff(1));
      if ((getbit8(conf.iftttpinED, 9) == 1) && (getbit8(conf.MbC8, 3) == 0)) // en OFF
        ifttttrigger(conucochar, conf.iftttkey, conf.aliasdevice, readdescr(filedesclocal, 5, 20), textonoff(0));
    }
    iftttchange[0] = 0;
  }
}

void task1()      // 1 segundo
{
  tini = millis();
  countfaulttime++;   // si se hace mayor que TempDesactPrg,desactiva ejecucion programas dependientes de fecha
  leevaloresAN();
  // gestión botón reset InitFabrica
  while (digitalRead(bt2Pin) == 0)  // pulsado botón 2, reset fábrica 20 seg, Reset Wifi: 10 seg.
  {
    tempbt2++;
    if ((tempbt2 == 10) || (tempbt2 == 20)) tictac(ledSt, 5, 100);
    delay(500);
  }
  if (tempbt2 >= 20) {
    initFab();  // Reset Fábrica
    ESP.reset();
  }
  else if (tempbt2 >= 10) {
    reinitWiFi();  // mod Reset Wifi
    ESP.reset();
  }
  tempbt2 = 0;

  //  if (countfaulttime < conf.TempDesactPrg) procesaeventos();
  procesaeventos();
  procesaTimeMax();
  for (byte j = 0; j < maxsalrem; j++)
    if (conf.idsalremote[j] > 0)
      if ((conf.senalrem[j] == 6) || (conf.senalrem[j] == 7)) contaremote[j]++;

  if (WiFi.isConnected()) testChange();
  {
    //    if ((millis()-tini)>5000) {Serial.print(F("1 SEG:"));  Serial.println(millis()-tini);}
    oled.setTextXY(4, 0); oled.putNumber(hour());   oled.putChar(':');    // hora
    oled.setTextXY(4, 3); oled.putNumber(minute()); oled.putChar(':');  // minutos
    oled.setTextXY(4, 6); oled.putNumber(second());                     // segundos
    mact1 = millis();
  }
}

void taskvar()
{

  tini = millis();
  //////////////////////////////////////////////
  /////////////////////////////////////////////
  //    if (!pendsave) lcdshowstatus();
  lastpro = 0; lastcode = 0; lastlen = 0; // pone a cero el último código 433
  leevaloresOW();
  procesaconsignas();
  if (conf.modo45 == 0) if ((conf.tipoED[0] == 2) || (conf.tipoED[1] == 2)) leevaloresDHT();
    else if (conf.modo45 == 2) leevaloresMB();
  if (WiFi.isConnected()) {
    unsigned long tini = millis();
    if (conf.modomyjson == 1) {
      putmyjson();
    }
    if (conf.mododweet == 1) {
      postdweet(mac);
    }
    if (conf.iottweetenable == 1) {
      postIoTweet();
    }
    //      actualizamasters();
    actualizaremotos();
    if (conf.mqttenable)
      if (!PSclient.connected()) {
        if (mqttreconnect()) {
          mqttsubscribevalues();
        }
      }
    if (PSclient.connected()) mqttpublishallvalues();
    //    for (byte i=0;i<3;i++) if (getbit8(conf.mqttsalenable,i)==1) mqttpublish(i);
  }
  //    if ((millis()-tini)>5000) {printhora(); Serial.print(F(" 10 SEG:")); Serial.println(millis()-tini);}
  mact10 = millis();
}

void task60()     // 60 segundos
{
  tini = millis();
  memset(bevenENABLE, sizeof(bevenENABLE), 0);
  //  if (countfaulttime < conf.TempDesactPrg)      {
  procesaSemanal();
  procesaFechas();
  //    }
  //  else
  //    {
  //    timeClient.setTimeOffset(7200);
  //    if (timeClient.update()==1) { countfaulttime=0; setTime(timeClient.getEpochTime());  }
  //    }
  if ((millis() - tini) > 5000) {
    Serial.print(60);
    Serial.print(F(" SEG:"));
    Serial.println(millis() - tini);
    }
  mact60 = millis();
}

void task3600()         // 3600 segundos=1 hora
{
  tini = millis();
  if (WiFi.isConnected()) {
    timeClient.setTimeOffset(7200);
    if (timeClient.update() == 1) {
      countfaulttime = 0;
      setTime(timeClient.getEpochTime());
    }
    checkMyIP();
    checkForUpdates();
  }
  if (millis() - tini > 1000) {
    Serial.print(3600);
    Serial.print(F(" seg.:"));
    Serial.println(millis() - tini);
  }
  mact3600 = millis();
}

void execcom()
{
  String command, param;
  boolean hecho=false;
  command = sinput.substring(0, sinput.indexOf(","));
  param = sinput.substring(sinput.indexOf(",") + 1);
  //  Serial.print("comando/param: "); Serial.print(command);Serial.print("/");Serial.println(param);
  if (command == "reset") 
    ESP.restart();  
  else if (command == "resetw") 
    reinitWiFi();
  else if (command == "resetf") 
    initFab();
  else if (command == "files")
    {
    Dir dir = SPIFFS.openDir(barra);
    while (dir.next())  {
      Serial.print(dir.fileName());
      Serial.print(b);
      File f = dir.openFile(letrar);
      Serial.println(f.size());
      }
    Serial.print("files ");  Serial.println(checkfiles() ? "OK" : "ERROR");
    }
  else if ((command == "help") || (command == "h"))
    {
    Serial.println("reset");
    Serial.println("resetw");
    Serial.println("resetf");
    Serial.println("wifim");
    Serial.println("files");
    Serial.println("st");
    Serial.println("id,iddevice");
    Serial.println("alias,aliasdevice");
    Serial.println("instal,instaldevice");
    Serial.println("ssid,SSIDname");
    Serial.println("pass,SSIDpass");
    Serial.println("ssidap,SSIDnameAP");
    Serial.println("passap,SSIDpassAP");
    Serial.println("seg,segnet");
    }
  else if (command == "st")
    {
    Serial.print("DEV:"); Serial.println(conf.iddevice);
    Serial.print("alias:"); Serial.println(conf.aliasdevice);
    Serial.print("IP:"); for (byte i=0;i<4;i++) {Serial.print(conf.EEip[i]);Serial.print(punto); } Serial.println();
    Serial.print("mask:"); for (byte i=0;i<4;i++) {Serial.print(conf.EEmask[i]); Serial.print(punto);}Serial.println();
    Serial.print("GW:");for (byte i=0;i<4;i++) {Serial.print(conf.EEgw[i]);Serial.print(punto); }Serial.println();
    Serial.print("SSID:"); Serial.println(conf.ssidSTA);
    Serial.print("Pass:"); Serial.println(conf.passSTA);
    Serial.print("SSIDAP:"); Serial.println(conf.ssidAP);
    Serial.print("PassAP:"); Serial.println(conf.passAP);
    Serial.print("Conn:"); Serial.println(WiFi.isConnected() ? ok : c(terror));
    Serial.println("Done");
    }
  else if (command == "id") {
    conf.iddevice = param.toInt();
    conf.EEip[3] = conf.iddevice;
    param.toCharArray(conf.mqttpath[2], 10);
    saveconf(); hecho=true;
    }
  else if (command == "alias") {
    param.toCharArray(conf.aliasdevice, 20);
    saveconf(); hecho=true;
    }
  else if (command == "instal") {
    param.toCharArray(conf.instname, 10);
    param.toCharArray(conf.mqttpath[1], 10);
    saveconf(); hecho=true;
    }
  else if (command == "ssid") {
    param.toCharArray(conf.ssidSTA, 20);
    saveconf(); hecho=true;
    }
  else if (command == "pass") {
    param.toCharArray(conf.passSTA, 20);
    saveconf(); hecho=true;
    }
  else if (command == "ssidap") {
    param.toCharArray(conf.ssidAP, 20);
    saveconf(); hecho=true;
    }
  else if (command == "passap") {
    param.toCharArray(conf.passAP, 20);
    saveconf(); hecho=true;
    }
  else if (command == "wifim") {
    conf.wifimode = param.toInt();
    saveconf(); hecho=true;
    }
  else if (command == "ip0") {
    conf.EEip[0] = param.toInt();
    conf.EEgw[0] = param.toInt();
    saveconf(); hecho=true;
    }
  else if (command == "ip1") {
    conf.EEip[1] = param.toInt();
    conf.EEgw[1] = param.toInt();
    saveconf(); hecho=true;
    }
  else if (command == "ip2") {
    conf.EEip[2] = param.toInt();
    conf.EEgw[2] = param.toInt();
    saveconf(); hecho=true;
    }
  else if (command == "ip3") {
    conf.EEip[3] = param.toInt();
    conf.EEgw[3] = 1;
    saveconf(); hecho=true;
    }
  else if (command == "seg") {
    conf.netseg = param.toInt();
    conf.EEip[2] = conf.netseg;
    conf.EEgw[2] = conf.netseg;
    saveconf(); hecho=true;
    }
  Serial.println(hecho?"Done":"Command not found");
}

void handleSerial()
{
  if (Serial.available())
    {
    char thisChar = Serial.read();
    if ((thisChar == '\n') || (thisChar == '\r')) {
      execcom();
      sinput = "";
      }
    else {
      sinput = sinput + thisChar;
      }
    }
}

///////////////////////////////////////////////////////////////////////////
void ICACHE_FLASH_ATTR loop(void)
{
  //////////////////////////////////////////////////
  unsigned long tini = millis();
  tini = millis();
  handleSerial();
  handleRF();
  if (conf.ftpenable) ftpSrv.handleFTP();        //make sure in loop you call handleFTP()!!
  server.handleClient();    // atiende peticiones http
  PSclient.loop();
  leevaloresDIG();
  //////////////////////////////////////////////////////////////////////////////////////////
  if ((millis() > (mact1 + 1000))) {  task1();  } // tareas que se hacen cada segundo
  if ((millis() > (mact10 + (conf.peractrem * 1000)))) {  taskvar();  } // tareas que se hacen cada "peractrem" segundos
  if ((millis() > (mact60 + 60000))) { task60();  } // tareas que se hacen cada 60 segundos:1 minuto
  if ((millis() > (mact3600 + 3600000))) { task3600();  } // tareas que se hacen cada 3600 segundos:1 hora
  if ((millis() > (mact86400 + 86400000)))    // tareas que se hacen cada 86400 segundos: 1 día
    {
    tini = millis();
    //    if (WiFi.isConnected()) { if (conf.iftttenable) ifttttrigger(conucochar, conf.iftttkey, conf.aliasdevice, "MyIP", conf.myippub); }
    //    if (millis()-tini>1000) {Serial.print(F("1 día.:")); Serial.println(millis()-tini);}
    mact86400 = millis();
    }
}
