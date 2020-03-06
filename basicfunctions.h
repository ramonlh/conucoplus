#include <EEPROM.h>
#include <Arduino.h>  // for type definitions

template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i=0; i<sizeof(value); i++) EEPROM.write(ee++, *p++);
    return i;
}

template <class T> int EEPROM_readAnything(int ee, T& value)
{
    byte* p=(byte*)(void*)&value;
    unsigned int i;
    for (i=0; i<sizeof(value); i++) *p++=EEPROM.read(ee++);
    return i;
}

boolean strcharcomp() { msg.toLowerCase(); msg.toCharArray(auxdesc, msg.length()+1); return (strcmp(auxchar,auxdesc)==0); }

char* ICACHE_FLASH_ATTR readdescr(char* namefile, byte ind, byte len)
{
  File auxfile=SPIFFS.open(namefile,letrar);
  if (auxfile)
    {
    auxfile.seek((len+2)*(ind), SeekSet);
    auxfile.readBytes(auxdesc,len+2);
    auxfile.close();
    auxdesc[len-1]='\0';
    byte n=strlen(auxdesc);
    while ((n>0) && ((auxdesc[n-1]==' ')||(auxdesc[n-1]=='\n')||(auxdesc[n-1]=='\r'))) n--;
    auxdesc[n]='\0';
    }
  return auxdesc;
}

char* ICACHE_FLASH_ATTR t(int pos)
{
  char auxlang[20]="";
  if (conf.lang==0) strcpy(auxlang,filespanish);
  if (conf.lang==1) strcpy(auxlang,fileenglish);
  File auxfile=SPIFFS.open(auxlang,letrar);
  if (auxfile)
    {
    auxfile.seek(42*(pos-1), SeekSet);
    auxfile.readBytes(auxdesc,42);
    auxfile.close();
    auxdesc[41]='\0';
    byte n=strlen(auxdesc);
    while ((n>0) && ((auxdesc[n-1]==' ')||(auxdesc[n-1]=='\n')||(auxdesc[n-1]=='\r'))) n--;
    auxdesc[n]='\0';
    }
  return auxdesc;
}

char* ICACHE_FLASH_ATTR c(int pos)
{
  File auxfile=SPIFFS.open(filecommon,letrar);
  if (auxfile)
    {
    auxfile.seek(42*(pos-1), SeekSet);
    auxfile.readBytes(auxdesc,42);
    auxfile.close();
    auxdesc[41]='\0';
    byte n=strlen(auxdesc);
    while ((n>0) && ((auxdesc[n-1]==' ')||(auxdesc[n-1]=='\n')||(auxdesc[n-1]=='\r'))) n--;
    auxdesc[n]='\0';
    }
  return auxdesc;
}

void ICACHE_FLASH_ATTR savedescr(char* namefile, char* descr, byte ind, byte len)
{
  File auxfile=SPIFFS.open(namefile, rmas);
  if (!auxfile) return;
  if (auxfile.seek((len+2)*(ind), SeekSet)) 
    {
    for (byte i=strlen(descr);i<len-1;i++) descr[i]=' ';  
    descr[len-1]='\0';
    auxfile.print(descr);auxfile.print('\0');
    auxfile.print(crlf);
    }
  auxfile.close();   
}

void ICACHE_FLASH_ATTR addlog(File fileact)
{
  if (day()<10) fileact.print(cero); fileact.print(day()); fileact.print(barra);
  if (month()<10) fileact.print(cero); fileact.print(month()); fileact.print(barra);
  if (year()<10) fileact.print(cero); fileact.print(year()); fileact.print(b);
  if (hour()<10) fileact.print(cero); fileact.print(hour()); fileact.print(dp);
  if (minute()<10) fileact.print(cero); fileact.print(minute()); fileact.print(dp);
  if (second()<10) fileact.print(cero); fileact.print(second()); fileact.print(b);
}

