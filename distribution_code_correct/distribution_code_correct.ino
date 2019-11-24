#include <gamma.h>
#include <RGBmatrixPanel.h>
#include <Adafruit_GFX.h>

// Variable for time since last loop
unsigned long loopTime = 0;

// Variable for time since last ball move
unsigned long ballTime = 0;

// Milliseconds per Frame, so 40 frames in a second
const int CYCLE_LENGTH = 25;

// Previous Potentiometer value, used to smooth
int prevPotValue = -10;

// define the wiring of the LED screen
const uint8_t CLK  = 8;
const uint8_t LAT = A3;
const uint8_t OE = 9;
const uint8_t A = A0;
const uint8_t B = A1;
const uint8_t C = A2;

// define the wiring of the inputs
const int POTENTIOMETER_PIN_NUMBER = 5;
const int BUTTON_PIN_NUMBER = 10;

// global constant for the number of Invaders in the game
const int NUM_ENEMIES = 16;

// a global variable that represents the LED screen
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

// the following functions are for printing messages
void print_level(int level);
void print_lives(int lives);
void game_over();


class Color {
  public:
    int red;
    int green;
    int blue;
    Color() {
      red = 0;
      green = 0;
      blue = 0;
    }
    Color(int r, int g, int b) {
      red = r;
      green = g;
      blue = b;
    }
    uint16_t to_333() const {
      return matrix.Color333(red, green, blue);
    }
};

const Color BLACK(0, 0, 0);
const Color RED(4, 0, 0);
const Color ORANGE(6, 1, 0);
const Color YELLOW(4, 4, 0);
const Color GREEN(0, 4, 0);
const Color BLUE(0, 0, 4);
const Color PURPLE(1, 0, 2);
const Color WHITE(4, 4, 4);
const Color LIME(2, 4, 0);
const Color AQUA(0, 4, 4);

class Invader {
  public:
    // Constructors
    Invader() {
      x = 0;
      y = 0;
      // invader is dead when strength = 0
      strength = 0;
    }
    // sets values for private data members x and y
    Invader(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    }
    // sets values for private data members
    Invader(int x_arg, int y_arg, int strength_arg) {
      x = x_arg;
      y = y_arg;
      strength = strength_arg;
    }
    // sets values for private data members
    void initialize(int x_arg, int y_arg, int strength_arg) {
      x = x_arg;
      y = y_arg;
      strength = strength_arg;
    }
    
    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    int get_strength() const {
      return strength;
    }

    // Moves the Invader down the screen by one row
    // Modifies: y
    void move() {
      y = y + 1;
    }
    
    // draws the Invader if its strength is greater than 0
    // calls: draw_with_rgb
    void draw() {
      if (strength == 1) {
        draw_with_rgb(RED, BLUE);
      }
      else if (strength == 2) {
        draw_with_rgb(ORANGE, BLUE);
      }
      else if (strength == 3) {
        draw_with_rgb(YELLOW, BLUE);
      }
      else if (strength == 4) {
        draw_with_rgb(GREEN, BLUE);
      }
      else if (strength == 5) {
        draw_with_rgb(BLUE, BLUE);
      }
      else if (strength == 6) {
        draw_with_rgb(PURPLE, BLUE);
      }
      else if (strength == 7) {
        draw_with_rgb(WHITE, BLUE);
      }
    }
    
    // draws black where the Invader used to be
    // calls: draw_with_rgb
    void erase() {
      draw_with_rgb(BLACK, BLACK);
    }    
    
    // Invader is hit by a Cannonball.
    // Modifies: strength
    // calls: draw, erase
    void hit() {
      if (strength > 1) {
        strength -= 1;
        draw();
      }
      else {
        erase();
        strength = 0;
      }
    }

  private:
    int x;
    int y;
    int strength;
    
