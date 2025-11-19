#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstdlib>

using namespace std;

// Utility function to generate simple captcha
string generateCaptcha() {
    string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    string captcha = "";
    for(int i = 0; i < 6; i++) {
        captcha += chars[rand() % chars.length()];
    }
    return captcha;
}

// Structure for Student
struct Student {
    string rollNo;
    string name;
    string className;
    bool viewed;
    bool submitted;
    bool completed;
    string submissionFile;
    
    Student() : viewed(false), submitted(false), completed(false) {}
};

// Structure for Project
struct Project {
    int id;
    string title;
    string description;
    string pdfFile;
    string uploadDate;
    string className;
    string facultyName;
    map<string, Student> students; // rollNo -> Student
    
    Project() : id(0) {}
};

// Structure for User
struct User {
    string username;
    string password;
    string role; // "faculty" or "student"
    string name;
    string className; // for students
};

class CampusHub {
private:
    map<string, User> users;
    map<int, Project> projects;
    map<string, vector<Student>> classStudents; // className -> students
    User currentUser;
    int nextProjectId;
    
    string usersFile = "users.txt";
    string projectsFile = "projects.txt";
    string studentsPrefix = "class_";
    
public:
    CampusHub() : nextProjectId(1) {
        loadUsers();
        loadProjects();
        loadAllClasses();
    }
    
    // Load users from file
    void loadUsers() {
        ifstream file(usersFile);
        if(!file.is_open()) {
            // Create default admin accounts
            User faculty;
            faculty.username = "faculty1";
            faculty.password = "pass123";
            faculty.role = "faculty";
            faculty.name = "Dr. Smith";
            users[faculty.username] = faculty;
            
            User student;
            student.username = "student1";
            student.password = "pass123";
            student.role = "student";
            student.name = "John Doe";
            student.className = "CS101";
            users[student.username] = student;
            
            saveUsers();
            return;
        }
        
        string line;
        while(getline(file, line)) {
            stringstream ss(line);
            User u;
            getline(ss, u.username, '|');
            getline(ss, u.password, '|');
            getline(ss, u.role, '|');
            getline(ss, u.name, '|');
            getline(ss, u.className, '|');
            users[u.username] = u;
        }
        file.close();
    }
    
    // Save users to file
    void saveUsers() {
        ofstream file(usersFile);
        for(auto& pair : users) {
            User& u = pair.second;
            file << u.username << "|" << u.password << "|" << u.role << "|" 
                 << u.name << "|" << u.className << "\n";
        }
        file.close();
    }
    
    // Load students from CSV for a class
    void loadClassStudents(string className) {
        string filename = studentsPrefix + className + ".csv";
        ifstream file(filename);
        if(!file.is_open()) {
            return;
        }
        
        string line;
        getline(file, line); // Skip header
        
        vector<Student> students;
        while(getline(file, line)) {
            stringstream ss(line);
            Student s;
            getline(ss, s.rollNo, ',');
            getline(ss, s.name, ',');
            s.className = className;
            students.push_back(s);
        }
        classStudents[className] = students;
        file.close();
    }
    
    // Load all class files
    void loadAllClasses() {
        vector<string> classes = {"CS101", "CS102", "CS103", "EE101", "ME101"};
        for(auto& cls : classes) {
            loadClassStudents(cls);
        }
    }
    
    // Create sample CSV files for classes
    void createSampleClassFiles() {
        // CS101
        ofstream file1(studentsPrefix + "CS101.csv");
        file1 << "RollNo,Name\n";
        file1 << "CS101001,Alice Johnson\n";
        file1 << "CS101002,Bob Smith\n";
        file1 << "CS101003,Charlie Brown\n";
        file1 << "CS101004,Diana Prince\n";
        file1 << "CS101005,Eve Wilson\n";
        file1.close();
        
        // CS102
        ofstream file2(studentsPrefix + "CS102.csv");
        file2 << "RollNo,Name\n";
        file2 << "CS102001,Frank Castle\n";
        file2 << "CS102002,Grace Hopper\n";
        file2 << "CS102003,Henry Ford\n";
        file2.close();
    }
    