void ICACHE_FLASH_ATTR addlog(byte tipo, int code, char *texto)
{
  File auxfile=SPIFFS.open("/log.txt","a+");
  if (auxfile)  
    { 
    if (day()<10) auxfile.print(cero); auxfile.print(day()); auxfile.print(barra);
    if (month()<10) auxfile.print(cero); auxfile.print(month()); auxfile.print(barra);
    if (year()<10) auxfile.print(cero); auxfile.print(year()); auxfile.print(b);
    if (hour()<10) auxfile.print(cero); auxfile.print(hour()); auxfile.print(dp);
    if (minute()<10) auxfile.print(cero); auxfile.print(minute()); auxfile.print(dp);
    if (second()<10) auxfile.print(cero); auxfile.print(second()); auxfile.print(b);
    auxfile.print(code); auxfile.print(b);
    auxfile.print(texto);
    auxfile.println(crlf); 
    auxfile.close();   }
}

void ICACHE_FLASH_ATTR addlog(byte tipo, int code, PGM_P texto)
{
//  File auxfile=SPIFFS.open("/log.txt",amas);
  File auxfile=SPIFFS.open("/log.txt","a+");
  if (auxfile)  
    { 
    if (day()<10) auxfile.print(cero); auxfile.print(day()); auxfile.print(barra);
    if (month()<10) auxfile.print(cero); auxfile.print(month()); auxfile.print(barra);
    if (year()<10) auxfile.print(cero); auxfile.print(year()); auxfile.print(b);
    if (hour()<10) auxfile.print(cero); auxfile.print(hour()); auxfile.print(dp);
    if (minute()<10) auxfile.print(cero); auxfile.print(minute()); auxfile.print(dp);
    if (second()<10) auxfile.print(cero); auxfile.print(second()); auxfile.print(b);
    auxfile.print(code); auxfile.print(b);
    auxfile.println(texto); 
    auxfile.close();   }
}

byte ICACHE_FLASH_ATTR getbit8(byte tabla[], byte pin)
  {  return ((tabla[pin/8] & tab[(pin % 8)])>0)?1:0;}

void ICACHE_FLASH_ATTR setbit8(byte tabla[], byte pin, byte value)  {
  tabla[pin/8]=((value==0)?tabla[pin/8]&(255^tab[(pin%8)]):tabla[pin/8]|tab[(pin%8)]);}

String ICACHE_FLASH_ATTR extrae(boolean eschar, String cad, String subcad)
  { subcad.concat("\"");
    return cad.substring(cad.indexOf(subcad)+subcad.length()+(eschar?2:1), cad.indexOf(",",cad.indexOf(subcad))-(eschar?1:0));      }

void ICACHE_FLASH_ATTR tictac(int pin, int n, int delayed)
  { for (int i=0;i<n;i++) 
    {digitalWrite(pin,1); delay(delayed); digitalWrite(pin, 0); delay(delayed);} }

void ICACHE_FLASH_ATTR printconceros(int value)  {  if (value<10) Serial.print(0); Serial.print(value); }

void ICACHE_FLASH_ATTR printhora() {
  printconceros(hour()); Serial.print(dp);
  printconceros(minute()); Serial.print(dp);
  printconceros(second());  
  }

int ICACHE_FLASH_ATTR posrem(int numrem)  {
  int p=0;
  boolean encontrado=false;
  while ((p<maxsalrem) && (!encontrado)) { encontrado=(conf.idremote[p++]==numrem); }
  return encontrado?p:0;
  }

int ICACHE_FLASH_ATTR readconf()
{
  File auxfile=SPIFFS.open(fileconf,letrar);
  if (!auxfile) return 0;
  int count=0;
  for (count=0;count<sizeof(conf);count++)*(buffconf+count)=auxfile.read();
  auxfile.close();
  return count;
}

void ICACHE_FLASH_ATTR saveconf()
{
  File auxfile=SPIFFS.open(fileconf, rmas);
  if (auxfile) { auxfile.write(buffconf,sizeof(conf)); auxfile.close();  }
}