    // draws the Invader
    void draw_with_rgb(Color body_color, Color eye_color) {
      matrix.drawPixel(x + 1, y, body_color.to_333());
      matrix.drawPixel(x + 2, y, body_color.to_333());
      matrix.drawPixel(x, y + 1, body_color.to_333());
      matrix.drawPixel(x + 1, y + 1, eye_color.to_333());
      matrix.drawPixel(x + 2, y + 1, eye_color.to_333());
      matrix.drawPixel(x + 3, y + 1, body_color.to_333());
      matrix.drawPixel(x, y + 2, body_color.to_333());
      matrix.drawPixel(x + 1, y + 2, body_color.to_333());
      matrix.drawPixel(x + 2, y + 2, body_color.to_333());
      matrix.drawPixel(x + 3, y + 2, body_color.to_333());
      matrix.drawPixel(x, y + 3, body_color.to_333());
      matrix.drawPixel(x + 3, y + 3, body_color.to_333());
      
    }
};


class Cannonball {
  public:
    Cannonball() {
      x = 0;
      y = 0;
      fired = false;
    }
    
    // resets private data members to initial values
    void reset() {
      x = 0;
      y = 0;
      fired = false;
    }
    
    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    bool has_been_fired() const {
      return fired;
    }
    
    // sets private data members
    void fire(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
      fired = true;
      draw();
    }
    
    // moves the Cannonball and detects if it goes off the screen
    // Modifies: y, fired
    void move() {
      if (y == 0){
        fired = false;
        erase();
      }
      else {
        erase();
        y = y - 1;
        draw();

      }
    }
    
    // resets private data members to initial values
    void hit() {
      erase();
      x = 0;
      y = 0;
      fired = false;
    }
    
    // draws the Cannonball, if it is fired
    void draw() {
      if (has_been_fired()) {
        matrix.drawPixel(x, y, ORANGE.to_333());
        matrix.drawPixel(x, y - 1, ORANGE.to_333());
      }
    }
    
    // draws black where the Cannonball used to be
    void erase() {
      matrix.drawPixel(x, y, BLACK.to_333());
      matrix.drawPixel(x, y - 1, BLACK.to_333());
    }

  private:
    int x;
    int y;
    bool fired;
};


class Player {
  public:
    Player() {
      x = 1;
      y = 15;
      lives = 3;
    }
    
    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    int get_lives() const {
      return lives;
    }
    
    // setter
    void set_x(int x_arg) {
      x = x_arg;
    }
    
    // Modifies: lives
    void die() {
      if(lives != 1){
        lives -= 1;
        matrix.fillScreen(BLACK.to_333());
        matrix.setCursor(0, 0);
        matrix.print("lives");
        matrix.print(lives);
        delay(2000);
        initialize(1, 15);
        
      }
      else{
        game_over();
      }
      
    }
    
    // draws the Player
    // calls: draw_with_rgb
    void draw() {
      draw_with_rgb(AQUA);
    }
    
    // draws black where the Player used to be
    // calls: draw_with_rgb
    void erase() {
      draw_with_rgb(BLACK);
    }

  private:
    int x;
    int y;
    int lives;

    // sets private data members x and y to initial values
    void initialize(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    }
    
    // draws the player
    void draw_with_rgb(Color color) {
      matrix.drawPixel(x, y, color.to_333());
      matrix.drawPixel(x + 1, y, color.to_333());
      matrix.drawPixel(x - 1, y, color.to_333());
      matrix.drawPixel(x, y - 1, color.to_333());
    }
};

class Game {
  public:
    Game() {
      level = 1;
      time = 0;
      cycles = 0;
    }
    
    int get_cycles() {
      return cycles;
    }

