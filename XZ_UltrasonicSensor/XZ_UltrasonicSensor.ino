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
Ultrasonic UltRight(6,5);

void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600);
	
}

void loop() {
	// put your main code here, to run repeatedly:
  	Serial.println(UltLeft.GetCms());
  	delay(1000);
}
