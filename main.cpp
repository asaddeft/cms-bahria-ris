// Simple College Management System - main.cpp
// All logic in a single file. Student records in students.csv, faculty login in FACULTY_LOGIN.txt
// Compile: g++ main.cpp -o cms.exe
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
using namespace std;

struct Student {
    string roll, name, semester, course, password;
    float obtainedGPA;
};

struct Faculty {
    string id, name, pass;
};
// Utility: Load faculty from Faculty_Login.csv
void loadFaculty(vector<Faculty>& faculties) {
    faculties.clear();
    ifstream fin("Faculty_Login.csv");
    string line;
    while (getline(fin, line)) {
        stringstream ss(line);
        Faculty faculty;
        getline(ss, faculty.id, ',');
        getline(ss, faculty.name, ',');
        getline(ss, faculty.pass, ',');
        // Remove possible whitespace and newline characters
        faculty.id.erase(0, faculty.id.find_first_not_of(" \t\r\n")); faculty.id.erase(faculty.id.find_last_not_of(" \t\r\n")+1);
        faculty.name.erase(0, faculty.name.find_first_not_of(" \t\r\n")); faculty.name.erase(faculty.name.find_last_not_of(" \t\r\n")+1);
        faculty.pass.erase(0, faculty.pass.find_first_not_of(" \t\r\n")); faculty.pass.erase(faculty.pass.find_last_not_of(" \t\r\n")+1);
        if (!faculty.id.empty() && !faculty.pass.empty()) faculties.push_back(faculty);
    }
    fin.close();
}
// Utility: Load students from CSV
void loadStudents(vector<Student>& students) {
    students.clear();
    ifstream fin("students.csv");
    string line;
    while (getline(fin, line)) {
        stringstream ss(line);
        Student s;
        getline(ss, s.roll, ',');
        getline(ss, s.name, ',');
        getline(ss, s.semester, ',');
        getline(ss, s.course, ',');
        string obt;
        getline(ss, obt, ',');
        s.obtainedGPA = obt.empty() ? 0 : stof(obt);
        getline(ss, s.password, ',');
        if (!s.roll.empty()) students.push_back(s);
    }
    fin.close();
}
// Utility: Save students to CSV
void saveStudents(const vector<Student>& students) {
    ofstream fout("students.csv");
    for (const auto& s : students) {
        fout << s.roll << ',' << s.name << ',' << s.semester << ',' << s.course << ',' << s.obtainedGPA << ',' << s.password << '\n';
    }
    fout.close();
}
// Utility: Calculate percentage
float calcPercent(float obtainedGPA) {
    return (obtainedGPA/4.0f)*100.0f;
}
// Pause
void pause() {
    cout << "\nPress Enter to continue...";
    cin.ignore();
}
// Student signup
void signupStudent(vector<Student>& students) {
    Student s;
    cout << "Enter Roll No: "; getline(cin, s.roll);
    cout << "Enter Name: "; getline(cin, s.name);
    cout << "Enter Semester: "; getline(cin, s.semester);
    cout << "Enter Course: "; getline(cin, s.course);
    s.obtainedGPA = 0;
    cout << "Set Password: "; getline(cin, s.password);
    students.push_back(s);
    saveStudents(students);
    cout << "Signup successful! Faculty will update your GPA later.\n";
    pause();
}
// Student login
bool loginStudent(const vector<Student>& students, Student& loggedIn) {
    string roll, pass;
    cout << "Enter Roll No: "; getline(cin, roll);
    cout << "Enter Password: "; getline(cin, pass);
    for (const auto& s : students) {
        if (s.roll == roll && s.password == pass) {
            loggedIn = s;
            cout << "Login successful!\n";
            return true;
        }
    }
    cout << "Invalid credentials.\n";
    pause();
    return false;
}
// Faculty login
bool loginFaculty(const vector<Faculty>& faculties) {
    string id, pass;
    cout << "Enter Faculty ID: "; getline(cin, id);
    cout << "Enter Password: "; getline(cin, pass);
    for (const auto& faculty : faculties) {
        if (id == faculty.id && pass == faculty.pass) {
            cout << "Welcome, " << faculty.name << "!\n";
            return true;
        }
    }
    cout << "Invalid credentials.\n";
    pause();
    return false;
}
// Student menu
void studentMenu(const Student& student, vector<Student>& mainStudents) {
    vector<Student> students;
    loadStudents(students);
    int idx = -1;
    for (int i = 0; i < students.size(); ++i) {
        if (students[i].roll == student.roll) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        cout << "Student record not found.\n";
        pause();
        return;
    }
    cout << "\nWelcome, " << students[idx].name << "!\n";
    while (true) {
        cout << "\n--- Student Panel ---\n1. View Record\n2. Change Password\n3. Logout\nChoice: ";
        int ch; cin >> ch; cin.ignore();
        if (ch == 1) {
            cout << "\n--- Student Record ---\n";
            cout << "Roll No: " << students[idx].roll << "\nName: " << students[idx].name << "\nSemester: " << students[idx].semester << "\nCourse: " << students[idx].course << "\nTotal GPA: 4\nObtained GPA: " << students[idx].obtainedGPA << "\nPercentage: " << fixed << setprecision(2) << calcPercent(students[idx].obtainedGPA) << "%\n";
            pause();
        } else if (ch == 2) {
            string oldPass, newPass, confirmPass;
            cout << "Enter Current Password: "; getline(cin, oldPass);
            if (oldPass != students[idx].password) {
                cout << "Incorrect current password.\n";
                pause();
                continue;
            }
            cout << "Enter New Password: "; getline(cin, newPass);
            cout << "Confirm New Password: "; getline(cin, confirmPass);
            if (newPass != confirmPass) {
                cout << "Passwords do not match.\n";
                pause();
                continue;
            }
            students[idx].password = newPass;
            saveStudents(students);
            // Update mainStudents vector as well
            for (auto& ms : mainStudents) {
                if (ms.roll == students[idx].roll) {
                    ms.password = newPass;
                    break;
                }
            }
            cout << "Password updated successfully!\n";
            pause();
        } else {
            break;
        }
    }
}
// Faculty menu
void facultyMenu(vector<Student>& students) {
    while (true) {
        cout << "\n--- Faculty Menu ---\n1. Add Student\n2. View All Students\n3. Edit Student\n4. Delete Student\n5. Logout\nChoice: ";
        int ch; cin >> ch; cin.ignore();
        if (ch == 1) {
            Student s;
            cout << "Enter Roll No: "; getline(cin, s.roll);
            cout << "Enter Name: "; getline(cin, s.name);
            cout << "Enter Semester: "; getline(cin, s.semester);
            cout << "Enter Course: "; getline(cin, s.course);
            cout << "Enter Obtained GPA (0-4): "; cin >> s.obtainedGPA; cin.ignore();
            s.password = "1234"; // Default password, student should change after login
            students.push_back(s);
            saveStudents(students);
            cout << "Student added. Default password: 1234\n";
            pause();
        } else if (ch == 2) {
            cout << "\n--- Student Records ---\n";
            for (const auto& s : students) {
                cout << "Roll No: " << s.roll << ", Name: " << s.name << ", Semester: " << s.semester << ", Course: " << s.course << ", Total GPA: 4, Obtained GPA: " << s.obtainedGPA << ", %: " << fixed << setprecision(2) << calcPercent(s.obtainedGPA) << endl;
            }
            pause();
        } else if (ch == 3) {
            string roll; cout << "Enter Roll No to Edit: "; getline(cin, roll);
            bool found = false;
            for (auto& s : students) {
                if (s.roll == roll) {
                    cout << "\n--- Current Student Data ---\n";
                    cout << "Roll No: " << s.roll << "\nName: " << s.name << "\nSemester: " << s.semester << "\nCourse: " << s.course << "\nObtained GPA: " << s.obtainedGPA << "\n";
                    while (true) {
                        cout << "\nSelect field to update:\n1. Name\n2. Roll No\n3. Semester\n4. Course\n5. Obtained GPA\n6. Password\n7. Done\nChoice: ";
                        int fieldChoice; cin >> fieldChoice; cin.ignore();
                        if (fieldChoice == 1) {
                            cout << "Enter New Name: "; getline(cin, s.name);
                        } else if (fieldChoice == 2) {
                            cout << "Enter New Roll No: "; getline(cin, s.roll);
                        } else if (fieldChoice == 3) {
                            cout << "Enter New Semester: "; getline(cin, s.semester);
                        } else if (fieldChoice == 4) {
                            cout << "Enter New Course: "; getline(cin, s.course);
                        } else if (fieldChoice == 5) {
                            cout << "Enter New Obtained GPA (0-4): "; cin >> s.obtainedGPA; cin.ignore();
                        } else if (fieldChoice == 6) {
                            cout << "Enter New Password: "; getline(cin, s.password);
                        } else if (fieldChoice == 7) {
                            break;
                        } else {
                            cout << "Invalid choice. Try again.\n";
                        }
                    }
                    found = true;
                    break;
                }
            }
            if (found) {
                saveStudents(students);
                cout << "Updated.\n";
            } else cout << "Not found.\n";
            pause();
        } else if (ch == 4) {
            string roll; cout << "Enter Roll No to Delete: "; getline(cin, roll);
            bool found = false;
            for (auto it = students.begin(); it != students.end(); ++it) {
                if (it->roll == roll) {
                    students.erase(it);
                    found = true;
                    break;
                }
            }
            if (found) {
                saveStudents(students);
                cout << "Deleted.\n";
            } else cout << "Not found.\n";
            pause();
        } else break;
    }
}
// Main
int main() {
    // Ensure faculty login file exists
    ifstream test("Faculty_Login.csv");
    if (!test.good()) {
        ofstream fout("Faculty_Login.csv");
        fout << "14812,altahanrauf,2005\n";
        fout.close();
    }
    test.close();
    vector<Student> students;
    vector<Faculty> faculties;
    loadStudents(students);
    loadFaculty(faculties);
    while (true) {
        cout << "\n===== College Management System =====\n1. Faculty Login\n2. Student Login\n3. Student Signup\nOther. Exit\nEnter choice: ";
        int choice; cin >> choice; cin.ignore();
        if (choice == 1) {
            if (loginFaculty(faculties)) {
                facultyMenu(students);
                saveStudents(students);
            }
        } else if (choice == 2) {
            Student loggedIn;
            if (loginStudent(students, loggedIn)) {
                studentMenu(loggedIn, students);
            }
        } else if (choice == 3) {
            signupStudent(students);
        } else {
            cout << "Exiting...\n";
            break;
        }
    }
    return 0;
}