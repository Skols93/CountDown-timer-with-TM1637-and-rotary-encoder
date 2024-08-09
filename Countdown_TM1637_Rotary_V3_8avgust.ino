#include <TM1637Display.h>
#include <Encoder.h>

// Define TM1637 pins
#define CLK 4
#define DIO 5

// Define encoder pins
#define ENCODER_CLK 2
#define ENCODER_DT 6
#define ENCODER_SW 3

const uint8_t SEG_DONE[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_C | SEG_E | SEG_G,                           // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
};

const uint8_t RSET[] = {
  SEG_E | SEG_G,    // R
  SEG_A | SEG_C | SEG_D | SEG_F | SEG_G, // S
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G, // E
  SEG_D | SEG_E | SEG_F | SEG_G         // T
};

const uint8_t STRT[] = {
  SEG_A | SEG_C | SEG_D | SEG_F | SEG_G, // S
  SEG_D | SEG_E | SEG_F | SEG_G,        // T
  SEG_E | SEG_G,                    // R
  SEG_D | SEG_E | SEG_F | SEG_G         // T
};
const uint8_t HI[] = {
  SEG_B | SEG_C | SEG_E | SEG_F | SEG_G, // H
   
  SEG_E | SEG_F                         // I
 
};

const uint8_t ENTR[] = {
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G, // E
  SEG_C | SEG_E | SEG_G,                 // n
  SEG_D | SEG_E | SEG_F | SEG_G,         // t
  SEG_E | SEG_G                          // r
};

// Initialize TM1637 display
TM1637Display display(CLK, DIO);

// Initialize encoder
Encoder myEnc(ENCODER_CLK, ENCODER_DT);
volatile bool encoderButtonPressed = false;
volatile unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 150;
// Variables to store encoder values
long oldPosition  = -999;
int menu = 1;
int menuP = 0;
bool inMenu = false;
int digitIndex = 0;
int digits[4] = {0, 0, 0, 0};
int broj;
int displayValue;
int Starbroj;

const int buttonPin = 3;         // Pin where the push button is attached
volatile bool buttonPressed = false; // Boolean variable to track button press
unsigned long debounceD = 200;   // Debounce time in microseconds

bool whileMenu = false;
volatile unsigned long totalSeconds = 0; // Counter for total seconds
volatile bool counting = false;          // Flag to indicate if counting is active

void buttonISR() {
  buttonPressed = true;             // Set the flag to true
  if(inMenu==false) inMenu = true; 
  whileMenu = true;
  delayMicroseconds(50); // Debounce delay in microseconds
  buttonPressed = false;            // Reset the flag to false
}

void Dis(int menu)
{
  switch(menu){
    case 1:
      //display.clear();
      display.setSegments(RSET);
      Serial.println("Reset!");
      delay(1);
      break;
    case 2:
      //display.clear();
      display.setSegments(STRT);
      Serial.println("Start!");
      delay(1);
      break;
  }
}

void setup() {
  // Set button pin as input
  pinMode(13, OUTPUT);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  digitalWrite(13, LOW);
  Serial.begin(9600);
  //pinMode(ENCODER_SW, INPUT_PULLUP); // Set button pin as input with internal pull-up resistor
  attachInterrupt(digitalPinToInterrupt(ENCODER_SW), buttonISR, FALLING);

  noInterrupts();           // Disable all interrupts
  TCCR1A = 0;               // Set entire TCCR1A register to 0
  TCCR1B = 0;               // Same for TCCR1B
  TCNT1 = 0;                // Initialize counter value to 0
  // Set compare match register for 1 Hz increments
  OCR1A = 15624;            // (16*10^6) / (1024*1) - 1 (must be <65536)
  // Turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // Enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  interrupts();             // Enable all interrupts
  
  // Initialize display
  display.setBrightness(0x0f); // Set brightness (0x00 to 0x0f)
  display.clear();
}
 
int digitLimit;
long newPosition;

