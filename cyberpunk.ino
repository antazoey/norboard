/*
 * Made by Sam n' Juliya 2019
 */

#include <GOFi2cOLED.h>
#include <avr/pgmspace.h>

GOFi2cOLED GOFoled;

const unsigned char PROGMEM bitmap[]= { 0xFF };

struct Apple {
  int x = 120;
  int y = 8;
};

struct Snake {
  int x = 0;
  int y = 0;
};
  
unsigned char slices[] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

#ifdef __AVR_ATtiny85__
// Connect a LED to this Pin. It might be different in different ATtiny micro controllers
int led = 1;
#else
int led = 13;
#endif

int apple_c = 10;
Apple** apples = (Apple**) malloc(apple_c * sizeof(Apple*));

Snake * snake =(Snake*) malloc(sizeof(Snake));

int score = 0;

bool eatLight = false;

void setup()   {                  
  pinMode(led, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  //default address is 0x3D.
  GOFoled.init(0x3C);
  // init done
  
  GOFoled.display(); // show splashscreen
  GOFoled.clearDisplay();

  GOFoled.setTextSize(1);
  GOFoled.setTextColor(WHITE);

  // Allocate 10 Apples
  for (int i = 0; i < apple_c; i++) {
    
    Apple * apple = (Apple*) malloc(sizeof(Apple));
    apple->x = i * 30;
    apple->y = i * 10;
    apples[i] = apple;
  }

  Serial.begin(9600);
}

void loop() {
  
  GOFoled.setCursor(2,2);
  GOFoled.print(score); 
  
  eatLight = false;
  const int total_snake_pix = 64;
  const int snake_start = 20;
  snake->y = drawsnake(total_snake_pix, snake_start);
  snake->x = snake_start + total_snake_pix;

  // Draw the apples.
  static int draw_i = 0;
  static bool apple_drawn = false;
  for (int j = 0; j < 10; j++) {
    
    Apple * apple = apples[j];

    drawapple(apple);
    eat(apple, snake);
    apple->x -= 1;

    if (apple->x < 0)
    {
      apple->x = 120;
    }
  }

  if (eatLight == true)
  {
    digitalWrite(7, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(7, LOW);
    digitalWrite(LED_BUILTIN, LOW);
  }
}


// Returns snake head_y
int drawsnake(int total, int start) {
  // Draw horizontally
  for (int i = 0; i < total; i++)
  {
    unsigned char slice = slices[i];
    GOFoled.drawBitmap(start + i, slice, bitmap, 1, 1, WHITE);

    // Draw vertically
    if (i > 0)
    {
      for (int j = 0; j < (slices[i - 1] - slice); j++)
      {
        GOFoled.drawBitmap(start + i, slice + j + 1, bitmap, 1, 1, WHITE);     
      }
      for (int j = 0; j > (slices[i - 1] - slice); j--)
      {
        GOFoled.drawBitmap(start + i, slice - j - 1, bitmap, 1, 1, WHITE);
      }
    }
  }
  GOFoled.display();
  delay(10);  
  GOFoled.clearDisplay();
  for( int i = 0; i < (total - 1); i++)
  {
    slices[i] = slices[i+1];
  }
  // Update head of snake with potieometer reading
  int r = analogRead(A0)/10;
  slices[63] = r;
  return r;
}


void drawapple(Apple * apple) {
  GOFoled.drawBitmap(apple->x, apple->y, bitmap, 1, 1, WHITE);
}

int distance(int x1, int y1, int x2, int y2) {
  int dx = (x1 - x2) * (x1 - x2);
  int dy = (y1 - y2) * (y1 - y2);
  return sqrt(dx + dy);
}

void eat(Apple * apple, Snake * snake) {
  if (distance(apple->x, apple->y, snake->x, snake->y) <= 2)
  {
    apple->x = 0;
    score += 20;
    eatLight = true;
    Serial.write("Nummy yum num nums\n");
  }
}
