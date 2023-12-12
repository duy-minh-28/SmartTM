#include "Oled.h"  

// SSD1306Wire display(0x3c, SDA, SCL); 
SSD1306 display(0x3c, 5, 4);

void OledSetup(){ 
  display.init(); 
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setColor(WHITE);
} 

void drawTextAlignmentDemo(){
  display.clear();
  display.setColor(WHITE);
  // Text alignment demo
  display.setFont(ArialMT_Plain_10);

  // The coordinates define the left starting point of the text
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 10, "Left aligned (0,0)");

  // The coordinates define the center of the text
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(0, 22, "Center aligned (64,22)");

  // The coordinates define the right end of the text
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(0, 33, "Right aligned (128,33)");
 
  display.display();
}

void drawReceiveFromSlave(int x, int y, String text){
  display.drawString(x, y, text);  
  display.display();   
}

void displayOLED(){
  display.display();   
}
void clearOLED(){ 
  display.clear();  
}
