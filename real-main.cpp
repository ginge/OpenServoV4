/*
 *  OpenServo v4 ARM port test
 * 
 * Barry Carter 2012
*/


/* Include libmaple and the FreeRTOS module. */
#include "OpenServoCommon.h"
#include "setup.h"
#include "FreeRTOSConfig.h"
#include "pid.h"
#include "pwm_os.h"
#include "os_adc.h"

/* Function Prototypes */
static void vPrintTask(void* pvParameters); /* To print our output */
static void vCommTask(void* pvParameters);
static void vMainTask(void* pvParameters);
static void vPositionADCTask(void* pvParameters);
static void vPIDTask(void* pvParameters);

/* Global Variables */
xQueueHandle xQueuePrint;
xQueueHandle xQueuePos;

PMessage xMessage;


/**
 * The main function.
 */
void setup( void )
{
    //enableDebugPorts();
    SerialUSB.begin();
    SerialUSB.println("Press any key to continue");
    SerialUSB.read();
    SerialUSB.println("OpenServo V4 test");

	/* Create the Queue for communication between the tasks */
	    xQueuePrint = xQueueCreate( 5, sizeof(struct PMessage *) );
	    xQueuePos = xQueueCreate( 5, sizeof(uint16_t) );

    /* Add the two tasks to the scheduler */
    xTaskCreate(vPrintTask, (const signed char*)"Print", configMINIMAL_STACK_SIZE, NULL, 1, NULL );
    xTaskCreate(vPositionADCTask, (const signed char*)"Position", configMINIMAL_STACK_SIZE, NULL, 1, NULL );
    xTaskCreate(vPIDTask, (const signed char*)"PID", configMINIMAL_STACK_SIZE, NULL, 2, NULL );
    
    setupOpenServo();

    /* Start the scheduler. */
    vTaskStartScheduler();
    pwm_enable();
    registers_write_word(REG_SEEK_POSITION_HI, REG_SEEK_POSITION_LO, 0x9C4);
    
}

void loop( void )
{
}

// Standard libmaple init() and main.
//
// The init() part makes sure your board gets set up correctly. It's
// best to leave that alone unless you know what you're doing. main()
// is the usual "call setup(), then loop() forever", but of course can
// be whatever you want.

__attribute__((constructor)) void premain() {
    init();
}

int main(void) {
    setup();

    while (true) {
        loop();
    }

    return 0;
}


/**
 * The Print task function.
 * It waits until a sensor value has been put in the queue,
 * and prints its value on the uart port.
 * \param pvParameter NULL is passed as parameter.
 */
static void vPrintTask(void* pvParameters)
{
    struct PMessage *pmsg;


    
    /* Infinite loop */
    while (1)
    {
        /* Wait until an element is received from the queue */
        if (xQueueReceive(xQueuePrint, &pmsg, portMAX_DELAY))
        {
            //samplecount++;
            /* Print the result on the uart port */
            SerialUSB.println(pmsg->data);

        }
    }
}

/**
 * The sensor measurement task function.
 * It reads the value from the sensor and puts it on the queue
 * \param pvParameters NULL is passed, unused here.
 */
static void vPositionADCTask(void* pvParameters)
{
    portTickType xLastWakeTime = xTaskGetTickCount();
    
    /* The sample period is 1000 ticks, about 1s */
    const portTickType xWakePeriod = 10;
    
    /* Infinite loop */
    while(1)
    {
        /* Read the sensor */
        //uint16_t msb = analogRead(sensor_pin);
        uint16 msb = adc_read_channels();
        
        /* Put the read value on the queue */
        xQueueSendToBack(xQueuePos, &msb, 0);
        
        /* Block until xWakePeriod(=1000) ticks since previous call */
        vTaskDelayUntil(&xLastWakeTime, xWakePeriod);
    }
}

int tval;

static void vPIDTask(void* pvParameters)
{
    uint16_t positionValue;
    struct PMessage *pxMessage;
  
    //portTickType xLastWakeTime = xTaskGetTickCount();
    
    /* The sample period is 10 ticks, about 10ms */
    //const portTickType xWakePeriod = 10;
    
    /* Infinite loop */
    while(1)
    {
      /* Wait until an element is received from the queue */
      /* If no new position is received, then the PID is never update! */
        if (xQueueReceive(xQueuePos, &positionValue, portMAX_DELAY))
        {
        	 pxMessage = & xMessage;

            //SerialUSB.print("Calculating PID: ");
            int32_t pwm = pidPositionToPwm(positionValue);
            //SerialUSB.println(pwm);

            //sprintf(pxMessage->data, "Got Sample: %d\n", tval);// %d : pwm %d", (int)positionValue, (int)pwm);
            ////xQueueSend( xQueuePrint, ( void * ) &pxMessage, ( portTickType ) 0 );
            //SerialUSB.println(tval);

            pwm_update(positionValue, pwm);
        }
      /* Block until xWakePeriod(=1000) ticks since previous call */
        //vTaskDelayUntil(&xLastWakeTime, xWakePeriod);
    }
}
// vim:cin:ai:sts=4 sw=4
