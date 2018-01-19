volatile unsigned long start_time_button;
volatile unsigned long end_time_button;
volatile unsigned long duration_button; 
volatile int button;
int green_button = 1;
int red_button = 2;
int blue_button = 3;
int green_led = 12;
int red_led = 11;
int blue_led = 10;
volatile int green_state = LOW;
volatile int red_state = LOW;
volatile int blue_state = LOW;
int photo_resist_pin = 4;
volatile int button_interrupt_counter = 0;
bool log_in = 0;
bool correct_pass_entered = 0;
String password;
String correct_password = "pass123";
volatile unsigned long start_time_in;
volatile unsigned long end_time_in;
volatile unsigned long duration_in;
volatile int state = LOW;
long mill_time = 0;

void setup() {
  Serial.begin(9600);
  cli();
  TCCR0A = 0;
  TCCR0B = 0;
  TCCR0B = _BV(CS00) | _BV(CS01); // prescale 64, counts to 250
  TIMSK0 |= (1 << 1);
  OCR0A = 250;
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(photo_resist_pin,OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(red_led, OUTPUT);
  pinMode(blue_led, OUTPUT);
  attachInterrupt(0,button_handler,CHANGE);
  attachInterrupt(1,photoresist_handler,CHANGE);
  Serial.println("setup complete");
  sei();
}

void loop() {
  digitalWrite(photo_resist_pin,state);

  if (log_in && password!=correct_password) {
    digitalWrite(photo_resist_pin,state);
    login();
  }
  
  //login successful
if (log_in) {
 
    digitalWrite(LED_BUILTIN, HIGH);   
    digitalWrite(green_led, green_state);
    digitalWrite(red_led, red_state);
    digitalWrite(blue_led, blue_state);
    if (analogRead(5)>=1000){
      blue_state = LOW;
      red_state = LOW;
      green_state = LOW;
      button = 0;
    }else if (analogRead(5)<200 && analogRead(5)>=165) {
      button = blue_button;
      blue_state = HIGH;
    } else if (analogRead(5)<100 && analogRead(5)>=85) {
      button = red_button;
      red_state = HIGH;
    } else if (analogRead(5)<=10) {
      button = green_button;
      green_state = HIGH;
    }
    
    if (button_interrupt_counter == 2 && analogRead(5)>1000) {
      if (button == blue_button) {
        Serial.print("Button BLUE held down: ");
        Serial.print(duration_button/1000.0);
        Serial.println(" seconds");
      } else if (button == red_button) {
        Serial.print("Button RED held down: ");
        Serial.print(duration_button/1000.0);
        Serial.println(" seconds");
      } else if (button == green_button) {
        Serial.print("Button GREEN held down: ");
        Serial.print(duration_button/1000.0);
        Serial.println(" seconds");
      } 
      button = 0;
      button_interrupt_counter = 0;
    }
   
    //logout when Flashlight is on
    if (!state) {
      Serial.print("state in logout: ");
      Serial.println(state);
      digitalWrite(LED_BUILTIN, LOW);   
      password = "";
      end_time_in = mill_time;
      duration_in = end_time_in - start_time_in;
      Serial.println("logged out");
      Serial.print("You were logged in for: ");
      Serial.print(duration_in/1000.0);
      Serial.println(" seconds.");
    } 
  }


ISR(TIMER0_COMPA_vect) {
  mill_time++;
  TCNT0 = 0;
}

void photoresist_handler() {
  log_in = !log_in;
  state = !state; 
}

void button_handler() {
  if (button_interrupt_counter >= 2) {
    button_interrupt_counter = 0;
  }
  if (button_interrupt_counter == 1) {
    end_time_button = mill_time;
    duration_button = (end_time_button - start_time_button);
  } else if (button_interrupt_counter == 0) {
    start_time_button = mill_time;
    button = 0;
  }
  button_interrupt_counter++;
}

void login() {
Serial.println("Enter the password: ");
  while(!Serial.available());
  password = Serial.readString();
  
  while (password != correct_password) {
    Serial.println("Incorrect password! Please re-enter the password: ");
    while(!Serial.available());
    password = Serial.readString();  
  }
  Serial.println("Correct Password Entered! Press a button.");
  start_time_in = mill_time;
  }


