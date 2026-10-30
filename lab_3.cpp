#include<iostream> 
#include<sys/types.h>
#include<sys/wait.h>
#include<vector>
#include<algorithm>

using namespace std;
void run (vector<pair><int,int>> &v) {
    for (auto &p : v) {
        pid_t pid = fork();
        if (pid < 0) {
            cerr << "Fork failed" << endl;
            exit(1);
        }
        if (pid == 0) {
            cout << "Child process: " << p.first << ", " << p.second << endl;
            exit(0);
        }
    }

    for (size_t i = 0; i < v.size(); ++i) {
        int status;
        wait(&status);
        if (WIFEXITED(status)) {
            cout << "Child process exited with status: " << WEXITSTATUS(status) << endl;
        }
    }
}