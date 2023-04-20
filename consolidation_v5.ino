/* Run a square wave of frequency 0.15-0.25 Hz at a certain frequency through
the electromagnet. Functionality should allow user to change frequency and time to run
(can be done through serial monitor/computer at this point still) */
// using namespace std;



// Button Variables ///////////////////////////////////////////////////////////
//config variables
#define NUM_LED_COLUMNS (4)
#define NUM_LED_ROWS (1)
#define NUM_BTN_COLUMNS (4)
#define NUM_BTN_ROWS (1)
#define NUM_COLORS (1)

#define MAX_DEBOUNCE (3)

// For the LCD /////////////////////////////////////////////////
#include <Wire.h>

#include <SerLCD.h> //Click here to get the library: http://librarymanager/All#SparkFun_SerLCD
SerLCD lcd; // Initialize the library with default I2C address 0x72

// Global variables
static bool LED_buffer[NUM_LED_COLUMNS][NUM_LED_ROWS];

static const uint8_t btncolumnpins[NUM_BTN_COLUMNS] = {13, 2, 3, 4};
static const uint8_t btnrowpins[NUM_BTN_ROWS]       = {9};
static const uint8_t ledcolumnpins[NUM_LED_COLUMNS] = {5, 6, 7, 8};
static const uint8_t colorpins[NUM_LED_ROWS]        = {12};

static int8_t debounce_count[NUM_BTN_COLUMNS][NUM_BTN_ROWS];
///////////////////////////////////////////////////////////////////
const int CLICK = 20;
const int SHORT_HOLD = 1000;
const int LONG_HOLD = 2000;

/////////////////////////////////////////


int pin = 10;
void variable_delay_us(int);
void play_note(double, unsigned long);
double freq;
unsigned long desired_time;
String time_string;
String freq_string;
bool start = true;
////////////////////////////////////////////////////////////////

    bool TL_state = false;
    bool TR_state = false;
    bool BL_state = false;
    bool BR_state = false;

////////////////////////////////////////////////////////////////

bool wave_choice = true;
bool freq_choice = false;
bool time_choice = false;


/////////////////////////////////////////////////////////////////////
  enum which_decision {wave_set, frequency_set, time_set};
  enum wave_decision{Pos_square_wave, pos_neg_square, DC_flat_line};

  wave_decision which_wave = Pos_square_wave; // have to declare and initialize the enum variables
  which_decision decision_enum = wave_set;

 double frequency = 0.15;
 long run_time = 0;

/////////////////////////////
  unsigned long time_of_initial_press; // millis() time stamp of when press was initiated


////////////////////////////////////////
// we need a counter for each digit??? 
// bottom buttons will move to the next digit
// top left will decrement
// top right will decrement

void decide_parameters(int type_decision, int wave_type, double curr_frequency, long curr_time) // probably have to add more arguments to determine the things within
{
  if (type_decision == wave_set)
  {
    lcd.clear();
    lcd.write("What waveform would you like?");
    lcd.setCursor(0, 2);
    // have them be able to cycle through a wave form 
    if (wave_type == 0)
    {
      lcd.write("Positive Square Wave");
    }
    else if (wave_type == 1)
    {
      lcd.write("Pos and Neg Square wave");
    }
    else if (wave_type == 2)
    {
      lcd.write("DC - FLat line");
    }
  }
  else if (type_decision == frequency_set)
  {
    lcd.clear();
    lcd.write("Enter desired frequency.");
    lcd.setCursor(0,2);
    lcd.write(curr_frequency);
  }
  else if (type_decision == time_set)
  {
    lcd.clear();
    lcd.write("Enter desired time:");
    lcd.setCursor(0,2);
    lcd.write(curr_time);
  }

}


void button_action_on(int which)
{
  if (which == 0) // determining previous state of the buttons
  {
      if (TL_state == false)
      {
        time_of_initial_press = millis();
      }
      // else if (TL_state == true)
      // {
      //     unsigned long press_length = millis() - time_of_initial_press;
      //     if (press_length >= LONG_HOLD)
      //     {
      //       Serial.println("Executed Long Hold");
      //     }
      // }
      lcd.clear();
      lcd.write("turning ON top left button");
      TL_state = true;
      Serial.print("TL_state: ");
      Serial.println(TL_state);
          
  }
  else if (which == 1)
  {
          lcd.clear();
          lcd.write("turning ON bottom left button");
          BL_state = true;
  }
  else if (which == 2)
  {
          lcd.clear();
          lcd.write("turning ON top right button");
          TR_state = true;
  }
  else if (which == 3)
  {
          lcd.clear();
          lcd.write("turning ON bottom right button");
          BR_state = true;
  }
}

