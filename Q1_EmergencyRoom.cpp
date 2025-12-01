// Q1_EmergencyRoom.cpp
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

int now_minutes_since_midnight() {
    using namespace std::chrono;
    auto now = system_clock::now();
    time_t t = system_clock::to_time_t(now);
    tm local_tm;
#ifdef _WIN32
    localtime_s(&local_tm, &t);
#else
    localtime_r(&t, &local_tm);
#endif
    return local_tm.tm_hour * 60 + local_tm.tm_min;
}

struct Patient {
    int patientID;
    string name;
    int severity; // 1=Critical,2=Urgent,3=Standard
    int arrivalTime; // minutes since midnight or any integer time
    int age;
    int admissionTime; // when admitted into system
    int assignedTime; // -1 if not assigned

    Patient() {}
    Patient(int id, string n, int sev, int arr, int a)
        : patientID(id), name(n), severity(sev), arrivalTime(arr), age(a),
          admissionTime(now_minutes_since_midnight()), assignedTime(-1) {}
};

struct PatientCompare {
    bool operator()(Patient const &A, Patient const &B) const {
        if (A.severity != B.severity) return A.severity < B.severity; // lower severity value = higher priority
        if (A.arrivalTime != B.arrivalTime) return A.arrivalTime < B.arrivalTime;
        if (A.age != B.age) return A.age > B.age; // older first
        return A.patientID < B.patientID; // ensures uniqueness in set
    }
};

class EmergencyRoomManager {
private:
    set<Patient, PatientCompare> pq;
    unordered_map<int, set<Patient,PatientCompare>::iterator> lookup;
    vector<Patient> assignedPatients; // history

public:
    EmergencyRoomManager() {}

    void admitPatient(const Patient &patient) {
        Patient p = patient;
        // make sure admissionTime is set now if it's default
        if (p.admissionTime == 0) p.admissionTime = now_minutes_since_midnight();
        auto it = pq.insert(p);
        if (it.second) {
            lookup[p.patientID] = it.first;
        } else {
            // unlikely: same patientID existing as different ordering key - ensure uniqueness by id externally
        }
    }

    void updatePatientCondition(int patientID, int newSeverity) {
        auto itmap = lookup.find(patientID);
        if (itmap == lookup.end()) {
            throw runtime_error("Patient not found or already assigned.");
        }
        auto it = itmap->second;
        Patient p = *it;
        if (p.assignedTime != -1) {
            throw runtime_error("Patient already assigned to a doctor; cannot update.");
        }
        pq.erase(it);
        p.severity = newSeverity;
        auto inserted = pq.insert(p);
        lookup[patientID] = inserted.first;
    }

    int assignDoctors(int availableDoctors) {
        int assigned = 0;
        for (int i = 0; i < availableDoctors; ++i) {
            if (pq.empty()) break;
            auto it = pq.begin();
            Patient p = *it;
            pq.erase(it);
            lookup.erase(p.patientID);
            p.assignedTime = now_minutes_since_midnight();
            assignedPatients.push_back(p);
            assigned++;
        }
        return assigned;
    }

    double getAverageWaitingTime(int severityLevel) {
        long long sum = 0;
        int cnt = 0;
        for (auto &p : assignedPatients) {
            if (p.severity == severityLevel && p.assignedTime >= 0) {
                sum += (p.assignedTime - p.admissionTime);
                cnt++;
            }
        }
        if (cnt == 0) return 0.0;
        return double(sum) / cnt;
    }

    // Helper to show current queue (for debugging/demo)
    void printQueue() {
        cout << "Current queue (top first):\n";
        for (auto &p : pq) {
            cout << p.patientID << " | " << p.name << " | sev:" << p.severity
                 << " arr:" << p.arrivalTime << " age:" << p.age << "\n";
        }
    }
};

// Sample usage / test
int main() {
    EmergencyRoomManager mgr;
    // Simulated arrival times (minutes)
    mgr.admitPatient(Patient(1, "Ali Khan", 2, 500, 30));
    mgr.admitPatient(Patient(2, "Zara Ahmed", 1, 505, 70));
    mgr.admitPatient(Patient(3, "Hassan", 2, 498, 68));
    mgr.admitPatient(Patient(4, "Aisha", 3, 499, 25));

    cout << "Before assignment:\n";
    mgr.printQueue();

    int assigned = mgr.assignDoctors(2);
    cout << "Assigned doctors to " << assigned << " patients.\n";

    cout << "Average waiting for severity 1: " << mgr.getAverageWaitingTime(1) << " minutes\n";
    cout << "Average waiting for severity 2: " << mgr.getAverageWaitingTime(2) << " minutes\n";
    return 0;
}
