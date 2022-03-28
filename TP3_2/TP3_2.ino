#include <Arduino_FreeRTOS.h>
#include <semphr.h>

SemaphoreHandle_t xSemaphore = NULL;
int var_global = 0;

void TaskInc( void *pvParameters );

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  xSemaphore = xSemaphoreCreateMutex();
  xTaskCreate(TaskInc,"Task1", 128, NULL, 1, NULL);
  xTaskCreate(TaskInc,"Task2", 128, NULL, 1, NULL);
 
  // Start Scheduler that will manage tasks
  vTaskStartScheduler();
}

void TaskInc( void *pvParameters )
{
  while (1)
  {
    if( xSemaphore != NULL )
    {
      if( xSemaphoreTake( xSemaphore, ( TickType_t ) 1 ) == pdTRUE )    //se n tiver certo esperar 10 ticks até tentar novamente  
        {
       
        int var_local = var_global;
        var_local++;
        vTaskDelay( random(800, 2000)/ portTICK_PERIOD_MS );
        var_global = var_local;
        Serial.print(pcTaskGetName(xTaskGetCurrentTaskHandle() ));
        Serial.print(": ");
        Serial.println(var_global);

        xSemaphoreGive( xSemaphore );
        vTaskDelay(1);
        }
    }
  }
}



void loop() {
  // put your main code here, to run repeatedly:

}