    void set_cycles(int cycleNum) {
      cycles = cycleNum;
    }
    // sets up a new game of Space Invaders
    // Modifies: global variable matrix
    void setupGame() {
      matrix.fillScreen(BLACK.to_333());
      matrix.setTextSize(1);
      matrix.setCursor(0, 0);
      matrix.print("level");
      matrix.print(level);
      delay(2000);
      matrix.fillScreen(BLACK.to_333());
      if (level == 1) {
        for (int i = 0; i < 8; i++) {
          enemies[i].initialize((i * 4), 0, 1);
          enemies[i].draw();
        }
        for (int j = 8; j < 16; j++) {
          enemies[j].initialize(((j - 8) * 4), 4, 0);
          enemies[j].draw();
        }
      }
      if (level == 2) {
        for(int i = 0; i < 8; i += 2){
          enemies[i].initialize((i * 4), 0, 1);
          enemies[i].draw();
          enemies[i + 1].initialize(((i + 1) * 4), 0, 2);
          enemies[i + 1].draw();
        }
        for(int i = 8; i < 16; i += 2){
          enemies[i].initialize(((i - 8) * 4), 4, 2);
          enemies[i].draw();
          enemies[i + 1].initialize((((i - 8) + 1) * 4), 4, 1);
          enemies[i + 1].draw();
        }
      }

      if (level == 3) {
        for(int i = 0; i < 5; i++){
          enemies[i].initialize((i * 4), 0, i + 1);
          enemies[i].draw();
        }
        for(int i = 5; i < 8; i++){
          enemies[i].initialize((i * 4), 0, i - 4);
          enemies[i].draw();
        }
        for(int i = 8; i < 10; i++){
          enemies[i].initialize(((i - 8) * 4), 4, i - 4);
          enemies[i].draw();
        }
        for(int i = 10; i < 15; i++){
          enemies[i].initialize(((i - 8) * 4), 4, i - 9);
          enemies[i].draw();
        }
        enemies[15].initialize(28, 4, 1);
        enemies[15].draw();
      }
      if (level == 4){
        for(int i = 0; i < 8; i += 2){
          enemies[i].initialize((i * 4), 0, 5);
          enemies[i].draw();
          enemies[i + 1].initialize(((i + 1) * 4), 0, 4);
          enemies[i + 1].draw();
        }
        for(int i = 8; i < 16; i += 2){
          enemies[i].initialize(((i - 8) * 4), 4, 2);
          enemies[i].draw();
          enemies[i + 1].initialize((((i - 8) + 1) * 4), 4, 3);
          enemies[i + 1].draw();
        }
      }
      if (level >= 5) {
        for (int i = 0; i < 8; i++) {
          enemies[i].initialize((i * 4), 0, random(1, 8));
          enemies[i].draw();
        }
        for (int j = 8; j < 16; j++) {
          enemies[j].initialize(((j - 8) * 4), 4, random(1, 8));
          enemies[j].draw();
        }
      }
    }

    bool moveInvader() {
        if (cycles % 120 == 0) {
          return true;
        }
        else {
          return false;
        }
    }
    
