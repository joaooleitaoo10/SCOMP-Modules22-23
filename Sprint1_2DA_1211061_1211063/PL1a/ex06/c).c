void main() {
    int i;
    int status;

    pid_t filho;

    for (i = 0; i < 4; i++) {
        if ((filho = fork()) == 0) {
            sleep(1); /*sleep(): unistd.h*/
            exit(0); //Exactly 4 child processes will be created.
        }else if(filho % 2 == 0){
            waitpid(filho, &status, 0); //The parent process waits for the child processes with even PID.
        }
    }

    printf("This is the end.\n").
} 