    // Load projects
    void loadProjects() {
        ifstream file(projectsFile);
        if(!file.is_open()) return;
        
        string line;
        while(getline(file, line)) {
            if(line.empty()) continue;
            
            stringstream ss(line);
            Project p;
            string idStr;
            getline(ss, idStr, '|');
            p.id = stoi(idStr);
            getline(ss, p.title, '|');
            getline(ss, p.description, '|');
            getline(ss, p.pdfFile, '|');
            getline(ss, p.uploadDate, '|');
            getline(ss, p.className, '|');
            getline(ss, p.facultyName, '|');
            
            // Load student submissions for this project
            string submFile = "submissions_" + to_string(p.id) + ".txt";
            ifstream subFile(submFile);
            if(subFile.is_open()) {
                string subLine;
                while(getline(subFile, subLine)) {
                    stringstream subs(subLine);
                    Student s;
                    getline(subs, s.rollNo, '|');
                    getline(subs, s.name, '|');
                    getline(subs, s.className, '|');
                    string viewedStr, submittedStr, completedStr;
                    getline(subs, viewedStr, '|');
                    getline(subs, submittedStr, '|');
                    getline(subs, completedStr, '|');
                    getline(subs, s.submissionFile, '|');
                    s.viewed = (viewedStr == "1");
                    s.submitted = (submittedStr == "1");
                    s.completed = (completedStr == "1");
                    p.students[s.rollNo] = s;
                }
                subFile.close();
            }
            
            projects[p.id] = p;
            if(p.id >= nextProjectId) {
                nextProjectId = p.id + 1;
            }
        }
        file.close();
    }
    
    // Save projects
    void saveProjects() {
        ofstream file(projectsFile);
        for(auto& pair : projects) {
            Project& p = pair.second;
            file << p.id << "|" << p.title << "|" << p.description << "|" 
                 << p.pdfFile << "|" << p.uploadDate << "|" << p.className << "|"
                 << p.facultyName << "\n";
            
            // Save submissions
            string submFile = "submissions_" + to_string(p.id) + ".txt";
            ofstream subFile(submFile);
            for(auto& sPair : p.students) {
                Student& s = sPair.second;
                subFile << s.rollNo << "|" << s.name << "|" << s.className << "|"
                       << (s.viewed ? "1" : "0") << "|"
                       << (s.submitted ? "1" : "0") << "|"
                       << (s.completed ? "1" : "0") << "|"
                       << s.submissionFile << "\n";
            }
            subFile.close();
        }
        file.close();
    }
    
    // Login with captcha
    bool login() {
        srand(time(0));
        string username, password, captcha, inputCaptcha;
        int attempts = 0;
        
        while(attempts < 3) {
            cout << "\n=== CampusHub Login ===\n";
            cout << "Username: ";
            cin >> username;
            cout << "Password: ";
            cin >> password;
            
            // Generate and verify captcha
            captcha = generateCaptcha();
            cout << "\nCAPTCHA: " << captcha << "\n";
            cout << "Enter CAPTCHA: ";
            cin >> inputCaptcha;
            
            if(captcha != inputCaptcha) {
                cout << "Invalid CAPTCHA! Try again.\n";
                attempts++;
                continue;
            }
            
            // Verify credentials
            if(users.find(username) != users.end() && users[username].password == password) {
                currentUser = users[username];
                cout << "\nLogin successful! Welcome " << currentUser.name << "\n";
                return true;
            } else {
                cout << "Invalid credentials! Try again.\n";
                attempts++;
            }
        }
        
        cout << "Too many failed attempts. Exiting...\n";
        return false;
    }
    
