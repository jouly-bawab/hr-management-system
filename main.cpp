/**
 * @file hr_management_system.cpp
 * @brief Comprehensive Human Resources (HR) Management System
 * @version 1.0
 * * This project showcases core C++ capabilities including Object-Oriented Programming (OOP),
 * encapsulation, file I/O streams, input validation, and data manipulation using standard containers.
 * Designed as a showcase piece for freelance portfolios.
 */

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <algorithm>

// ==========================================
// 1. DATA MODEL (Employee Class)
// ==========================================
class Employee {
private:
    std::string id;
    std::string name;
    std::string position;
    double salary;

public:
    // Constructor
    Employee(std::string id, std::string name, std::string position, double salary)
        : id(std::move(id)), name(std::move(name)), position(std::move(position)), salary(salary) {}

    // Getters (Encapsulation)
    std::string getId() const { return id; }
    std::string getName() const { return name; }
    std::string getPosition() const { return position; }
    double getSalary() const { return salary; }

    // Setters
    void setName(const std::string& newName) { name = newName; }
    void setPosition(const std::string& newPosition) { position = newPosition; }
    void setSalary(double newSalary) { salary = newSalary; }

    // Serialize employee data to a CSV format line
    std::string toCSV() const {
        return id + "," + name + "," + position + "," + std::to_string(salary);
    }
};

// ==========================================
// 2. INPUT VALIDATION UTILITIES
// ==========================================
class InputValidator {
public:
    // Ensures strings are not empty or just whitespace
    static std::string getValidString(const std::string& prompt) {
        std::string value;
        while (true) {
            std::cout << prompt;
            std::getline(std::cin, value);
            
            // Trim leading/trailing whitespace checks can be added, basic empty check here
            if (!value.empty() && value.find_first_not_of(" \t\n\v\f\r") != std::string::npos) {
                return value;
            }
            std::cout << "[Error] Input cannot be empty. Please try again.\n";
        }
    }

    // Ensures numeric double input is positive and valid
    static double getValidDouble(const std::string& prompt) {
        double value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value && value > 0) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            }
            std::cout << "[Error] Invalid input. Please enter a positive numeric value.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    // Ensures the menu choice is a valid integer
    static int getValidChoice() {
        int choice;
        while (!(std::cin >> choice)) {
            std::cout << "[Error] Invalid choice. Enter a number: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clear trailing newline
        return choice;
    }
};

// ==========================================
// 3. HR SYSTEM CONTROLLER (CRUD Operations)
// ==========================================
class HRSystem {
private:
    std::vector<Employee> employees;
    const std::string filename = "employee_records.csv";

    // Helper to find employee index by ID
    int findEmployeeIndex(const std::string& id) {
        for (size_t i = 0; i < employees.size(); ++i) {
            if (employees[i].getId() == id) {
                return static_cast<int>(i);
            }
        }
        return -1; // Not found
    }

    // Load data from file automatically on startup
    void loadFromFile() {
        std::ifstream file(filename);
        if (!file.is_open()) return; // File doesn't exist yet, which is fine for first run

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            std::stringstream ss(line);
            std::string id, name, position, salaryStr;

            if (std::getline(ss, id, ',') &&
                std::getline(ss, name, ',') &&
                std::getline(ss, position, ',') &&
                std::getline(ss, salaryStr, ',')) {
                try {
                    double salary = std::stod(salaryStr);
                    employees.emplace_back(id, name, position, salary);
                } catch (...) {
                    // Ignore corrupted lines
                }
            }
        }
        file.close();
    }

    // Save data back to the file
    void saveToFile() const {
        std::ofstream file(filename, std::ios::trunc);
        if (!file.is_open()) {
            std::cout << "[Error] Could not save data to database file.\n";
            return;
        }
        for (const auto& emp : employees) {
            file << emp.toCSV() << "\n";
        }
        file.close();
    }

