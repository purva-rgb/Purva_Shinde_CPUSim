#include <iostream>
#include <vector>
#include <queue>
#include <deque>
#include <unordered_set>
#include <string>
#include <sstream>
using namespace std;

struct Task {
    string id;
    int burst;
    int remaining;
    vector<string> mem;
    int memIndex = 0;
    //constructor to initialize the task with its id, burst time, and memory requests
    Task(string i, int b, vector<string> m) {
        id = i;
        burst = b;
        remaining = b;
        mem = m;
    }
};

class Cache {
public:
    int L1_MAX = 32;//maximum number of blocks in L1 cache
    int L2_MAX = 128;
    int L3_MAX = 512;

    deque<string> L1, L2, L3; //represents cache level
    unordered_set<string> s1, s2, s3;

    int ramAccesses = 0;
    //function to remove a memory block from the cache and its corresponding set
    void removeFrom(deque<string>& q, unordered_set<string>& s, string val) {

        if (!s.count(val))
            return;

        s.erase(val);

        deque<string> temp;
        //iterate through the deque and copy all elements except the one to be removed
        while (!q.empty()) {

            if (q.front() != val)
                temp.push_back(q.front());

            q.pop_front();
        }

        q = temp;
    }

    //function to update the position of a memory block in the cache to show its recent access
    void touch(deque<string>& q, unordered_set<string>& s, string mem) {

        if (!s.count(mem))
            return;

        removeFrom(q, s, mem);

        q.push_back(mem);
        s.insert(mem);
    }

    void insertL3(string mem) {

        if (s3.count(mem)) {
            touch(L3, s3, mem);//if the block is already in L3, update its position to show recent access
            return;
        }

        if ((int)L3.size() == L3_MAX) {

            string old = L3.front();//find the least recently used block in L3

            L3.pop_front();
            s3.erase(old);
        }

        L3.push_back(mem);//insert the new block into L3 and update the set
        s3.insert(mem);
    }

    void insertL2(string mem) {

        if (s2.count(mem)) {
            touch(L2, s2, mem);//if the block is already in L2, update its position to show recent access
            return;
        }

        if ((int)L2.size() == L2_MAX) {

            string evicted = L2.front(); //find the least recently used block in L2

            L2.pop_front();
            s2.erase(evicted);//remove from L2 and add to L3

            insertL3(evicted);
        }

        L2.push_back(mem);//insert the new block into L2 and update the set
        s2.insert(mem);
    }

    void insertL1(string mem) {

        if (s1.count(mem)) {
            touch(L1, s1, mem);
            return;
        }

        if ((int)L1.size() == L1_MAX) {

            string evicted = L1.front(); //find the least recently used block in L1

            L1.pop_front();
            s1.erase(evicted);//remove from L1 and add to L2

            insertL2(evicted);
        }

        L1.push_back(mem);
        s1.insert(mem);
    }

    string access(string mem, int &latency) {

        if (s1.count(mem)) {

            touch(L1, s1, mem);
            latency = 4;
            return "L1 HIT";
        }

        if (s2.count(mem)) {

            removeFrom(L2, s2, mem);//remove from L2 add to L1 to show recent access
            insertL1(mem);
            latency = 12;
            return "L2 HIT";
        }

        if (s3.count(mem)) {

            removeFrom(L3, s3, mem);//remove from L3 add to L1 to show recent access
            insertL1(mem);
            latency = 40;
            return "L3 HIT";
        }
        ramAccesses++; //if not found in cache, access RAM and add to L1 cache

        insertL1(mem);
        latency = 200;
        return "RAM MISS";
    }
    //function to print the current state of the cache levels
    void print() {

        cout << "L1: [ ";
        for (auto &x : L1)
            cout << x << " ";
        cout << "]\n";

        cout << "L2: [ ";
        for (auto &x : L2)
            cout << x << " ";
        cout << "]\n";

        cout << "L3: [ ";
        for (auto &x : L3)
            cout << x << " ";
        cout << "]\n";
    }
};

class Scheduler {
public:
    int quantum; //time slice for round robin scheduling
    int cycle = 0;//total cycles taken to complete all tasks
    Cache cache;
    int completedTasks = 0;

    Scheduler(int q) {
        quantum = q;
    }

    void run(vector<Task*> tasks) {

        queue<Task*> q;
        for (auto t : tasks)
            q.push(t);

        while (!q.empty()) {

            Task* t = q.front();
            q.pop();

            int runTime = min(quantum, t->remaining);
            //execution of task for determined timeslice
            for (int i = 0; i < runTime; i++) {

                t->remaining--;
                
                string mem = "M0";
                //if the task has memory requests,access the next one in the list, otherwise default to M0
                if (!t->mem.empty()) {
                    mem = t->mem[t->memIndex % t->mem.size()];
                    t->memIndex++;
                }

                int latency;
                string result = cache.access(mem, latency);

                cycle++; 

                cout << "\nCycle "<< cycle<< " - Running: "<< t->id<< " Requesting: "<< mem << "\n";
                     
                cout << result<< " | Latency: "<< latency<< " cycles\n";

                cache.print();
            }
            //after executing for the timeslice, check if the task is completed or needs to be re-queued
            if (t->remaining > 0)
                q.push(t);
            else
                completedTasks++;
        }

        cout << "\t ~~Final Results~~";
        cout << "\nTotal Cycles: "<< cycle;
        cout << "\nTOTAL TASKS LOADED = "<< tasks.size();
        cout << "\nTasks Completed: "<< completedTasks;
        cout << "\nScheduler: Round Robin quantum = "<< quantum;
        cout << "\nRAM Accesses: "<< cache.ramAccesses;
             
    }
};
//function to parse input lines and create Task objects accordingly
vector<Task*> parseInput() {

    vector<Task*> tasks;
    string line;

    while (getline(cin, line)) {

        if (line.empty()) continue;
        if (line.find("TASK") != 0) continue;

        stringstream ss(line);

        string taskWord, id, burstWord, memWord;
        int burst;

        ss >> taskWord >> id >> burstWord >> burst >> memWord;

        vector<string> mem;
        string m;
        
        while (ss >> m)
            mem.push_back(m);

        tasks.push_back(new Task(id, burst, mem));
    }
    return tasks;
}

int main() {
    vector<Task*> tasks = parseInput();
    
    Scheduler scheduler(3);
    scheduler.run(tasks);
    return 0;
}