    // Register new user
    void registerUser() {
        User u;
        cout << "\n=== User Registration ===\n";
        cout << "Username: ";
        cin >> u.username;
        
        if(users.find(u.username) != users.end()) {
            cout << "Username already exists!\n";
            return;
        }
        
        cout << "Password: ";
        cin >> u.password;
        cout << "Full Name: ";
        cin.ignore();
        getline(cin, u.name);
        cout << "Role (faculty/student): ";
        cin >> u.role;
        
        if(u.role == "student") {
            cout << "Class Name (e.g., CS101): ";
            cin >> u.className;
        }
        
        users[u.username] = u;
        saveUsers();
        cout << "Registration successful!\n";
    }
    
    // Get current date/time
    string getCurrentDateTime() {
        time_t now = time(0);
        char buf[80];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
        return string(buf);
    }
    
    // Faculty: Upload project
    void uploadProject() {
        Project p;
        p.id = nextProjectId++;
        p.facultyName = currentUser.name;
        
        cout << "\n=== Upload New Project ===\n";
        cout << "Project Title: ";
        cin.ignore();
        getline(cin, p.title);
        cout << "Description: ";
        getline(cin, p.description);
        cout << "PDF File Path: ";
        getline(cin, p.pdfFile);
        cout << "Class Name (e.g., CS101): ";
        cin >> p.className;
        
        p.uploadDate = getCurrentDateTime();
        
        // Initialize students from class
        if(classStudents.find(p.className) != classStudents.end()) {
            for(auto& student : classStudents[p.className]) {
                p.students[student.rollNo] = student;
            }
        }
        
        projects[p.id] = p;
        saveProjects();
        
        cout << "\nProject uploaded successfully! Project ID: " << p.id << "\n";
    }
    
    // Faculty: View all projects
    void viewFacultyProjects() {
        cout << "\n=== Your Projects ===\n";
        bool found = false;
        
        for(auto& pair : projects) {
            Project& p = pair.second;
            if(p.facultyName == currentUser.name) {
                found = true;
                cout << "\nProject ID: " << p.id << "\n";
                cout << "Title: " << p.title << "\n";
                cout << "Class: " << p.className << "\n";
                cout << "Upload Date: " << p.uploadDate << "\n";
                cout << "Description: " << p.description << "\n";
                
                int viewed = 0, submitted = 0, completed = 0;
                for(auto& sPair : p.students) {
                    if(sPair.second.viewed) viewed++;
                    if(sPair.second.submitted) submitted++;
                    if(sPair.second.completed) completed++;
                }
                
                cout << "Students: " << p.students.size() 
                     << " | Viewed: " << viewed 
                     << " | Submitted: " << submitted
                     << " | Completed: " << completed << "\n";
                cout << string(50, '-') << "\n";
            }
        }
        
        if(!found) {
            cout << "No projects found.\n";
        }
    }
    
    // Faculty: View project details with student status
    void viewProjectDetails() {
        int projectId;
        cout << "\nEnter Project ID: ";
        cin >> projectId;
        
        if(projects.find(projectId) == projects.end()) {
            cout << "Project not found!\n";
            return;
        }
        
        Project& p = projects[projectId];
        
        if(p.facultyName != currentUser.name) {
            cout << "Access denied! This is not your project.\n";
            return;
        }
        
        cout << "\n=== Project Details ===\n";
        cout << "Title: " << p.title << "\n";
        cout << "Class: " << p.className << "\n";
        cout << "PDF: " << p.pdfFile << "\n\n";
        
        cout << "Sort by: (1) Name (2) Viewed (3) Submitted (4) Completed\n";
        int sortChoice;
        cin >> sortChoice;
        
        vector<Student> studentList;
        for(auto& sPair : p.students) {
            studentList.push_back(sPair.second);
        }
        
        // Sort based on choice
        if(sortChoice == 1) {
            sort(studentList.begin(), studentList.end(), 
                 [](Student& a, Student& b) { return a.name < b.name; });
        } else if(sortChoice == 2) {
            sort(studentList.begin(), studentList.end(), 
                 [](Student& a, Student& b) { return a.viewed > b.viewed; });
        } else if(sortChoice == 3) {
            sort(studentList.begin(), studentList.end(), 
                 [](Student& a, Student& b) { return a.submitted > b.submitted; });
        } else if(sortChoice == 4) {
            sort(studentList.begin(), studentList.end(), 
                 [](Student& a, Student& b) { return a.completed > b.completed; });
        }
        
        cout << "\n" << setw(15) << "Roll No" << setw(25) << "Name" 
             << setw(10) << "Viewed" << setw(12) << "Submitted" 
             << setw(12) << "Completed" << "\n";
        cout << string(74, '-') << "\n";
        
        for(auto& s : studentList) {
            cout << setw(15) << s.rollNo 
                 << setw(25) << s.name 
                 << setw(10) << (s.viewed ? "Yes" : "No")
                 << setw(12) << (s.submitted ? "Yes" : "No")
                 << setw(12) << (s.completed ? "✓" : "-") << "\n";
        }
        
        // Option to mark as completed
        cout << "\nMark student as completed? (y/n): ";
        char choice;
        cin >> choice;
        
        if(choice == 'y' || choice == 'Y') {
            string rollNo;
            cout << "Enter Student Roll No: ";
            cin >> rollNo;
            
            if(p.students.find(rollNo) != p.students.end()) {
                p.students[rollNo].completed = !p.students[rollNo].completed;
                saveProjects();
                cout << "Status updated!\n";
            } else {
                cout << "Student not found!\n";
            }
        }
    }
    