public:
    HRSystem() {
        loadFromFile();
    }

    ~HRSystem() {
        saveToFile();
    }

    // CREATE
    void addEmployee() {
        std::cout << "\n--- Add New Employee ---\n";
        std::string id = InputValidator::getValidString("Enter Employee ID: ");

        if (findEmployeeIndex(id) != -1) {
            std::cout << "[Error] An employee with ID " << id << " already exists!\n";
            return;
        }

        std::string name = InputValidator::getValidString("Enter Employee Name: ");
        std::string position = InputValidator::getValidString("Enter Position: ");
        double salary = InputValidator::getValidDouble("Enter Salary ($): ");

        employees.emplace_back(id, name, position, salary);
        saveToFile();
        std::cout << "[Success] Employee added successfully!\n";
    }

    // READ (All)
    void displayAllEmployees() const {
        std::cout << "\n--- Employee Directory ---\n";
        if (employees.empty()) {
            std::cout << "No records found.\n";
            return;
        }

        // Print formatted Table Header
        std::cout << std::left << std::setw(12) << "ID" 
                  << std::setw(25) << "Name" 
                  << std::setw(20) << "Position" 
                  << std::setw(15) << "Salary ($)" << "\n";
        std::cout << std::string(72, '-') << "\n";

        // Print data rows
        for (const auto& emp : employees) {
            std::cout << std::left << std::setw(12) << emp.getId()
                      << std::setw(25) << emp.getName()
                      << std::setw(20) << emp.getPosition()
                      << std::fixed << std::setprecision(2) << std::setw(15) << emp.getSalary() << "\n";
        }
    }

    // UPDATE
    void updateEmployee() {
        std::cout << "\n--- Update Employee Record ---\n";
        std::string id = InputValidator::getValidString("Enter Employee ID to update: ");
        
        int index = findEmployeeIndex(id);
        if (index == -1) {
            std::cout << "[Error] Employee with ID " << id << " not found.\n";
            return;
        }

        std::cout << "\nRecord Found. Leave input blank and press Enter to keep current value.\n";
        
        // Dynamic name update
        std::cout << "Current Name [" << employees[index].getName() << "]. ";
        std::string newName;
        std::getline(std::cin, newName);
        if (!newName.empty()) employees[index].setName(newName);

        // Dynamic position update
        std::cout << "Current Position [" << employees[index].getPosition() << "]. ";
        std::string newPos;
        std::getline(std::cin, newPos);
        if (!newPos.empty()) employees[index].setPosition(newPos);

        // Dynamic salary update
        std::cout << "Current Salary [$" << std::fixed << std::setprecision(2) << employees[index].getSalary() << "].\n";
        std::cout << "Do you want to update the salary? (y/n): ";
        char choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        if (choice == 'y' || choice == 'Y') {
            double newSalary = InputValidator::getValidDouble("Enter New Salary ($): ");
            employees[index].setSalary(newSalary);
        }

        saveToFile();
        std::cout << "[Success] Employee record updated successfully!\n";
    }

    // DELETE
    void deleteEmployee() {
        std::cout << "\n--- Delete Employee Record ---\n";
        std::string id = InputValidator::getValidString("Enter Employee ID to delete: ");

        int index = findEmployeeIndex(id);
        if (index == -1) {
            std::cout << "[Error] Employee with ID " << id << " not found.\n";
            return;
        }

        // Erase element using modern iterator erasure pattern
        employees.erase(employees.begin() + index);
        saveToFile();
        std::cout << "[Success] Employee record deleted successfully.\n";
    }
};

// ==========================================
// 4. MAIN ENTRY POINT (Menu Logic)
// ==========================================
int main() {
    HRSystem hr;
    int choice = 0;

    while (true) {
        std::cout << "\n====================================\n";
        std::cout << "    HR MANAGEMENT SYSTEM CONSOLE    \n";
        std::cout << "====================================\n";
        std::cout << "1. Add New Employee\n";
        std::cout << "2. Display All Employees\n";
        std::cout << "3. Update Employee Record\n";
        std::cout << "4. Delete Employee Record\n";
        std::cout << "5. Exit System\n";
        std::cout << "------------------------------------\n";
        std::cout << "Enter your choice (1-5): ";

        choice = InputValidator::getValidChoice();

        switch (choice) {
            case 1:
                hr.addEmployee();
                break;
            case 2:
                hr.displayAllEmployees();
                break;
            case 3:
                hr.updateEmployee();
                break;
            case 4:
                hr.deleteEmployee();
                break;
            case 5:
                std::cout << "\nExiting system securely. All changes saved.\n";
                return 0;
            default:
                std::cout << "[Error] Invalid option selected. Please choose 1-5.\n";
        }
    }
}