#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <cassert>
#include <bits/stdc++.h>


using namespace std;

mutex coutMutex;

condition_variable cv;
volatile int activeOrchatas;
volatile int syncInt;
int nOrchatas;
volatile unsigned int activeThreads;
vector<int> finishTime;
queue<pair<int, int>> queueOrders;

static const short orchatas_time = 2000;

class getInfo {
    string textFile;
    string line;
    vector<string> args; //contains all arguments
    vector<string> orchataPath; //temp, contains a orchata path
    vector<vector<string>> orchataPaths; //contains orchata paths

public:

    getInfo(string file) {
        textFile = file;
    }

    int getNumberOfOrchatas() {
        return nOrchatas;
    }

    static bool sortbysec(const pair<int, int> &a,
            const pair<int, int> &b) {
        return (a.second < b.second);
    }

    void parseFile() {
        ifstream myfile(textFile);
        if (myfile.is_open()) {
            int input = -1;
            int lineCounter = -2;
            unsigned int maxsize = 0;
            while (getline(myfile, line)) {
                string tempArg = "";
                string tempOrchataStep = "";
                int tempOrchataStepint = -1;
                for (unsigned int characterCounter = 0; characterCounter < line.length(); characterCounter++) {
                    if (line[characterCounter] != ' ') {
                        tempArg = tempArg + line[characterCounter];
                        tempOrchataStep = tempOrchataStep + line[characterCounter];
                    }
                    if (line[characterCounter] == ' ' || characterCounter == line.length() - 1) {
                        input++;
                        args.resize(input);
                        args.insert(args.begin(), tempArg);
                        tempArg = "";
                    }
                    if ((line[characterCounter] == ' ' || characterCounter == line.length() - 1) && lineCounter >= -1) {
                        tempOrchataStepint++;
                        orchataPath.resize(tempOrchataStepint);
                        orchataPath.insert(orchataPath.begin(), tempOrchataStep);
                        tempOrchataStep = "";
                    }
                }
                lineCounter++;
                if (lineCounter >= 0) {
                    reverse(orchataPath.begin(), orchataPath.end());

                    orchataPaths.resize(lineCounter);
                    orchataPaths.insert(orchataPaths.begin(), orchataPath);
                    if (orchataPath.size() > maxsize) maxsize = orchataPath.size();
                    cout << " debug Orchata line: " << orchataPaths[0][0] << "\n"; //access like a 2d array
                }
            }
            reverse(args.begin(), args.end());
            reverse(orchataPaths.begin(), orchataPaths.end());
            nOrchatas = stoi(args[0]);
            myfile.close();
            for (unsigned int j = 0; j < maxsize; j++) {
                vector< pair <int, int> > vect;
                for (unsigned int i = 0; i < orchataPaths.size(); i++) {
                    if (orchataPaths[i].size() > j)
                        vect.push_back(make_pair((i + 1), stoi(orchataPaths[i][j])));

                }
                //Try to fill the queue giving priority to the smaller orders
                sort(vect.begin(), vect.end(), sortbysec);
                for (unsigned int i = 0; i < vect.size(); i++) {
                    queueOrders.push(make_pair(vect[i].second, vect[i].first));
                }
            }
            cout << "number of orchateros: " << nOrchatas << "\n";
            cout << "Number of Orchata tables: " << orchataPaths.size() << "\n";
        } else
            cout << "Unable to open file";
    }
};

vector<string> setOrchataNames(int nOrchatas) {
    vector<string> returnVec;
    returnVec.resize(nOrchatas);
    char orchataName = 64;
    for (int i = 0; i < nOrchatas; i++) {
        orchataName++;
        returnVec[i] = orchataName;
    }
    return returnVec;
}

void workerThread(vector<string> orchataNames, int identifier) {
    //variables only accessable by the current thread
    int timeStep = 0;
    pair<int, int> pairqueue;


    while ((queueOrders.size() > 0) && syncInt) {
        coutMutex.lock();
        pairqueue = queueOrders.front();
        queueOrders.pop();
        coutMutex.unlock();
        //Simulate making orchatas
        this_thread::sleep_for(chrono::milliseconds(orchatas_time * (pairqueue.first)));
        timeStep++;
        coutMutex.lock();
        cout << "At time step: " << timeStep << " orchatero " << orchataNames[identifier] << " table " << pairqueue.second
                << " completed order for " << pairqueue.first << " orchatas. " << "Orders left: " << queueOrders.size() << "\n";
        coutMutex.unlock();
    }

    finishTime[identifier] = timeStep;

}

void controllerThread(int identifier) {
    //variables only accessable by the current thread
    // Wait for single character
    //system("stty raw");
    getchar();
    // Reset terminal to normal "cooked" mode 
    //system("stty cooked"); 
    cout << "\n------------Exit requested, after current orders--------\n";
    coutMutex.lock();
    syncInt = 0;
    coutMutex.unlock();
}

int main(int argc, char** argv) {
    getInfo getInfoObject(argv[1]);
    getInfoObject.parseFile();
    nOrchatas = getInfoObject.getNumberOfOrchatas();
    vector<string> orchataNames = setOrchataNames(nOrchatas); //holds orchata names
    thread** t = new thread*[nOrchatas + 1];
    activeOrchatas = nOrchatas;
    finishTime.resize(nOrchatas);
    syncInt = 1;

    cout << "Starting simulation...\n";
    cout << "\nPress Enter to exit earlier\n\n";
    for (int threadNum = 0; threadNum < nOrchatas; threadNum++) {
        t[threadNum] = new thread(workerThread, orchataNames, threadNum);
    }
    
    t[nOrchatas] = new thread(controllerThread, nOrchatas);

    for (int threadNum = 0; threadNum < nOrchatas; threadNum++) {
        t[threadNum]->join();
    }
    cout << "Simulation complete. \n\n";
    if(syncInt) {
		for (int identifier = 0; identifier < nOrchatas; identifier++) {
			cout << "Orchatero " << orchataNames[identifier] << " completed its order at time step " << finishTime[identifier] << "\n";
		}
    }
    return 0;
}
