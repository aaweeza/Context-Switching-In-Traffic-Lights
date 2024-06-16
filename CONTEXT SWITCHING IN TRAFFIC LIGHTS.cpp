#include <iostream>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <mutex>

using namespace std;

class TrafficLight {
public:
    enum LightState { RED, GREEN };

    TrafficLight() : state(RED) {}

    void setGreen() { state = GREEN; }
    void setRed() { state = RED; }
    bool isGreen() const { return state == GREEN; }
    LightState getState() const { return state; }

private:
    LightState state;
};

class Intersection {
public:
    enum EmergencyVehicle { NONE, AMBULANCE, POLICE, FIRE_BRIGADE };

    Intersection() : emergencyRoad(-1), emergencyVehicle(NONE) {
        // Initialize the traffic lights
        for (int i = 0; i < 4; ++i) {
            lights[i] = TrafficLight();
        }
        srand(static_cast<unsigned>(time(0)));
    }

    void generateTraffic() {
        // Generate random traffic for each road
        for (int i = 0; i < 4; ++i) {
            traffic[i] = rand() % 20;  // Random traffic between 0 and 19
        }
        // Randomly assign an emergency vehicle to one of the roads
        emergencyRoad = rand() % 4;
        emergencyVehicle = static_cast<EmergencyVehicle>(rand() % 3 + 1);  // Randomly pick one of the three emergency vehicles
    }

    void manageTraffic() {
        // Create threads for each road
        vector<thread> threads;
        for (int i = 0; i < 4; ++i) {
            threads.push_back(thread(&Intersection::handleRoad, this, i));
        }

        // Wait for all threads to finish (in reality, this will be an infinite loop)
        for (auto& t : threads) {
            t.join();
        }
    }

private:
    TrafficLight lights[4];
    int traffic[4] = {0};
    int emergencyRoad;
    EmergencyVehicle emergencyVehicle;
    mutex trafficMutex;

    int getMaxTrafficIndex() {
        lock_guard<mutex> lock(trafficMutex);
        int maxIndex = 0;
        for (int i = 1; i < 4; ++i) {
            if (traffic[i] > traffic[maxIndex]) {
                maxIndex = i;
            }
        }
        return maxIndex;
    }

    void switchLights(int greenIndex) {
        lock_guard<mutex> lock(trafficMutex);
        for (int i = 0; i < 4; ++i) {
            if (i == greenIndex) {
                lights[i].setGreen();
            } else {
                lights[i].setRed();
            }
        }
    }

    void decreaseTraffic(int greenIndex) {
        lock_guard<mutex> lock(trafficMutex);
        if (traffic[greenIndex] > 0) {
            traffic[greenIndex] -= 5;  // Decrease traffic by 5 units
            if (traffic[greenIndex] < 0) {
                traffic[greenIndex] = 0;
            }
        }
    }

    void printTrafficStatus() {
        lock_guard<mutex> lock(trafficMutex);
        cout << "Traffic status:\n";
        for (int i = 0; i < 4; ++i) {
            cout << "Road " << i + 1 << ": " << traffic[i] << " cars";
            if (i == emergencyRoad) {
                cout << " and an " << getEmergencyVehicleName(emergencyVehicle);
            }
            cout << ", Light: " << (lights[i].isGreen() ? "GREEN" : "RED") << "\n";
        }
        cout << "------------------------\n";
    }

    string getEmergencyVehicleName(EmergencyVehicle vehicle) const {
        switch (vehicle) {
            case AMBULANCE: return "ambulance";
            case POLICE: return "police car";
            case FIRE_BRIGADE: return "fire brigade";
            default: return "";
        }
    }

    void handleRoad(int roadIndex) {
        while (true) {
            bool done = false;
            {
                lock_guard<mutex> lock(trafficMutex);
                // Check if all roads are cleared
                done = traffic[0] == 0 && traffic[1] == 0 && traffic[2] == 0 && traffic[3] == 0;
            }

            if (done) {
                cout << "All roads are cleared. Stopping the program.\n";
                exit(0);  // Exit the program
            }

            // Check if this road has the emergency vehicle
            if (roadIndex == emergencyRoad) {
                switchLights(roadIndex);
                this_thread::sleep_for(chrono::seconds(5));  // Simulate time for each green light
                decreaseTraffic(roadIndex);
                printTrafficStatus();

                lock_guard<mutex> lock(trafficMutex);
                if (traffic[roadIndex] == 0) {
                    emergencyVehicle = NONE;
                    emergencyRoad = -1;
                }
            } else if (roadIndex == getMaxTrafficIndex()) {
                // If no emergency vehicle, handle based on max traffic
                switchLights(roadIndex);
                this_thread::sleep_for(chrono::seconds(5));  // Simulate time for each green light
                decreaseTraffic(roadIndex);
                printTrafficStatus();
            } else {
                // Yield execution to other threads
                this_thread::yield();
            }
        }
    }
};

void clearConsole() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

int main() {
    cout << "\n\n\n\n\n\n\n\n\n\n\n\t\t\t\tWELCOME :)\n";

    this_thread::sleep_for(chrono::seconds(4));
    clearConsole();

    for (int i = 1; i <= 100; ++i) {
        this_thread::sleep_for(chrono::milliseconds(50)); // Adjust the sleep duration as needed
        clearConsole();
        cout << "\n\n\n\n\n\n\n\n\n\n\n\t\t\tCONTEXT SWITCHING IN TRAFFIC LIGHTS\n\t\t\tBY AAWEEZA FAROOQUI, HAMZA MEHBOOB, M. SAJEEL HUSSAIN" << endl;
        cout << "\n\n\n\n\n\n\n\n\n\n\n\n\t\t\t\t\t\t\t\tTRAFFIC STATUS LOADING " << i << "%";
        if (i == 100) {
            this_thread::sleep_for(chrono::seconds(1));
        }
    }
    clearConsole();
    Intersection intersection;
    intersection.generateTraffic();
    intersection.manageTraffic();

    return 0;
}

