#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <limits>

using namespace std;

class Student {
private:
    string name;
    string rollNo;

public:
    Student() : name(""), rollNo("") {}
    
    Student(string n, string r) : name(n), rollNo(r) {}
    
    string getName() const { return name; }
    string getRollNo() const { return rollNo; }
    
    void setName(string n) { name = n; }
    void setRollNo(string r) { rollNo = r; }
    
    void display() const {
        cout << left << setw(25) << name << setw(15) << rollNo;
    }
    
    string toString() const {
        return name + "|" + rollNo;
    }
    
    static Student fromString(const string& str) {
        size_t pos = str.find('|');
        if (pos != string::npos) {
            return Student(str.substr(0, pos), str.substr(pos + 1));
        }
        return Student();
    }
};

class ProjectGroup {
private:
    string projectTitle;
    string className;
    vector<Student> members;  

public:
    ProjectGroup() : projectTitle(""), className("") {}
    
    ProjectGroup(string title, string cls) : projectTitle(title), className(cls) {}
    
    string getProjectTitle() const { return projectTitle; }
    string getClassName() const { return className; }
    vector<Student> getMembers() const { return members; }
    
    void setProjectTitle(string title) { projectTitle = title; }
    void setClassName(string cls) { className = cls; }
    
    void addMember(const Student& student) {
        members.push_back(student);
    }
    
    void display() const {
        cout << "\n" << string(70, '=') << endl;
        cout << "Project Title: " << projectTitle << endl;
        cout << "Class Name   : " << className << endl;
        cout << string(70, '-') << endl;
        cout << left << setw(25) << "Student Name" << setw(15) << "Roll Number" << endl;
        cout << string(70, '-') << endl;
        
        for (const auto& student : members) {
            cout << "  ";
            student.display();
            cout << endl;
        }
        cout << string(70, '=') << endl;
    }
    
    bool hasStudentWithRoll(const string& rollNo) const {
        for (const auto& student : members) {
            if (student.getRollNo() == rollNo) {
                return true;
            }
        }
        return false;
    }
    
    string toString() const {
        string result = projectTitle + "~" + className + "~";
        for (size_t i = 0; i < members.size(); i++) {
            result += members[i].toString();
            if (i < members.size() - 1) {
                result += ";";
            }
        }
        return result;
    }
    
    static ProjectGroup fromString(const string& str) {
        ProjectGroup pg;
        size_t pos1 = str.find('~');
        size_t pos2 = str.find('~', pos1 + 1);
        
        if (pos1 != string::npos && pos2 != string::npos) {
            pg.projectTitle = str.substr(0, pos1);
            pg.className = str.substr(pos1 + 1, pos2 - pos1 - 1);
            
            string studentsStr = str.substr(pos2 + 1);
            size_t start = 0;
            size_t end = studentsStr.find(';');
            
            while (end != string::npos) {
                pg.members.push_back(Student::fromString(studentsStr.substr(start, end - start)));
                start = end + 1;
                end = studentsStr.find(';', start);
            }
            if (start < studentsStr.length()) {
                pg.members.push_back(Student::fromString(studentsStr.substr(start)));
            }
        }
        return pg;
    }
};

class CampusHub {
private:
    vector<ProjectGroup> groups;  
    const string filename = "campushub.txt";
    
    void merge(vector<ProjectGroup>& arr, int left, int mid, int right, bool sortByTitle) {
        int n1 = mid - left + 1;
        int n2 = right - mid;
        
        vector<ProjectGroup> L(n1), R(n2);
        
        for (int i = 0; i < n1; i++)
            L[i] = arr[left + i];
        for (int j = 0; j < n2; j++)
            R[j] = arr[mid + 1 + j];
        
        int i = 0, j = 0, k = left;
        
        while (i < n1 && j < n2) {
            string leftVal = sortByTitle ? L[i].getProjectTitle() : L[i].getClassName();
            string rightVal = sortByTitle ? R[j].getProjectTitle() : R[j].getClassName();
            
            if (leftVal <= rightVal) {
                arr[k] = L[i];
                i++;
            } else {
                arr[k] = R[j];
                j++;
            }
            k++;
        }
        
        while (i < n1) {
            arr[k] = L[i];
            i++;
            k++;
        }
        
        while (j < n2) {
            arr[k] = R[j];
            j++;
            k++;
        }
    }
    
