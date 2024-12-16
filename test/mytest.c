#include <stdio.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <pico/cyw43_arch.h>
#include <unity.h>
#include "loop.h"

//This is the first test thread and therefore it has the hoighest priorityu. FreeRTOS higher number delinites higher prioity
#define TEST_RUNNER_PRIORITY ( tskIDLE_PRIORITY + 5UL)

void setUp(void) {}

void tearDown(void) {}

//Activity 2 Unit test for thread 

// 
void test_for_blocked_loop(void)
{
    SemaphoreHandle_t semaphore = xSemaphoreCreateCounting(1, 1); //Intitate the Semaphore with the timer embedded
    int counter = 0; //Start the timer global variable that also exhists in loop.h
    xSemaphoreTake(semaphore, portMAX_DELAY); //take the semaphore


    int output = do_loop(semaphore, &counter, "test", 10);
    //Now we run the two UNITY test that test if the Semaphores release over a blocked loop
    TEST_ASSERT_EQUAL_INT(pdFALSE, output);
    TEST_ASSERT_EQUAL_INT(0, counter);

}

void test_for_looping(void)
{
    SemaphoreHandle_t semaphore = xSemaphoreCreateCounting(1, 1); //Intitate the Semaphore with the timer embedded
    int counter = 0; //Start the timer global variable that also exhists in loop.h

    int output = do_loop(semaphore, &counter, "test", 10);

    //Now we run the two UNITY test that test if the Semaphores release over a running loop
    TEST_ASSERT_EQUAL_INT(pdFALSE, output);
    TEST_ASSERT_EQUAL_INT(1, counter);
}


//Activity 3 
//First we need to define each stack and each thread priority.

#define BARONESS_STACK configMINIMAL_STACK_SIZE
#define BARONESS_PRIORITY ( TEST_RUNNER_PRIORITY - 1UL)
#define BARON_STACK configMINIMAL_STACK_SIZE
#define BARON_PRIORITY ( TEST_RUNNER_PRIORITY - 1UL)

void test_lock(void) 
{

    //Define tasks and semaphores
    TaskHandle_t Baroness_thread, Baron_thread, Emperor_thread;
    SemaphoreHandle_t Baroness = xSemaphoreCreateCounting(1, 1);
    SemaphoreHandle_t Baron = xSemaphoreCreateCounting(1, 1);

    printf("- start of thread\n");

    struct DeadlockArgs Baroness_args = {Baroness, Baron, 0, 'a'};
    struct DeadlockArgs Baron_args = {Baron, Baroness, 7, 'b'};

    BaseType_t Baroness_status =
        xTaskCreate(deadlock, "Baroness", BARONESS_STACK,
            (void *)&Baroness_args, BARONESS_PRIORITY, &Baroness_thread);
    BaseType_t Baron_status =
        xTaskCreate(deadlock, "Baron", BARON_STACK,
            (void *)&Baron_args, BARON_PRIORITY, &Baron_thread);   

    printf("- New thread run\n"); //Finished check of deadlocking the threads and seeing if they are capable of releasing themselves

    vTaskDelay(3000);//Pause before running next task


    printf("- Paused test_for_looping for 3000 ticks\n");

    TEST_ASSERT_EQUAL_INT(uxSemaphoreGetCount(Baroness), 0);
    TEST_ASSERT_EQUAL_INT(uxSemaphoreGetCount(Baron), 0);

    TEST_ASSERT_EQUAL_INT(2, Baroness_args.counter);
    TEST_ASSERT_EQUAL_INT(9,Baron_args.counter);
    printf("- Killing threads\n");
    vTaskDelete(Baroness_thread);
    vTaskDelete(Baron_thread);
    printf("- Killed threads");
    
}

//Activity 4

void test_orphaned(void)
{
    int counter = 1;
    SemaphoreHandle_t semaphore = xSemaphoreCreateCounting(1, 1);

    int result = orphaned_lock(semaphore, 500, &counter);
    TEST_ASSERT_EQUAL_INT(2, counter);
    TEST_ASSERT_EQUAL_INT(pdTRUE, result);
    TEST_ASSERT_EQUAL_INT(1, uxSemaphoreGetCount(semaphore));

    result = orphaned_lock(semaphore, 500, &counter);
    TEST_ASSERT_EQUAL_INT(3, counter);
    TEST_ASSERT_EQUAL_INT(pdFALSE, result);
    TEST_ASSERT_EQUAL_INT(0, uxSemaphoreGetCount(semaphore));

    result = orphaned_lock(semaphore, 500, &counter);
    TEST_ASSERT_EQUAL_INT(3, counter);
    TEST_ASSERT_EQUAL_INT(pdFALSE, result);
    TEST_ASSERT_EQUAL_INT(0, uxSemaphoreGetCount(semaphore));

}

void test_unorphaned_lock(void)
{
    int counter = 1;
    SemaphoreHandle_t semaphore = xSemaphoreCreateCounting(1, 1);
    int result;
    result = unorphaned_lock(semaphore, 500, &counter);
    TEST_ASSERT_EQUAL_INT(2, counter);
    TEST_ASSERT_EQUAL_INT(pdTRUE, result);
    TEST_ASSERT_EQUAL_INT(1, uxSemaphoreGetCount(semaphore));
    result = unorphaned_lock(semaphore, 500, &counter);
    TEST_ASSERT_EQUAL_INT(3, counter);
    TEST_ASSERT_EQUAL_INT(pdTRUE, result);
    TEST_ASSERT_EQUAL_INT(1, uxSemaphoreGetCount(semaphore));
}


//Now we need to run each test in a runner thread

void runner_thread(__unused void *args)
{
    for(;;) {
        printf("Start tests\n");
        UNITY_BEGIN();
        RUN_TEST(test_for_blocked_loop);
        RUN_TEST(test_for_looping);
        RUN_TEST(test_lock);
        RUN_TEST(test_orphaned);
        RUN_TEST(test_unorphaned_lock);

        UNITY_END();
        sleep_ms(10000);
    }
}



int main (void)
{
    stdio_init_all();
    hard_assert(cyw43_arch_init() == PICO_OK);
    xTaskCreate(runner_thread, "TestRunner",
                    configMINIMAL_STACK_SIZE, NULL, TEST_RUNNER_PRIORITY, NULL);
    vTaskStartScheduler();
    return 0;

}
