

// Init the Pins used for PWM
const int redPin = 9;
const int greenPin = 10;
const int bluePin = 11;
const int IRpin =A0;                                    // analog pin for reading the IR sensor
int prevVal = 100;
const int zeroThresold = 15;

#define MIDI_COMMAND_NOTE_OFF       0x80
#define MIDI_COMMAND_NOTE_ON        0x90
#define MIDI_COMMAND_NOTE_CC        0xB0

/* The format of the message to send via serial */
typedef union {
  struct {
    uint8_t command;
    uint8_t channel;
    uint8_t data2;
    uint8_t data3;
  } 
  msg;
  uint8_t raw[4];
} 
t_midiMsg;



void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  Serial.begin(115200);
  delay(200);
}

void loop() {
  float volts = analogRead(IRpin);//* 0.0048828125;   // value from sensor * (5/1024) - if running 3.3.volts then change 5 to 3.3
  // float distance = 20*pow(volts, -1.10);          // worked out from graph 65 = theretical distance / (1/Volts)S - luckylarry.co.uk
  volts = constrain(map(volts,0,620,1,127),0,127);
  t_midiMsg midiMsg;
  
    midiMsg.msg.command = MIDI_COMMAND_NOTE_CC;  //CC command
    midiMsg.msg.channel = 1;     // on channell 1
    midiMsg.msg.data2   = 2;     // CC #2
    
  if(volts > zeroThresold){
    midiMsg.msg.data3   = volts; 	/* Actual CC value */
    /* Send CC value */
    Serial.write(midiMsg.raw, sizeof(midiMsg));
  }else if(volts < zeroThresold && prevVal > 15){
    /* Send 0 for every value below thresold*/
    midiMsg.msg.data3   = 0; 	
    Serial.write(midiMsg.raw, sizeof(midiMsg));
  }  
  prevVal = volts;
  
  //Send data to leds
  meter(volts);
  
  //Wait to let IR sensor re-shot
  delay(40);    
  
}


void midiCC(byte cmd, byte data1, byte data2) {
  digitalWrite(13,HIGH);  // indicate we're sending MIDI data
  Serial.write(cmd);
  Serial.write(data1); // Controller number
  Serial.write(data2); // Controller value
  digitalWrite(13,LOW);
}

void meter(int level){
  // 0 - 127
  int r,g,b;
  if(level < 15){
    //blue
    rgbLed(0,0, map(level,0,15,0,255));
  }else if(level > 15 && level <= 75){
    //green
    g = map(level,15,75,50,255);
    b = map(level,15,75,200,0);
    r = map(level,15,75,0,100);
    rgbLed(0,g, 0);

  }
  else if(level > 75){
    //red
    g = map(level,75,127,255,20);
    r = map(level,75,127,0,255);
    rgbLed(r,g, 0);
  }

}


void rgbLed(int red, int green, int blue){
  analogWrite(redPin, red);
  analogWrite(bluePin, blue);
  analogWrite(greenPin, green);
}

