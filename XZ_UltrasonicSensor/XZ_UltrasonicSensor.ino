//=====[ LIBRARIES ]========================================================================================
#include "Ultrasonic.h"

//=====[ OBJECTS ]==========================================================================================
Ultrasonic UltLeft(3,2);
Ultrasonic UltRight(12,11);

//=====[ CONSTANTS ]========================================================================================
const int size = 6;		// For arrays
const int sizeBin = 10;		// For binary arrays
const int thresholdBin = 3;
const int threshold = 70; 	// In cms

//=====[ VARIABLES ]========================================================================================
float leftArray[size];
float rightArray[size];
float leftBinArray[size];
float rightBinArray[size];
float leftAverageArray[size];
float rightAverageArray[size];
int count = 0;
int countBin = 0;
enum MotionDetection{NoMotion, RightMotion, LeftMotion, ErrorMotion, OutOfRangeMotion};
MotionDetection motion;

int leftDetectObj = 0;
int rightDetectObj = 0;

//=====[ INIT ]=============================================================================================
void setup() {
	Serial.begin(9600);
}

//=====[ MAIN LOOP ]======================================================================================== 
void loop() {

	leftArray[count] = UltLeft.GetCms();
	rightArray[count] = UltRight.GetCms();

	leftAverageArray[count] = GetAverage(leftArray);
	rightAverageArray[count] = GetAverage(rightArray);
	
	CalculateBinArray(leftAverageArray, leftBinArray);
	CalculateBinArray(rightAverageArray, rightBinArray);

	leftDetectObj = DetectObject(leftBinArray);
	rightDetectObj = DetectObject(rightBinArray);

  	motion = DetectMotion(leftDetectObj, rightDetectObj);
  
	Serial.print(millis()); Serial.print(",");
	Serial.print(leftArray[count]);	Serial.print(","); Serial.print(rightArray[count]); Serial.print(",");
	Serial.print(leftAverageArray[count]); Serial.print(","); Serial.print(rightAverageArray[count]); Serial.print(",");
	Serial.print(leftDetectObj); Serial.print(","); Serial.print(rightDetectObj);
	Serial.print(","); Serial.print(motion);
	Serial.println("");

	count = (count == (size-1)) ? (0) : (count+1);
	
	//delay(50);
}

//=====[ FUNCTIONS ]========================================================================================
float GetAverage(float data[]){
	float average=0;

	for (int i = 0; i < size; i++)
		average += data[i];

	return (average / size);
}

void CalculateBinArray(float *rawArray, float *binArray){
	for (int i = 0; i < size; i++){
		if (rawArray[i] < threshold){
			binArray[i] = 1;
		}else{
			binArray[i] = 0;
		}
	}
}

int DetectObject(float *binArray){
	int sum=0;

	for (int i = 0; i < size; i++)
		sum += binArray[i];

	if (sum  > ((size/2)-1))
		return 1;

	return 0;

}


int DetectMotion(int left, int right){
	int event = 0;
	MotionDetection detection;
	static int prevEvent = 0;


	event = (left) ? (event | 0x2) : (event);
	event = (right) ? (event | 0x1) : (event);

	event |= ((prevEvent << 2) & 0x06);

	switch (event){
		//No motion detected
		case 0:
		case 1:
		case 2:
		case 3:
		case 5:	
		case 10:
			detection = NoMotion;
			break;

		// Right motion
		case 4:
		case 9: // Left-Right
		case 11:
		case 13:
			detection = RightMotion;
			break;

		// Left motion
		case 6: // Right-Left
		case 7:
		case 8:
		case 14:
			detection = LeftMotion;
			break;

		// Not possible
		case 12: // Center-No
		case 15: // Center-Center
			detection = ErrorMotion;
			break;
		default:
			detection = OutOfRangeMotion;
	}

  prevEvent = event & 0x03;

  return detection;

}
