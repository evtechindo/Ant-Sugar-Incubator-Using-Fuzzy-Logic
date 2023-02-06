/*--------------------------Library----------------------------------------------------------------------*/
#include<Wire.h>
#include<LiquidCrystal_I2C.h>
#include "DHT.h"
#include <dimmable_light.h>
/*--------------------------Deklarasi Pin DHT11----------------------------------------------------------*/
#define pinDHT 4 //deklarasi pin DHT
#define tipeDHT DHT22 //definisi tipe DHT
/*--------------------------Deklarasi Pin Aktuator-------------------------------------------------------*/
#define pinKipas 10
#define pinLampu 11
/*--------------------------Deklarasi Pin Tombol---------------------------------------------------------*/
#define pinMenu 5
#define pinUp 6
#define pinDown 7
#define pinOk 8
/*--------------------------Deklarasi Pembacaan Button---------------------------------------------------*/
#define Menu digitalRead(pinMenu)
#define Up digitalRead(pinUp)
#define Down digitalRead(pinDown)
#define Ok digitalRead(pinOk)

#define syncPin 2
/*--------------------------Deklarasi Objek--------------------------------------------------------------*/
LiquidCrystal_I2C lcd(0x3F, 16, 2); //objek lcd
DHT dht(pinDHT, tipeDHT); //objek dht

int lockMenu = 0;
int lockUp = 0;
int lockDown = 0;
int lockOk = 0;
int dis;
int dis1;
int mode = 0;
/*--------------------------Variabel DHT-----------------------------------------------------------------*/
float temp = 0.00;
float hum = 0.00;
String kondisiSuhu;
String kondisiKelembapan;

/*--------------------------Variabel Status Aktuator-----------------------------------------------------*/
String sKipas;
String sLampu;

int pwmLampu;
int tanda = 0;

unsigned long sekarang = 0;
unsigned long sebelum = 0;

int rule;

DimmableLight lampu(pinLampu);

int period = 100;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  dht.begin();
  /*--------------------------Konfidurasi Pin IO--------------------------------------------------------------*/
  pinMode(pinMenu,  INPUT);
  pinMode(pinUp,    INPUT);
  pinMode(pinDown,  INPUT);
  pinMode(pinOk,    INPUT);
  pinMode(pinKipas, OUTPUT);

  Serial.print("Initializing the dimmable light class... ");
  DimmableLight::setSyncPin(syncPin);
  DimmableLight::begin();
  Serial.println("Done!");

  lcd.setCursor(2, 0);// kolom, baris
  lcd.print("Fuzzy System");
  lcd.setCursor(2, 1);
  lcd.print("Intan  Putri");
  delay(2000);//jeda
  lcd.clear();//hapus
  lcd.setCursor(1, 0);
  lcd.print("Teknik Elektro");
  lcd.setCursor(2, 1);
  lcd.print("Untidar 2022");
  delay(2000);
  lcd.clear();
}

void loop() {
  if ((sekarang - sebelum > 10000) && dis == 1) {
    lcd.clear();
    tanda = 0;
    dis = 0;
    sebelum = millis();
  }

  bacaSetup();
  tampil();
  bacaDHT();
  logicFuzzy();
  logicKipas();
  logicLampu();
}
void tampil() {
  /*--------------------------Fungsi Tampil Awal--------------------------------------------------------------*/
  if (tanda == 0) {
    dis = 0;
    lcd.setCursor(0,0);
    lcd.print("T:"+String(temp,2));
    lcd.setCursor(0,1);
    lcd.print("H:"+String(hum,2));
    lcd.setCursor(8,0);
    lcd.print("F:"+String(sKipas));
    lcd.setCursor(8,1);
    lcd.print("L:"+String(sLampu));
  }
}
/*--------------------------Fungsi Pembacaan Suhu--------------------------------------------------------------*/
void bacaDHT() {
  temp = dht.readTemperature();
  hum = dht.readHumidity();

  if(temp<=21.0){
    kondisiSuhu="DINGIN";
  }else if(temp>=21.0&&temp<=28.0){
    kondisiSuhu="NORMAL";
  }else if(temp>=28.0){
    kondisiSuhu="PANAS";
  }

  if(hum<=64.0){
    kondisiKelembapan="KERING";
  }else if(hum>=64.0&&hum<=77.0){
    kondisiKelembapan="LEMBAP";
  }else if(hum>=77&hum<=100){
    kondisiKelembapan="BASAH";
  }

}

