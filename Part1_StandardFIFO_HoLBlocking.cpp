#include <iostream>
#include <vector>
#include <queue>
#include <string>

using namespace std;

struct Packet {
    string id;
    int src;
    int dest;
    int arrival;
};

int main() {
    vector<Packet> trace = {
        {"p1", 0, 0, 0}, {"p2", 0, 1, 0}, {"p3", 1, 0, 0}, {"p4", 1, 2, 0}, {"p5", 2, 0, 0},
        {"p6", 0, 2, 1}, {"p7", 2, 1, 1},
        {"p8", 1, 1, 2}, {"p9", 2, 2, 2},
        {"p10", 0, 1, 3}, {"p11", 1, 0, 3}, {"p12", 2, 1, 3},
        {"p13", 0, 0, 4}, {"p14", 1, 2, 4}, {"p15", 2, 2, 4},
        {"p16", 0, 2, 5}, {"p17", 1, 1, 5}, {"p18", 2, 0, 5}
    };

    queue<Packet> inputs[3]; 
    vector<int> backlog_history; // To store the backlog at each time slot
    int time = 0;
    int packetsSent = 0;
    int totalPackets = trace.size();

    cout << "Step-by-Step Log:" << endl;

    while (packetsSent < totalPackets) {
        // A. Handle Arrivals
        for (int i = 0; i < totalPackets; i++) {
            if (trace[i].arrival == time) {
                inputs[trace[i].src].push(trace[i]);
            }
        }

        // B. Scheduling Logic (FIFO + Tie-breaking: I0 > I1 > I2)
        bool outputBusy[3] = {false, false, false};
        int winnerForInput[3] = {-1, -1, -1};

        for (int i = 0; i < 3; i++) {
            if (!inputs[i].empty()) {
                int targetOutput = inputs[i].front().dest;
                if (!outputBusy[targetOutput]) {
                    outputBusy[targetOutput] = true;
                    winnerForInput[i] = targetOutput;
                }
            }
        }

        // C. Execute the transfers
        cout << "T=" << time << ": ";
        for (int i = 0; i < 3; i++) {
            if (winnerForInput[i] != -1) {
                Packet p = inputs[i].front();
                cout << "[" << p.id << ": I" << i << "->O" << p.dest << "] ";
                inputs[i].pop();
                packetsSent++;
            }
        }

        // D. Backlog Calculation
        // Count all packets currently waiting in all input queues
        int currentBacklog = 0;
        for (int i = 0; i < 3; i++) {
            currentBacklog += inputs[i].size();
        }
        backlog_history.push_back(currentBacklog);

        // E. HoL Blocking Detection
        for (int i = 0; i < 3; i++) {
            if (!inputs[i].empty() && winnerForInput[i] == -1) {
                if (inputs[i].size() > 1) {
                    Packet head = inputs[i].front();
                    inputs[i].pop(); 
                    Packet second = inputs[i].front();
                    if (!outputBusy[second.dest]) {
                        cout << "(HoL at I" << i << " for " << second.id << ") ";
                    }
                    queue<Packet> temp;
                    temp.push(head);
                    while(!inputs[i].empty()) { temp.push(inputs[i].front()); inputs[i].pop(); }
                    inputs[i] = temp;
                }
            }
        }

        cout << endl;
        time++;
        if (time > 50) break;
    }

    // F. Final State
    backlog_history.push_back(0); // Ends at 0 when empty

    cout << "--------------------------------" << endl;
    cout << "Total Service Time: " << time << endl;
    
    // Backlog Print
    cout << "fifo_backlog = [";
    for(int i = 0; i < backlog_history.size(); i++) {
        cout << backlog_history[i] << (i == backlog_history.size()-1 ? "" : ", ");
    }
    cout << "]" << endl;

    return 0;
}