void ICACHE_FLASH_ATTR printlinea(PGM_P texto) { for (byte i=0;i<20;i++) Serial.print(texto);Serial.println(); }

void ICACHE_FLASH_ATTR createhost(byte ip)
{
  strcpy(host,hostraiz); 
  strcat(host,itoa(conf.netseg, buff, 10)); 
  strcat(host,punto); 
  strcat(host, itoa(ip, buff, 10));
}

void calcindices(int i)
{
  param_number=server.argName(i).toInt();
  if (mp<=0) mp=1;
  indice=param_number/mp;
  resto=param_number%mp;
  mival=server.arg(i).toInt();
}

int callhttpGET(char *host, int port, boolean readresp, unsigned long timeout)
{
  HTTPClient http;
  http.begin(host,port,msg);
  http.setConnectTimeout(timeout);
  int httpCode=http.GET();
  msg="";
  if (readresp) if(httpCode==HTTP_CODE_OK) { msg=http.getString(); }
  http.end();
  return httpCode;
}

boolean checkfile(char* namefile)
{  if (!SPIFFS.exists(namefile)) { Serial.print(namefile); Serial.println(" no existe"); return false; }  return true; }

boolean checkfiles()
{
  boolean auxB=true;
  auxB=auxB && checkfile(fileconf); 
  auxB=auxB && checkfile(filezonas);
  auxB=auxB && checkfile(filedevrem);
  auxB=auxB && checkfile(filesalrem); 
  auxB=auxB && checkfile(filewebcall); 
  auxB=auxB && checkfile(fileurlwebcall); 
  auxB=auxB && checkfile(filedescprg); 
  auxB=auxB && checkfile(filedescesc); 
  auxB=auxB && checkfile(filemacdevrem); 
  auxB=auxB && checkfile(fileidmyjsonrem); 
  auxB=auxB && checkfile(fileunitsalrem); 
  auxB=auxB && checkfile(filedesclocal); 
  auxB=auxB && checkfile(filedesctemp); 
  auxB=auxB && checkfile(filei2ctypes); 
  auxB=auxB && checkfile(filecommon); 
  auxB=auxB && checkfile(filespanish); 
  auxB=auxB && checkfile(fileenglish); 
  filesok=auxB;
  return auxB;
}

void ICACHE_FLASH_ATTR printP(PGM_P texto1) { char c;  while ((c = pgm_read_byte(texto1++))) msg += c; }
void ICACHE_FLASH_ATTR printP(PGM_P texto1, PGM_P texto2) { printP(texto1); printP(texto2);}
void ICACHE_FLASH_ATTR printP(PGM_P texto1, PGM_P texto2, PGM_P texto3) { printP(texto1, texto2); printP(texto3);}
void ICACHE_FLASH_ATTR printP(PGM_P texto1, PGM_P texto2, PGM_P texto3, PGM_P texto4)
  { printP(texto1,texto2);  printP(texto3,texto4); }
void ICACHE_FLASH_ATTR printP(PGM_P texto1, PGM_P texto2, PGM_P texto3, PGM_P texto4, PGM_P texto5)
  { printP(texto1, texto2, texto3, texto4);  printP(texto5); }
void ICACHE_FLASH_ATTR printP(PGM_P texto1, PGM_P texto2, PGM_P texto3, PGM_P texto4, PGM_P texto5, PGM_P texto6)
  { printP(texto1, texto2, texto3, texto4, texto5); printP(texto6); }

void ICACHE_FLASH_ATTR printI(int value)  { printP(itoa(value, buff, 10));  }
void ICACHE_FLASH_ATTR printH(int value)  { printP(itoa(value, buff, 16));  }
void ICACHE_FLASH_ATTR printL(long value)  { printP(ltoa(value, buff, 10));  }

void ICACHE_FLASH_ATTR printF(float value, byte deci) {
  float pdec=value-int(value);
  printI(int(value)); if (deci>0) printP(punto);  
  for (byte i=0;i<deci;i++) if (int(pdec*pow(10,i+1))%10==0) printP(cero); else printI(abs(int(pdec*pow(10,i+1))%10));
  }