void logicFuzzy(){
  if(kondisiSuhu=="DINGIN"&&kondisiKelembapan=="KERING"){
    rule=1;
  }else if(kondisiSuhu=="DINGIN"&&kondisiKelembapan=="LEMBAP"){
    rule=2;
  }else if(kondisiSuhu=="DINGIN"&&kondisiKelembapan=="BASAH"){
    rule=3;
  }else if(kondisiSuhu=="NORMAL"&&kondisiKelembapan=="KERING"){
    rule=4;
  }else if(kondisiSuhu=="NORMAL"&&kondisiKelembapan=="LEMBAP"){
    rule=5;
  }else if(kondisiSuhu=="NORMAL"&&kondisiKelembapan=="BASAH"){
    rule=6;
  }else if(kondisiSuhu=="PANAS"&&kondisiKelembapan=="KERING"){
    rule=7;
  }else if(kondisiSuhu=="PANAS"&&kondisiKelembapan=="LEMBAP"){
    rule=8;
  }else if(kondisiSuhu=="PANAS"&&kondisiKelembapan=="BASAH"){
    rule=9;
  }

  logicRule();
}

void logicRule(){
  if(rule==1){
    sLampu="HANGAT";
    sKipas="NYALA";
  }else if(rule==2){
    sLampu="HANGAT";
    sKipas="NYALA";
  }else if(rule==3){
    sLampu="PANAS";
    sKipas="NYALA";
  }else if(rule==4){
    sLampu="DINGIN";
    sKipas="MATI";
  }else if(rule==5){
    sLampu="DINGIN";
    sKipas="MATI";
  }else if(rule==6){
    sLampu="HANGAT";
    sKipas="NYALA";
  }else if(rule==7){
    sLampu="DINGIN";
    sKipas="MATI";
  }else if(rule==8){
    sLampu="DINGIN";
    sKipas="MATI";
  }else if(rule==9){
    sLampu="HANGAT";
    sKipas="NYALA";
  }
}

void logicKipas(){
  if(sKipas=="NYALA"){
    digitalWrite(pinKipas,HIGH);
  }else if(sKipas=="MATI"){
    digitalWrite(pinKipas,LOW);
  }
}

void logicLampu(){
  if(sLampu=="DINGIN"){
    pwmLampu=50;
  }else if(sLampu=="HANGAT"){
    pwmLampu=160;
  }else if(sLampu=="PANAS"){
    pwmLampu=255;
  }
  lampu.setBrightness(pwmLampu);
  delay(period);
}