void button_action_off(int which)
{
  if (which == 0) // determining previous state of the buttons
  {
      lcd.clear();
      Serial.println("do we ever ever enter this?");
      lcd.write("turning OFF top left button");
      if (TL_state == true)
      {
          unsigned long press_length = millis() - time_of_initial_press;
          if (press_length >= LONG_HOLD)
          {
            Serial.println("Executed Long Hold");
            lcd.clear();
            lcd.write("DID A LONG HOLD YAAAAY");
          }
      }
      TL_state = false;
      // this is a decrement option
      if (wave_choice)
      {
        which_wave = which_wave - 1; // moves to the next wave option
        decide_parameters(decision_enum, which_wave, frequency, run_time);
      }
      else if (freq_choice)
      {

      }
      else if (time_choice)
      {

      }
  }
  else if (which == 1)
  {
          lcd.clear();
          lcd.write("turning OFF bottom left button");
          BL_state = false;
          if (wave_choice)
          { // have to rethink this bc a lot of it is unnecessary
            decision_enum = time_set;
            decide_parameters(decision_enum, wave_choice, frequency, run_time);
            wave_choice = false;
            time_choice = true;
          }
          else if (freq_choice)
          {

          }
          else if (time_choice)
          {

          }
          
  }
  else if (which == 2)
  {
          lcd.clear();
          lcd.write("turning OFF top right button");
          TR_state = false;
          if (wave_choice)
          {
            wave_choice = wave_choice + 1; // moves to the next wave option
            decide_parameters(decision_enum, wave_choice, frequency, run_time);
          }
          else if (freq_choice)
          {

          }
          else if (time_choice)
          {

          }
  }
  else if (which == 3)
  {
          lcd.clear();
          lcd.write("turning OFF bottom right button");
          BR_state = false;
           if (wave_choice)
          {
            decision_enum = frequency_set;
            decide_parameters(decision_enum, wave_choice, frequency, run_time);
            wave_choice = false;
            freq_choice = true;
          }
          else if (freq_choice)
          {

          }
          else if (time_choice)
          {

          }
  }
}



static void setuppins() // for the buttons
{
  uint8_t i;

  // initialize
  // select lines
  // LED columns
  for (i = 0; i < NUM_LED_COLUMNS; i++)
  {
    pinMode(ledcolumnpins[i], OUTPUT);

    // with nothing selected by default
    digitalWrite(ledcolumnpins[i], HIGH);
  }

  // button columns
  for (i = 0; i < NUM_BTN_COLUMNS; i++)
  {
    pinMode(btncolumnpins[i], OUTPUT);

    // with nothing selected by default
    digitalWrite(btncolumnpins[i], HIGH);
  }

  // button row input lines
  for (i = 0; i < NUM_BTN_ROWS; i++)
  {
    pinMode(btnrowpins[i], INPUT_PULLUP);
  }

  // LED drive lines
  for (i = 0; i < NUM_LED_ROWS; i++)
  {
    pinMode(colorpins[i], OUTPUT);
    digitalWrite(colorpins[i], LOW);
  }

  // Initialize the debounce counter array
  for (uint8_t i = 0; i < NUM_BTN_COLUMNS; i++)
  {
    for (uint8_t j = 0; j < NUM_BTN_ROWS; j++)
    {
      debounce_count[i][j] = 0;
    }
  }
}

