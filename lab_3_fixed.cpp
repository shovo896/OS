#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <algorithm>
#include <utility>
#include <cstdlib>

using namespace std;

void run(vector<pair<int, int>>& v) {
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
        waitpid(v[i].first, &status, 0);
        if (WIFEXITED(status)) {
            cout << "Child process exited with status: " << WEXITSTATUS(status) << endl;
        }
    }
}

int main() {
    vector<pair<int, int>> values = {{1, 2}, {3, 4}, {5, 6}};
    run(values);
    return 0;
}