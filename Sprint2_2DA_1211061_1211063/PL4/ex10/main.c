#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define TRAIN_CAPACITY 20
#define MOVE_TROUGH_DOOR_TIME_MS 200
#define INIT_PASSENGERS 15
#define WANT_TO_LEAVE_AT_A 5
#define WANT_TO_ENTER_AT_A 20
#define WANT_TO_LEAVE_AT_B 10
#define WANT_TO_ENTER_AT_B 5

typedef struct
{
    int n_passengers;
    int departured;
    int want_to_leave;
} TrainData;

typedef struct
{
    int inits;
} Init;

// Padrões: Sincronização de Processos / Barreira / Acesso exclusivo a recursos partilhados

// Semaphore responsible for controlling the access to the train data related to the number of passengers
sem_t *sem_train_data;
// Semaphore responsible for controlling the access to the train data related to the passengers that want to leave
sem_t *sem_want_to_leave;
// Semaphore that will be used as sort of a barrier to make sure that the train only leaves when it is full
sem_t *sem_ride;
// Array of semaphores that will be used to control the access to the doors
sem_t *door_mutex[3];
// Semaphore to wait for the arrival of the train at the station
sem_t *sem_train_arrival;
// Semaphore to wait for everyone to leave the train
sem_t *sem_all_passengers_left;
// Semaphore responsible for controlling the access to the initialization data
sem_t *sem_init_data;

// Train passengers
TrainData *train_data;
// Initialization data
Init *init_data;

void send_train()
{
    // Wait for all the doors to be available, to ensure that no passenger is entering or leaving the train
    for (int i = 0; i < 3; i++)
    {
        sem_wait(door_mutex[i]);
    }

    train_data->departured = 1;

    printf("\033[0;33m### The train is full! It is now departing! ###\n\033[0;37m");

    sem_post(sem_ride);
    sem_post(sem_train_data);

    // Make the doors available again
    for (int i = 0; i < 3; i++)
    {
        sem_post(door_mutex[i]);
    }
}

// Simulate the passenger entering the train
int passenger_enter_train(pid_t passenger_id)
{
    // Increase the number of passengers in the train and check if the train is full
    sem_wait(sem_train_data);
    if (train_data->n_passengers == TRAIN_CAPACITY && train_data->departured == 0 && train_data->want_to_leave <= 0)
    {
        send_train();
        exit(EXIT_SUCCESS);
    }
    // This scenario happens when the train is full but there are still passengers that want to leave
    else if (train_data->n_passengers == TRAIN_CAPACITY && train_data->departured == 0)
    {
        sem_post(sem_train_data);
        return -1;
    }
    else if (train_data->n_passengers == TRAIN_CAPACITY && train_data->departured == 1)
    {
        sem_post(sem_train_data);
        exit(EXIT_SUCCESS);
    }
    train_data->n_passengers++;

    // Randomly select a door to enter from 0 to 2
    srand(time(NULL) * passenger_id);
    int door = rand() % 3;

    // Wait until the selected door is available
    sem_wait(door_mutex[door]);
    // Simulate the time it takes to move through the door
    usleep(MOVE_TROUGH_DOOR_TIME_MS);

    // Print the passenger entering the train
    printf("\033[0;32mPassenger %d entered the train through Door %d.\n\033[0;37m", passenger_id, door + 1);
    sem_post(sem_train_data);
    // Release the selected door
    sem_post(door_mutex[door]);

    // Return the selected door
    return door;
}

// Simulate the passenger leaving the train
void passenger_leave_train(pid_t passenger_id, int door)
{
    // Wait for the passenger door to be available
    sem_wait(door_mutex[door]);

    // Simulate the time it takes to move through the door
    usleep(MOVE_TROUGH_DOOR_TIME_MS);

    printf("\033[0;35mPassenger %d just left the train! Door: %d\n\033[0;37m", passenger_id, door + 1);

    // Release the selected door
    sem_post(door_mutex[door]);

    // Decrease the number of passengers in the train
    sem_wait(sem_train_data);
    train_data->n_passengers--;
    sem_post(sem_train_data);
}

