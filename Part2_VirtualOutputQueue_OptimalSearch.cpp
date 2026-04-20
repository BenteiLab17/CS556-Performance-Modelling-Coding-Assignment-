#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

struct Packet {
    string id;
    int src, dest, arrival;
};

// Global trace
vector<Packet> trace = {
    {"p1", 0, 0, 0}, {"p2", 0, 1, 0}, {"p3", 1, 0, 0}, {"p4", 1, 2, 0}, {"p5", 2, 0, 0},
    {"p6", 0, 2, 1}, {"p7", 2, 1, 1}, {"p8", 1, 1, 2}, {"p9", 2, 2, 2},
    {"p10", 0, 1, 3}, {"p11", 1, 0, 3}, {"p12", 2, 1, 3},
    {"p13", 0, 0, 4}, {"p14", 1, 2, 4}, {"p15", 2, 2, 4},
    {"p16", 0, 2, 5}, {"p17", 1, 1, 5}, {"p18", 2, 0, 5}
};

int minServiceTime = 99;
vector<string> bestSchedule;
vector<int> bestBacklogHistory; // To store the backlog of the optimal path

// Check if all VOQs are empty and all packets arrived
bool isFinished(vector<Packet> voqs[3][3], int time) {
    if (time <= 5) return false; 
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (!voqs[i][j].empty()) return false;
        }
    }
    return true;
}

void solve(vector<Packet> voqs[3][3], int time, vector<string> currentLog, vector<int> currentBacklogHistory) {
    // Pruning
    if (time >= minServiceTime) return;

    // Add arriving packets to VOQs
    for (auto &p : trace) {
        if (p.arrival == time) {
            voqs[p.src][p.dest].push_back(p);
        }
    }

    // Check if finished
    if (isFinished(voqs, time)) {
        minServiceTime = time;
        bestSchedule = currentLog;
        // The switch is empty at the end of the last slot
        currentBacklogHistory.push_back(0); 
        bestBacklogHistory = currentBacklogHistory;
        return;
    }

    // Calculate current backlog before transfers
    int currentBacklog = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            currentBacklog += voqs[i][j].size();
        }
    }

    // Generate all 6 possible bipartite matchings
    vector<int> matching = {0, 1, 2};
    bool possibleToMove = false;

    do {
        vector<Packet> nextVoqs[3][3];
        for(int i=0; i<3; i++) for(int j=0; j<3; j++) nextVoqs[i][j] = voqs[i][j];

        string slotStep = "T=" + to_string(time) + ": ";
        bool movedInThisMatching = false;
        int sentThisSlot = 0;

        for (int i = 0; i < 3; i++) {
            int out = matching[i];
            if (!nextVoqs[i][out].empty()) {
                slotStep += "[" + nextVoqs[i][out].front().id + "] ";
                nextVoqs[i][out].erase(nextVoqs[i][out].begin());
                movedInThisMatching = true;
                possibleToMove = true;
                sentThisSlot++;
            }
        }

        if (movedInThisMatching) {
            vector<string> nextLog = currentLog;
            nextLog.push_back(slotStep);
            
            // Record backlog for this path (Backlog AFTER transfers in slot t)
            vector<int> nextBacklogHistory = currentBacklogHistory;
            nextBacklogHistory.push_back(currentBacklog - sentThisSlot);
            
            solve(nextVoqs, time + 1, nextLog, nextBacklogHistory);
        }

    } while (next_permutation(matching.begin(), matching.end()));

    // Idle slot if no moves possible
    if (!possibleToMove) {
        vector<string> nextLog = currentLog;
        nextLog.push_back("T=" + to_string(time) + ": Idle");
        
        vector<int> nextBacklogHistory = currentBacklogHistory;
        nextBacklogHistory.push_back(currentBacklog);
        
        solve(voqs, time + 1, nextLog, nextBacklogHistory);
    }
}

int main() {
    vector<Packet> initialVoqs[3][3];
    vector<string> log;
    vector<int> backlogHistory;

    cout << "Running Exhaustive Simulation..." << endl;
    
    solve(initialVoqs, 0, log, backlogHistory);

    cout << "--- OPTIMAL VOQ RESULTS ---" << endl;
    for (const string& step : bestSchedule) {
        cout << step << endl;
    }
    cout << "Best Total Service Time: " << minServiceTime << endl;

    // Print Backlog
    cout << "optimal_backlog = [";
    for(size_t i = 0; i < bestBacklogHistory.size(); i++) {
        cout << bestBacklogHistory[i] << (i == bestBacklogHistory.size()-1 ? "" : ", ");
    }
    cout << "]" << endl;

    return 0;
}