    // Faculty: View submissions for a project
    void viewSubmissions() {
        int projectId;
        cout << "\nEnter Project ID: ";
        cin >> projectId;
        
        if(projects.find(projectId) == projects.end()) {
            cout << "Project not found!\n";
            return;
        }
        
        Project& p = projects[projectId];
        
        if(p.facultyName != currentUser.name) {
            cout << "Access denied!\n";
            return;
        }
        
        cout << "\n=== Submissions for: " << p.title << " ===\n\n";
        
        bool hasSubmissions = false;
        for(auto& sPair : p.students) {
            Student& s = sPair.second;
            if(s.submitted) {
                hasSubmissions = true;
                cout << "Roll No: " << s.rollNo << "\n";
                cout << "Name: " << s.name << "\n";
                cout << "Submission File: " << s.submissionFile << "\n";
                cout << "Completed: " << (s.completed ? "Yes ✓" : "No") << "\n";
                cout << string(50, '-') << "\n";
            }
        }
        
        if(!hasSubmissions) {
            cout << "No submissions yet.\n";
        }
    }
    
    // Student: View available projects
    void viewStudentProjects() {
        cout << "\n=== Available Projects ===\n";
        bool found = false;
        
        for(auto& pair : projects) {
            Project& p = pair.second;
            if(p.className == currentUser.className) {
                found = true;
                cout << "\nProject ID: " << p.id << "\n";
                cout << "Title: " << p.title << "\n";
                cout << "Description: " << p.description << "\n";
                cout << "Upload Date: " << p.uploadDate << "\n";
                cout << "Faculty: " << p.facultyName << "\n";
                
                // Find student's roll number from users
                string rollNo = "";
                for(auto& uPair : users) {
                    if(uPair.second.username == currentUser.username && 
                       uPair.second.role == "student") {
                        rollNo = uPair.first; // Using username as rollNo for simplicity
                        break;
                    }
                }
                
                if(!rollNo.empty() && p.students.find(rollNo) != p.students.end()) {
                    Student& s = p.students[rollNo];
                    cout << "Your Status - Viewed: " << (s.viewed ? "Yes" : "No")
                         << " | Submitted: " << (s.submitted ? "Yes" : "No")
                         << " | Completed: " << (s.completed ? "Yes ✓" : "No") << "\n";
                }
                
                cout << string(50, '-') << "\n";
            }
        }
        
        if(!found) {
            cout << "No projects available for your class.\n";
        }
    }
    
