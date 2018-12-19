#include "ParkingControl.h"
#include "UltraSonicSensor.h"


ParkingControl::ParkingControl(UltraSonicSensor* sensor) 
{
    this->sensor = sensor;
}


bool ParkingControl::searchForCar()
{
    int delta = -1;
    int localThresholdNeg = -1; // ex. 300 - 30 = 270
    int localThresholdPos = -1; // ex. 300 + 30 = 330
   
    int distance = sensor->measureDistance();
    int movingAverageDistance = movingAverageMesurments(distance);

    if (printEnabled()) 
    {
        Serial.println(distance, DEC);
    }

    /*
    Algorithms:

    hard threshold:
        - Distance to ground shorter than threshold
    smart threshold:
        - Distance to ground shorter than threshold or distance longer than actual distance. 
        - If distance is longer than than typicaly means that a object do not reflect echo back to sensor.

    deviation in percent:
        - Disturbance in percent from threshold (either way)
        
    deviation in cm:
        - Disturbance in cm from threshold (either way)

    */

   switch (activeAlgorith) 
   {
        case ALGO_THRESHOLD_BASIC:
            return (threshold < distance);
            break;
        
        case ALGO_THRESHOLD_BASIC_MOVING:
            return (threshold < movingAverageDistance);
            break;

        case ALGO_THRESHOLD_SMART:
            return (threshold < movingAverageDistance || distance > movingAverageDistance);
            break;

        case ALGO_THRESHOLD_SMART_MOVING:
            return (threshold < movingAverageDistance || movingAverageDistance > normalDistance);
            break;
        
        case ALGO_DEVIATION_PERCENT:
            /// This algorithm defines a deadspace (in %) and only if sensor reports readings outside of this region we trigger alarm
            delta = (movingAverageDistance * deviation_percent)/100;
            localThresholdNeg = movingAverageDistance - delta; // ex. 300 - 30 = 270
            localThresholdPos = movingAverageDistance + delta; // ex. 300 + 30 = 330

            //if dist less than min or grater than max or outside the (expected value + headroom)
            return (localThresholdNeg > movingAverageDistance  || movingAverageDistance > localThresholdPos  || movingAverageDistance > normalDistance);
            break;
            
        
        case ALGO_DEVIATION_FIXED:
            localThresholdNeg = movingAverageDistance - deviation_cm;
            localThresholdPos = movingAverageDistance + deviation_cm;
            //if dist less than min or grater than max or outside the (expected value + headroom)
            return (localThresholdNeg > movingAverageDistance  || movingAverageDistance > localThresholdPos  || movingAverageDistance > normalDistance);
            break;

        default:
            //Bad algo
            //Always return false
            return false;
   }

    return false;

}

void ParkingControl::setNormalDistance(int dist)
{
    this->normalDistance = dist;
}

int ParkingControl::movingAverageMesurments(int mesurment)
{

    //Serial.println(mesurment, DEC);


    ///Naive implementation
    //shift all
    int curAvg = 0;
    
    for (int i = 0; i < MOVING_AVG_BUF_SIZE-1; i++)
    {
        avg[i] = avg[i+1];
        curAvg += avg[i+1];
    }

    avg[MOVING_AVG_BUF_SIZE-1] = mesurment;
    curAvg += mesurment;
    curAvg /= MOVING_AVG_BUF_SIZE;

    return curAvg;
}

void ParkingControl::iterateStatemachine()
{
    
    bool detected;

    switch (state)
    {
        case IDLE:
            ///Prepare system. Start sensor and discard first 100 readings (loads up moving average)
            for(int i = 0; i < 100; i++)
            {
                movingAverageMesurments(sensor->measureDistance());
            }

            // System active. 
            state = SEARCHING;
            break;

        case SEARCHING:

            //Serial.println("STATE: SEARCHING");

            ///Run Sensor. 
            // IF sensor deviates, something is detected
            detected = searchForCar();
            if (detected) 
            {
                //state = CAR_DETECTED;
                //Serial.println("Car detected. State not changed");
            }


            break;
        
        case CAR_DETECTED:
            ///Filter and determain if this is actualy what we are looking for
            Serial.println("STATE: CAR_DETECTED");

            

            break;

        case CAR_PARKED:
            // if sensor deviates, the car change state to CAR_REMOVED
            Serial.println("STATE: CAR_PARKED");
            break;

        case CAR_REMOVED:
            //Reset logic

            Serial.println("STATE: CAR_REMOVED");
            state = SEARCHING;

            break;

        default:
            break;
    }
    
}

int ParkingControl::algoThresholdBasic(int measurment)
{
    
}

void ParkingControl::setThreshold(int threshold)
{
    this->threshold = threshold;
}

void ParkingControl::changeAlgorithm(int algo)
{
    activeAlgorith = algo;
}


void ParkingControl::setPrint(bool onOff) 
{
    this->print = onOff;
}

bool ParkingControl::printEnabled() 
{
    return this->print;
}


bool ParkingControl::setAlgorithm(int algo)
{
    if (algo >= 0 || algo < ParkingControl::ALGO_LAST_NOT_APPLICABLE) 
    {
        this->activeAlgorith = algo;
        return true;
    }

    return false;
    
}

void ParkingControl::printAlgorithms()
{

    Serial.print(F("\n\n"));
    Serial.println(F("Algorithms:"));

    Serial.print(ALGO_THRESHOLD_BASIC, DEC);
    Serial.println(F(": Basic Threshold"));

    Serial.print(ALGO_THRESHOLD_BASIC_MOVING, DEC);
    Serial.println(F(": Basic Threshold With Moving Average"));

    Serial.print(ALGO_THRESHOLD_SMART, DEC);
    Serial.println(F(": Smart Threshold"));

    Serial.print(ALGO_THRESHOLD_SMART_MOVING, DEC);
    Serial.println(F(": Smart Threshold With Moving Average"));

    Serial.print(ALGO_DEVIATION_PERCENT, DEC);
    Serial.println(F(": Deviation Algorithm In Percent"));

    Serial.print(ALGO_DEVIATION_FIXED, DEC);
    Serial.println(F(": Deviation Algorithm In Fixed CM's"));

  
    Serial.flush();
}



/*
void ParkingControl::checkForCar() 
{

}

void ParkingControl::signalOn() 
{
    
    //playAlarm();
    ///indicator(true); 
}

bool ParkingControl::timerExpired() 
{

}

void ParkingControl::monitorParked() 
{
    //while(carParked() && !timerExpired()){}
}

bool ParkingControl::carParked() 
{
    //return (threshold < distance);
}

bool ParkingControl::carDetected() 
{


}
*/