void ICACHE_FLASH_ATTR printIP(long valor, const  char *texto) { printI(valor); printP(texto); }

void ICACHE_FLASH_ATTR printPiP(const char *texto1, int num, const char *texto2)
  { printP(texto1); printI(num); printP(texto2);}




bool ICACHE_FLASH_ATTR autOK()
{
  return true;      // provisional
  if (conf.usepassDev == 0) return true;
  if (server.hasHeader("Cookie"))
    if (server.header("Cookie").indexOf("ESPSESSIONID=1") != -1)
      return true;
  return false;
}

void ICACHE_FLASH_ATTR printOpc(boolean colorea, boolean activa, char* texto)
{
  if (activa)
    printP(th, texto, th_f);
  else  {
    printP(colorea?th:td, href_i, comillas, auxchar, comillas, mayor);
    printP(texto, href_f, colorea?th_f:td_f);
  }
}

void ICACHE_FLASH_ATTR printOpc(boolean colorea, boolean activa, PGM_P texto)
{
  if (activa)
    printP(th, texto, th_f);
  else  {
    printP(colorea?th:td, href_i, comillas, auxchar, comillas, mayor);
    printP(texto, href_f, colorea?th_f:td_f);
  }
}

void ICACHE_FLASH_ATTR printOpc(boolean colorea, boolean activa, PGM_P texto, PGM_P link)
{
  if (activa)
    printP(th, texto, th_f);
  else  {
    printP(colorea?th:td, href_i, comillas, link, comillas, mayor);
    printP(texto, href_f, colorea ? th_f : td_f);
  }
}

void ICACHE_FLASH_ATTR printOpc(boolean colorea, boolean activa, PGM_P texto, PGM_P link, byte i)
{
  if (activa)
    printP(th, texto, th_f);
  else  {
    printP(colorea?th:td, href_i, comillas, link, interr, letran);
    printP(ig);
    printI(i);
    printP(comillas, mayor, texto, href_f, colorea ? th_f : td_f);
  }
}

void ICACHE_FLASH_ATTR writeMenu(byte opcprin, byte opcsec)
{
  printP(c(body_i), menor);
  printP(table, b);
  printP(c(tclass));
  printP(ig, tmenu, mayor, tr); // formato menú
  if (conf.modofi==0)
    printOpc(false, (opcprin==1), t(zonas), panelhtm); // PANEL
  else if (conf.modofi==1)
    printOpc(false, (opcprin==1), c(panel), panelhtm); // Bomba de calor
  printOpc(false, (opcprin==3), t(configuracion), sdhtm); // CONFIGURACIÓN
  if (conf.modofi==0) printOpc(false, (opcprin==2), t(programas), sprghtm); // Programas
  printOpc(false, (opcprin==4), t(sistema), espsyshtm); // Sistema

  if (conf.usepassDev)
  {
    printP(td, href_i, comillas, barra, loghtm);
    if (autOK())
      printP(interr, c(DISCONNECTYES), comillas, mayor, t(des));
    else
      printP(comillas, b, c(color000000), mayor);
    printP(t(conectar), href_f, td_f);
  }
  printP(tr_f, menor, barra, table, mayor);
  printP(menor, table, b);
  printP(c(tclass), ig);
  printP(tmenu, mayor, tr);  // formato menú
  if (opcprin == 1) // PANELES
    {
    if (conf.modofi!=1) 
      {
      for (byte i=0; i<maxpaneles; i++)
        if (getbit8(conf.bshowpanel, i))
          printOpc(false, opcsec==i, readdescr(filezonas, i, 20), panelhtm, i);
      }
    }
  else if (opcprin==2) // PROGRAMACIÓN
    {
    if (conf.modofi!=1) printOpc(false, opcsec==5, t(programas), sprghtm);
    if (conf.modofi!=1) printOpc(false, opcsec==1, t(semanal), ssehtm);
    if (conf.modofi!=1) printOpc(false, opcsec==2, t(condiciones), svhtm);
    if (conf.modofi!=1) printOpc(false, opcsec==3, t(fecha), sfhtm);
    if (conf.modofi!=1) printOpc(false, opcsec==4, t(escenas), seschtm);
    if (conf.modofi!=1) printOpc(false, opcsec==7, t(webcalls), swchtm);
    }
  else if (opcprin==3) // CONFIGURACIÓN
    {
    printOpc(false, opcsec==0, t(dispositivo), sdhtm);
    if (conf.modofi==0) printOpc(false, opcsec==5, t(zonas), sphtm);
    if (conf.modofi==0) printOpc(false, opcsec==2, t(mandorf), rfhtm);
    printOpc(false, opcsec==3, t(tred), snehtm);
    printOpc(false, opcsec==4, t(servred), snshtm);
    if (conf.modofi==0) printOpc(false, opcsec==1, c(senales), siohtm);
    if (conf.modofi==0) printOpc(false, opcsec==10, t(remotos), slkhtm);
    if (conf.modofi==0) printOpc(false, opcsec==11, c(salremotas), sremhtm);
    if (conf.modofi==1) printOpc(false, opcsec==12, t(bombacalor), sbhtm);
    }
  else if (opcprin==4) // SISTEMA
    {
    printOpc(false, opcsec==4, t(statust), espsyshtm);
    if (conf.modofi!=1) printOpc(false, opcsec==3, t(files), fileshtm);
    printOpc(false, opcsec==5, t(seguridad), sshtm);
    printOpc(false, opcsec==1, t(actualizar), suhtm);
    printOpc(false, opcsec==2, treset, rshtm);
    }
  printP(tr_f, menor, barra, table, mayor, br);
}