void simulate_train_trip()
{
    srand(time(NULL)); // Seed the random number generator

    // Initiliaze 15 passengers (Create 15 processes)
    int door;
    pid_t passenger;
    for (int i = 0; i < INIT_PASSENGERS; i++)
    {
        passenger = fork();
        if (passenger == 0)
        {
            door = passenger_enter_train(getpid());
            sem_wait(sem_init_data);
            init_data->inits++;
            if (init_data->inits == INIT_PASSENGERS)
            {
                sem_post(sem_ride);
            }
            sem_post(sem_init_data);
            break;
        }
    }

    // Passengers that were already in the train
    if (passenger == 0)
    {
        // Wait for the train to arrive at Station A --- STATION A
        sem_wait(sem_train_arrival);
        // Unlock the train arrival semaphore so that other passengers know
        sem_post(sem_train_arrival);

        // Wait for the information about the number of passengers that want to leave the train is available
        sem_wait(sem_want_to_leave);
        if (train_data->want_to_leave > 0)
        {
            // If there are passengers that want to leave the train, then leave the train
            passenger_leave_train(getpid(), door);
            // Decrease the number of passengers that want to leave the train
            train_data->want_to_leave--;
            // Unlock the semaphore to make the information available to other passengers
            sem_post(sem_want_to_leave);
            exit(EXIT_SUCCESS);
        }
        else if (train_data->want_to_leave == 0)
        {
            // If there are no passengers that want to leave the train, then inform that all the passengers that wanted left the train
            sem_post(sem_all_passengers_left);
        }
        // This decrease in not real, it is used to assure the semaphore of the passengers that left is only upped 1 time, this value will be updated later
        train_data->want_to_leave--;
        // Unlock the semaphore to make the information available to other passengers
        sem_post(sem_want_to_leave);

        // Wait for the train to arrive at Station B --- STATION B
        sem_wait(sem_train_arrival);
        // Unlock the train arrival semaphore so that other passengers know
        sem_post(sem_train_arrival);

        // Wait for the information about the number of passengers that want to leave the train is available
        sem_wait(sem_want_to_leave);
        if (train_data->want_to_leave > 0)
        {
            // If there are passengers that want to leave the train, then leave the train
            passenger_leave_train(getpid(), door);
            // Decrease the number of passengers that want to leave the train
            train_data->want_to_leave--;
            // Unlock the semaphore to make the information available to other passengers
            sem_post(sem_want_to_leave);
            exit(EXIT_SUCCESS);
        }
        else if (train_data->want_to_leave == 0)
        {
            // If there are no passengers that want to leave the train, then inform that all the passengers that wanted left the train
            sem_post(sem_all_passengers_left);
        }
        // This decrease in not real, it is used to assure the semaphore of the passengers that left is only upped 1 time, this value will be updated later
        train_data->want_to_leave--;
        // Unlock the semaphore to make the information available to other passengers
        sem_post(sem_want_to_leave);
        exit(EXIT_SUCCESS);
    }
    else
    {
        // Wait for all the INIT passengers to enter the train
        sem_wait(sem_ride);

        // The parent process will be the train and informs it just arrived at Station A
        printf("\033[0;33m### The train just arrived at Station A! ###\n\033[0;37m");
        train_data->want_to_leave = WANT_TO_LEAVE_AT_A;
        sem_post(sem_train_arrival);

        // Wait for the passengers to leave the train
        sem_wait(sem_all_passengers_left);

        // Set the semaphore of the train arrival to 0
        sem_wait(sem_train_arrival);

        // Make the passengers that want to enter the train try to enter
        for (int i = 0; i < WANT_TO_ENTER_AT_A; i++)
        {
            passenger = fork();
            if (passenger == 0)
            {
                break;
            }
        }

        if (passenger == 0)
        {
            do
            {
                // The passenger tries to enter the train while it has not departed and there are still passengers leaving
                door = passenger_enter_train(getpid());
            } while (door == -1);

            // Wait for the train to arrive at Station B --- STATION B
            sem_wait(sem_train_arrival);
            sem_post(sem_train_arrival);

            // Wait for the information about the number of passengers that want to leave the train is available
            sem_wait(sem_want_to_leave);
            if (train_data->want_to_leave > 0)
            {
                // If there are passengers that want to leave the train, then leave the train
                passenger_leave_train(getpid(), door);

                // Decrease the number of passengers that want to leave the train
                train_data->want_to_leave--;
                // Unlock the semaphore to make the information available to other passengers
                sem_post(sem_want_to_leave);
                exit(EXIT_SUCCESS);
            }
            else if (train_data->want_to_leave == 0)
            {
                // If there are no passengers that want to leave the train, then inform that all the passengers that wanted left the train
                sem_post(sem_all_passengers_left);
            }
            // This decrease in not real, it is used to assure the semaphore of the passengers that left is only upped 1 time, this value will be updated later
            train_data->want_to_leave--;
            // Inform that all the passengers that wanted left the train
            sem_post(sem_all_passengers_left);
            // Unlock the semaphore to make the information available to other passengers
            sem_post(sem_want_to_leave);
            exit(EXIT_SUCCESS);
        }
        else
        {
            // Wait for the train to be able to leave
            sem_wait(sem_ride);

            printf("### Traveling... ###\n");
            // Train travels to Station B
            sleep(2);
            // train_data->departured = 0;
            printf("\033[0;33m### The train just arrived at Station B! ###\n\033[0;37m");

            train_data->want_to_leave = WANT_TO_LEAVE_AT_B;
            sem_post(sem_train_arrival);
            sem_wait(sem_all_passengers_left);

            // Make the passengers that want to enter the train try to enter
            for (int i = 0; i < WANT_TO_ENTER_AT_B; i++)
            {
                passenger = fork();
                if (passenger == 0)
                {
                    break;
                }
            }

            if (passenger == 0)
            {
                do
                {
                    // The passenger tries to enter the train while it has not departed and there are still passengers leaving
                    door = passenger_enter_train(getpid());
                } while (door == -1);
                exit(EXIT_SUCCESS);
            }
        }

        // Wait for all the passengers created to enter to cess the train operation
        for (int i = 0; i < WANT_TO_ENTER_AT_B + WANT_TO_ENTER_AT_A + INIT_PASSENGERS; i++)
        {
            wait(NULL);
        }

        // Inform that all the operations are done
        printf("\033[0;33m### All train operations completed! ###\n\033[0;37m");
    }
}

