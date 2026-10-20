#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "Fork failed" << std::endl;
        return 1;
    }

    if (pid == 0) {
        std::cout << "Child process" << std::endl;
        return 0;
    }

    int status = 0;
    std::cout << "Parent process" << std::endl;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
        std::cout << "Child process exited with status: " << WEXITSTATUS(status) << std::endl;
    }

    return 0;
}