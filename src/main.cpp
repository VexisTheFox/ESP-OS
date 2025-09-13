#include <Arduino.h>
// (included above)
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* menuItems[] = {"Item 1", "Item 2", "Item 3"};
const int menuLength = sizeof(menuItems)/sizeof(menuItems[0]);
int selected = 0;

#define BTN_UP   D5
#define BTN_DOWN D6
#define BTN_OK   D7
#define BTN_BACK D4
#define HALT_PIN D3

// Halt logic configuration
static const uint8_t HALT_STARTUP_SAMPLES = 20;      // number of samples at boot
static const uint16_t HALT_STARTUP_INTERVAL_MS = 5;  // spacing between samples
static const uint8_t HALT_RUNTIME_CONFIRM = 8;       // consecutive LOW reads needed at runtime
static bool halted = false;                          // latched halt state
static uint8_t haltRunCount = 0;                     // runtime low streak

bool lastUp=false, lastDown=false, lastOk=false, lastBack=false;
bool showSelectedMsg=false;
bool showBackMsg=false;
bool inSelection=false; // lock state: after OK, require BACK to exit

void drawMenu();
void showSelectedMessage();
void showBackMessage();
void showHaltScreen();
void haltForever();

void drawMenu(){
  display.clearDisplay();
  display.setTextSize(1);
  for(int i=0;i<menuLength;i++){
    int y=i*16;
    if(i==selected){
      display.fillRect(0,y,SCREEN_WIDTH,16,SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
    } else {
      display.setTextColor(SSD1306_WHITE,SSD1306_BLACK);
    }
    display.setCursor(5,y+4);
    display.print(menuItems[i]);
  }
  display.display();
}

void showSelectedMessage(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(8,26);
  display.print(menuItems[selected]);
  display.print(" selected!");
  display.display();
}

void showBackMessage(){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30,22);
  display.print("Back");
  display.display();
}

void showHaltScreen(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(8,26);
  display.print("HALTED D3 LOW");
  display.display();
}

void haltForever(){
  while(true){ delay(1000); }
}

void setup(){
  Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC,0x3C)){
    Serial.println(F("SSD1306 allocation failed"));
    haltForever();
  }
  pinMode(BTN_UP,INPUT_PULLUP);
  pinMode(BTN_DOWN,INPUT_PULLUP);
  pinMode(BTN_OK,INPUT_PULLUP);
  pinMode(BTN_BACK,INPUT_PULLUP);
  pinMode(HALT_PIN,INPUT_PULLUP);

  // Stabilize before sampling (let pull-ups settle)
  delay(30);
  // Multi-sample HALT pin to avoid spurious boot strap influence (GPIO0 is a boot strap pin)
  uint8_t lowCount=0;
  for(uint8_t i=0;i<HALT_STARTUP_SAMPLES;i++){
    if(!digitalRead(HALT_PIN)) lowCount++;
    delay(HALT_STARTUP_INTERVAL_MS);
  }
  Serial.print(F("[HALT] Startup samples LOW="));
  Serial.print(lowCount);
  Serial.print(F("/"));
  Serial.println(HALT_STARTUP_SAMPLES);
  if(lowCount == HALT_STARTUP_SAMPLES){
    Serial.println(F("[HALT] All samples LOW -> entering halt"));
    halted = true;
    showHaltScreen();
    haltForever();
  } else if(lowCount > (HALT_STARTUP_SAMPLES/2)) {
    Serial.println(F("[HALT] Majority LOW, ignoring (treat as noise)."));
  }

  drawMenu();
}

void loop(){
  if(!halted){
    if(!digitalRead(HALT_PIN)){
      // count consecutive lows
      if(haltRunCount < 255) haltRunCount++;
      if(haltRunCount >= HALT_RUNTIME_CONFIRM){
        Serial.println(F("[HALT] Runtime confirm -> HALT"));
        halted = true;
        showHaltScreen();
        haltForever();
      }
    } else {
      // reset streak on any HIGH
      if(haltRunCount){ haltRunCount = 0; }
    }
  }

  // If displaying a temporary message (selected/back), wait for any key.
  // However: if inSelection (after OK), only BACK should exit.
  if(showSelectedMsg || showBackMsg){
    bool upRaw   = !digitalRead(BTN_UP);
    bool downRaw = !digitalRead(BTN_DOWN);
    bool okRaw   = !digitalRead(BTN_OK);
    bool backRaw = !digitalRead(BTN_BACK);

    if(inSelection){
      // Only BACK exits selection
      if(backRaw && !lastBack){
        inSelection = false;
        showSelectedMsg = false;
        showBackMsg = true; // show back confirmation screen
        showBackMessage();
      }
      return; // stay until BACK used
    } else {
      // Normal (not locked): any button clears message and returns to menu
      if(upRaw || downRaw || okRaw || backRaw){
        showSelectedMsg=false;
        showBackMsg=false;
        drawMenu();
      }
      return;
    }
  }

  bool up = !digitalRead(BTN_UP);
  bool down = !digitalRead(BTN_DOWN);
  bool ok = !digitalRead(BTN_OK);
  bool back = !digitalRead(BTN_BACK);

  if(!inSelection && up && !lastUp){
    selected = (selected - 1 + menuLength) % menuLength;
    drawMenu();
  }
  if(!inSelection && down && !lastDown){
    selected = (selected + 1) % menuLength;
    drawMenu();
  }
  if(ok && !lastOk){
    if(!inSelection){
      inSelection = true;
      showSelectedMsg = true;
      showSelectedMessage();
    } else {
      // ignore OK while locked
    }
  }
  if(back && !lastBack){
    if(inSelection){
      // Back exits selection state via dedicated back message
      inSelection = false;
      showSelectedMsg = false;
      showBackMsg = true;
      showBackMessage();
    } else {
      showBackMsg = true;
      showBackMessage();
    }
  }

  lastUp=up; lastDown=down; lastOk=ok; lastBack=back;
}
