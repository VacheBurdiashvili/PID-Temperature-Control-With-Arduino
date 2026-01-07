  #include <LiquidCrystal.h>
  #include <Encoder.h>
  #include <max6675.h>
  #include <PID_v1.h>

  // Creating an instance of lcd class and Encoder for precise readings on the screen
  LiquidCrystal lcd(2, 3, 4, 5, 6, 7); 

  // Creating an encoder instance for rotary encoder
  const int dtPin = 9;
  const int clkPin = 8;
  const int swPin = 12;

  Encoder myEnc(dtPin ,clkPin);

  // Creating a MAx6675 module instance for temperature readings
  const int sckPin = A3;
  const int csPin = A4;
  const int soPin = A5;
  MAX6675 thermocouple(sckPin, csPin, soPin);

  // Initializing Variables
  const int pwmPin = 10;
  float temperature = 20.0;
  float currentTemp = 0.0;
  long oldPosition = -999;
  unsigned long lastUpdate = 0;
  bool lastSW_State = HIGH;
  bool emergencyStop = false; 
  bool firstTime = true;
  double heaterPWM = 0;
  double setPoint = 0;
  double input;

  // PID parameters (Using double because that is what this Library requires)
  double kP = 3.75; // Proportional gain: increases response speed to reach setpoint faster -3.75
  double kI = 0.045; // Integral gain: eliminates steady-state error for accurate temperature control -0.045
  double kD = 0.1; // Derivative gain: reduces oscillations and stabilizes the system 

  double kP_start = 5.0; // These values is used for start (Integral will be needed later)
  double kI_start = 0.005; 
  double kD_start = 0.15; 

  //Creating a PID instance
  PID myPID(&input, &heaterPWM, &setPoint, kP, kI, kD, DIRECT);

  // Created this new type to help me change states on the lcd screen
  enum States
  {
    Setting_Temperature,
    PID_Temperature_Control
  };

  States currentState = Setting_Temperature;

  void setup() {

    // Configuring the Pins
    pinMode(swPin, INPUT_PULLUP);
    pinMode(dtPin, INPUT_PULLUP);
    pinMode(clkPin, INPUT_PULLUP);
    pinMode(sckPin, OUTPUT);
    pinMode(csPin, OUTPUT);
    pinMode(soPin, INPUT);
    pinMode(pwmPin, OUTPUT);

    // Enabling the PID
    myPID.SetMode(AUTOMATIC);
    myPID.SetOutputLimits(0, 255);
    myPID.SetSampleTime(600); // Appropriate for Max6675 module's sampling time

    // Starting an LCD and Serial communication 
    lcd.begin(16,2);
    Serial.begin(9600);
    delay(500);
  }

  void loop() {

    if(!emergencyStop)
    {
      temperatureSet();
      handlingStates();
      updatingScreen();
      checkSafety();

      if(currentState == PID_Temperature_Control)
      {
        setPoint = temperature;  
        input = currentTemp;

        Serial.print(millis());
        Serial.print(", ");
        Serial.print(input);
        Serial.print(", ");
        Serial.print(setPoint);
        Serial.print(", ");
        Serial.println(heaterPWM);

        if (firstTime)
        {
          myPID.SetTunings(kP_start, kI_start, kD_start); // If the heater is turned on for the first time we use these values

          if (input >= setPoint - 1.0)  // If we are 2 degrees belowe setPoint -1.2
          {
            firstTime = false; 
          }
        }
        else
        {
            myPID.SetTunings(kP, kI, kD);  // let integral part recover
        }

        myPID.Compute();
        
      }
      else
      {
        heaterPWM = 0; // Turning off PWM when we are in 'Setting Temperature' mode
      }

      analogWrite(pwmPin, int(heaterPWM)); 

    }

    else
    {
      heaterPWM = 0;
      analogWrite(pwmPin, int(heaterPWM));
      lcd.setCursor(0,0);
      lcd.print("EMERGENCY STOP!");    
      lcd.setCursor(0, 1);
      lcd.print("Reset Manually. ");
    }

  
  }


  // Logic For an user to input a desired Temperature
  void temperatureSet()
  {
    long newPosition = myEnc.read();

    if(currentState == Setting_Temperature && newPosition != oldPosition) // Checking if we are in correct state otherwise temperature remains unchanged
    {
    temperature = 20 + newPosition * 0.25;
    oldPosition = newPosition;
    }

  }

  // This is where a user chooses which state he/she wants to be in
  void handlingStates()
  {
    bool currentSW = digitalRead(swPin);

    if(currentSW == LOW && lastSW_State == HIGH)
    {
      if(currentState == Setting_Temperature)
      {
        currentState = PID_Temperature_Control;

        firstTime = true;

        myPID.SetMode(MANUAL); // Basically, This is done to reset PID (and it's past memoery) when changing the states
        heaterPWM = 0;
        myPID.SetMode(AUTOMATIC);
      }
      else
      {
        currentState = Setting_Temperature;
        setPoint = 0;
      }
    }

    lastSW_State = currentSW;
  }

  // Updating the screen depending on which state we are in
  void updatingScreen()
  {
    static States prevState;

    if(currentState != prevState) // This is done to avoid overwriting when the states change
    {
      lcd.clear();
      prevState = currentState;
    }

    if(currentState == Setting_Temperature)
    {
      lcd.setCursor(0, 0);
      lcd.print("Set Temp: (C*)");
      lcd.setCursor(0, 1);
      lcd.print("      ");
      lcd.print(temperature, 1);

      unsigned long timer = millis();
      if(timer - lastUpdate >= 300) // Using this to match sample rate of MAX6675 module (We cant show currentTemp every few milliseconds)
      {
        currentTemp = thermocouple.readCelsius(); // We are also measeruing current temp also in Setting_Temperature mode so our checkSafety() can work
        lastUpdate = timer;
      } 

    }
    else // If we are in PID_Control State
    {
      lcd.setCursor(0, 0);
      lcd.print("Desired C: ");
      lcd.print(temperature, 1);
      lcd.setCursor(0, 1);
      lcd.print("Current C: ");

      unsigned long timer = millis();
      if(timer - lastUpdate >= 300) // Using this to match sample rate of MAX6675 module (We cant show currentTemp every few milliseconds)
      {
        currentTemp = thermocouple.readCelsius();

        Serial.print(currentTemp);
        Serial.print(" ");
        Serial.print(temperature);


        lcd.setCursor(11, 1);
        lcd.print("    ");
        lcd.setCursor(11, 1);
        lcd.print(currentTemp, 1);

        lastUpdate = timer;
      } 

    } 

  }

  // Stoping the heater in case it exceeds or under-exceeds limits
  void checkSafety()
  {
    if(currentTemp > 150.0 || (setPoint != 0 && currentTemp > setPoint + 15)) // Added currentTemp > setPoint + 10 for extra safety
    {
      analogWrite(pwmPin, 0);
      emergencyStop = true;
    }
    
  }



