static void scan() // for the buttons
{
    // we need bools to determine what state the button is at
    // can we write to the LCD within this?
    // down := false
    // up := true
    int which_button = -1;



  static uint8_t current = 0;
  uint8_t val;
  uint8_t i, j;

  // Select current columns
  digitalWrite(btncolumnpins[current], LOW);
  digitalWrite(ledcolumnpins[current], LOW);

  // output LED row values
  for (i = 0; i < NUM_LED_ROWS; i++)
  {
    if (LED_buffer[current][i])
    {
      digitalWrite(colorpins[i], HIGH);
    }
  }

  // pause a moment
  delay(1);

  // Read the button inputs
  for ( j = 0; j < NUM_BTN_ROWS; j++)
  {
    val = digitalRead(btnrowpins[j]);

    if (val == LOW)
    {
      // active low: val is low when btn is pressed
      if ( debounce_count[current][j] < MAX_DEBOUNCE)
      {
        debounce_count[current][j]++;
        if ( debounce_count[current][j] == MAX_DEBOUNCE )
        {
          Serial.print("Key Down ");
          Serial.println((current * NUM_BTN_ROWS) + j);
            which_button = ((current * NUM_BTN_ROWS) + j);

            button_action_on(which_button);

          // Do whatever you want to with the button press here:
          // toggle the current LED state
          LED_buffer[current][j] = 1;
        }
      }
    }
    else
    {
      // otherwise, button is released
      if ( debounce_count[current][j] > 0)
      {
        debounce_count[current][j]--;
        if ( debounce_count[current][j] == 0 )
        {
          Serial.print("Key Up ");
          Serial.println((current * NUM_BTN_ROWS) + j);
          which_button = ((current * NUM_BTN_ROWS) + j);

          // If you want to do something when a key is released, do it here:
          button_action_off(which_button);
          LED_buffer[current][j] = 0;
        }
      }
    }
  }// for j = 0 to 3;

  delay(1);

  digitalWrite(btncolumnpins[current], HIGH);
  digitalWrite(ledcolumnpins[current], HIGH);

  for (i = 0; i < NUM_LED_ROWS; i++)
  {
    digitalWrite(colorpins[i], LOW);
  }

  current++;
  if (current >= NUM_LED_COLUMNS)
  {
    current = 0;
  }

}

void setup() {
  
  Serial.begin(9600);
  delay(500); 

  Wire.begin();
  lcd.begin(Wire);
  Wire.setClock(400000); //Optional - set I2C SCL to High Speed Mode of 400kHz

  setuppins();

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(pin, OUTPUT);

  // Wire.begin();
  // lcd.begin(Wire);
  // Wire.setClock(400000); //Optional - set I2C SCL to High Speed Mode of 400kHz

  

  // while (!Serial.available() > 0) 
  // {
  // //    freq_string = Serial.readString();
  // }

  for (uint8_t i = 0; i < NUM_LED_COLUMNS; i++)
  {
    for (uint8_t j = 0; j < NUM_LED_ROWS; j++)
    {
      LED_buffer[i][j] = 0;
    }
  }

  Serial.println("Setup Complete for the Buttons.");



  while (!Serial) { // wait for Serial port to open
    delay(10);
  }
  // delay(500);

 

  // init global variables
  lcd.clear();
  lcd.write("Enter desired frequency.");


  // String freq_string;
  Serial.println("Enter desired frequency:");
  while (!Serial.available() > 0) 
  {
  //    freq_string = Serial.readString();
  }
  freq_string = Serial.readString();
  freq = freq_string.toDouble();
  // unsigned long A = atol(time_string.c_str());

  Serial.print("Entered freq value: ");
  Serial.println(freq_string);
  Serial.print("Converted value: ");
  Serial.println(freq);


  Serial.println("Entered desired run_time in seconds");
  String time_string;
  while (!Serial.available() > 0) 
  {
    // time_string = Serial.readString();
  }
  time_string = Serial.readString();
  desired_time = atol(time_string.c_str());


  Serial.print("Entered time value: ");
  Serial.println(time_string);
  Serial.print("Converted value: ");
  Serial.println(desired_time);

    // freq = 0.15;
    // desired_time = 20;


}

void loop() {
  // while (time < desired_time)
  // {
    // Serial.print("desired_time: ");
    // Serial.println(desired_time);

    scan();

    // lcd.write("Did we make it to this?");
    
    // delay(1000);
    // if (start)
    // {////
    //   play_note(freq, desired_time);
    // }
    
  // }
}


void play_note(double freq, unsigned long desired_time)
{
    
    // unsigned long desired_time;

    desired_time *= 1000;

    unsigned long period;

    period = 1000 / freq;      // Period of note in microseconds
	  unsigned long half_period = period / 2;

    Serial.print("half_period: ");
    Serial.println(half_period);

    unsigned long time = 0;
    Serial.print("desired time: ");
    Serial.println(desired_time);
    Serial.print("actual time passed: ");
    Serial.println(time);

    while (desired_time > time)
    {
        // keep making the square wave
        // PORTB |= (1 << PB4);
        digitalWrite(pin, HIGH);
        // Use variable_delay_us to delay for half the period
        delay(half_period);
        // Make PB4 low
        
        digitalWrite(pin, LOW);
        // Delay for half the period again
        delay(half_period);

        time += millis();
        Serial.print("time: ");
        Serial.println(time/1000);
        Serial.print("desired_time: ");
        Serial.println(desired_time / 1000);
        if (time > desired_time)
        {
          Serial.println("STOP");
          start = false;
        }
    }
    if (time > desired_time)
    {
      Serial.println("STOP again");
    }
}


