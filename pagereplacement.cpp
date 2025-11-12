#include <bits/stdc++.h>
#include <chrono>
#include <thread>
using namespace std;

// ---------------- FIFO ----------------
void fifo(vector<int>& pages, int framesize) {
    unordered_set<int> frames;
    queue<int> pageOrder;
    int pageFaults = 0;

    cout << "\n--- FIFO Page Replacement ---\n";

    for (int i = 0; i < pages.size(); i++) {
        int page = pages[i];

        // If page not in frame (Page Fault)
        if (frames.find(page) == frames.end()) {
            pageFaults++;

            if (frames.size() < framesize) {
                frames.insert(page);
                pageOrder.push(page);
            } else {
                int oldestPage = pageOrder.front();
                pageOrder.pop();
                frames.erase(oldestPage);
                frames.insert(page);
                pageOrder.push(page);
            }
        }

        cout << "Step " << i + 1 << " -> Frames: ";
        for (const auto &f : frames)
            cout << f << " ";
        cout << endl;
    }

    cout << "Total Page Faults (FIFO): " << pageFaults << "\n";
}

// ---------------- LRU ----------------
void lru(vector<int>& pages, int framesize) {
    list<int> frames;
    unordered_map<int, list<int>::iterator> position;
    int pageFaults = 0;

    cout << "\n--- LRU Page Replacement ---\n";

    for (int i = 0; i < pages.size(); i++) {
        int page = pages[i];

        // If page not found -> Page Fault
        if (position.find(page) == position.end()) {
            pageFaults++;

            // If frame full -> remove least recently used (front)
            if (frames.size() == framesize) {
                int lruPage = frames.front();
                frames.pop_front();
                position.erase(lruPage);
            }

            // Insert current page at end (most recently used)
            frames.push_back(page);
            position[page] = prev(frames.end());
        }
        else {
            // Move page to the end (most recently used)
            frames.erase(position[page]);
            frames.push_back(page);
            position[page] = prev(frames.end());
        }

        cout << "Step " << i + 1 << " -> Frames: ";
        for (const auto &f : frames)
            cout << f << " ";
        cout << endl;
    }

    cout << "Total Page Faults (LRU): " << pageFaults << "\n";
}

// ---------------- OPTIMAL ----------------
void optimal(vector<int>& pages, int framesize) {
    vector<int> frames;
    int pageFaults = 0;

    cout << "\n--- Optimal Page Replacement ---\n";

    for (int i = 0; i < pages.size(); i++) {
        int page = pages[i];

        // If page already in frame → skip
        if (find(frames.begin(), frames.end(), page) != frames.end()) {
            cout << "Step " << i + 1 << " -> Frames: ";
            for (auto f : frames) cout << f << " ";
            cout << endl;
            continue;
        }

        pageFaults++;

        // If frames not full → just add page
        if (frames.size() < framesize) {
            frames.push_back(page);
        } else {
            // Find page to replace using future knowledge
            int farthest = i + 1, replaceIndex = -1;

            for (int j = 0; j < frames.size(); j++) {
                int nextUse = INT_MAX;
                for (int k = i + 1; k < pages.size(); k++) {
                    if (pages[k] == frames[j]) {
                        nextUse = k;
                        break;
                    }
                }

                if (nextUse > farthest) {
                    farthest = nextUse;
                    replaceIndex = j;
                }
            }

            // If all pages will be used later, remove the first one
            if (replaceIndex == -1) replaceIndex = 0;

            frames[replaceIndex] = page;
        }

        cout << "Step " << i + 1 << " -> Frames: ";
        for (auto f : frames) cout << f << " ";
        cout << endl;
    }

    cout << "Total Page Faults (Optimal): " << pageFaults << "\n";
}

// ---------------- MAIN ----------------
int main() {
    int n, framesize;
    cout << "Enter number of pages: ";
    cin >> n;

    vector<int> pages(n);
    cout << "Enter the page reference string (space-separated): ";
    for (int i = 0; i < n; i++)
        cin >> pages[i];

    cout << "Enter number of frames (memory size): ";
    cin >> framesize;

    fifo(pages, framesize);
    lru(pages, framesize);
    optimal(pages, framesize);

    cout << "\nSimulation Finished.\n";
    return 0;
}