    // advances the game simulation one step and renders the graphics
    // see spec for details of game
    // Modifies: global variable matrix
    void update(int potentiometer_value, bool button_pressed) {

      
      // Moving the Player
      if(((player.get_x()) != ceil((double(potentiometer_value) / 1024.0) * 29) + 1)
                                    && (abs(prevPotValue - potentiometer_value) > 15)){
        prevPotValue = potentiometer_value;
        player.erase();
        player.set_x(ceil((double(potentiometer_value) / 1024.0) * 29) + 1);
        player.draw();
      }
      
       // Moving the Canonball
      if(ball.has_been_fired() && (millis() - ballTime > 3)){
        ballTime = millis();
        ball.move();
      }
      
      // Firing the Canonball
      if(button_pressed && !ball.has_been_fired()){
        ball.fire(player.get_x(), 13);
        ballTime = millis();
      }

      //Moving the Invaders

      //Counting the amount of dead Invaders in the front row
      int front_Dead = 0;
      for (int i = 8; i < 16; i++) {
        if (enemies[i].get_strength() == 0) {
          front_Dead++; 
        }
      }

      //Counting the amount of dead Invaders in the back row
      int back_Dead = 0;
      for (int i = 0; i < 8; i++) {
        if (enemies[i].get_strength() == 0) {
          back_Dead++; 
        }
      }
      
      if (front_Dead < 8 && moveInvader() == true) {
        // move invaders 8-15 
        for (int bot = 8; bot < 16; bot++) {
          int x = enemies[bot].get_x();
          int y = enemies[bot].get_y();
          int s = enemies[bot].get_strength();
          y = y + 1;
          enemies[bot].erase();
          enemies[bot].initialize(x, y, s);
          enemies[bot].draw();
        }
      }
     else if (front_Dead == 8 && moveInvader() == true) {
        for (int top = 0; top < 8; top++) {
          int x = enemies[top].get_x();
          int y = enemies[top].get_y();
          int s = enemies[top].get_strength();
          y = y + 1;
          enemies[top].erase();
          enemies[top].initialize(x, y, s);
          enemies[top].draw();
      }
     }

     // Checking for inavder collision with player or edge
      for(int i = 0; i < 16; i++){
        if(enemies[i].get_strength() != 0){
          if(enemies[i].get_y() == 13){
            player.die();
            setupGame();
          }
         if((enemies[i].get_y() == 12) && (enemies[i].get_x() - player.get_x() <= 1) && (enemies[i].get_x() - player.get_x() >= -4)){
           player.die();
           setupGame();
         }
         if((enemies[i].get_y() == 11) && ((player.get_x() == enemies[i].get_x()) || (player.get_x() == enemies[i].get_x() + 3))){
          player.die();
          setupGame();
        }
      }
    }
     

      //Moving the ball
      
     if (ball.has_been_fired() == true) {
      int ball_x = ball.get_x();
      int ball_y = ball.get_y();
      for (int i = 0; i < 16; i++) {
        int inv_x = enemies[i].get_x(); 
        int inv_y = enemies[i].get_y();
        if (enemies[i].get_strength() != 0 && ((ball_x >= inv_x && ball_x <= inv_x + 3) && (ball_y >= inv_y && ball_y <= inv_y + 3))) {
          ball.hit();
          enemies[i].hit();
        }
      }
     }

     //Checking if the level has been completed
     if(back_Dead == 8){
      level += 1;
      setupGame();
    }
  }



  private:
    int level;
    unsigned long time;
    Player player;
    Cannonball ball;
    Invader enemies[NUM_ENEMIES];
    unsigned long cycles;

    // check if Player defeated all Invaders in current level
    bool level_cleared() {
    }

    // set up a level
    void reset_level() {
    }
};

// a global variable that represents the game Space Invaders
Game game;

// see https://www.arduino.cc/reference/en/language/structure/sketch/setup/
void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN_NUMBER, INPUT);
  matrix.begin();
  Player player;
  randomSeed(analogRead(0));
  matrix.print("lives");
  matrix.print(3);
  delay(2000);
  game.setupGame();
  
}


// see https://www.arduino.cc/reference/en/language/structure/sketch/loop/
void loop() {
  int potentiometer_value = analogRead(POTENTIOMETER_PIN_NUMBER);
  bool button_pressed = (digitalRead(BUTTON_PIN_NUMBER) == HIGH);
  if((millis() - loopTime) > CYCLE_LENGTH){
    loopTime = millis();
    game.set_cycles(game.get_cycles() + 1);
    game.update(potentiometer_value, button_pressed);
    
  }
  
}

// displays Level
void print_level(int level) {
}

// displays number of lives
void print_lives(int lives) {
}

// displays "game over"
void game_over() {
  matrix.fillScreen(BLACK.to_333());
  matrix.setCursor(0, 0);
  matrix.print("GAME ");
  matrix.print("OVER");
  while (1 == 1){
    
  }
}
