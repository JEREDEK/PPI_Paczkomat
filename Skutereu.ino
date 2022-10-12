#define DAC_PIN A0 // Make code a bit more legible

//Power level in %
int power = 0;
//Direction, 1 = forward; 0 = backward
int direct = 1;
//Push power to the engine, independent setting from "power"
bool throttle = 0;


void setup() {
  SerialUSB.begin(115200); //Initialize a serial connection
  
  analogWriteResolution(10); // Set analog out resolution to max, 10-bits
  pinMode(10, INPUT); //Emergency shutdown switch
}

void loop() {  
  //Check for new commands and store new values.
  SerialComm();

  //Once everything is prepared, check for failsafes and write the values stored to the engine.
  SetPower();
}

void SerialComm()
{
  //If there is a new serial command avaliable
  if (SerialUSB.available() > 0)
  {
    //Store the first char and react accordingly
    char serialIn = SerialUSB.read();

    switch (serialIn)
    {      
      //Set direction to forwards
      case 'f':
      {
        direct = 1;
        SerialUSB.println("Set direction: Forward");
      }break;
      
      //Set direction to backwards
      case 'b':
      {
        direct = 0;
        SerialUSB.println("Set direction: Backward");
      }break;
      
      //Set power setting in %
      case 'p':
      {
        //Store the value in a temporary variable
        int tempPower = SerialUSB.parseInt();
        //Check for illegal values
        if (tempPower > 100 or tempPower < 0)
        {
          SerialUSB.println("err:IllegalValue");
        }
        //If not, store the input and send a confirmation
        else 
        {
          power = tempPower;
          SerialUSB.println("Set new power: " + String(power) + "%");
        }
        if (power < 20)
        {
          SerialUSB.println("WARNING! Power set to " + String(power) + "%, the engine may not start! \nMinimum power to see a reaction is 20%, for actual movement, it's dependant on the battery charge and speed setting.");
        }
      }break;
      
      //Return general config AKA power, direction and throttle state
      case 'g':
      {
        SerialUSB.println("start;p=" + String(power) + ";d=" + String(direct) + ";t=" + throttle + ";end");
      }break;
      
      //Enable engine output
      case 's':
      {
        //Notify the user and set the throttle to on
        if (!digitalRead(10))
        {
          SerialUSB.println("WARNING! Emergency shutdown switch is on, command has been ignored...");
        }
        else
        {
          throttle = 1;
          SerialUSB.println("Activating engine");
        }
        //Due to the cheap design of the scooter, it will not react unless the power value is 20% or greater...
        
      }break;

      //Disable engine output
      case 't':
      {
        if (!digitalRead(10))
        {
          SerialUSB.println("WARNING! Emergency shutdown switch is on, command has been ignored...");
        }
        else
        {
          //Notify the user and set the throttle to off
          SerialUSB.println("Stopping engine");
          throttle = 0;
        }
      }break;

      //If none of the commands are recognised, throw an error
      default:
      {
        SerialUSB.println("err:UnknownCommand");
      }break;
    }
   
   //clear the buffer after handling the commands
   SerialUSB.flush();
  }
}


void SetPower()
{  
  //Variable that decides if the engine is allowed to start,
  //DO NOT USE, GO TO "throttle" INSTEAD AS IT BYPASSES THE FAILSAFE ON PIN 10!!!
  private bool engineState;

  
  //Regular control with manual override on pin 10
  //It needs to be high, ensuring that a failsafe switch is actually installed before operation
  //I advise not to mess around with this, unless you want to disable your only security measure...
  if (digitalRead(10) and throttle)
  {
    //Allow the engine to start
    engineState = 1;
  }
  else
  {
    //Disable engine and cancel the start comm0and
    //The start command will need to be entered again after the switch is off
    engineState = 0;
    throttle = 0;
  }

  //Make a temporary variable to store raw values to feed to the DAC
  int dac_power;
  
  //If direction setting is set to forward
  if (direct)
  {
    //Map the power % from 0 to 100 into voltage from 2.3V to 0V
    dac_power = map(power, 0, 100, 716, 0);
  }
  else // but if the direction setting is backward
  {
    //Map the power % from 0 to 100 into voltage from 2.3V to 3.3V, because that's the max a seeduino allows.
    dac_power = map(power, 0, 100, 716, 1023);
  }
  
  //If the the engine is allowed to start
  if (engineState)
  {
    //Feed the raw power values into the DAC
    analogWrite(DAC_PIN, dac_power);
  }
  else
  {
    //If not, set it to idle (2.3V or 716 in raw dac input)
    analogWrite(DAC_PIN, 716);
  }
}

//And, that's it...
