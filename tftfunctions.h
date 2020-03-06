

void clearTFT()
{
  if (conf.TFTenabled==0) return;
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

void drawStarting()
  {
  if (conf.TFTenabled==0) return;
  clearTFT();
  tft.setTextSize(2);
  tft.drawString("Starting",0,0);
  tft.drawString("Vers.",0,25);
  tft.drawNumber(versinst,100,25);
  tft.drawString("Connecting WiFi",0,50);
  }

void initTFT()
{
  if (conf.TFTenabled==0) return;
//  digitalWrite(TFT_RST,0); delay(50); digitalWrite(TFT_RST,1);
  tft.init(); tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  lasttouch=millis();
  tfton=true;
  drawStarting(); 
}

void drawTE()   // temperaturas
{
  if (conf.TFTenabled==0) return;
  tft.setTextColor(TFT_WHITE, TFT_BLACK); tft.setTextSize(2);
  for (byte i=0;i<4;i++)
      {
      tft.setTextColor(TFT_WHITE, TFT_BLACK); tft.setTextSize(2);
      tft.drawString(readdescr(filedesclocal,i,20),0,50*i+15);
      dtostrf(MbR[i]*0.01,4,1,auxdesc);
      btTE[i].initButtonUL(&tft,100,50*i,58,40,2,TFT_WHITE,TFT_BLACK,auxdesc,2);
      btTE[i].drawButton();
      }
  for (byte i=4;i<maxTemp;i++)
      {
      tft.setTextColor(TFT_WHITE, TFT_BLACK); tft.setTextSize(2);
      tft.drawString(readdescr(filedesclocal,i,20),165,50*(i-4)+15);
      dtostrf(MbR[i]*0.01,4,1,auxdesc);
      btTE[i].initButtonUL(&tft,265,50*(i-4),58,40,2,TFT_WHITE,TFT_BLACK,auxdesc,2);
      btTE[i].drawButton();
      }
}

void drawSO()   // sondas y códigos
{
  if (conf.TFTenabled==0) return;
  tft.setTextColor(TFT_WHITE, TFT_BLACK); tft.setTextSize(2);
  for (byte i=0;i<maxTemp;i++)
    {
    tft.setTextColor(TFT_WHITE, TFT_BLACK); tft.setTextSize(2);
    tft.drawString(letraS,0,25*i); tft.drawNumber(i, 15, 25*i);
    if (conf.nprobe[i]>0) 
      {
      tft.drawNumber(conf.nprobe[i], 35,25*i);
      for (byte j=0;j<8;j++) 
        {
        tft.drawHex(conf.probecode[i][j], 75+(j*30), 25*i);
        if (j<7) tft.drawString(":", 97+(j*30), 25*i);
        }
      }
    else 
      tft.drawString("NA",35,25*i);
    }
}

void drawED()   // entradas digitales
{
  if (conf.TFTenabled==0) return;
  for (byte i=0;i<4;i++)
    {
    btED[i].initButtonUL(&tft,0,50*i,158,40,2,getbit8(conf.MbC8,i+8)==1?TFT_YELLOW:TFT_WHITE,TFT_BLACK,readdescr(filedesclocal,i+8,20),2);
    btED[i].drawButton();
    tft.setTextColor(TFT_WHITE, TFT_BLACK);  tft.setTextSize(2);
    tft.drawString("      ", 180,50*i+10);
    tft.drawNumber(conf.contadores[i],180, 50*i+10); 
    }
}

void drawSD()   // salidas digitales
{
  if (conf.TFTenabled==0) return;
  for (byte i=0;i<4;i++)
    {
    btSD[i].initButtonUL(&tft,0,50*i,158,40,2,getbit8(conf.MbC8,i)==1?TFT_YELLOW:TFT_WHITE,TFT_BLACK,readdescr(filedesclocal,i+12,20),2);
    btSD[i].drawButton();
    }
  for (byte i=4;i<maxSD;i++)
    {
    btSD[i].initButtonUL(&tft,160,50*(i-4),158,40,2,getbit8(conf.MbC8,i)==1?TFT_YELLOW:TFT_WHITE,TFT_BLACK,readdescr(filedesclocal,i+12,20),2);
    btSD[i].drawButton();
    }
    
}

void drawGPIOs()   // GPIOS variables
{
  if (conf.TFTenabled==0) return;
  byte j=0;
  for (byte i=0;i<maxgpiovar;i++) 
    if (gpiovis(i))
      {
      int posy=34*j+5;
      strcpy(auxdesc,readdescr(filedescgpio,i,20)); 
      if (conf.gpiosensortype[i]==0)     // INPUT
        {
        btGPIO[i].initButtonUL(&tft,0,34*j,180,33,2,getbit8(conf.MbC8gpio,i+16)==1?TFT_YELLOW:TFT_WHITE,TFT_BLACK,auxdesc,2);
        btGPIO[i].drawButton();
        tft.setTextColor(TFT_WHITE, TFT_BLACK);  tft.setTextSize(2);
        tft.drawString("      ", 190,34*j+10);
        tft.drawNumber(conf.contadoresgpio[i],190, 34*j+10); 
        }
      else if (conf.gpiosensortype[i]==1)     // OUTPUT
        {
        btGPIO[i].initButtonUL(&tft,0,34*j,180,33,2,getbit8(conf.MbC8gpio,i)==0?TFT_WHITE:TFT_YELLOW,TFT_BLACK,auxdesc,2);
        btGPIO[i].drawButton();
        }
      else if (conf.gpiosensortype[i]==2)     // ADC estándar
        {
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString(auxdesc,0,posy);
        tft.drawFloat(MbRgpio[i],1,270,posy); 
        }
      else if (conf.gpiosensortype[i]==4)    // DHT
        {
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString(auxdesc,0,posy);
        tft.drawFloat(dhtdata[i][0],1,170,posy);  tft.drawString("C/",220,posy);
        tft.drawFloat(dhtdata[i][1],1,240,posy);  tft.drawString("%",300,posy);
        }
      else if ((conf.gpiosensortype[i]==5) || (conf.gpiosensortype[i]==6))
        {
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString(auxdesc,0,posy);
        tft.drawFloat(MbRgpio[i]+conf.gpiogamma[i],1,240,posy); tft.drawString("C",300,posy);
        }
      else if (conf.gpiosensortype[i]==7)     // ACS712  sensor de corriente
        {
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString(auxdesc,0,posy);
        tft.drawFloat(MbRgpio[i]+conf.gpiogamma[i],1,270,posy); 
        }
      else
        {
        btGPIO[i].initButtonUL(&tft,0,34*j,180,33,2,TFT_WHITE,TFT_BLACK,auxdesc,2);
        btGPIO[i].drawButton();
        }
      j++;
      }
}

void drawIP()   // valores LAN
{
  if (conf.TFTenabled==0) return;
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.drawString(WiFi.isConnected()?"Connected":"NOT Connected",0,0);
  tft.drawString(WiFi.SSID(),150,0);
  for (byte i=0;i<4;i++)
    {
    tft.drawString("IP",0,30); tft.drawNumber(conf.EEip[i], 50*i+70, 30); if (i<3) tft.drawString(".",50*i+105,30);
    tft.drawString("Mask",0,60); tft.drawNumber(conf.EEmask[i], 50*i+70, 60); if (i<3) tft.drawString(".",50*i+105,60);
    tft.drawString("GW",0,90); tft.drawNumber(conf.EEgw[i], 50*i+70, 90); if (i<3) tft.drawString(".",50*i+105,90);
    tft.drawString("DNS",0,120); tft.drawNumber(conf.EEdns[i], 50*i+70, 120); if (i<3) tft.drawString(".",50*i+105,120);
    }
//  btIP[0].initButtonUL(&tft,0,150,75,40,2,TFT_WHITE,TFT_BLACK,"ID",2);  btIP[0].drawButton();
  btIP[1].initButtonUL(&tft,100,150,75,40,2,TFT_WHITE,TFT_BLACK,"SCAN",2);  btIP[1].drawButton();
}

void drawST(boolean flechas, boolean escok)     // barra navegación
{
  if (conf.TFTenabled==0) return;
  if (flechas)
    {
    btST[0].initButtonUL(&tft,0,200,75,40,2,TFT_WHITE,TFT_BLACK,flecha[0],2);  btST[0].drawButton();
    btST[3].initButtonUL(&tft,245,200,75,40,2,TFT_WHITE,TFT_BLACK,flecha[3],2);  btST[3].drawButton();
    }
  if (escok)
    {
    btST[1].initButtonUL(&tft,80,200,75,40,2,TFT_WHITE,TFT_BLACK,"ESC",2);  btST[1].drawButton();
    btST[2].initButtonUL(&tft,165,200,75,40,2,TFT_WHITE,TFT_BLACK,"OK",2);  btST[2].drawButton();
    }
  else
    {
    tft.setTextSize(2);
    tft.setTextColor(WiFi.isConnected()?TFT_GREEN:TFT_RED, TFT_BLACK);
    tft.drawString(WiFi.isConnected()?"CON":"DIS",105,220);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawNumber(tftpage,85,220);
    if (hour()<10) tft.drawNumber(0,167,220); 
    tft.drawNumber(hour(),hour()<10?167:155,220); 
    tft.drawString(":",178,220);
    if (minute()<10) tft.drawNumber(0,188,220); 
    tft.drawNumber(minute(),minute()<10?200:188,220); 
    tft.drawString(":",210,220);
    if (second()<10) { tft.drawNumber(0,220,220); }
    tft.drawNumber(second(),second()<10?232:220,220);   
    }
}

void drawSET()     // barra setup
{
  if (conf.TFTenabled==0) return;
  btSET[0].initButtonUL(&tft,0,0,100,40,2,TFT_WHITE,TFT_BLACK,"RESET",2);  btSET[0].drawButton();
  btSET[1].initButtonUL(&tft,0,50,100,40,2,TFT_WHITE,TFT_BLACK,"XXXX",2);  btSET[1].drawButton();
  btSET[2].initButtonUL(&tft,0,100,100,40,2,TFT_WHITE,TFT_BLACK,"XXXXX",2);  btSET[2].drawButton();
  btSET[3].initButtonUL(&tft,0,150,100,40,2,TFT_WHITE,TFT_BLACK,"XXXXX",2);  btSET[3].drawButton();
}

void drawFT817()
{
  if (conf.TFTenabled==0) return;
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);  tft.setTextSize(5);
  tft.drawFloat(145.35512,5,5,5); 
  bt817[0].initButtonUL(&tft,0,200,60,40,2,TFT_YELLOW,TFT_BLACK,"ON",2);  bt817[0].drawButton();
  bt817[1].initButtonUL(&tft,60,200,60,40,2,TFT_YELLOW,TFT_BLACK,"OFF",2);  bt817[1].drawButton();
  
}

