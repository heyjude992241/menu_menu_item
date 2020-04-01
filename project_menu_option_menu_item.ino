#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "Menu.h"

const uint8_t INPUT_PIN = A0;
const uint8_t BTN_PIN = 8;
const uint8_t SCREEN_WIDTH = 128;
const uint8_t SCREEN_HEIGHT = 64;
const uint8_t MAX_TITLE_LEN = 8;
const uint8_t MAX_ITEMTITLE_LEN = 20;
const uint8_t MENU_COUNT = 3;
const uint8_t ITEM_PER_PAGE = 4;

Menu curr_menu;
MenuItem curr_item;
int analog_read;
char buff[MAX_ITEMTITLE_LEN] = {'0'};
uint8_t mapped_read;
uint8_t now_item;
uint8_t now_page;
unsigned char instruction;
unsigned char screen_type;

const uint8_t bitmap_icons[MENU_COUNT][128] PROGMEM = {
  // 'home', 32x32px
  //home title cursor 40, 45 (for printing title)
  {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x0e, 0x7f, 0xf8, 
    0x00, 0x1c, 0x3f, 0xf8, 0x00, 0x38, 0x1c, 0x18, 0x00, 0x70, 0x0c, 0x18, 0x00, 0xe0, 0x00, 0x18, 
    0x01, 0xc0, 0x00, 0x18, 0x03, 0x80, 0x00, 0x18, 0x07, 0x00, 0x00, 0x18, 0x0c, 0x00, 0x00, 0x18, 
    0x18, 0x00, 0x00, 0x18, 0x70, 0x00, 0x00, 0x0e, 0xe0, 0x00, 0x00, 0x07, 0xf8, 0x00, 0x00, 0x1f, 
    0xfc, 0x00, 0x00, 0x3f, 0x0c, 0x00, 0x00, 0x30, 0x0c, 0x00, 0x00, 0x30, 0x0c, 0x00, 0x00, 0x30, 
    0x0c, 0x07, 0xe0, 0x30, 0x0c, 0x0f, 0xf0, 0x30, 0x0c, 0x0c, 0x30, 0x30, 0x0c, 0x0c, 0x30, 0x30, 
    0x0c, 0x0c, 0x30, 0x30, 0x0c, 0x0c, 0x30, 0x30, 0x0c, 0x0c, 0x30, 0x30, 0x0c, 0x0c, 0x30, 0x30, 
    0x0c, 0x0c, 0x30, 0x30, 0x0f, 0xfc, 0x3f, 0xf0, 0x0f, 0xfc, 0x3f, 0xf0, 0x00, 0x00, 0x00, 0x00
  },
  // 'gps', 32x32px
  //setting cursor title to 45, 45 for printing gps
  {
    0x00, 0x07, 0xe0, 0x00, 0x00, 0x1f, 0xfc, 0x00, 0x00, 0x78, 0x1e, 0x00, 0x00, 0xe0, 0x07, 0x00, 
    0x01, 0xc0, 0x03, 0x80, 0x01, 0x80, 0x01, 0xc0, 0x03, 0x01, 0x80, 0xc0, 0x03, 0x07, 0xe0, 0x60, 
    0x06, 0x0e, 0x70, 0x60, 0x06, 0x0c, 0x38, 0x60, 0x06, 0x18, 0x18, 0x60, 0x06, 0x18, 0x18, 0x60, 
    0x06, 0x0c, 0x38, 0x60, 0x06, 0x0e, 0x70, 0x60, 0x06, 0x07, 0xe0, 0x60, 0x03, 0x01, 0xc0, 0xc0, 
    0x03, 0x00, 0x00, 0xc0, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x00, 0xc0, 0x03, 0x80, 
    0x00, 0xc0, 0x03, 0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 0x30, 0x0c, 0x00, 
    0x00, 0x38, 0x1c, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x0c, 0x30, 0x00, 0x00, 0x06, 0x60, 0x00, 
    0x00, 0x07, 0xe0, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00
  },
  // 'setting', 32x32px
  //setting cursor title at 22, 45 for printing setting
  {
    0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x0d, 0xf6, 0x00, 0x00, 0x0f, 0xfe, 
    0x00, 0x00, 0x1f, 0x1f, 0x00, 0x00, 0x0e, 0x0e, 0x00, 0x00, 0x0e, 0x0c, 0x00, 0x00, 0x0e, 0x0e, 
    0x00, 0x7c, 0x1f, 0x1f, 0x00, 0x7c, 0x1f, 0xff, 0x00, 0x7c, 0x0f, 0xfe, 0x00, 0xfe, 0x00, 0xe0, 
    0x31, 0xff, 0x18, 0xe0, 0x7f, 0xff, 0xfc, 0x00, 0x7f, 0xff, 0xfc, 0x00, 0xff, 0xc7, 0xfe, 0x00, 
    0xff, 0x01, 0xfe, 0x00, 0x7f, 0x01, 0xfc, 0x00, 0x1e, 0x00, 0xf0, 0x00, 0x1e, 0x00, 0xf0, 0x00, 
    0x1e, 0x00, 0xf0, 0x00, 0x1e, 0x00, 0xf0, 0x00, 0x7f, 0x01, 0xfc, 0x00, 0xff, 0x03, 0xfe, 0x00, 
    0xff, 0xc7, 0xfe, 0x00, 0x7f, 0xff, 0xfc, 0x00, 0x7f, 0xff, 0xfc, 0x00, 0x31, 0xff, 0x18, 0x00, 
    0x00, 0xfe, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00
  }
};