    // Student: View project and mark as viewed
    void viewProject() {
        int projectId;
        cout << "\nEnter Project ID to view: ";
        cin >> projectId;
        
        if(projects.find(projectId) == projects.end()) {
            cout << "Project not found!\n";
            return;
        }
        
        Project& p = projects[projectId];
        
        if(p.className != currentUser.className) {
            cout << "This project is not for your class!\n";
            return;
        }
        
        cout << "\n=== Project Details ===\n";
        cout << "Title: " << p.title << "\n";
        cout << "Description: " << p.description << "\n";
        cout << "PDF File: " << p.pdfFile << "\n";
        cout << "Faculty: " << p.facultyName << "\n\n";
        cout << "You are viewing the project PDF...\n";
        
        // Mark as viewed - find student by username
        for(auto& sPair : p.students) {
            if(sPair.second.name == currentUser.name) {
                p.students[sPair.first].viewed = true;
                saveProjects();
                cout << "Project marked as viewed.\n";
                break;
            }
        }
    }
    
    // Student: Submit project
    void submitProject() {
        int projectId;
        cout << "\nEnter Project ID to submit: ";
        cin >> projectId;
        
        if(projects.find(projectId) == projects.end()) {
            cout << "Project not found!\n";
            return;
        }
        
        Project& p = projects[projectId];
        
        if(p.className != currentUser.className) {
            cout << "This project is not for your class!\n";
            return;
        }
        
        string submissionFile;
        cout << "Enter your submission file path: ";
        cin.ignore();
        getline(cin, submissionFile);
        
        // Find and update student
        for(auto& sPair : p.students) {
            if(sPair.second.name == currentUser.name) {
                p.students[sPair.first].submitted = true;
                p.students[sPair.first].submissionFile = submissionFile;
                p.students[sPair.first].viewed = true;
                saveProjects();
                cout << "\nSubmission successful!\n";
                return;
            }
        }
        
        cout << "Could not find your record in this project.\n";
    }
    
    // Faculty Menu
    void facultyMenu() {
        int choice;
        do {
            cout << "\n=== Faculty Menu ===\n";
            cout << "1. Upload Project\n";
            cout << "2. View My Projects\n";
            cout << "3. View Project Details & Student Status\n";
            cout << "4. View Submissions\n";
            cout << "5. Logout\n";
            cout << "Choice: ";
            cin >> choice;
            
            switch(choice) {
                case 1: uploadProject(); break;
                case 2: viewFacultyProjects(); break;
                case 3: viewProjectDetails(); break;
                case 4: viewSubmissions(); break;
                case 5: cout << "Logging out...\n"; break;
                default: cout << "Invalid choice!\n";
            }
        } while(choice != 5);
    }
    
    // Student Menu
    void studentMenu() {
        int choice;
        do {
            cout << "\n=== Student Menu ===\n";
            cout << "1. View Available Projects\n";
            cout << "2. View Project Details\n";
            cout << "3. Submit Project\n";
            cout << "4. Logout\n";
            cout << "Choice: ";
            cin >> choice;
            
            switch(choice) {
                case 1: viewStudentProjects(); break;
                case 2: viewProject(); break;
                case 3: submitProject(); break;
                case 4: cout << "Logging out...\n"; break;
                default: cout << "Invalid choice!\n";
            }
        } while(choice != 4);
    }
    
    // Main run function
    void run() {
        int choice;
        
        // Create sample class files on first run
        createSampleClassFiles();
        loadAllClasses();
        
        do {
            cout << "\n=== CampusHub ===\n";
            cout << "1. Login\n";
            cout << "2. Register\n";
            cout << "3. Exit\n";
            cout << "Choice: ";
            cin >> choice;
            
            switch(choice) {
                case 1:
                    if(login()) {
                        if(currentUser.role == "faculty") {
                            facultyMenu();
                        } else {
                            studentMenu();
                        }
                    }
                    break;
                case 2:
                    registerUser();
                    break;
                case 3:
                    cout << "Thank you for using CampusHub!\n";
                    break;
                default:
                    cout << "Invalid choice!\n";
            }
        } while(choice != 3);
    }
};

int main() {
    CampusHub hub;
    hub.run();
    return 0;
}