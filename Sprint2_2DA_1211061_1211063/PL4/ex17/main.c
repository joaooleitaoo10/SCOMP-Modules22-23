#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/shm.h>

#define SEM_ALARM_SENS "/sem_alarm_sens"
#define SEM_MEASUREMENTS "/sem_measurements"
#define SEM_NOTIFIER "/sem_notifier"
#define SENSORSDATA_SHM_NAME "/sensdata_shm"
#define N_SENSORS 5
#define N_MEASUREMENTS 6
#define ALARM_VALUE 50

// Padrões: Sincronização de execução

typedef struct
{
    int totalMeasurements;
    int measurements[N_MEASUREMENTS * N_SENSORS];
    int nSensorsInAlarm;
    char lastAlarmMessage[100];
} SensorsData;

SensorsData *sensorsData;
sem_t *sem_alarm_sens;
sem_t *sem_measurements;
sem_t *sem_notifier;

void initializeSharedMemory()
{
    // Create the shared memory for the tickets
    int data_size = sizeof(SensorsData);
    int fd = shm_open(SENSORSDATA_SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        printf("Error: Couldn't create the shared memory area\n");
        exit(1);
    }
    ftruncate(fd, data_size);
    if (ftruncate(fd, data_size) == -1)
    {
        printf("Error: Couldn't truncate shared memory area!\n");
        exit(1);
    }
    sensorsData = (SensorsData *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (sensorsData == MAP_FAILED)
    {
        printf("Error: Couldn't map shared memory area!\n");
        exit(1);
    }
    sensorsData->nSensorsInAlarm = 0;
    sensorsData->totalMeasurements = 0;
}

void initializeSemaphores()
{
    // Creating the semaphores
    sem_alarm_sens = sem_open(SEM_ALARM_SENS, O_CREAT | O_EXCL, 0644, 1);
    if (sem_alarm_sens == SEM_FAILED)
    {
        printf("Error: Couldn't create the alarm semaphore!\n");
        exit(1);
    }
    sem_measurements = sem_open(SEM_MEASUREMENTS, O_CREAT | O_EXCL, 0644, 1);
    if (sem_measurements == SEM_FAILED)
    {
        printf("Error: Couldn't create the measurements semaphore!\n");
        exit(1);
    }
    sem_notifier = sem_open(SEM_NOTIFIER, O_CREAT | O_EXCL, 0644, 0);
    if (sem_notifier == SEM_FAILED)
    {
        printf("Error: Couldn't create the notifier measure semaphore!\n");
        exit(1);
    }
}

void sensorExecution(int sensorId)
{
    int isInAlarmState = 0;
    int measurements[N_MEASUREMENTS];

    for (int i = 0; i < N_MEASUREMENTS; i++)
    {
        srand(time(NULL) * getpid() * rand() / 3 * getpid() * rand() * rand());
        // Generate random values for the measurements, from 0 to 100
        int generatedValue = rand() % 101;
        measurements[i] = generatedValue;

        // Wait for the measurements to be available
        sem_wait(sem_measurements);

        // Add the measurement to the shared memory
        sensorsData->measurements[sensorsData->totalMeasurements] = generatedValue;

        // Increment the total number of measurements
        sensorsData->totalMeasurements++;

        // Check if sensor already has 3 measurements, if so, check if it is in alarm state and if it should not be anymore
        if (i >= 2 && isInAlarmState == 1)
        {
            // Check the last 2 measurements were below ALARM_VALUE
            if (measurements[i] < ALARM_VALUE && measurements[i - 1] < ALARM_VALUE)
            {
                // Sensor is no longer in alarm state
                isInAlarmState = 0;

                // Decrement the number of sensors in alarm
                sensorsData->nSensorsInAlarm--;

                // Set the alarm message
                sprintf(sensorsData->lastAlarmMessage,
                        "Sensor %d is no longer in alarm state\nLast two measures: %d;%d", sensorId, measurements[i - 1], generatedValue);
            }
        }
        else if (isInAlarmState == 0 && generatedValue >= ALARM_VALUE)
        {
            // Set the sensor to be in alarm state
            isInAlarmState = 1;

            // Increment the number of sensors in alarm
            sensorsData->nSensorsInAlarm++;

            // Set the alarm message
            sprintf(sensorsData->lastAlarmMessage,
                    "Sensor %d is now in alarm state!", sensorId);
        }
        // Notify the controller that there is a new measurement
        sem_post(sem_notifier);

        sleep(1);
    }
}

void sensorApp()
{
    // Create the sensors
    for (int i = 0; i < N_SENSORS; i++)
    {
        pid_t pid = fork();
        if (pid == -1)
        {
            printf("Error: Couldn't create the sensor %d!\n", i);
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            sensorExecution(i + 1);
            exit(EXIT_SUCCESS);
        }
    }
}

void waitSensors()
{
    // Wait for the sensors to finish
    for (int i = 0; i < N_SENSORS; i++)
    {
        wait(NULL);
    }
}

void cleanUp()
{
    // Close the semaphores
    sem_close(sem_alarm_sens);
    sem_close(sem_measurements);
    sem_close(sem_notifier);

    // Unlink the semaphores
    sem_unlink(SEM_ALARM_SENS);
    sem_unlink(SEM_MEASUREMENTS);
    sem_unlink(SEM_NOTIFIER);

    // Unlink the shared memory
    shm_unlink(SENSORSDATA_SHM_NAME);
}

int main()
{
    initializeSharedMemory();
    initializeSemaphores();

    // Execute the sensors
    sensorApp();

    int previousNsensorsInAlarmState = 0;
    // Wait for the notifications from the sensors
    for (int i = 0; i < N_SENSORS * N_MEASUREMENTS; i++)
    {
        // Wait to receive the notification
        sem_wait(sem_notifier);

        // Print the received value
        printf("\033[0;32mValue received: %d\n\e[0;37m", sensorsData->measurements[i]);

        // Check if there was a change in the number of sensors in alarm state
        if (previousNsensorsInAlarmState != sensorsData->nSensorsInAlarm)
        {
            // Print the last alarm message
            printf("\033[0;31m%s\n\e[0;37m", sensorsData->lastAlarmMessage);

            // Print the number of sensors in alarm state
            printf("\033[0;31mNumber of sensors in alarm state: %d\n\e[0;37m", sensorsData->nSensorsInAlarm);
            previousNsensorsInAlarmState = sensorsData->nSensorsInAlarm;
        }
        sem_post(sem_measurements);
    }

    // Print the total number of measurements
    printf("\nTotal number of measurements: %d\n", sensorsData->totalMeasurements);

    // Wait for the sensors to finish
    waitSensors();

    // Clean up the semaphores and shared memory
    cleanUp();

    return 0;
}