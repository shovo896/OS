#include<iostream> 
#include<sys.types.h>
#include<sys/wait.h> 
#include<unistd.h>
#include<cstdlib>

int main(){ 
    pid_t pid = fork();
    if(pid < 0){
        std::cerr << "Fork failed" << std::endl;
        return 1;
    }
    if(pid == 0){
        // Child process
        std::cout << "Child process" << std::endl;
    } else {
        // Parent process
        std::cout << "Parent process" << std::endl;
        int status;
        wait(&status);
    }
    return 0;
    int status= 0 ; 
    waitpid(pid, &status, 0); 
    std::cout << "Child process exited with status: " << status << std::endl;
    return 0;
}