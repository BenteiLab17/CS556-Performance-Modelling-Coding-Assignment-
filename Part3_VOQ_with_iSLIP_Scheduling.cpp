#include <iostream>
#include <vector>
#include <string>
#include <queue>

using namespace std;

struct Packet {
    string id;
    int src, dest, arrival;
};

int main() {
    vector<Packet> trace = {
        {"p1", 0, 0, 0}, {"p2", 0, 1, 0}, {"p3", 1, 0, 0}, {"p4", 1, 2, 0}, {"p5", 2, 0, 0},
        {"p6", 0, 2, 1}, {"p7", 2, 1, 1}, {"p8", 1, 1, 2}, {"p9", 2, 2, 2},
        {"p10", 0, 1, 3}, {"p11", 1, 0, 3}, {"p12", 2, 1, 3},
        {"p13", 0, 0, 4}, {"p14", 1, 2, 4}, {"p15", 2, 2, 4},
        {"p16", 0, 2, 5}, {"p17", 1, 1, 5}, {"p18", 2, 0, 5}
    };

    queue<Packet> voqs[3][3];
    vector<int> backlog_history; // To store the backlog at each time slot
    int grant_ptr[3] = {0, 0, 0};  
    int accept_ptr[3] = {0, 0, 0}; 
    
    int time = 0;
    int packetsSent = 0;
    int totalPackets = trace.size();

    cout << "--- iSLIP Simulation Log ---" << endl;

    while (packetsSent < totalPackets) {
        // Handle Arrivals
        for (const auto& p : trace) {
            if (p.arrival == time) voqs[p.src][p.dest].push(p);
        }

        // iSLIP Step 1: Request
        bool requests[3][3] = {false};
        for(int i=0; i<3; i++) {
            for(int j=0; j<3; j++) {
                if(!voqs[i][j].empty()) requests[i][j] = true;
            }
        }

        // iSLIP Step 2: Grant
        int grants[3] = {-1, -1, -1}; 
        for (int j = 0; j < 3; j++) { 
            for (int k = 0; k < 3; k++) {
                int i = (grant_ptr[j] + k) % 3; 
                if (requests[i][j]) {
                    grants[j] = i;
                    break; 
                }
            }
        }

        // iSLIP Step 3: Accept
        int acceptances[3] = {-1, -1, -1}; 
        for (int i = 0; i < 3; i++) { 
            for (int k = 0; k < 3; k++) {
                int j = (accept_ptr[i] + k) % 3;
                if (grants[j] == i) {
                    acceptances[i] = j;
                    break;
                }
            }
        }

        // Output transfers and Update Pointers
        cout << "T=" << time << ": ";
        for (int i = 0; i < 3; i++) {
            if (acceptances[i] != -1) {
                int j = acceptances[i];
                Packet p = voqs[i][j].front();
                cout << "[" << p.id << ": I" << i << "->O" << j << "] ";
                
                grant_ptr[j] = (i + 1) % 3;
                accept_ptr[i] = (j + 1) % 3;
                
                voqs[i][j].pop();
                packetsSent++;
            }
        }

        // 6. Backlog Calculation
        // Sum sizes of all 9 Virtual Output Queues
        int currentBacklog = 0;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                currentBacklog += voqs[i][j].size();
            }
        }
        backlog_history.push_back(currentBacklog);

        cout << " | Ptrs: G{" << grant_ptr[0] << grant_ptr[1] << grant_ptr[2] 
             << "} A{" << accept_ptr[0] << accept_ptr[1] << accept_ptr[2] << "}" << endl;

        time++;
        if (time > 50) break; 
    }

    // Final entry for the empty switch
    backlog_history.push_back(0);

    cout << "--------------------------------" << endl;
    cout << "Total Service Time: " << time << endl;
    
    // Print Backlog
    cout << "islip_backlog = [";
    for(size_t i = 0; i < backlog_history.size(); i++) {
        cout << backlog_history[i] << (i == backlog_history.size()-1 ? "" : ", ");
    }
    cout << "]" << endl;

    return 0;
}