    void mergeSort(vector<ProjectGroup>& arr, int left, int right, bool sortByTitle) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            mergeSort(arr, left, mid, sortByTitle);
            mergeSort(arr, mid + 1, right, sortByTitle);
            merge(arr, left, mid, right, sortByTitle);
        }
    }
    
    int binarySearch(const string& title) {
        int left = 0, right = groups.size() - 1;
        
        while (left <= right) {
            int mid = left + (right - left) / 2;
            string midTitle = groups[mid].getProjectTitle();
            
            if (midTitle == title) {
                return mid;
            } else if (midTitle < title) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        return -1;  
    }
    
    int linearSearchTitle(const string& title) {
        for (size_t i = 0; i < groups.size(); i++) {
            if (groups[i].getProjectTitle() == title) {
                return i;
            }
        }
        return -1;  
    }

public:
    CampusHub() {
        loadFromFile();
    }
    
    void addProject() {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        string title, className;
        cout << "\n--- Add New Project Group ---\n";
        cout << "Enter Project Title: ";
        getline(cin, title);
        cout << "Enter Class Name: ";
        getline(cin, className);
        
        ProjectGroup pg(title, className);
        
        int numStudents;
        cout << "Enter number of students: ";
        cin >> numStudents;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        for (int i = 0; i < numStudents; i++) {
            string name, rollNo;
            cout << "\nStudent " << (i + 1) << ":\n";
            cout << "  Name: ";
            getline(cin, name);
            cout << "  Roll No: ";
            getline(cin, rollNo);
            
            pg.addMember(Student(name, rollNo));
        }
        
        groups.push_back(pg);
        cout << "\n[SUCCESS] Project group added successfully!\n";
    }
    
    void displayAll() const {
        if (groups.empty()) {
            cout << "\n[INFO] No project groups found in the system.\n";
            return;
        }
        
        cout << "\n===== ALL PROJECT GROUPS =====\n";
        for (size_t i = 0; i < groups.size(); i++) {
            cout << "\nGroup #" << (i + 1);
            groups[i].display();
        }
    }
    
    void searchByTitle(bool useBinary = false) {
        if (groups.empty()) {
            cout << "\n[INFO] No project groups to search.\n";
            return;
        }
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string title;
        cout << "\nEnter Project Title to search: ";
        getline(cin, title);
        
        int index = -1;
        
        if (useBinary) {
            vector<ProjectGroup> tempGroups = groups;
            mergeSort(tempGroups, 0, tempGroups.size() - 1, true);
            
            int left = 0, right = tempGroups.size() - 1;
            while (left <= right) {
                int mid = left + (right - left) / 2;
                if (tempGroups[mid].getProjectTitle() == title) {
                    cout << "\n[FOUND] Using Binary Search:\n";
                    tempGroups[mid].display();
                    return;
                } else if (tempGroups[mid].getProjectTitle() < title) {
                    left = mid + 1;
                } else {
                    right = mid - 1;
                }
            }
        } else {
            index = linearSearchTitle(title);
        }
        
        if (index != -1) {
            cout << "\n[FOUND] Using Linear Search:\n";
            groups[index].display();
        } else {
            cout << "\n[NOT FOUND] No project with title \"" << title << "\" exists.\n";
        }
    }
    
    void searchByRoll() {
        if (groups.empty()) {
            cout << "\n[INFO] No project groups to search.\n";
            return;
        }
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string rollNo;
        cout << "\nEnter Student Roll Number to search: ";
        getline(cin, rollNo);
        
        bool found = false;
        
        for (size_t i = 0; i < groups.size(); i++) {
            if (groups[i].hasStudentWithRoll(rollNo)) {
                if (!found) {
                    cout << "\n[FOUND] Student with Roll No \"" << rollNo << "\" found in:\n";
                    found = true;
                }
                cout << "\nGroup #" << (i + 1);
                groups[i].display();
            }
        }
        
        if (!found) {
            cout << "\n[NOT FOUND] No student with Roll No \"" << rollNo << "\" exists.\n";
        }
    }
    
    void sortByTitle() {
        if (groups.empty()) {
            cout << "\n[INFO] No project groups to sort.\n";
            return;
        }
        
        mergeSort(groups, 0, groups.size() - 1, true);
        cout << "\n[SUCCESS] Projects sorted by Title (A-Z) using Merge Sort!\n";
        displayAll();
    }
    
    void sortByClassName() {
        if (groups.empty()) {
            cout << "\n[INFO] No project groups to sort.\n";
            return;
        }
        
        mergeSort(groups, 0, groups.size() - 1, false);
        cout << "\n[SUCCESS] Projects sorted by Class Name (A-Z) using Merge Sort!\n";
        displayAll();
    }
    
    void saveToFile() {
        ofstream outFile(filename);
        
        if (!outFile) {
            cout << "\n[ERROR] Unable to open file for writing.\n";
            return;
        }
        
        for (const auto& group : groups) {
            outFile << group.toString() << endl;
        }
        
        outFile.close();
        cout << "\n[SUCCESS] Data saved to " << filename << " successfully!\n";
    }
    
    void loadFromFile() {
        ifstream inFile(filename);
        
        if (!inFile) {
            return;
        }
        
        groups.clear();
        string line;
        
        while (getline(inFile, line)) {
            if (!line.empty()) {
                groups.push_back(ProjectGroup::fromString(line));
            }
        }
        
        inFile.close();
        
        if (!groups.empty()) {
            cout << "[INFO] Loaded " << groups.size() << " project group(s) from file.\n";
        }
    }
    
    int getTotalGroups() const {
        return groups.size();
    }
};

