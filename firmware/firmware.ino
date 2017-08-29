#include <Servo.h>

#define PISTON_STEP_PIN 2
#define PISTON_ENABLE_PIN 4
#define PISTON_DIR_PIN 3
#define MASS_STEP_PIN 5
#define MASS_ENABLE_PIN 7
#define MASS_DIR_PIN 6



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
      if ( FrontPinMode != -1 && this->getFrontPinVal() != FrontPinMode ) //Если режим датчиков не установлен, то считается, что их нет
      {
        return 1;
      }
      if ( BackPinMode != -1 && this->getBackPinVal() != BackPinMode )
      {
        return 2;
      }
      return 0;
    }

    // Do move while engine don't come to front end 
    void moveToFront(int MinStep, int speedDiv) 
    { 
      while ( this->stopMovee() != 1 ) 
      { 
        if (stepEngine == 0) 
        { 
          this->movee(MinStep, speedDiv); 
        }
      }
      TurnMax = turn;
    } 

    // Do move while engine don't come to back end 
    void moveToBack(int MinStep, int speedDiv) 
    { 
      while ( this->stopMovee() != 2 ) 
      { 
        if (stepEngine == 0) 
        { 
          this->movee(-MinStep, speedDiv); 
        }
      }
      TurnMin = turn;
    }

    void Calibration(int MinStep, int speedDiv)
    {
      this->moveToFront(MinStep,speedDiv);
      this->moveToBack(MinStep,speedDiv);
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
    
    // Set angle and speed of move and start
    void movee(int stepEn, unsigned int sd)
    {
      stepEngine = stepEn;
      speedDivider = sd;
      sD = 2*abs(stepEngine)*speedDivider;
    }

    void engineStepPos()
    {
      int checkMovee=this->stopMovee();
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
      int checkMovee=this->stopMovee();
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

    int getTurnMax()
    {
      return TurnMax;
    }
    int getTurnMin()
    {
      return TurnMin;
    }

    void interr()
    {
      if ( getStep() != 0 )
      {
        dicSD(); //Counter for speed divider
      }
      
      // In one side
      if ( getStep()>0 && (getSD() % (2*getSpeedE()) == 0) )
      {
        engineStepPos(); // Move in positive direction
      }
    
      // In other side
      if ( getStep()<0 && (getSD() % (2*getSpeedE()) == 0) )
      {
        engineStepNeg(); // Move in negative direction
      }
      
      if ( !getSD() )
      {
        resetSD();
      }    
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
engine piston(PISTON_STEP_PIN,PISTON_ENABLE_PIN,PISTON_DIR_PIN, A0, A1, 0, 0);
engine fin();
engine mass(MASS_STEP_PIN,MASS_ENABLE_PIN,MASS_DIR_PIN, A2, A3, 0, 0);


void setup()
{
  Serial.begin(9600);
  piston.installization();
  mass.installization();
  OCR0A = 0xAF; // If we want max speed, we can reduce the timer time by 2 times, because we increased the number of interrupts by 2 times
  TIMSK0 |= _BV(OCIE0A);
  //piston.Calibration(10,1);
}


SIGNAL(TIMER0_COMPA_vect)
{
  piston.interr();
  mass.interr();
}

void loop()
{
  /*
  Serial.print(piston.getTurnMax());
  Serial.print(" ");
  Serial.println(piston.getTurnMin());
  delay(1000);
  */
  piston.movee(50,1);
  delay(1000);
  mass.movee(-50,1);
  delay(1000);
  piston.movee(-50,1);
  delay(1000);
  mass.movee(50,1);
  delay(2000);
}