void drawAP(byte nact)
{
    clearTFT();
    for (int i=0; i<nAP; i++)
      //if (WiFi.RSSI(i)>-90)
        {
        WiFi.SSID(i).toCharArray(auxchar, 20);
        if (i==tftapactual) { tft.setTextColor(TFT_BLACK, TFT_WHITE);    }
        tft.drawNumber(i,0,i*20);
        tft.drawString(auxchar,20,i*20); 
        tft.drawNumber(WiFi.RSSI(i),240,i*20);
        tft.drawString("dbm",280,i*20); 
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        }
    drawST(true,true);
}

void ICACHE_FLASH_ATTR selectapTFT()
{
  tftpage=7; 
  nAPact=0;
  tftapactual=0;
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  clearTFT();
  tft.drawString("Scanning...",0,0); 
  nAP=WiFi.scanNetworks(false, false);  if (nAP>maxAP) nAP=maxAP;
  drawAP(tftapactual);
}

void drawTFT()
{
  if (conf.TFTenabled==0) return;
  if (conf.modofi==0)
    {
    if (tftpage==0) { drawSD(); }
    else if (tftpage==1) { drawED(); }
    else if (tftpage==2) { drawTE(); }
    else if (tftpage==3) { drawGPIOs(); }
    else if (tftpage==4) { drawIP(); }
    else if (tftpage==5) { drawSET(); }
    else if (tftpage==6) { drawSO(); }
    drawST(true,false);
    }
  else if (conf.modofi==1)  // bombacalor
    {
    if (tftpage==0) { drawSD(); }
    else if (tftpage==1) { drawED(); }
    else if (tftpage==2) { drawTE(); }
    else if (tftpage==3) { drawGPIOs(); }
    else if (tftpage==4) { drawIP(); }
    else if (tftpage==5) { drawSET(); }
    else if (tftpage==6) { drawSO(); }
    drawST(true,false);
    }
  else if (conf.modofi==2)    // ft817
    {
    drawFT817();
    }
}