// Reset menu - reset the entire counter to zero and the values
void menu1() {  
  totalSeconds = 0;
  digits[0] = digits[1] = digits[2] = digits[3] = 0;
  display.clear();
  display.showNumberDec(0, true); // Show 00:00 on display
  inMenu = true; // Exit menu
  goto Labela;
}
void loop() {
  
  counting = false; 
  Serial.println("Start!");
  // Show Enter for 3 seconds
  unsigned long startTime = millis();
  while (millis() - startTime < 3000) {
    display.setSegments(ENTR);
  }


Labela:
  int buttonPressCount = 0;
  digitIndex = 0;
  display.clear();
  // Loop to set the four digit values
  while(buttonPressCount < 4) {
     Serial.println("Four digit values setting!");
     newPosition = myEnc.read() / 4; // Dividing by 4 to match your incrementsPerDetent

    if (newPosition != 0) {
      int increment = (newPosition > 0) ? 1 : -1;
      myEnc.write(0); // Reset encoder position

      digitLimit = (digitIndex % 2 == 0) ? 5 : 9;
      if (digitIndex < 2) {
        digitLimit = (digitIndex == 0) ? 9 : 9; // 0-9 for hours digits
      }
      digits[digitIndex] += increment;
      if (digits[digitIndex] < 0) digits[digitIndex] = digitLimit;
      if (digits[digitIndex] > digitLimit) digits[digitIndex] = 0;
    }

    // Read the switch state (if your encoder has a switch)
    int btnState = digitalRead(ENCODER_SW);
    if (btnState == LOW && (millis() - lastButtonPress) > debounceDelay) {
      encoderButtonPressed = true;
      lastButtonPress = millis();
    }

    if (encoderButtonPressed) {
      encoderButtonPressed = false;
      buttonPressCount++;
      if (buttonPressCount < 4) {
        digitIndex++;
      }
    }
    

    switch(digitIndex)
    {
      case 0:
        display.showNumberDec(digits[0],false,1,digitIndex);
      break;
      case 1:
        display.showNumberDec(digits[1],false,1,digitIndex);
      break;
      case 2:
       display.showNumberDec(digits[2],false,1,digitIndex);
      break;
      case 3:
        display.showNumberDec(digits[3],false,1,digitIndex);
      break;

    }
    int displayValue = (digits[0] * 1000) + (digits[1] * 100) + (digits[2] * 10) + digits[3];
    //display.showNumberDecEx(displayValue, 0b01000000 << digitIndex, true);
    
    delay(10);
  }
  inMenu = false;
  // Menu selection loop
  display.clear();
  broj = 1;
  Dis(broj);
  while (inMenu==false) {
    Serial.print(", inMenu:");
    Serial.println(inMenu);
    // Read the encoder position
    newPosition = myEnc.read() / 4; // Adjust for sensitivity
    
    if(newPosition != oldPosition) {
     if(newPosition % 2 == 0) {
        
        broj = 1;
      } 
      else 
      {
        broj = 2;
      }
      
      
      oldPosition = newPosition;
      if(Starbroj != broj)
      {
        Dis(broj);
      }
     

      

      Starbroj = broj;
    }

    // Check if the button is pressed
    if(buttonPressed){
      inMenu=true;
    }
      
        oldPosition = 0;
    
  }

  // In a menu, handle menu-specific logic
  handleMenu(broj);

  display.clear();
  whileMenu=false;
  while(whileMenu == false)
  {
    Serial.print("buttonPressed - ");
    Serial.println(buttonPressed);
    digitalWrite(13, HIGH);
    display.setSegments(SEG_DONE);
  }
  digitalWrite(13, LOW);
}



void enterMenu(int menu) {
  
  display.clear();
}

void handleMenu(int menu) {
  // Placeholder for handling menu logic
  switch(menu) {
    case 1:  // Reset menu
      menu1();
      break;
    case 2:  // Start menu
      menu2();
      break;
    default:
      break;
  }
}



// Start menu - start the countdown
void menu2() {
  // Convert digits to total seconds
  totalSeconds = (digits[0] * 3600) + (digits[1] * 3600) + (digits[2] * 60) + (digits[3] * 60);
  counting = true;   
  while(totalSeconds != 0)
  {
    unsigned long secondsRemaining = totalSeconds;
    digits[0] = secondsRemaining / 36000; // Tens of hours
    secondsRemaining %= 36000;
    digits[1] = secondsRemaining / 3600;  // Hours
    secondsRemaining %= 3600;
    digits[2] = secondsRemaining / 600;   // Tens of minutes
    secondsRemaining %= 600;
    digits[3] = secondsRemaining / 60;    // Minutes

    if((digits[0] == 0) && (digits[1] == 0) && (digits[2]== 0) && (digits[3] == 0) && (totalSeconds < 60))
    {
        digits[0] = digits[1] = 0;
        digits[2] = totalSeconds / 10;
        digits[3] = totalSeconds % 10;
        displayValue = (digits[0]*1000) + (digits[1]*100) + (digits[2]*10) + digits[3];
        display.showNumberDec(displayValue, true);
    }
    else
    {
      displayValue = (digits[0] * 1000) + (digits[1] * 100) + (digits[2] * 10) + digits[3];
    display.showNumberDec(displayValue, true);
    }

    
    
  }
  inMenu = true; // Exit menu
}
ISR(TIMER1_COMPA_vect) {    // Timer1 ISR
  if (counting && totalSeconds > 0) {
    totalSeconds--;         
    Serial.print("Counting! ");
    Serial.println(totalSeconds);
  } 
}


