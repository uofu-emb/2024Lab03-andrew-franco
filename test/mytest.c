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

void test_variable_assignment()
{
    int x = 1;
    TEST_ASSERT_TRUE_MESSAGE(x == 1,"Variable assignment failed.");
}

void test_multiplication(void)
{
    int x = 30;
    int y = 6;
    int z = x / y;
    TEST_ASSERT_TRUE_MESSAGE(z == 5, "Multiplication of two integers returned incorrect value.");
}

int main (void)
{
    stdio_init_all();
    sleep_ms(5000); // Give time for TTY to attach.
    printf("Start tests\n");
    UNITY_BEGIN();
    RUN_TEST(test_variable_assignment);
    RUN_TEST(test_multiplication);
    sleep_ms(5000);
    return UNITY_END();
}
