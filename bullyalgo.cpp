#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct Process
{
    int id;
    bool active;
};

// --- 1. BULLY ALGORITHM ---
// Concept: The highest active process "bullies" everyone else to become leader.
void runBully(vector<Process> &processes, int initiatorIndex)
{
    int n = processes.size();
    int currentID = processes[initiatorIndex].id;

    cout << "\n--- Process " << currentID << " initiates Election ---\n";

    vector<int> responders; // Store who responded

    // 1. Broadcast ELECTION to ALL higher processes
    for (int i = 0; i < n; i++)
    {
        if (processes[i].id > currentID)
        {
            cout << "  Process " << currentID << " sends ELECTION -> Process " << processes[i].id;

            if (processes[i].active)
            {
                cout << " [OK]\n";
                responders.push_back(i);
            }
            else
            {
                cout << " [FAILED]\n";
            }
        }
    }

    // 2. Analyze results
    if (responders.empty())
    {
        // Case A: No one higher is alive. I am the boss.
        cout << ">>> Process " << currentID << " received no OKs.\n";
        cout << ">>> Process " << currentID << " becomes COORDINATOR! 👑\n";

        // (Optional: Broadcast "I am Coordinator" to lower nodes here)
    }
    else
    {
        // Case B: Higher nodes are alive. They take over.
        cout << ">>> Process " << currentID << " received OKs. Stopping election.\n";

        // strictly speaking, ALL responders start an election.
        // To keep output readable, we recursively simulate just the next active node
        // (since it will eventually message the others anyway).
        runBully(processes, responders[0]);
    }
}
// --- 2. RING ALGORITHM ---
// Concept: A token moves in a circle. Active nodes add their ID to the list.
void runRing(vector<Process> &processes, int initiatorIndex)
{
    int n = processes.size();
    cout << "\n--- Ring Election Started by Process " << processes[initiatorIndex].id << " ---\n";

    vector<int> activeList;

    // Loop exactly 'n' times starting from the initiator
    for (int i = 0; i < n; i++)
    {
        // Use modulo to wrap around the array (e.g., index 5 becomes 0)
        int curr = (initiatorIndex + i) % n;

        if (processes[curr].active)
        {
            activeList.push_back(processes[curr].id);
            cout << "  Token passed to Process " << processes[curr].id << "\n";
        }
    }

    // Find the highest ID in our list
    int maxID = -1;
    for (int id : activeList)
    {
        if (id > maxID)
            maxID = id;
    }

    cout << ">>> Process " << maxID << " is the new COORDINATOR! 👑\n";
}

int main()
{
    // Setup: 5 processes. ID 50 is the highest.
    // Topology: 10 -> 20 -> 30 -> 40 -> 50
    vector<Process> processes = {
        {10, true},
        {20, true},
        {30, true},
        {40, true},
        {50, true}};

    // Scenario: The leader (50) crashes
    processes[4].active = false;
    cout << "Status: Process 50 has crashed.\n";

    // Scenario: Process 20 notices the crash and starts election
    int initiatorIndex = 1; // Index of ID 20

    // Run Bully
    runBully(processes, initiatorIndex);

    // Run Ring
    runRing(processes, initiatorIndex);

    return 0;
}
