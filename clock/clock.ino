class Digits
{
  private:
	uint8_t _dataPin;
	uint8_t _clockPin;
	uint8_t _latchPin;
	byte _dataArray[10];
	void shiftDigit(byte myDataOut);

  public:
	Digits(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin);
	void show( int digits[], int len );
};

Digits::Digits(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin)
{
    _dataArray[0] = 0x3F; //00111111 - 0
    _dataArray[1] = 0x06; //00000110 - 1
    _dataArray[2] = 0x5B; //01011011 - 2
    _dataArray[3] = 0x4F; //01001111 - 3
    _dataArray[4] = 0x66; //01100110 - 4
    _dataArray[5] = 0x6D; //01101101 - 5
    _dataArray[6] = 0x7D; //01111101 - 6
    _dataArray[7] = 0x07; //00000111 - 7
    _dataArray[8] = 0x7F; //01111111 - 8
    _dataArray[9] = 0x67; //01100111 - 9
	_dataPin = dataPin;
	_clockPin = clockPin;
	_latchPin = latchPin;
	pinMode(_latchPin, OUTPUT);
}

void Digits::show( int digits[], int len )
{
	digitalWrite(_latchPin, 0);
 
	for( int i=len-1; i>=0; i--)
	{
		shiftDigit(_dataArray[digits[i]]);
	}
	
	digitalWrite(_latchPin, 1);
}

void Digits::shiftDigit(byte myDataOut)
{
  int i=0;
  int pinState;
  pinMode(_clockPin, OUTPUT);
  pinMode(_dataPin, OUTPUT);

  digitalWrite(_dataPin, 0);
  digitalWrite(_clockPin, 0);

  for (i=7; i>=0; i--)  
  {
    digitalWrite(_clockPin, 0);
    if( myDataOut & (1<<i) ) 
    {
	pinState= 1;
    }
    else 
    {
	pinState= 0;
    }

    digitalWrite(_dataPin, pinState);
    digitalWrite(_clockPin, 1);
    digitalWrite(_dataPin, 0);
  }
  digitalWrite(_clockPin, 0);
}

const uint8_t DATA_PIN = 13; 
const uint8_t CLOCK_PIN = 12; 
const uint8_t LATCH_PIN = 11; 

Digits disp = Digits(DATA_PIN, CLOCK_PIN, LATCH_PIN);

int startValue = 60;
int data = 0;
int length = 2;
int data_array[2] = {};

void setup(void) {
  Serial.begin(9600); 
}

void loop(void){
   data_array[0] = data / 10;
   data_array[1] = data % 10;
   disp.show(data_array,length);
   delay(1000);
   
   data-=1;
   if(-1 == data){
     data = startValue;
   }
}

void loop2(void) {
  if(Serial.available() > 0){
    data = Serial.parseInt();
    Serial.print(data, DEC);
    data_array[0] = data;
    disp.show(data_array,1);
    delay(1000);
  }
}


