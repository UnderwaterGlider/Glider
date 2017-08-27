#include <Servo.h>

#define M1_STEP_PIN 13
#define M1_ENABLE_PIN 10
#define M1_DIR_PIN 12

using namespace std;

// Static variable
unsigned int sD1 = 0;
unsigned int sD2 = 0;


// Engine class (for three engines, class methods get angles in degrees and full speed divider)
class engine
{
  public:
    // Constructor
    engine(int stepP, int enableP, int dirP)
    {
      // cout << "Engine addition" << endl;
      angle = 0;
      speedDivider = 0;
      turn = 0;
      switcher = 0;
      StepPin = stepP;
      EnablePin = enableP;
      DirectionPin = dirP;
    }

    void installization()
    {
      pinMode(EnablePin, OUTPUT); //Enable
      pinMode(StepPin, OUTPUT); //Step
      pinMode(DirectionPin, OUTPUT); //Direction
      digitalWrite(EnablePin, LOW); //Turn on voltage
    }

    // Return speed divider
    unsigned int getSpeedE()
    {
      return speedDivider;
    }

    // Return angle
    double getAngle()
    {
      return angle;
    }

    // Do one step in one side(+1.8 / 2 degrees)
    void dicAngle()
    {
      angle -= 0.5;
      turn += 0.5;
    }

    // Do one step in other side(-1.8 degrees)
    void incAngle()
    {
      angle += 0.5;
      turn -= 0.5;
    }

    // Set angle and speed of moving and start
    void movee(double ang, unsigned int sd)
    {
      angle = ang;
      speedDivider = sd;
    }

    void engineStepPos()
    {
      switcher = !switcher;
      digitalWrite(DirectionPin, HIGH);
      digitalWrite(StepPin, switcher); // First - low, following - high - /is it normal?/ In first interrupt sD = 1, and it's normal. When we have even number, last bit in sD ever is 0
      this->dicAngle();
    }

    void engineStepNeg()
    {
      switcher = !switcher;
      digitalWrite(DirectionPin, LOW);
      digitalWrite(StepPin, switcher); // First - low, following - high - /is it normal?/ In first interrupt sD = 1, and it's normal. When we have even number, last bit in sD ever is 0
      this->incAngle();
    }

    void interr()
    {
      if (getAngle() == 0)
      {
        return;
      }

      sD++; //Counter for speed divider

      // In one side
      if ((getAngle() > 0) && (sD  % getSpeedE() == 0))
      {
        engineStepPos(); // Move in positive direction
      }

      // In other side
      if ((getAngle() < 0) && (sD % getSpeedE() == 0))
      {
        engineStepNeg(); // Move in negative direction
      }

      if (sD  == getAngle()*getSpeedE() * 2)
      {
        sD = 0;
        resetSD();
        switcher = 0;
      }
    }
    // Reset speedDivider
    void resetSD()
    {
      speedDivider = 0;
    }
  protected:
    double turn;
    double angle;
    unsigned int speedDivider;
    bool  switcher;
    int sD;

    //Pins installization
    int StepPin;
    int EnablePin;
    int DirectionPin;
};


// Object installization. Set number of pins (StepPin, EnablePin, DirectionPin)
engine piston(13, 10, 12);
engine fin(8, 11, 15);
engine mass();


void setup()
{
  Serial.begin(9600);
  engine piston(13, 10, 12);
  engine fin(8, 11, 15);
  engine mass();
  piston.installization();
  fin.installization();
  //  OCR2A = 0x57; // If we want max speed, we can reduce the timer time by 2 times, because we increased the number of interrupts by 2 times
  //  TIMSK2 |= _BV(OCIE2A);
  //set timer2 interrupt at 8kHz
  cli();
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
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


SIGNAL(TIMER2_COMPA_vect)
{
  piston.interr();
  fin.interr();
}

void loop()
{
  piston.movee(10, 5);
  delay(1000);
  fin.movee(5, 2);
  delay(1000);
}
