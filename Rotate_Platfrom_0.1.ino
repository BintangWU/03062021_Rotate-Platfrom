#include <SPI.h>
#include <LiquidCrystal.h>
#include <IRremote.h>

LiquidCrystal             lcd(9);
IRrecv                    ir(A1);
decode_results            results;

#define btnPin            A0

//Data Remot MP3
#define MODE              0xFF22DD
#define PREV              0xFFE01F
#define NEXT              0xFFA857
#define PLAY_PAUSE        0xFF906F

//Data Tombol
#define OK                1
#define UP                2
#define DOWN              3
#define NONE              0

#define STOP              8

#define FULL_STEP         1.0
#define HALF_STEP         0.5

#define constaStepRev     400
#define stepDegree        1.8

#define status(x)         (x == 0 ? "*" : " ")

uint8_t pin[4] = {7, 6, 5, 4};
int16_t counter, stepRev, sudut, lastSudut;
unsigned long irData, btn;

void step(uint8_t StepMove);
void test();
uint8_t readButtons(uint8_t adcIn, uint8_t n = 10);

void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2);

  lcd.setCursor(0, 0);
  lcd.print("  HD Pro Audio  ");
  lcd.setCursor(0, 1);
  lcd.print("     =v0.1=     ");

  for (byte i = 0; i < 4; i++)
    pinMode(pin[i], OUTPUT);

  test();
  step(STOP);

  ir.enableIRIn();
  delay(1000);
  lcd.clear();
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Sudut: " + String(sudut));
  lcd.print((char)223);
  lcd.print(" ");

  lcd.setCursor(0, 1);
  lcd.print("Last : " + String(lastSudut));
  lcd.print((char)223);
  lcd.print(" ");

  lcd.setCursor(15, 1);
  lcd.print(status(stepRev));

  btn = readButtons(btnPin);
  if (ir.decode(&results)) {
    irData = results.value;
    ir.resume();
  }

  if (btn == UP || irData == NEXT) {
    if (sudut < 90) {
      sudut += 5;
      delay(200);
    }
    else sudut = 90;
  }
  else if (btn == DOWN || irData == PREV) {
    if (sudut > -90) {
      sudut -= 5;
      delay(200);
    }
    else sudut = -90;
  }
  else if (btn == OK || irData == PLAY_PAUSE) {
    counter = 0;
    stepRev = (sudut - lastSudut) / (stepDegree * HALF_STEP);
    //stepRev = (constaStepRev * (sudut - lastSudut)) / 360;
    lastSudut = sudut;

    //RUMUS
    //step baru = (step sekarang * sudut) / sudut terukur

    Serial.println("==OK==");
    Serial.println("Sudut: " + String(sudut) + "\t" + "Stepp: " + String(stepRev));
    delay(200);

    while (counter < stepRev) {
      counter += 1;
      step(counter % 8);
      delay(4);
    }

    while (counter > stepRev) {
      counter -= 1;
      step(7 - (abs(counter) % 8));
      delay(4);
    }
    step(STOP);
  }
  irData = 0;
}

void step(uint8_t StepMove) {
  switch (StepMove) {
    case 0  :
      digitalWrite(7, HIGH);
      digitalWrite(6, LOW);
      digitalWrite(5, HIGH);
      digitalWrite(4, LOW); break;
    case 1  :
      digitalWrite(7, HIGH);
      digitalWrite(6, LOW);
      digitalWrite(5, LOW);
      digitalWrite(4, LOW); break;
    case 2  :
      digitalWrite(7, HIGH);
      digitalWrite(6, LOW);
      digitalWrite(5, LOW);
      digitalWrite(4, HIGH); break;
    case 3  :
      digitalWrite(7, LOW);
      digitalWrite(6, LOW);
      digitalWrite(5, LOW);
      digitalWrite(4, HIGH); break;
    case 4  :
      digitalWrite(7, LOW);
      digitalWrite(6, HIGH);
      digitalWrite(5, LOW);
      digitalWrite(4, HIGH); break;
    case 5  :
      digitalWrite(7, LOW);
      digitalWrite(6, HIGH);
      digitalWrite(5, LOW);
      digitalWrite(4, LOW); break;
    case 6  :
      digitalWrite(7, LOW);
      digitalWrite(6, HIGH);
      digitalWrite(5, HIGH);
      digitalWrite(4, LOW); break;
    case 7  :
      digitalWrite(7, LOW);
      digitalWrite(6, LOW);
      digitalWrite(5, HIGH);
      digitalWrite(4, LOW); break;
    case 8  :
      digitalWrite(7, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(5, HIGH);
      digitalWrite(4, HIGH); break;
  }
}

void test() {
  for (int i = 0; i < 400; i++) {
    step(i % 8);
    delay(4);
  }
}

uint8_t readButtons(uint8_t adcIn, uint8_t n = 10) {
  uint16_t adc, adcSum = 0;

  for (uint8_t i = 0; i < n; i++)
    adcSum += analogRead(adcIn);

  adc = adcSum / n;

  if (adc > 491 && adc < 531)       return 1; //OK
  else if (adc > 651 && adc < 701)  return 2; //UP
  else if (adc > 746 && adc < 786)  return 3; //DOWN
  else return 0; //NONE
}