void ICACHE_FLASH_ATTR serversend200() { server.send(200, texthtml, msg); msg=vacio; }
void ICACHE_FLASH_ATTR printtiempo(unsigned long segundos)
{
  if (segundos < 60)  {
    printIP(segundos, comillas);
    }
  else     {
    unsigned long minutos = segundos / 60;
    segundos = segundos % 60;
    if (minutos < 60)     {
      printIP(minutos, comilla);
      printPiP(b, segundos, comillas);
      }
    else      {
      unsigned long horas = minutos / 60;
      minutos = minutos % 60;
      if (horas < 24)  {
        printIP(horas, letrah);
        printPiP(b, minutos, comilla);
      }
      else          {
        unsigned int dias = horas / 24;
        horas = horas % 24;
        printIP(dias, letrad);
        printPiP(b, horas, letrah);
        }
      }
    }
}

boolean gpiovis(byte i)
  {
  boolean auxgpiovis=true;
  if      (i==0) auxgpiovis=(conf.TX433enabled==0);                            // GPIO 02
  else if (i==1) auxgpiovis=(conf.RX433enabled==0);                            // GPIO 04
  else if (i==2) auxgpiovis=((conf.SPIenabled==0) && (conf.TFTenabled==0));    // GPIO 12
  else if (i==3) auxgpiovis=((conf.SPIenabled==0) && (conf.TFTenabled==0));    // GPIO 13
  else if (i==4) auxgpiovis=((conf.SPIenabled==0) && (conf.TFTenabled==0));    // GPIO 14
  else if (i==5) auxgpiovis=false;    // no se puede usar con facilidad        // GPIO 15
  else if (i==6) auxgpiovis=((conf.I2Cenabled==0) && (conf.TFTenabled==0));    // GPIO 21
  else if (i==7) auxgpiovis=((conf.I2Cenabled==0) && (conf.SERIAL2enabled==0));// GPIO 22
  else if (i==8) auxgpiovis=(conf.SERIAL2enabled==0) ;                         // GPIO 32
  else if (i==9) auxgpiovis=false;   // no se puede usar  con facilidad        // GPIO 33
  return auxgpiovis;   
  }
