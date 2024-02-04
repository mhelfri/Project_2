//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#define TIME_INCREMENT_MS 10
#define TIME_DEBOUNCE_MS 30
#define DELAY_LIGHTS_OFF_MS 2000
#define DELAY_LIGHTS_ON_MS 1000

//=====[Declaration of public data types]======================================

typedef enum{
    BUTTON_UP,
    BUTTON_FALLING,
    BUTTON_DOWN,
    BUTTON_RISING
} debouncedIgnitionReleasedStateMachine_t;

//=====[Declaration and initialization of public global objects]===============


DigitalIn ignition(BUTTON1);
DigitalIn driverSeatOccupancy(D2);

DigitalOut blueLed(LED2);
DigitalOut rightHeadlight(PE_10);
DigitalOut leftHeadlight(PE_12);

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

AnalogIn potentiometer(A0);
AnalogIn lightSensor(A1);

//=====[Declaration and initialization of public global variables]=============

int accumulatedDebounceButtonTime = 0;
int accumulatedHeadlightDelayTime_ON = 0;
int accumulatedHeadlightDelayTime_OFF = 0;


bool engineStarted = OFF;

debouncedIgnitionReleasedStateMachine_t ignitionState;
//ignitionReleaseStateMachine_t ignitionState;

//=====[Declarations (prototypes) of public functions]=========================

void inputsInit();
void outputsInit();

void debounceIgnitionInit();
bool debounceIgnition();

void ignitionSubsystem();

int headlightsSettings();
void headlightsSubsystem();


//=====[Main function, the program entry point after power on or reset]========

int main()
{
    inputsInit();
    outputsInit();
    while (true) {
        ignitionSubsystem();
        headlightsSubsystem();
        delay(TIME_INCREMENT_MS);
    }
}

//=====[Implementations of public functions]===================================

void inputsInit()
{
    ignition.mode(PullDown);
    driverSeatOccupancy.mode(PullDown);
}

void outputsInit()
{
    blueLed = OFF;
    rightHeadlight = OFF;
    leftHeadlight = OFF;
}

void debounceIgnitionInit()
{
    if( ignition) {
        ignitionState = BUTTON_UP;
    } else {
        ignitionState = BUTTON_DOWN;
    }
}

bool debounceIgnition()
{
    bool ignitionReleasedEvent = false;

    // begin FSM where button presses register only on release, with a debouncing feature
    switch( ignitionState ) {
    
    case BUTTON_UP:
        // state should be changed if the button is pressed
        if( ignition) {
            ignitionState = BUTTON_FALLING;
            //set debounce timer equal to zero
            accumulatedDebounceButtonTime = 0;
            
        }
        break;

    case BUTTON_FALLING:
        // must check if an appropriate amount of time has passed by incrementing an int variable by 10 each loop (below), and comparing it to 40.
        // as long as an appropriate time has passed and the button is still being pressed, the state should advance to BUTTON_DOWN. 
        // otherwise it will go back to BUTTON_UP. This is to ensure that the signal is in a steady state and there is no bouncing.
        if( accumulatedDebounceButtonTime >= TIME_DEBOUNCE_MS ) {
            if( ignition ) {
                ignitionState = BUTTON_DOWN;
            } else {
                ignitionState = BUTTON_UP;
            }
        }
        // increment debounce timer by 10 for each loop
        accumulatedDebounceButtonTime = accumulatedDebounceButtonTime +
                                        TIME_INCREMENT_MS;
        break;

    case BUTTON_DOWN:
        // FSM remains in this state until the button is released, then it advances to BUTTON_RISING
        if(ignition == OFF) {
            ignitionState = BUTTON_RISING;
            //set debounce timer equal to zero
            accumulatedDebounceButtonTime = 0;
        }
        break;

    case BUTTON_RISING:
        // must check if an appropriate amount of time has passed by incrementing an int variable by 10 each loop (below), and comparing it to 40.
        // as long as an appropriate time has passed and the button is still not being pressed, the state should advance to BUTTON_UP. 
        // otherwise it will go back to BUTTON_DOWN. This is to ensure that the signal is in a steady state and there is no bouncing.
        if( accumulatedDebounceButtonTime >= TIME_DEBOUNCE_MS ) {
            if( ignition == OFF) {
                ignitionState = BUTTON_UP;
                // set this bool equal to 1, this is what will be returned, and will signify a button press.
                // this is done in this state because we want the button press to register on release (BUTTON_DOWN to BUTTON_UP)
                ignitionReleasedEvent = true;
            } else {
                ignitionState = BUTTON_DOWN;
            }
        }
         // increment debounce timer by 10 for each loop
        accumulatedDebounceButtonTime = accumulatedDebounceButtonTime +
                                        TIME_INCREMENT_MS;
        break;

    default:
        // reinitialize state machine
        debounceIgnitionInit();
        break;
    }
    // return bool true or false
    return ignitionReleasedEvent;
}


