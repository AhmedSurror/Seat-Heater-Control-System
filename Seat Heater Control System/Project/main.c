/* Kernel includes. */
#include <heatingsystem.h>
#include <HAL/POTS/pots.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "GPTM.h"
#include "gpio.h"
#include "uart0.h"
#include "HAL/RGB_LED/rgb.h"

/* Defines the periodicity of runtime measurements task */
#define RUNTIME_MEASUREMENTS_TASK_PERIODICITY (1000U)

/* Task prototypes */
static void prvSetupHardware(void);
void vDisplaySystemStateTask(void *pvParameters);
void vcpuLoadMeasurementTask(void *pvParameters);
void vtasksTimeMeasurementTask(void *pvParameters);
void vSeat1AdjustHeaterTask(void *pvParameters);
void vSeat2AdjustHeaterTask(void *pvParameters);
void vgetSeat1CurrentTempTask(void *pvParameters);
void vgetSeat2CurrentTempTask(void *pvParameters);
void vCheckSeat1HeatingLevelChange(void *pvParameters);
void vCheckSeat2HeatingLevelChange(void *pvParameters);

/* Global variables */
SystemStateStructureType SystemState = {0, HEATING_OFF, HEATER_OFF, 0, HEATING_OFF, HEATER_OFF};

/* Tasks Handlers */
TaskHandle_t vDisplaySystemStateTaskHandle;
TaskHandle_t vcpuLoadMeasurementTaskHandle;
TaskHandle_t vtasksTimeMeasurementTaskHandle;
TaskHandle_t vSeat1AdjustHeaterHandle;
TaskHandle_t vSeat2AdjustHeaterHandle;
TaskHandle_t vgetSeat1CurrentTempTaskHandle;
TaskHandle_t vgetSeat2CurrentTempTaskHandle;
TaskHandle_t vCheckSeat1HeatingLevelChangeHandle;
TaskHandle_t vCheckSeat2HeatingLevelChangeHandle;

/* Semaphores */
xSemaphoreHandle xMutex;

/* Arrays to store task execution times */
uint32 ullTasksOutTime[10];
uint32 ullTasksInTime[10];
uint32 ullTasksTotalTime[10];


int main()
{
    /* Setup the hardware for use with the Tiva C board. */
    prvSetupHardware();

    /* Create a mutex semaphore */
    xMutex = xSemaphoreCreateMutex();

    /* Create tasks */

    xTaskCreate(vtasksTimeMeasurementTask, "Tasks Time Measurements Task", 256, NULL, 1, &vtasksTimeMeasurementTaskHandle);
    xTaskCreate(vcpuLoadMeasurementTask, "CPU Load Measurement Task", 32, NULL, 2, &vcpuLoadMeasurementTaskHandle);
    xTaskCreate(vDisplaySystemStateTask, "Displaying System State Task", 32, (void*)&SystemState, 2, &vDisplaySystemStateTaskHandle);
    xTaskCreate(vSeat1AdjustHeaterTask, "Adjusting Seat 1 Heater Intensity Task", 32, (void*)&SystemState, 2, &vSeat1AdjustHeaterHandle);
    xTaskCreate(vSeat2AdjustHeaterTask, "Adjusting Seat 2 Heater Intensity Task", 32, (void*)&SystemState, 2, &vSeat2AdjustHeaterHandle);
    xTaskCreate(vgetSeat1CurrentTempTask, "Getting Seat 1 Current Temperature Task", 32, (void*)&SystemState, 2, &vgetSeat1CurrentTempTaskHandle);
    xTaskCreate(vgetSeat2CurrentTempTask, "Getting Seat 2 Current Temperature Task", 32, (void*)&SystemState, 2, &vgetSeat2CurrentTempTaskHandle);
    xTaskCreate(vCheckSeat1HeatingLevelChange, "Getting Seat 1 Heating Level Changes Task", 32, (void*)&SystemState, 3, &vCheckSeat1HeatingLevelChangeHandle);
    xTaskCreate(vCheckSeat2HeatingLevelChange, "Getting Seat 2 Heating Level Changes Task", 32, (void*)&SystemState, 3, &vCheckSeat2HeatingLevelChangeHandle);


    vTaskSetApplicationTaskTag( vtasksTimeMeasurementTaskHandle, ( TaskHookFunction_t ) 1 );
    vTaskSetApplicationTaskTag( vcpuLoadMeasurementTaskHandle, ( TaskHookFunction_t ) 2 );
    vTaskSetApplicationTaskTag( vDisplaySystemStateTaskHandle, ( TaskHookFunction_t ) 3 );
    vTaskSetApplicationTaskTag( vSeat1AdjustHeaterHandle, ( TaskHookFunction_t ) 4 );
    vTaskSetApplicationTaskTag( vSeat2AdjustHeaterHandle, ( TaskHookFunction_t ) 5 );
    vTaskSetApplicationTaskTag( vgetSeat1CurrentTempTaskHandle, ( TaskHookFunction_t ) 6 );
    vTaskSetApplicationTaskTag( vgetSeat2CurrentTempTaskHandle, ( TaskHookFunction_t ) 7 );
    vTaskSetApplicationTaskTag( vCheckSeat1HeatingLevelChangeHandle, ( TaskHookFunction_t ) 8 );
    vTaskSetApplicationTaskTag( vCheckSeat2HeatingLevelChangeHandle, ( TaskHookFunction_t ) 9 );

    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();

    /* Should never reach here! */
    for (;;);

}


