#include <Servo.h>

#define M1_STEP_PIN 13
#define M1_ENABLE_PIN 10
#define M1_DIR_PIN 12

using namespace std;

// Static variable


// Engine class (for three engines, class methods get angles in degrees and full speed divider)
class engine
{
  public:
    // Constructor
    engine(int stepP, int enableP, int dirP, int frontP = -1, int backP = -1, int frontPM = -1, int backPM = -1)
      : StepPin (stepP), EnablePin (enableP), DirectionPin(dirP), FrontPin(frontP), BackPin(backP), FrontPinMode(frontPM), BackPinMode(backPM)
    {
      // cout « "Engine addition" « endl;
      stepEngine = 0;
      speedDivider = 1;
      turn = 0;
      switcher = 0;
      TurnMax = 0;
      TurnMin = 0;
      sD = 0;
    }

    void installization()
    {
      pinMode(EnablePin, OUTPUT); //Enable
      pinMode(StepPin, OUTPUT); //Step
      pinMode(DirectionPin, OUTPUT); //Direction
      if(FrontPin != -1) {  pinMode(FrontPin, INPUT); };
      if(BackPin != -1) {  pinMode(BackPin, INPUT); };
      digitalWrite(EnablePin, LOW); //Turn on voltage
    }

    int getFrontPinVal()
    { 
      if (FrontPin==-1)
      {
        return -1;
      }
      else
      {
        return digitalRead(FrontPin);
      }
    }
    
    int getBackPinVal()
    {
      if (BackPin==-1)
      {
        return -1;
      }
      else
      {
        return digitalRead(BackPin);
      }
    }

    int stopMovee() //Возвращает 1 если закрыт Front концевик, 2 если закрыт Back концевик, 0 если остановка не нужна
    {
      if ( this->getFrontPinVal() != FrontPinMode ) //Может случится ошибка когда pin концевика задан, но не задан его режим
      {
        return 1;
      }
      if ( this->getBackPinVal() != BackPinMode )
      {
        return 2;
      }
      return 0;
    }

    void Calibration(int MinAngle, int speedDiv)
    {
      TurnMax=0;
      TurnMin=0;
      if (FrontPin == - 1 && BackPin == -1)
      {
        return;
      }
      if (FrontPin != -1)
      {
        while ( !(this->stopMovee()) )
        {
          if (stepEngine == 0)
          {
            this->movee(MinAngle,speedDiv);
            TurnMax += MinAngle;
          }
        }
      }
     if (BackPin != -1)
      {
        while ( !(this->stopMovee()) )
        {
          if (stepEngine == 0)
          {
            this->movee(-MinAngle,speedDiv);
            TurnMin -= MinAngle;
          }
        }
      }
    }

    // Return speed divider
    unsigned int getSpeedE()
    {
      return speedDivider;
    }

    // Return angle
    int getStep()
    {
      return stepEngine;
    }

    // Do one step in one side(? degrees)
    void dicStepEngine()
    {
      stepEngine -= 1;
      turn += 1;
    }

    // Do one step in other side(? degrees )
    void incStepEngine()
    {
      stepEngine += 1;
      turn -= 1;
    }
    /* Изменить название функции т.к. stop это стандартная ф-ция, да и вообще зачем плодить сущности без необходимости
     *  можно использовать функиции movee и resetSD для обнуления
    //Emergency stop
    void stop ()
    {
      stepEngine = 0;
      speedDivider = 0;
      sD = 0;
    }
    */
    // Set angle and speed of move and start
    void movee(int stepEn, unsigned int sd)
    {
      stepEngine = stepEn;
      speedDivider = sd;
      sD = 2*abs(stepEngine)*speedDivider;
    }

    void engineStepPos()
    {
      int checkMovee=0;//this->stopMovee();
      if (checkMovee != 1){
        switcher = !switcher;
        digitalWrite(DirectionPin, HIGH);
        digitalWrite(StepPin, switcher); // First - low, following - high - /is it normal?/ In first interrupt sD = 1, and it's normal. When we have even number, last bit in sD ever is 0
        this->dicStepEngine();
      }
      else
      {
        this->movee(0,0);
        this->resetSD();
      }
    }

    void engineStepNeg()
    {
      int checkMovee=0;//this->stopMovee();
      if (checkMovee != 2){
        switcher = !switcher;
        digitalWrite(DirectionPin, LOW);
        digitalWrite(StepPin, switcher); // First - low, following - high - /is it normal?/ In first interrupt sD = 1, and it's normal. When we have even number, last bit in sD ever is 0
        this->incStepEngine();
      }
      else
      {
        this->movee(0,0);
        this->resetSD();
      }
    }
    
    void dicSD()
    {
      sD--;
    }

    unsigned int getSD()
    {
      return sD;
    }

    // Reset speedDivider
    void resetSD()
    {
      speedDivider = 0;
      sD = 0; 
    }
  protected:
    
    int turn;
    int stepEngine;
    unsigned int speedDivider;
    bool switcher;
    int TurnMax;
    int TurnMin;
    
    //Pins installization
    int StepPin;
    int EnablePin;
    int DirectionPin;
    int FrontPin;
    int BackPin;
    int FrontPinMode;
    int BackPinMode;
    unsigned int sD ;
};


// Object installization. Set number of pins (StepPin, EnablePin, DirectionPin)
engine piston(13, 10, 12);
engine fin();
engine mass();


void setup()
{
  Serial.begin(9600);
  engine piston(13, 10, 12);
  engine fin();
  engine mass();
  piston.installization();
cli(); 
TCCR2A = 0;// set entire TCCR2A register to 0 
TCCR2B = 0;// same for TCCR2B 
TCNT2 = 0;//initialize counter value to 0 
// set compare match register for 8khz increments 
OCR2A = 249;// = (16*10^6) / (8000*8) - 1 (must be <256) 
// turn on CTC mode 
TCCR2A |= (1 << WGM21); 
// Set CS21 bit for 8 prescaler 
TCCR2B |= (1 << CS21); 
// enable timer compare interrupt 
TIMSK2 |= (1 << OCIE2A); 
sei();
}


SIGNAL(TIMER0_COMPA_vect)
{
  if (piston.getStep() != 0 )
  {
    piston.dicSD(); //Counter for speed divider  
  }
  // In one side
  if ( piston.getStep()>0 && (piston.getSD() % (2*piston.getSpeedE()) == 0) )
  {
    piston.engineStepPos(); // Move in positive direction
    
  }
  // In other side
  if ( piston.getStep()<0 && (piston.getSD() % (2*piston.getSpeedE()) == 0) )
  {
    piston.engineStepNeg(); // Move in negative direction
  }
  if ( !piston.getSD() )
  {
    piston.resetSD();
  }
  Serial.println(piston.getStep());
}

void loop()
{
  piston.movee(800, 1);
  delay(1000);
  piston.movee(-800, 1);
  delay(1000);
}