void bacaSetup() {
  if (Menu == 0 && lockMenu == 0) {
    lockMenu = 1;
  }
  if (Menu == 0 && dis == 0) {
    lcd.clear();
    lockMenu = 0;
    dis = 1;
    tanda = 1;
  }
  /*--------------------------Tampilan Awal Menu-------------------------------------------------------------*/
  if (dis == 1) {
    sekarang=millis();
    lcd.setCursor(1, 0);
    lcd.print("RULE:"+String(rule));
    lcd.setCursor(1, 1);
    lcd.print("TEMP");
    lcd.setCursor(9, 0);
    lcd.print("HUM");
    lcd.setCursor(9, 1);
    lcd.print("OUTPUT");

    if (Down == 0 && lockDown == 0) {
      lockDown = 1;
    }
    if (Down != 0 && lockDown == 1) {
      lcd.clear();
      lockDown = 0;
      mode++;
      if (mode > 3) {
        mode = 0;
      }
    }

    if (Up == 0 && lockUp == 0) {
      lockUp = 1;
    }
    if (Up != 0 && lockUp == 1) {
      lcd.clear();
      lockUp = 0;
      mode--;
      if (mode < 0) {
        mode = 3;
      }
    }

    switch (mode) {
      case 0 : lcd.setCursor(0, 0); lcd.print(">"); break;
      case 1 : lcd.setCursor(0, 1); lcd.print(">"); break;
      case 2 : lcd.setCursor(8, 0); lcd.print(">"); break;
      case 3 : lcd.setCursor(8, 1); lcd.print(">"); break;
    }

    if (Ok == 0 && lockOk == 0) {
      lockOk = 1;
    }
    if (Ok != 0 && lockOk == 1 && mode == 0) {
      lockOk = 0;
      dis = 2;
      lcd.clear();
    }
    if (Ok != 0 && lockOk == 1 && mode == 1) {
      lockOk = 0;
      dis = 3;
      lcd.clear();
    }
    if (Ok != 0 && lockOk == 1 && mode == 2) {
      lockOk = 0;
      dis = 4;
      lcd.clear();
    }
    if (Ok != 0 && lockOk == 1 && mode == 3) {
      lockOk = 0;
      dis = 5;
      lcd.clear();
    }
  }
  /*--------------------------Menu DHT-------------------------------------------------------------*/
 if (dis == 2) {
    lcd.setCursor(2, 0);
    lcd.print("INPUT SISTEM");
    lcd.setCursor(0, 1);
    lcd.print(kondisiSuhu);
    lcd.setCursor(10, 1);
    lcd.print(kondisiKelembapan);

    if (Ok == 0 && lockOk == 0) {
      lockOk = 1;
    }
    if (Ok != 0 && lockOk == 1) {
      lockOk = 0;
      dis = 1;
      lcd.clear();
    }
    if (Menu == 0 && lockMenu == 0) {
      lockMenu = 1;
    }
    if (Menu == 0 && dis == 2) {
      lcd.clear();
      lockMenu = 0;
      dis = 1;
    }
  }

  /*--------------------------Menu LDR-------------------------------------------------------------*/
  if (dis == 3) {
    lcd.setCursor(0, 0);
    lcd.print("TEMPERATURE");
    lcd.setCursor(0,1);
    lcd.print("Suhu:"+String(temp,2)+" C");
  
    if (Ok == 0 && lockOk == 0) {
      lockOk = 1;
    }
    if (Ok != 0 && lockOk == 1) {
      lockOk = 0;
      dis = 1;
      lcd.clear();
    }
    if (Menu == 0 && lockMenu == 0) {
      lockMenu = 1;
    }
    if (Menu == 0 && dis == 3) {
      lcd.clear();
      lockMenu = 0;
      dis = 1;
    }
  }
  /*--------------------------Menu TDS-------------------------------------------------------------*/
 if (dis == 4) {
    lcd.setCursor(0, 0);
    lcd.print("HUMIDITY");
    lcd.setCursor(0,1);
    lcd.print("Humidity:"+String(hum,2)+" %");
  
    if (Ok == 0 && lockOk == 0) {
      lockOk = 1;
    }
    if (Ok != 0 && lockOk == 1) {
      lockOk = 0;
      dis = 1;
      lcd.clear();
    }
    if (Menu == 0 && lockMenu == 0) {
      lockMenu = 1;
    }
    if (Menu == 0 && dis == 4) {
      lcd.clear();
      lockMenu = 0;
      dis = 1;
    }
  }
  /*--------------------------Menu HCSR-04-------------------------------------------------------------*/
  if (dis == 5) {
    lcd.setCursor(0, 0);
    lcd.print("Rule:"+String(rule));
    lcd.setCursor(0, 1);
    lcd.print("F:" + String(sKipas));
    lcd.setCursor(8, 1);
    lcd.print("L:" + String(sLampu));
  
    if (Ok == 0 && lockOk == 0) {
      lockOk = 1;
    }
    if (Ok != 0 && lockOk == 1) {
      lockOk = 0;
      dis = 1;
      lcd.clear();
    }
    if (Menu == 0 && lockMenu == 0) {
      lockMenu = 1;
    }
    if (Menu == 0 && dis == 5) {
      lcd.clear();
      lockMenu = 0;
      dis = 1;
    }
  }
}