/* Setup hardware initialization function */
static void prvSetupHardware( void )
{
    /* Place here any needed HW initialization such as GPIO, UART, etc.  */
    UART0_Init();
    GPTM_WTimer0Init();
    GPIO_BuiltinButtonsLedsInit();
    POT1_init();
    RGB_init();

    RGB_RedLedOff();
    RGB_GreenLedOff();
    RGB_BlueLedOff();

    GPIO_RedLedOff();
    GPIO_GreenLedOff();
    GPIO_BlueLedOff();
}

/* Task to measure the execution time of other tasks */
void vtasksTimeMeasurementTask(void *pvParameters){


    vTaskDelay(pdMS_TO_TICKS(2000));

    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
        UART0_SendString("CPU Load Measurement Task execution time is ");
        UART0_SendInteger(ullTasksTotalTime[2] / 10);
        UART0_SendString(" msec \r\n");

        UART0_SendString("Display System State Task execution time is ");
        UART0_SendInteger(ullTasksTotalTime[3] / 10);
        UART0_SendString(" msec \r\n");

        UART0_SendString("Adjusting Seat 1 Heater Intensity Task execution time is ");
        UART0_SendInteger(ullTasksTotalTime[4] / 10);
        UART0_SendString(" msec \r\n");

        UART0_SendString("Adjusting Seat 2 Heater Intensity Task execution time is ");
        UART0_SendInteger(ullTasksTotalTime[5] / 10);
        UART0_SendString(" msec \r\n");

        UART0_SendString("Getting Seat 1 Current Temperature Task execution time is ");
        UART0_SendInteger(ullTasksTotalTime[6] / 10);
        UART0_SendString(" msec \r\n");

        UART0_SendString("Getting Seat 2 Current Temperature Task execution time is ");
        UART0_SendInteger(ullTasksTotalTime[7] / 10);
        UART0_SendString(" msec \r\n");

        UART0_SendString("Getting Seat 1 Heating Level Changes Task execution time is ");
        UART0_SendInteger(ullTasksTotalTime[8] / 10);
        UART0_SendString(" msec \r\n");

        UART0_SendString("Getting Seat 2 Heating Level Changes Task execution time is ");
        UART0_SendInteger(ullTasksTotalTime[9] / 10);
        UART0_SendString(" msec \r\n");
        xSemaphoreGive(xMutex); // Give back the semaphore here
        vTaskDelete(NULL);
    }


}

