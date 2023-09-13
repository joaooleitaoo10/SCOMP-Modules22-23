| Signal | Number | Action |
|------------------|------------------|------------------|
| SIGHUP | 1 | Reports that the user session has terminated, usually when the process' controlling terminal is closed. |
| SIGINT | 2 | Terminates the process, by keyboard input CTRL-C. |
| SIGQUIT | 3 | Terminates the proces and generates a core dump. |
| SIGILL | 4 | Indicates that an illegal instruction was executed by the process, and the process is terminated. |
| SIGABRT | 6 | Indicates that there was an abonormal termination and generates a core dump. |
| SIGFPE | 8 | Terminates the process, happens when there is a floating point exception. |
| SIGKILL | 9 | Terminates the process immediately, without letting the process peform a clean up of its own state, or execute any code. |
| SIGUSR1 | 10 | User-defined signal, its action depends on the handler. |
| SIGSEGV | 11 | Terminates the process when there is an invalid memory reference. |
| SIGUSR2 | 12 | User-defined signal, its action depends on the handler. |
| SIGPIPE | 13 | Terminates the process when it attempts to write to a pipe with no readers. |
| SIGALRM | 14 | Terminates the process when a timer set by the process or the system using the alarm. |
| SIGTERM | 15 | Terminates the process gracefully when a termination signal is sent to it, allowing it to perform cleanup actions before exiting. |
| SIGCHLD | 17 | Notifies a parent process when one of its child processes terminates. |
| SIGCONT | 18 | Resumes the execution of a process if stopped. |
| SIGSTOP | 19 | Stops the execution of a process, by kerboard input CTRL-Z. |
| SIGTSTP | 20 | Stops process issued from tty. |
| SIGTTIN | 21 | Stops process, when it requires input. |
| SIGTTOU | 22 | Stops process, when it requires output. |
