class Ultrasonic{
public:
	Ultrasonic(int trigger, int echo){
		pinMode(trigger, OUTPUT);
  		digitalWrite(trigger, LOW);
  		pinMode(echo, INPUT);
  		_echo = echo;
  		_trigger = trigger;
	}

	float GetCms(){
		return GetPulse()/ 58.0;
	}

private:
    int _echo;
    int _trigger;
    unsigned long _t1;
  	unsigned long _t2;
  	unsigned long _pulse_width;
  	const unsigned int MAX_DIST = 23200;
  	enum timeoutFlag {notUsed, DataReady, SentSignal, TimeOutResponse, TimeOutWaitingSignal};
  	timeoutFlag _flag;

    unsigned long GetPulse(){
		digitalWrite(_trigger, HIGH);
		delayMicroseconds(10);
		digitalWrite(_trigger, LOW);

		_flag = notUsed;

		_t1 = micros();
		while ( digitalRead(_echo) == 0){
			if (micros() - _t1 > 2000){
				_flag = TimeOutResponse;
				break;
			}else{
				_flag = SentSignal;
			}
		}
    
    	_t1 = micros();
    	if (_flag == SentSignal){
			while ( digitalRead(_echo) == 1){
				if (micros() - _t1 > MAX_DIST){
					_flag = TimeOutWaitingSignal;
					break;
				}else{
					_flag = DataReady;
				}
			}
			_t2 = micros();
			_pulse_width = _t2 - _t1;
		}

		if ( _pulse_width < MAX_DIST  || _flag == DataReady) {
			return _pulse_width;
		}

		return 0;
  	}
};

Ultrasonic UltLeft(3,2);
Ultrasonic UltRight(12,11);

const int size = 10;		// For arrays
const int sizeBin = 10;		// For binary arrays
const int thresholdBin = 3;
const int threshold = 140; 	// In cms

float leftArray[size];
float rightArray[size];
float leftBinArray[size];
float rightBinArray[size];
float leftAverageArray[size];
float rightAverageArray[size];
int count = 0;
int countBin = 0;



void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600);
	
}
 
void loop() {
	// put your main code here, to run repeatedly:
	
	leftArray[count] = UltLeft.GetCms();
	rightArray[count] = UltRight.GetCms();

	leftAverageArray[count] = GetAverage(leftArray);
	rightAverageArray[count] = GetAverage(rightArray);
	
	CalculateBinArray(leftAverageArray, leftBinArray);
	CalculateBinArray(rightAverageArray, rightBinArray);


	Serial.print(millis());
	Serial.print(",");

	Serial.print(leftArray[count]);
	Serial.print(",");
	Serial.print(rightArray[count]);
	Serial.print(",");

	Serial.print(leftAverageArray[count]);
	Serial.print(",");
	Serial.print(rightAverageArray[count]);
	Serial.print(",");

	Serial.print(DetectObject(leftBinArray));
	Serial.print(",");
	Serial.print(DetectObject(rightBinArray));
	Serial.println("");


	

	count++;
	if (count == size){
		count = 0;
	}
   
  	//delay(20);
}

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

	if (sum  > ((size/2)-1)){
		return 1;
	}

	return 0;

}