int main()
{
    // Create the semaphores
    sem_want_to_leave = sem_open("sem_want_to_leave", O_CREAT | O_EXCL, 0644, 1);
    sem_train_data = sem_open("sem_train_data", O_CREAT | O_EXCL, 0644, 1);
    sem_ride = sem_open("sem_ride", O_CREAT | O_EXCL, 0644, 0);
    sem_train_arrival = sem_open("sem_train_arrival", O_CREAT | O_EXCL, 0644, 0);
    sem_all_passengers_left = sem_open("sem_all_passengers_left", O_CREAT | O_EXCL, 0644, 0);
    sem_init_data = sem_open("sem_init_data", O_CREAT | O_EXCL, 0644, 1);
    for (int i = 0; i < 3; i++)
    {
        door_mutex[i] = sem_open("door_mutex" + i, O_CREAT | O_EXCL, 0644, 1);
    }

    // Create the shared memory
    int data_size = sizeof(TrainData);
    int fd = shm_open("/shm_data", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(fd, data_size) == -1)
    {
        perror("Error calling ftruncate()");
        exit(EXIT_FAILURE);
    }
    train_data = (TrainData *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (train_data == MAP_FAILED)
    {
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }
    train_data->n_passengers = 0;
    train_data->departured = 0;

    data_size = sizeof(Init);
    fd = shm_open("/shm_init", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(fd, data_size) == -1)
    {
        perror("Error calling ftruncate()");
        exit(EXIT_FAILURE);
    }
    init_data = (Init *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (init_data == MAP_FAILED)
    {
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }
    init_data->inits = 0;
    // END Create shared memory area

    simulate_train_trip();

    // Close and unlink the semaphores
    sem_close(sem_train_data);
    sem_unlink("sem_train_data");
    sem_close(sem_ride);
    sem_unlink("sem_ride");
    sem_close(sem_train_arrival);
    sem_unlink("sem_train_arrival");
    sem_close(sem_want_to_leave);
    sem_unlink("sem_want_to_leave");
    sem_close(sem_all_passengers_left);
    sem_unlink("sem_all_passengers_left");
    sem_close(sem_init_data);
    sem_unlink("sem_init_data");
    for (int i = 0; i < 3; i++)
    {
        sem_close(door_mutex[i]);
        sem_unlink("door_mutex" + i);
    }

    // Close and unlink the shared memory
    close(fd);
    shm_unlink("/shm_data");
    shm_unlink("/shm_init");

    return 0;
}

// Seria possível fazer com que os passageiros fossem entrando e saindo ao mesmo tempo, mas dada a complexidade de tal implementação,
// optou-se por fazer de forma sequencial
