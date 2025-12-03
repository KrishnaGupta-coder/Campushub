# 📘 CampusHub – Project & Submission Management System
Lightweight | Secure | File-Based | Node.js + C++ Hybrid

---

## 📌 Overview

**CampusHub** is a lightweight Project and Assignment Submission Management System designed to streamline communication between **faculty** and **students**.  
It digitizes the complete assignment lifecycle — from **project upload** to **student submission tracking**, timestamp logging, secure authentication, and C++-based CLI access.

CampusHub is built to run efficiently on **local college networks**, without requiring heavy databases or servers.

---

## 🚀 Features

### 👨‍🏫 For Faculty
- Upload project documents (PDF/ZIP)
- Track student progress:
  - Viewed / Submitted / Completed status
- View class-wise dashboards
- Verify submissions with timestamps
- Manage multiple projects

### 👨‍🎓 For Students
- Secure login with CAPTCHA
- View assigned project files
- Upload submission files
- Instant submission confirmation
- Auto-updated status tracking

### 🧩 System Features
- Role-based authentication (Student/Faculty)
- CAPTCHA protection
- Multer-based secure file uploading
- Lightweight text-file database (TXT + JSON)
- Node.js REST API architecture
- C++ CLI module using fstream
- Zero SQL dependency → Ultra lightweight

---

## 🏗️ Tech Stack

### Backend
- Node.js  
- Express.js  
- Multer  
- Cookie-parser  

### Frontend
- HTML  
- CSS  
- JavaScript  

### Database
- Flat-file storage  
  - `users.txt`  
  - `projects.txt`  
  - `class_data.json`  

### Additional Module
- C++ (CLI Module)

---

## 📂 Project Structure

CampusHub/
│
├── server.js # Node.js Backend
├── campushub.cpp # C++ CLI Interface
│
├── public/
│ ├── login.html
│ ├── faculty.html
│ ├── student.html
│ ├── styles.css
│ └── script.js
│
├── data/
│ ├── users.txt
│ ├── projects.txt
│ └── class_data.json
│
└── uploads/ # Student submissions


---

## 🔐 Authentication Flow

1. User enters username/password  
2. CAPTCHA gets validated  
3. Role (Student/Faculty) identified  
4. Session cookie created  
5. Redirect to dashboard

---

## 🔄 Submission Flow

Student → Select File → Validate Type
→ Save in /uploads → Update JSON Status
→ Return ‘Submission Successful’


Filename format:
RollNo_ProjectID.pdf


---

## 📊 Data Flow Diagrams (DFD)

### Level 0 – Context
- Inputs: Login credentials, PDFs, ZIPs, submission files  
- Outputs: Dashboards, status updates, submission receipts  

### Level 1 – Major Processes
1. Authentication  
2. Project Upload  
3. Submission Handling  

### Level 2 – Submission Logic
- Validate → Rename → Store → JSON Update → Acknowledge  

---

## 🧮 UML Diagrams

### Use Case Diagram (Explanation)
Actors:
- Student  
- Faculty  

Use Cases:
- Login  
- View/Upload Project  
- Submit File  
- Track Status  
- Mark Completed  

### Class Diagram (Explanation)
- `User` (base class)  
- `Student` (inherits User)  
- `Faculty` (inherits User)  
- `Project` (manages project data)

---

## 🧪 Testing Performed

### ✔ Black Box Testing
- Incorrect CAPTCHA → Rejected  
- Invalid file type upload → Blocked  
- Unauthorized access → Blocked  

### ✔ White Box Testing
- Logical path validation  
- File I/O operations tested  

### ✔ Load Testing
- Simulated simultaneous uploads  

### ✔ C++ Sync Testing
- Web UI changes reflected in CLI  

---

## 📦 Installation & Setup

### 1️⃣ Clone Repository
```bash
git clone https://github.com/yourusername/CampusHub.git
cd CampusHub

### 2️⃣ Install Dependencies
npm install

### 3️⃣ Start Server
node server.js


Server runs at:

http://localhost:3000

### 4️⃣ Compile & Run C++ Module (Optional)
g++ campushub.cpp -o campushub
./campushub

## 🧭 SDLC Adopted: Agile Model
Sprints:

Backend API development

Frontend UI creation

C++ CLI module integration

Each sprint included planning → design → build → testing.

## 📈 Future Enhancements

Android/iOS mobile app

Email/SMS submission reminders

Cloud file storage (AWS S3 / Google Drive)

Chat/announcement system

AI plagiarism detection

Progress analytics dashboard

## 🧑‍💻 Team Members

Krishna Gupta

Mohit Sharma

Priyanshu Kumar

Koshi Jain

Jagriti Sahu

## 🎓 Guide / Mentor

Er. Manish Dwivedi
Associate Professor, AI & DS
Arya College of Engineering & I.T., Jaipur

## 📝 License

MIT License © 2025 CampusHub Development Team

## ⭐ Support the Project

If you like this project, please ⭐ star the repository!