#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 
#define SCREEN_WIDTH 128 // Width pixel
#define SCREEN_HEIGHT 64 // Height pixel
//reset pin, necessary for Adafruit_SSD1306 library
#define OLED_RESET 6
//Key Pin
#define KEY_UP_PIN 2
#define KEY_DOWN_PIN 3
#define KEY_LEFT_PIN 4
#define KEY_RIGHT_PIN 5
//Key Status (Low Active)
#define KEY_NULL 4
#define KEY_UP 0
#define KEY_DOWN 1
#define KEY_LEFT 2
#define KEY_RIGHT 3
//Screen Status
#define SCREEN_GAMEOVER 2
#define SCREEN_MAINMENU 0
#define SCREEN_INGAME 1
#define SNAKE_LENGTH_MAX 127

//instantiation
Adafruit_SSD1306 screen(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);  

uint8_t snakeX[SNAKE_LENGTH_MAX];
uint8_t snakeY[SNAKE_LENGTH_MAX];
uint8_t snakeXHead = 50;
uint8_t snakeYHead = 39;
uint8_t foodPos[2]; //the location of the ramdomly spawned food
uint8_t snakeLength = 4;
uint8_t currentDirection = KEY_RIGHT; //how snake moves
bool isFrozen = false;  //when dead, let player have a chance to take a glance at the last scenary
uint8_t speed = 5;  //how fast the snake run
int score = 0;  
uint8_t currentScreen = SCREEN_MAINMENU;
//uint8_t lastKey = KEY_NULL;
uint8_t currentKey = KEY_NULL;
uint8_t lastKey = KEY_NULL; //the key state of last loop

void foodGenerate() //randomly generate food position continiously until not on snake body
{
  bool isOnSnake = true;
  uint8_t randX;
  uint8_t randY;
  while(isOnSnake)
  {
    isOnSnake = false;
    randX = random(64)*2;   //match the snake's position(X have to be even, and Y have to be odd, because one snake body and food are all 2*2pixel)
    randY = 11 + random(0,26)*2;  //ensure not generate on title bar
    for(int i=0;i<snakeLength;i++)
    { //if food is generated on snake's body, then regenerate
      if(randX == snakeX[i] && randY == snakeY[i]) isOnSnake = true;  
      break;
    }
    if(randX == snakeXHead && randY == snakeYHead) isOnSnake = true;  
  }
  foodPos[0] = randX;
  foodPos[1] = randY;
}

void keyHandler()
{
  if(!digitalRead(KEY_LEFT_PIN)) currentKey = KEY_LEFT;
  else if(!digitalRead(KEY_RIGHT_PIN)) currentKey = KEY_RIGHT;
  else if(!digitalRead(KEY_UP_PIN)) currentKey = KEY_UP;
  else if(!digitalRead(KEY_DOWN_PIN)) currentKey = KEY_DOWN;
  else currentKey = KEY_NULL;
}

void keyMainMenuHandler()
{
  if( (currentKey == KEY_UP && lastKey != KEY_UP) || (currentKey == KEY_DOWN && lastKey != KEY_DOWN)) currentScreen = SCREEN_INGAME;
  if(currentKey == KEY_LEFT && lastKey != KEY_LEFT)
    if(speed>1) speed -= 1;
  if(currentKey == KEY_RIGHT && lastKey != KEY_RIGHT)
    if(speed<9) speed += 1;
}

void keyGameOverHandler()
{
  if(currentKey != KEY_NULL) currentScreen = SCREEN_MAINMENU;
}

void keyInGameHandler()
{
  if(isFrozen)
  {
    if(currentKey != KEY_NULL && lastKey == KEY_NULL)
    {
      currentScreen = SCREEN_GAMEOVER;
      isFrozen = false;
      snakeXHead = 50;
      snakeYHead = 39;
      snakeLength = 4;
      
      currentDirection = KEY_RIGHT;
      for(int i=4;i<SNAKE_LENGTH_MAX;i++) 
      {
        snakeX[i] = 0;
        snakeY[i] = 0;
      }
      for(int i=0;i<4;i++) snakeY[i] = 50;
      snakeX[0] = 48;
      snakeX[1] = 46;
      snakeX[2] = 44;
      snakeX[3] = 42;
      foodGenerate();
    } 
  }
  else
  {
    if (currentDirection == KEY_RIGHT || currentDirection == KEY_LEFT)
      if(currentKey == KEY_UP)  currentDirection = KEY_UP;
      if(currentKey == KEY_DOWN) currentDirection = KEY_DOWN;
    if (currentDirection == KEY_UP || currentDirection == KEY_DOWN)
      if(currentKey == KEY_LEFT)  currentDirection = KEY_LEFT;
      if(currentKey == KEY_RIGHT) currentDirection = KEY_RIGHT;
  }
}