void ignitionSubsystem()
{
    //button press only recognized on release
    if (debounceIgnition())
    {   
        // ignition is pressed when engine is on
        if (engineStarted)
        {
            engineStarted = OFF;
            blueLed = OFF;
            uartUsb.write("\r\nEngine turned off\r\n", 25);
            
        }
        // driver must be sitting and engine cannot be on already
        else if (driverSeatOccupancy && !engineStarted)
        {
            engineStarted = ON;
            uartUsb.write("\r\nEngine started\r\n", 20);
            blueLed = ON;
            
        }
        // driver must not be sitting and engine cannot be on already
        else if (!driverSeatOccupancy && !engineStarted)
        {
            uartUsb.write("\r\nError: Driver must be seated\r\n", 35);
        }
        
        
        
    }
}


int headlightsSettings()
{   
    static int potentiometerState = 0;
    // read values for potentiometer
    float potentiometerReadingScaled = potentiometer.read();
    
    // analog values for potentiometer range from 0 to 1, because we are using it to specify 3 settings,
    // each setting is represented by splitting the range into thirds.
    if (potentiometerReadingScaled <= 0.33)
    {
        // Headlights OFF
        potentiometerState = 1;
    }
    else if (potentiometerReadingScaled > 0.33 && potentiometerReadingScaled < 0.66)
    {
        //Headlights AUTO
        potentiometerState = 2;
    }
    else if (potentiometerReadingScaled >= 0.66)
    {
        //Headlights ON
        potentiometerState = 3;
    }
    return (potentiometerState);
}



void headlightsSubsystem()
{
    static float lightSenorReading = 0.0;

    if (engineStarted)  
    {  
        if (headlightsSettings() == 1) 
        {
            //headlight settings set to OFF, switch both headlights OFF
            rightHeadlight = OFF;
            leftHeadlight = OFF;
        }
        else if (headlightsSettings() == 3) 
        {
            //headlight settings set to ON, switch both headlights ON
            rightHeadlight = ON;
            leftHeadlight = ON;
        }
        else if (headlightsSettings() == 2) 
        {
            // headlights settings switched to AUTO, both headlights are toggled on or off based on light sensor analog values (between 0 and 1)
            lightSenorReading = lightSensor.read();

            // if the room is generally bright (analog reads >= 0.66), headlights should switch OFF.
            if (lightSenorReading >= 0.66 )
            {
                // the LEDs should only toggle if the light sensor detects this value, >= 0.66, for more than 2000 miliseconds
                accumulatedHeadlightDelayTime_OFF = accumulatedHeadlightDelayTime_OFF + TIME_INCREMENT_MS;

                if (accumulatedHeadlightDelayTime_OFF >= DELAY_LIGHTS_OFF_MS){
                    // switch headlights off after a delay of 2 seconds
                    accumulatedHeadlightDelayTime_OFF = 0;
                    accumulatedHeadlightDelayTime_ON = 0;
                    rightHeadlight = OFF;
                    leftHeadlight = OFF;
                }
            }
            // if the room is generally dark (analog reads <= 0.33), headlights should switch ON.
            else if (lightSenorReading <= 0.33)
            {  
                // the LEDs should only toggle if the light sensor detects this value, <= 0.33, for more than 1000 miliseconds
                accumulatedHeadlightDelayTime_ON = accumulatedHeadlightDelayTime_ON + TIME_INCREMENT_MS;
                if (accumulatedHeadlightDelayTime_ON >= DELAY_LIGHTS_ON_MS){
                    // switch headlights on after a delay of 1 second
                    accumulatedHeadlightDelayTime_ON = 0;
                    accumulatedHeadlightDelayTime_OFF = 0;
                    rightHeadlight = ON;
                    leftHeadlight = ON;
                }  
            }
        
            // when the lighting remains ambient (between 0.33 and 0.66) there should be no change in LED status
            else{
                // must reset the delay time so that the delay will not accumulate over many different instances of flashing lights
                // should only trigger LEDs if the delay accumulates over one instance of lighting change (lasting for specified duration, 1 second or 2 seconds)
                accumulatedHeadlightDelayTime_ON = 0;
                accumulatedHeadlightDelayTime_OFF = 0;

            }
            
    }
    }
    // if the engine isn't started, or is turned off, LEDs should be OFF
    else {
        rightHeadlight = OFF;
        leftHeadlight = OFF;
        }
}