MenuItem gps_item[] = {
  //MenuItem(uint8_t x, uint8_t y, uint8_t order, uint8_t page, bool nextPage, unsigned char fnct)
  MenuItem(5, 7, 0, 0, true, 's'),
  MenuItem(5, 21, 1, 0, true, 'p'),
  MenuItem(5, 35, 2, 0, true, 'w'),
  MenuItem(5, 49, 3, 0, true, 'd'),
  MenuItem(5, 7, 4, 1, true, 'e'),
  MenuItem(5, 21, 5, 1, false, 'x')
};

MenuItem home_item[] = {
  //MenuItem(uint8_t x, uint8_t y, uint8_t order, uint8_t page, bool nextPage, unsigned char fnct)
  MenuItem(5, 7, 0, 0, true, 'a'),
  MenuItem(5, 21, 1, 0, true, 'b'),
  MenuItem(5, 35, 2, 0, true, 'c'),
  MenuItem(5, 49, 3, 0, false, 'x')
};

MenuItem setting_item[] = {
  //MenuItem(uint8_t x, uint8_t y, uint8_t order, uint8_t page, bool nextPage, unsigned char fnct)
  MenuItem(5, 7, 0, 0, true, 'd'),
  MenuItem(5, 21, 1, 0, true, 'e'),
  MenuItem(5, 35, 2, 0, true, 'f'),
  MenuItem(5, 49, 3, 0, false, 'x')
};

const char home_item_1[] PROGMEM = {"Main screen"};
const char home_item_2[] PROGMEM = {"Date & Time"};
const char home_item_3[] PROGMEM = {"Temperature"};
const char home_item_4[] PROGMEM = {"Exit"};

const char gps_item_1[] PROGMEM = {"Show screen"};
const char gps_item_2[] PROGMEM = {"GPS status"};
const char gps_item_3[] PROGMEM = {"Start recording"};
const char gps_item_4[] PROGMEM = {"Pause"};
const char gps_item_5[] PROGMEM = {"Total distance"};
const char gps_item_6[] PROGMEM = {"Exit"};

const char setting_item_1[] PROGMEM = {"Erase data"};
const char setting_item_2[] PROGMEM = {"Set interval"};
const char setting_item_3[] PROGMEM = {"Battery saving"};
const char setting_item_4[] PROGMEM = {"Exit"};

const char* const item_title[][10] PROGMEM = {
  {home_item_1, home_item_2, home_item_3, home_item_4},//index 0 for home
  {gps_item_1,gps_item_2, gps_item_3, gps_item_4, gps_item_5, gps_item_6}, //index 1 for gps
  {setting_item_1, setting_item_2, setting_item_3, setting_item_4}, //index 2 for setting
};

Menu menu[] = {
  //Menu(MenuItem* item, uint8_t item_count, uint8_t order, bool isNext, bool isPrev)
  Menu(home_item, 4, 0, true, false),  //home menu
  Menu(gps_item, 6, 1, true, true), //gps menu
  Menu(setting_item, 4, 2, false, true) //setting menu
};