/* Task to measure CPU load */
void vcpuLoadMeasurementTask(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
            uint8_t ucCounter, ucCPU_Load;
            uint32_t ullTotalTasksTime = 0;

            for(ucCounter = 1; ucCounter < 9; ucCounter++)
            {
                ullTotalTasksTime += ullTasksTotalTime[ucCounter];
            }
            ucCPU_Load = (ullTotalTasksTime * 100) /  GPTM_WTimer0Read();

            taskENTER_CRITICAL();
            UART0_SendString("------------------------ CPU Load is ");
            UART0_SendInteger(ucCPU_Load);
            UART0_SendString("%  ---------------------------\r\n");
            taskEXIT_CRITICAL();

            xSemaphoreGive(xMutex); // Give back the semaphore here
        }

        vTaskDelayUntil(&xLastWakeTime, RUNTIME_MEASUREMENTS_TASK_PERIODICITY);
    }
}

/* Task to display system state */
void vDisplaySystemStateTask(void *pvParameters)
{
    SystemStateStructureType* systemState = (SystemStateStructureType*)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;) {
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
            UART0_SendString("Seat1 Temperature: ");
            UART0_SendInteger(systemState->ui8Seat1TempValueC);
            UART0_SendString("�C\t\t|\t");
            UART0_SendString("Seat2 Temperature: ");
            UART0_SendInteger(systemState->ui8Seat2TempValueC);
            UART0_SendString("�C\r\n");

            UART0_SendString("Seat1 Heating Level: ");
            switch (systemState->Seat1heatingLevel) {
            case HEATING_OFF:
                UART0_SendString("OFF\t|\t");
                break;
            case HEATING_LOW:
                UART0_SendString("LOW\t|\t");
                break;
            case HEATING_MEDIUM:
                UART0_SendString("MEDIUM\t|\t");
                break;
            case HEATING_HIGH:
                UART0_SendString("HIGH\t|\t");
                break;
            default:
                break;
            }

            UART0_SendString("Seat2 Heating Level: ");
            switch (systemState->Seat2heatingLevel) {
            case HEATING_OFF:
                UART0_SendString("OFF\r\n");
                break;
            case HEATING_LOW:
                UART0_SendString("LOW\r\n");
                break;
            case HEATING_MEDIUM:
                UART0_SendString("MEDIUM\r\n");
                break;
            case HEATING_HIGH:
                UART0_SendString("HIGH\r\n");
                break;
            default:
                break;
            }

            UART0_SendString("Seat1 Heater Intensity: ");
            switch (systemState->Seat1heaterState) {
            case HEATER_OFF:
                UART0_SendString("OFF\t|\t");
                break;
            case HEATER_LOW:
                UART0_SendString("LOW\t|\t");
                break;
            case HEATER_MEDIUM:
                UART0_SendString("MEDIUM\t|\t");
                break;
            case HEATING_HIGH:
                UART0_SendString("HIGH\t|\t");
                break;
            default:
                break;
            }

            UART0_SendString("Seat2 Heater Intensity: ");
            switch (systemState->Seat2heaterState) {
            case HEATER_OFF:
                UART0_SendString("OFF\r\n");
                break;
            case HEATER_LOW:
                UART0_SendString("LOW\r\n");
                break;
            case HEATER_MEDIUM:
                UART0_SendString("MEDIUM\r\n");
                break;
            case HEATING_HIGH:
                UART0_SendString("HIGH\r\n");
                break;
            default:
                break;
            }

            UART0_SendString("=====================================================================\r\n");
            xSemaphoreGive(xMutex);
            vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 1000 ) );

        }
    }
}

/* Task to adjust heater for Seat 1 */
void vgetSeat1CurrentTempTask(void *pvParameters)
{
    SystemStateStructureType* systemState = (SystemStateStructureType*)pvParameters;
    uint8_t ui8seat1CurrentTempValueC;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;) {
        ui8seat1CurrentTempValueC = (POT1_getValue() * 45) / POT1_MAX_VALUE;
        systemState->ui8Seat1TempValueC = ui8seat1CurrentTempValueC;
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 100 ) );
    }

}

/* Task to adjust heater for Seat 2 */
void vgetSeat2CurrentTempTask(void *pvParameters)
{
    SystemStateStructureType* systemState = (SystemStateStructureType*)pvParameters;
    uint8_t ui8seat2CurrentTempValueC;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;) {
        ui8seat2CurrentTempValueC = (POT2_getValue() * 45) / POT2_MAX_VALUE;
        systemState->ui8Seat2TempValueC = ui8seat2CurrentTempValueC;
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 100 ) );
    }

}

