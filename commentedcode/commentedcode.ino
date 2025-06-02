// INTIAL VARIABLES
// LCD display
#include <LiquidCrystal.h>
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);//arduino pin(rs=2,e=3,D4=4,D5=5,D6=6,D7=7)
int backlightpin = 12;
bool clear = false;
int prev;
const int limit = 75;

// Microphone
int analogpin = A0;
int analogread;

const int sample_time = 50;
unsigned long current;
unsigned long last = 0;
unsigned long elapsed = 0;
int highestread;
int lowestread;
int basevalue;
const int maxsamples = 10;
int samplecount = 0;
int sum;


// LOCAL FUNCTIONS
void update_display(int dB) { // updates the lcd display
  if (clear == true) { // checks the clear condition, if true it clears
    lcd.clear();
  }
  clear = false;

  lcd.setCursor(0,0); // displays the loudness in dB
  lcd.print("Loudness: ");
  lcd.print(dB);
  lcd.print(" dB");

  if (dB > limit) { // if its too loud
    lcd.setCursor(0,1);
    lcd.print("OVER LIMIT");

    for (int i = 0; i<10; i++){ // visual flashing alert
      digitalWrite(backlightpin, LOW); // turns off the backlight LED
      delay(100);
      digitalWrite(backlightpin, HIGH); // turns on the backlight LED
      delay(100);
    }

    delay(1000);
    clear = true;
  }

  if ((prev > 10) && (dB < 10)) { // just so the display looks clean(less clears = smoother looking)
    clear = true;
  }

  prev = dB; // also for display cleanliness
}

// ARDUINO FUNCTIONS
void setup() {
  pinMode(analogpin, INPUT); // microphone analog pin setup
  pinMode(backlightpin, OUTPUT); // lcd display backlight pin setup
  Serial.begin(9600);
  lcd.begin(16, 2);// set up the LCD's number of columns and rows
  digitalWrite(backlightpin, HIGH); // turns on the display backlight
  basevalue = map(analogRead(analogpin), 470, 570, 0, 1023); // setting an initial value for finding max/min in sampling later
}

void loop() {
  analogread = analogRead(analogpin); // takes analog reading
  analogread = map(analogread, 470, 570, 0, 1023); // maps reading into 0 to 1023 (for greater range)

  current = millis();
  elapsed = current - last; // tracking the sample time

  if (analogread > highestread){ // finding max in a sample
    highestread = analogread;
  }
  else if (analogread < lowestread){ // find min in a sample
    lowestread = analogread;
  }

  if (elapsed > sample_time){ // taking the delta the end of a sample
    samplecount++; // counting samples
    int delta = highestread-lowestread;
    sum += delta; // sum of all deltas in 10 samples

    if (samplecount > maxsamples){ // taking the average after a set of samples
      int average = sum/maxsamples;
      // int dB = 50 + 10*log10(average/30); // doesnt work lmao
      int dB = map(average, 0, 512, 50, 100); // maps sensor readings into decibels (tries to)
      update_display(dB); // updates display
      
      sum = 0; // resetting sum
      samplecount = 0; // resetting sample count

    }
    

    highestread = basevalue; // resets values stored in variables for the next sample
    lowestread = basevalue;
    last = current;
  }
  
}

