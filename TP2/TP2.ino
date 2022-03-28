#include <Arduino_FreeRTOS.h>
#include <MultiFunctionShield.h>

TaskHandle_t PrintT;

int emergency = 0;

void TaskBlink1(void *pvParameters)
{
  pinMode(LED_BUILTIN, OUTPUT);
  while (1)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    digitalWrite(LED_BUILTIN, LOW);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}


void TaskPrint(void *pvParameters)
{
  int counter = 0;
  while (1)
  {
    counter++;
    Serial.println(counter);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}


void TaskDisplay(void *pvParameters)
{
  MultiFunctionShield MFS;
  MFS.begin();
  pinMode(BUTTON_1_PIN, INPUT);

  long counter = 0;
  while (1)
  {
    if (!digitalRead(BUTTON_1_PIN))
    {
      counter++;
      MFS.Display(counter);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}


void TaskBuzz(void *pvParameters) {

  while (1) {

    pinMode(BUTTON_3_PIN, INPUT);

    if (!digitalRead(BUTTON_3_PIN)) {

      vTaskSuspend(PrintT);
      if (emergency == 0)Serial.println("Emergency");
      emergency = 1;
    }
    else {
      vTaskResume(PrintT);
      emergency = 0;
    }
  }
}




void setup(void) {
  // put your setup code here, to run once:
  Serial.begin(9600);

  xTaskCreate(TaskBlink1, "Task1", 128, NULL, 1, NULL);
  xTaskCreate(TaskPrint, "Task3", 128, NULL, 1, &PrintT);
  xTaskCreate(TaskDisplay, "Task2", 128, NULL, 1, NULL);
  xTaskCreate(TaskBuzz, "Task2", 128, NULL, 1, NULL);

  vTaskStartScheduler();

}


void loop(void) {
  // put your main code here, to run repeatedly:

}
