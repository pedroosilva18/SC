const int ledPin = LED_BUILTIN; // define which LED to use
const long interval = 200; // define interval to toggle LED
unsigned long time0 = 0;
unsigned long time1;
volatile byte parar=LOW;

void setup() 
{
pinMode(2, INPUT_PULLUP);
pinMode (ledPin, OUTPUT); // set the ledPin as output
attachInterrupt(digitalPinToInterrupt(2), stop, FALLING);
}
// run

void loop() 
{
  
  time1 = millis();
  if ((time1-time0 > interval) &&(time1-time0 < 2*interval))
    digitalWrite(ledPin, LOW);
  if ((time1-time0> 2*interval) && (parar==LOW))  
  {
    digitalWrite(ledPin, HIGH);
    time0=millis();
  }
}

void stop() 
{
  digitalWrite(ledPin, LOW);
  parar=!parar;
}