int main() {
    CampusHub hub;
    int choice;
    
    cout << "\n";
    cout << "============================================================\n";
    cout << "          CAMPUS HUB - PROJECT RECORD SYSTEM              \n";
    cout << "       Demonstrating OOP + DSA Concepts in C++            \n";
    cout << "============================================================\n";
    
    do {
        cout << "\n";
        cout << "------------------------------------------------------------\n";
        cout << "                     MAIN MENU                              \n";
        cout << "------------------------------------------------------------\n";
        cout << "  1. Add New Project Group                                 \n";
        cout << "  2. Display All Project Groups                            \n";
        cout << "  3. Search Project by Title (Linear Search)               \n";
        cout << "  4. Search Project by Title (Binary Search)               \n";
        cout << "  5. Search Student by Roll Number                         \n";
        cout << "  6. Sort Projects by Title (A-Z)                          \n";
        cout << "  7. Sort Projects by Class Name (A-Z)                     \n";
        cout << "  8. Save Data to File                                     \n";
        cout << "  9. Exit                                                  \n";
        cout << "------------------------------------------------------------\n";
        cout << "\nTotal Project Groups: " << hub.getTotalGroups() << endl;
        cout << "Enter your choice (1-9): ";
        cin >> choice;
        
        switch (choice) {
            case 1:
                hub.addProject();
                break;
            case 2:
                hub.displayAll();
                break;
            case 3:
                hub.searchByTitle(false);  
                break;
            case 4:
                hub.searchByTitle(true);   
                break;
            case 5:
                hub.searchByRoll();
                break;
            case 6:
                hub.sortByTitle();
                break;
            case 7:
                hub.sortByClassName();
                break;
            case 8:
                hub.saveToFile();
                break;
            case 9:
                hub.saveToFile();
                cout << "\n============================================================\n";
                cout << "  Thank you for using Campus Hub!                          \n";
                cout << "  All data has been saved. Goodbye!                        \n";
                cout << "============================================================\n\n";
                break;
            default:
                cout << "\n[ERROR] Invalid choice! Please enter a number between 1-9.\n";
        }
        
    } while (choice != 9);
    
    return 0;
}