const char title_home[] PROGMEM = {"Home"};
const char title_gps[] PROGMEM = {"GPS"};
const char title_setting[] PROGMEM = {"Setting"};
const char *const menu_titles[3] PROGMEM = {title_home, title_gps, title_setting};

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0X3C)){Serial.println("SSD1306 allocation failed"); while(true){}}
  screen_type = 'm';
  instruction = 'o';
}

void loop() {
  // put your main code here, to run repeatedly:
  selectScreen();
}

void selectScreen(){
  switch(screen_type){
    case 'm':
      screenMenu();
      break;

    case 'i':
      screenItem();
      break;
  }
}

void screenMenu(){
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  analog_read = analogRead(INPUT_PIN);
  mapped_read = map(analog_read, 0, 1000, 0, 5); //make toHigh n+1 so that last reading are readable rather than flactuate between the boudary
  Serial.print("Analog value: ");
  Serial.println(analog_read);
  Serial.print("Mapped value: ");
  Serial.println(mapped_read);

  curr_menu = mapped_read <= (MENU_COUNT-1) ? menu[mapped_read] : menu[MENU_COUNT-1];
  display.clearDisplay();
  display.drawBitmap(50, 0, bitmap_icons[curr_menu.getOrder()], 32, 32, 1);
  strcpy_P(buff, (char *)pgm_read_word(&(menu_titles[curr_menu.getOrder()])));
  display.setCursor(getTitleStartX(buff), 45);
  display.print(buff);
  if(curr_menu.getIsHasNext()){drawArrowTriangle('n');}
  if(curr_menu.getIsHasPrev()){drawArrowTriangle('p');}
  display.display();

  if(digitalRead(BTN_PIN) == HIGH){
    screen_type = 'i';
  }
}

void screenItem(){
  analog_read = analogRead(INPUT_PIN);
  mapped_read = map(analog_read, 0, 1018, 0, 10); //there will be (10+1) options can be scrolled
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  
  curr_item = (mapped_read <= curr_menu.getItemCount()-1) ? *(curr_menu.getMenuItem()+mapped_read) : *(curr_menu.getMenuItem()+(curr_menu.getItemCount()-1));
  //since getMenuItem() return pointer to the array, we will access the pointer element using dereference operator
    
  display.fillTriangle(curr_item.getX(), curr_item.getY()+1, curr_item.getX(), curr_item.getY()+5, curr_item.getX()+3, curr_item.getY()+2, WHITE);
  display.setCursor(curr_item.getX()+8, curr_item.getY());
  //This is the critical part-reading the PROGMEM stored string
  strcpy_P(buff, (char *)pgm_read_word(&(item_title[curr_menu.getOrder()][curr_item.getOrder()])));
  display.print(buff);
  memset(buff, 0, MAX_ITEMTITLE_LEN);
    
  for(int i=0; i<curr_menu.getItemCount(); i++){
    MenuItem item = *(curr_menu.getMenuItem()+i);
    if(item.getPage() == curr_item.getPage() && (item.getOrder() != curr_item.getOrder())){
      display.setCursor(item.getX(), item.getY());
      strcpy_P(buff, (char *)pgm_read_word(&(item_title[curr_menu.getOrder()][item.getOrder()])));
      display.print(buff);
      memset(buff, 0, MAX_ITEMTITLE_LEN);
    }
  }
  display.display();  
  if(digitalRead(BTN_PIN) == HIGH){
    instruction = curr_item.getFunction();
    Serial.print("Instruction: ");
    Serial.println(curr_item.getFunction());
  }
  doFunction();
}

void drawArrowTriangle(unsigned char next_prev){
  switch(next_prev){
    case 'n':
      display.fillTriangle(120, 27, 120, 37, 125, 32, WHITE);
      break;

    case 'p':
      display.fillTriangle(3, 32, 8, 37, 8, 27, WHITE);
      break;
  }
}

void doFunction(){
  switch(instruction){
    case 'x':
      screen_type = 'm';
      break;
  }
}

uint8_t getTitleStartX(char* title){
  uint8_t elem_length = 0;
  while(*(title+elem_length) != '\0'){
    ++elem_length;
  }
  //Serial.print("Elemen length: ");
  //Serial.println(elem_length);
  return (SCREEN_WIDTH-(elem_length*10))/2;
}