/* Task to get current temperature of Seat 1 */
void vSeat1AdjustHeaterTask(void *pvParameters)
{
    SystemStateStructureType* systemState = (SystemStateStructureType*)pvParameters;
    uint8_t ui8seat1DesiredTempValueC;
    uint8_t ui8seat1CurrentTempValueC;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;) {
        switch (systemState->Seat1heatingLevel) {
        case HEATING_LOW:
            ui8seat1DesiredTempValueC = 25;
            break;
        case HEATING_MEDIUM:
            ui8seat1DesiredTempValueC = 30;
            break;
        case HEATING_HIGH:
            ui8seat1DesiredTempValueC = 35;
            break;
        default:
            break;
        }
        ui8seat1CurrentTempValueC = systemState->ui8Seat1TempValueC;


        if(ui8seat1CurrentTempValueC <= 40 && ui8seat1CurrentTempValueC >= 5){

            if((ui8seat1DesiredTempValueC > ui8seat1CurrentTempValueC) && ((systemState->Seat1heatingLevel) != HEATING_OFF)){

                if((ui8seat1DesiredTempValueC - ui8seat1CurrentTempValueC) >= 10){
                    systemState->Seat1heaterState = HEATER_HIGH;

                    RGB_RedLedOff();
                    RGB_GreenLedOn();
                    RGB_BlueLedOff();

                }
                else if((ui8seat1DesiredTempValueC - ui8seat1CurrentTempValueC) >= 5){
                    systemState->Seat1heaterState = HEATER_MEDIUM;
                    RGB_RedLedOff();
                    RGB_GreenLedOff();
                    RGB_BlueLedOn();
                }
                else if(
                        (((ui8seat1DesiredTempValueC - ui8seat1CurrentTempValueC) >= 2) && (systemState->Seat1heaterState != HEATER_OFF)) ||
                        (((ui8seat1DesiredTempValueC - ui8seat1CurrentTempValueC) > 3) && (systemState->Seat1heaterState == HEATER_OFF))
                )
                {
                    systemState->Seat1heaterState = HEATER_LOW;
                    RGB_RedLedOff();
                    RGB_GreenLedOn();
                    RGB_BlueLedOn();
                }
                else{
                    systemState->Seat1heaterState = HEATER_OFF;
                    RGB_GreenLedOff();
                    RGB_BlueLedOff();
                }
            }
            else{
                systemState->Seat1heaterState = HEATER_OFF;
                RGB_RedLedOff();
                RGB_GreenLedOff();
                RGB_BlueLedOff();
            }
        }
        else{
            systemState->Seat1heaterState = HEATER_OFF;
            RGB_RedLedOn();
            RGB_GreenLedOff();
            RGB_BlueLedOff();
        }
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 100 ) );
    }
}