void commonDisplay()
{
  screen.clearDisplay();
  screen.setTextSize(1);
  screen.setCursor(0, 0);
  screen.print("Nano Snake");
  screen.drawLine(0,10,128,10,SSD1306_WHITE);
}

void displayMainMenu()
{
  commonDisplay();
  screen.setCursor(0, 55);
  screen.print("  UP / DOWN TO START ");
  screen.setCursor(49, 14);
  screen.print("SPEED");
  screen.setTextSize(2);
  screen.setCursor(0, 30);
  screen.print("<");
  screen.setCursor(58, 30);
  screen.print(speed);
  screen.setCursor(116, 30);
  screen.print(">");
  screen.display();
}

void displayGameOver()
{
  
  commonDisplay();
  screen.setTextSize(2);
  screen.setCursor(16, 20);
  screen.print("GAMEOVER");
  screen.setCursor(4, 50);
  screen.print("SCORE:");
  screen.print(score);
  screen.display();
}

void inGameAlgorithm()
{
  if(!isFrozen)
  {
    //handle snake movement
    for(int i=snakeLength;i>0;i--)
    {
      snakeX[i] = snakeX[i-1];
      snakeY[i] = snakeY[i-1];
    }
    snakeX[0] = snakeXHead;
    snakeY[0] = snakeYHead;
    if(currentDirection == KEY_RIGHT) snakeXHead += 2;
    else if(currentDirection == KEY_LEFT) snakeXHead -= 2;
    else if(currentDirection == KEY_UP) snakeYHead -= 2;
    else if(currentDirection == KEY_DOWN) snakeYHead += 2;
    //handle if eat food
    if(snakeXHead == foodPos[0] && snakeYHead == foodPos[1])
    {
      score += speed;
      snakeLength += 1;
      foodGenerate();
    }
    //handle if dead (out of range / eat itself)
    if(snakeXHead < 0 || snakeXHead > 127 || snakeYHead < 11 || snakeYHead > 63)
      isFrozen = true;
    for(int i=0;i<snakeLength;i++)
      if(snakeXHead == snakeX[i] && snakeYHead == snakeY[i])
      {
        isFrozen = true;
        break;
      }
  }
}

void displayInGame()
{
  if(!isFrozen)
  {
    commonDisplay();
    screen.setCursor(104, 0);
    screen.print(score);
    for(int i=0;i<snakeLength;i++)
      screen.drawRect(snakeX[i], snakeY[i], 2, 2, SSD1306_WHITE);
    screen.drawRect(snakeXHead, snakeYHead, 2, 2, SSD1306_WHITE);
    screen.drawRect(foodPos[0], foodPos[1], 2, 2, SSD1306_WHITE);
    screen.display();
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //MsTimer2::set(20, keyHandler); 
  //MsTimer2::start();  // enables the interrupt.
  pinMode(KEY_UP_PIN, INPUT_PULLUP);
  pinMode(KEY_DOWN_PIN, INPUT_PULLUP);
  pinMode(KEY_LEFT_PIN, INPUT_PULLUP);
  pinMode(KEY_RIGHT_PIN, INPUT_PULLUP);
  screen.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  screen.setTextColor(WHITE);
  randomSeed(analogRead(A0)); //for food generating
  foodGenerate();
  for(int i=4;i<SNAKE_LENGTH_MAX;i++) 
  {
    snakeX[i] = 0;
    snakeY[i] = 0;
  }
  for(int i=0;i<4;i++) snakeY[i] = 39;
  snakeX[0] = 48;
  snakeX[1] = 46;
  snakeX[2] = 44;
  snakeX[3] = 42;
}

void loop() {
  // put your main code here, to run repeatedly:
  keyHandler();
  Serial.println(currentKey);
  if(currentScreen == SCREEN_MAINMENU)
  {
    score = 0;
    keyMainMenuHandler();
    displayMainMenu();
  }
  else if(currentScreen == SCREEN_GAMEOVER)
  {
    keyGameOverHandler();
    displayGameOver();
  }
  else if(currentScreen == SCREEN_INGAME)
  {
    keyInGameHandler();
    inGameAlgorithm();
    displayInGame();
  }

  switch(speed)
  {
    case 1:
      delay(150);
      break;
    case 2:
      delay(130);
      break;
    case 3:
      delay(110);
      break;
    case 4:
      delay(90);
      break;
    case 5:
      delay(70);
      break;
    case 6:
      delay(50);
      break;
    case 7:
      delay(40);
      break;
    case 8:
      delay(30);
      break;
    case 9:
      delay(20);
      break;
  }
  lastKey = currentKey;
}
