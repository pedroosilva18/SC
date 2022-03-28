#include <Arduino_FreeRTOS.h>
#include <semphr.h>


SemaphoreHandle_t xSemaphore = NULL;

const int ledPin = LED_BUILTIN;

volatile byte toggle=LOW;
unsigned long time0 = 0;
unsigned long time1;
unsigned long interval = 1000;


void TaskBlink1(void *pvParameters)
{
  pinMode(ledPin,OUTPUT);
  while(1)
  {
    time1 = millis();
   
    if ((time1-time0) > interval){
      if (toggle==LOW)toggle=HIGH;
      else toggle=LOW;
      digitalWrite(ledPin, toggle);
      time0=time1;
     
    }
  }
}

void TaskSerial(void *pvParameters)
{
  while(1){
    int intervalTemp=-1;
    if (Serial.available() > 0)intervalTemp=Serial.parseInt();
   

    if (intervalTemp>0)
    {
      while(1)
      {
           if( xSemaphoreTake( xSemaphore, ( TickType_t ) 1 ) == pdTRUE )
          {
            interval=intervalTemp;
            xSemaphoreGive( xSemaphore );
            break;
          }        
      }
    }
  }
}



void setup() {
 
  Serial.begin(9600);
  pinMode (ledPin, OUTPUT);
 
  xSemaphore = xSemaphoreCreateMutex();
 
  xTaskCreate(TaskBlink1,"Task1",128,NULL,1,NULL);
  xTaskCreate(TaskSerial,"Task2",128,NULL,1,NULL);
  vTaskStartScheduler();
 
}


void loop() {
  // put your main code here, to run repeatedly:
}
