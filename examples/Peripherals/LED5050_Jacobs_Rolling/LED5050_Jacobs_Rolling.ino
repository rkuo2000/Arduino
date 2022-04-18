
#define RED_PIN   D1 //5
#define GREEN_PIN D2 //6
#define BLUE_PIN  D3 //3

// number of milliseconds to pause between each pass of the main Arduino loop
#define LOOP_DELAY 20
  
// number of loop cycles to wait before changing the brightness level
float RED_IDLES = 2;
float GREEN_IDLES = 4;
float BLUE_IDLES = 6;
  
  
// Brightness level of each color (0..255)
int Brightness_R;
int Brightness_G;
int Brightness_B;
  
// Direction of change in brightness level (-1 or +1)
int Direction_R;
int Direction_G;
int Direction_B;
  
// Counter of loop cycles left to wait before changing the pin value
int Timer_R;
int Timer_G;
int Timer_B;


void setup() {
  Serial.begin(115200);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  
  // set the initial brightness levels
  Brightness_R = 127;
  Brightness_G = 127;
  Brightness_B = 127;

  // set the initial direction
  Direction_R = 1;
  Direction_G = -1;
  Direction_B = 1;

  // set the initial timer values to the full count
  Timer_R = RED_IDLES;
  Timer_G = GREEN_IDLES;
  Timer_B = BLUE_IDLES;
}

void loop() {
  Serial.println(Brightness_B);
  // decrement each color's idle timer count
  Timer_R = Timer_R - 1;
  Timer_G = Timer_G - 1;
  Timer_B = Timer_B - 1;

  // if a color's idle timer has expired, change its brightness level, reverse the direction of change if completely bright or dark, and reset the timer
  if (Timer_R == 0) {
    Brightness_R = Brightness_R + Direction_R; 
    Timer_R = RED_IDLES;
    }
    
  if (Brightness_R == 0 or Brightness_R == 255) {
    Direction_R = Direction_R * -1;
    }
      
    
  if (Timer_G == 0){
    Brightness_G = Brightness_G + Direction_G;
    Timer_G = GREEN_IDLES;
    }  
  if (Brightness_G == 0 or Brightness_G == 255) {
    Direction_G = Direction_G * -1;
    } 

  if (Timer_B == 0) {
    Brightness_B = Brightness_B + Direction_B;
    Timer_B = BLUE_IDLES;
    }
  if (Brightness_B == 0 or Brightness_B == 255) {
    Direction_B = Direction_B * -1;
    }
    
  Brightness_B = constrain(Brightness_B, 0, 255);
  Brightness_G = constrain(Brightness_G, 0, 255);
  Brightness_R = constrain(Brightness_R, 0, 255); //brightness_r + direction_r?
    
  // send the LED levels to the Arduino pins
  analogWrite (RED_PIN, Brightness_R);
  analogWrite (GREEN_PIN, Brightness_G);
  analogWrite (BLUE_PIN, Brightness_B);
    
  // slow the loop down a bit
  delay (LOOP_DELAY);
}