/* Task to get current temperature of Seat 2 */
void vSeat2AdjustHeaterTask(void *pvParameters)
{

    SystemStateStructureType* systemState = (SystemStateStructureType*)pvParameters;
    uint8_t ui8seat2DesiredTempValueC;
    uint8_t ui8seat2CurrentTempValueC;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;) {
        switch (systemState->Seat2heatingLevel) {
        case HEATING_LOW:
            ui8seat2DesiredTempValueC = 25;
            break;
        case HEATING_MEDIUM:
            ui8seat2DesiredTempValueC = 30;
            break;
        case HEATING_HIGH:
            ui8seat2DesiredTempValueC = 35;
            break;
        default:
            break;
        }
        ui8seat2CurrentTempValueC = systemState->ui8Seat2TempValueC;


        if(ui8seat2CurrentTempValueC <= 40 && ui8seat2CurrentTempValueC >= 5){

            if((ui8seat2DesiredTempValueC > ui8seat2CurrentTempValueC) && ((systemState->Seat2heatingLevel) != HEATING_OFF)){

                if((ui8seat2DesiredTempValueC - ui8seat2CurrentTempValueC) >= 10){
                    systemState->Seat2heaterState = HEATER_HIGH;

                    GPIO_RedLedOff();
                    GPIO_GreenLedOn();
                    GPIO_BlueLedOff();

                }
                else if((ui8seat2DesiredTempValueC - ui8seat2CurrentTempValueC) >= 5){
                    systemState->Seat2heaterState = HEATER_MEDIUM;
                    GPIO_RedLedOff();
                    GPIO_GreenLedOff();
                    GPIO_BlueLedOn();
                }
                else if(
                        (((ui8seat2DesiredTempValueC - ui8seat2CurrentTempValueC) >= 2) && (systemState->Seat2heaterState != HEATER_OFF)) ||
                        (((ui8seat2DesiredTempValueC - ui8seat2CurrentTempValueC) > 3) && (systemState->Seat2heaterState == HEATER_OFF))
                )
                {
                    systemState->Seat2heaterState = HEATER_LOW;
                    GPIO_RedLedOff();
                    GPIO_GreenLedOn();
                    GPIO_BlueLedOn();
                }
                else{
                    systemState->Seat2heaterState = HEATER_OFF;
                    GPIO_GreenLedOff();
                    GPIO_BlueLedOff();
                }
            }
            else{
                systemState->Seat2heaterState = HEATER_OFF;
                GPIO_RedLedOff();
                GPIO_GreenLedOff();
                GPIO_BlueLedOff();
            }
        }
        else{
            systemState->Seat2heaterState = HEATER_OFF;
            GPIO_RedLedOn();
            GPIO_GreenLedOff();
            GPIO_BlueLedOff();
        }
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 100 ) );
    }
}

/* Task to check and change heating level for Seat 1 */
void vCheckSeat1HeatingLevelChange(void *pvParameters)
{
    SystemStateStructureType* systemState = (SystemStateStructureType*)pvParameters;
    HeatingLevelType seat1CurrentHeatingLevel;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {

        seat1CurrentHeatingLevel = systemState->Seat1heatingLevel;

        if((GPIO_EXTSWGetState() == PRESSED) || (GPIO_SW1GetState() == PRESSED)){
            vTaskDelay(pdMS_TO_TICKS(30));
            if((GPIO_EXTSWGetState() == PRESSED) || (GPIO_SW1GetState() == PRESSED)){

                switch (seat1CurrentHeatingLevel) {
                case HEATING_OFF:
                    systemState->Seat1heatingLevel = HEATING_LOW;
                    break;
                case HEATING_LOW:
                    systemState->Seat1heatingLevel = HEATING_MEDIUM;
                    break;
                case HEATING_MEDIUM:
                    systemState->Seat1heatingLevel = HEATING_HIGH;
                    break;
                case HEATING_HIGH:
                    systemState->Seat1heatingLevel = HEATING_OFF;
                    break;
                default:
                    break;
                }
                vTaskDelay(pdMS_TO_TICKS(500));
            }
        }
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 100 ) );

    }
}

/* Task to check and change heating level for Seat 2 */
void vCheckSeat2HeatingLevelChange(void *pvParameters)
{
    SystemStateStructureType* systemState = (SystemStateStructureType*)pvParameters;
    HeatingLevelType seat2currentHeatingLevel;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {

        seat2currentHeatingLevel = systemState->Seat2heatingLevel;

        if(GPIO_SW2GetState() == PRESSED){
            vTaskDelay(pdMS_TO_TICKS(30));
            if(GPIO_SW2GetState() == PRESSED){

                switch (seat2currentHeatingLevel) {
                case HEATING_OFF:
                    systemState->Seat2heatingLevel = HEATING_LOW;
                    break;
                case HEATING_LOW:
                    systemState->Seat2heatingLevel = HEATING_MEDIUM;
                    break;
                case HEATING_MEDIUM:
                    systemState->Seat2heatingLevel = HEATING_HIGH;
                    break;
                case HEATING_HIGH:
                    systemState->Seat2heatingLevel = HEATING_OFF;
                    break;
                default:
                    break;
                }
                vTaskDelay(pdMS_TO_TICKS(500));
            }
        }
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 100 ) );

    }
}

/*-----------------------------------------------------------*/
