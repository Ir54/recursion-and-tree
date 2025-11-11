#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <string>
#include <ctime>
using namespace std;

struct Patient {
    int patientID;
    string name;
    int severity;       // 1 = Critical, 2 = Urgent, 3 = Standard
    int arrivalTime;    // minutes since midnight
    int age;
    int admissionTime;
    int assignedTime;

    Patient(int id, string n, int s, int aTime, int ag, int adTime)
        : patientID(id), name(n), severity(s),
          arrivalTime(aTime), age(ag),
          admissionTime(adTime), assignedTime(-1) {}
};

// Custom comparator for priority queue
struct ComparePatients {
    bool operator()(const Patient& a, const Patient& b) const {
        if (a.severity != b.severity)
            return a.severity > b.severity;       // lower severity = higher priority
        else if (a.arrivalTime != b.arrivalTime)
            return a.arrivalTime > b.arrivalTime; // earlier = higher priority
        else
            return a.age < b.age;                 // older = higher priority
    }
};

class EmergencyRoomManager {
private:
    priority_queue<Patient, vector<Patient>, ComparePatients> pq;
    unordered_map<int, Patient> allPatients;
    vector<Patient> assignedPatients;

public:
    // Admit new patient
    void admitPatient(const Patient& p) {
        pq.push(p);
        allPatients[p.patientID] = p;
    }

    // Update severity
    void updatePatientCondition(int patientID, int newSeverity) {
        if (allPatients.find(patientID) == allPatients.end()) {
            cout << "Patient not found.\n";
            return;
        }
        Patient updated = allPatients[patientID];
        if (updated.assignedTime != -1) {
            cout << "Cannot update condition. Patient already assigned.\n";
            return;
        }
        updated.severity = newSeverity;
        allPatients[patientID] = updated;

        // rebuild queue (simple way for now)
        priority_queue<Patient, vector<Patient>, ComparePatients> newPQ;
        for (auto& p : allPatients) {
            if (p.second.assignedTime == -1)
                newPQ.push(p.second);
        }
        pq = move(newPQ);
    }

    // Assign doctors to top priority patients
    int assignDoctors(int availableDoctors) {
        int count = 0;
        int currentTime = time(nullptr) % 100000; // simple timestamp substitute

        while (!pq.empty() && availableDoctors > 0) {
            Patient top = pq.top();
            pq.pop();

            allPatients[top.patientID].assignedTime = currentTime;
            assignedPatients.push_back(allPatients[top.patientID]);

            availableDoctors--;
            count++;
        }
        return count;
    }

    // Compute average waiting time for a given severity
    double getAverageWaitingTime(int severityLevel) {
        int total = 0, count = 0;
        for (auto& p : assignedPatients) {
            if (p.severity == severityLevel) {
                int wait = p.assignedTime - p.admissionTime;
                total += wait;
                count++;
            }
        }
        if (count == 0) return 0.0;
        return (double)total / count;
    }

    // For testing/debugging
    void displayQueue() {
        priority_queue<Patient, vector<Patient>, ComparePatients> temp = pq;
        cout << "\nCurrent waiting patients:\n";
        while (!temp.empty()) {
            Patient p = temp.top();
            temp.pop();
            cout << "ID: " << p.patientID << ", Name: " << p.name
                 << ", Severity: " << p.severity
                 << ", Arrival: " << p.arrivalTime
                 << ", Age: " << p.age << "\n";
        }
    }
};

// ----------------------
// Example usage
// ----------------------
int main() {
    EmergencyRoomManager er;

    er.admitPatient(Patient(1, "Alice", 2, 100, 70, 100));
    er.admitPatient(Patient(2, "Bob", 1, 110, 40, 110));
    er.admitPatient(Patient(3, "Charlie", 2, 105, 80, 105));

    cout << "Before assignment:";
    er.displayQueue();

    er.assignDoctors(2);

    cout << "\nAverage waiting time (Urgent): " << er.getAverageWaitingTime(2) << endl;
    return 0;
}
