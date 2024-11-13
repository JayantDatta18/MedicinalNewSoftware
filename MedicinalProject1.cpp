#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>  // For working with dates        
#include <iomanip> // For formatting output

using namespace std;
 
 int localtime_s(tm *current,long *now);
struct Medicine {
    string name;
    string med_type;   // Tablet, Syrup, etc.
    double price;
    int quantity;
    string expiry_date;

    Medicine() = default;
    Medicine(string n, string t, double p, int q, string e)
        : name(n), med_type(t), price(p), quantity(q), expiry_date(e) {}
};

vector<Medicine> medicines;  // Main inventory list

// Helper function to get the current date (used for expiration check)
tm getCurrentDate() {
    time_t now = time(0);
    tm current;
    localtime_s(&current,&now);
    return current;
}

// Function to check if a medicine is expiring soon (within a month)
bool isExpiringSoon(const Medicine &med) {
    int expiry_year, expiry_month, expiry_day;
    sscanf(med.expiry_date.c_str(), "%d-%d-%d", &expiry_year, &expiry_month, &expiry_day);

    tm current_date = getCurrentDate();

    // Check if the expiry date is within 1 month from the current date
    if ((expiry_year == (current_date.tm_year + 1900)) && (expiry_month - (current_date.tm_mon + 1)) <= 1)
        return true;
    
    return false;
}

// Function to read medicine inventory from file
void loadMedicinesFromFile(const string &filename) {
    ifstream file(filename);
    if (file.is_open()) {
        medicines.clear();
        string name, type, expiry_date;
        double price;
        int quantity;

        while (file >> name >> type >> price >> quantity >> expiry_date) {
            medicines.emplace_back(name, type, price, quantity, expiry_date);
        }
        file.close();
    }
}

// Function to write medicine inventory to file
void saveMedicinesToFile(const string &filename) {
    ofstream file(filename);
    if (file.is_open()) {
        for (const auto &med : medicines) {
            file << med.name << " " << med.med_type << " " << med.price << " " 
                 << med.quantity << " " << med.expiry_date << endl;
        }
        file.close();
    }
}

// Function to update expiring medicines file
void updateExpiringMedicines(const string &filename) {
    ofstream file(filename);
    if (file.is_open()) {
        for (const auto &med : medicines) {
            if (isExpiringSoon(med)) {
                file << med.name << " " << med.med_type << " " << med.price << " " 
                     << med.quantity << " " << med.expiry_date << endl;
            }
        }
        file.close();
    }
}

// Function to update short stock medicines file
void updateShortStockMedicines(const string &filename, int threshold) {
    ofstream file(filename);
    if (file.is_open()) {
        for (const auto &med : medicines) {
            if (med.quantity <= threshold) {
                file << med.name << " " << med.med_type << " " << med.price << " " 
                     << med.quantity << " " << med.expiry_date << endl;
            }
        }
        file.close();
    }
}

// Function to add a new medicine
void addMedicine() {
    string name, type, expiry_date;
    double price;
    int quantity;

    cout << "Enter medicine name: ";
    cin >> name;
    cout << "Enter medicine type (e.g., Tablet, Syrup): ";
    cin >> type;
    cout << "Enter price: ";
    cin >> price;
    cout << "Enter quantity: ";
    cin >> quantity;
    cout << "Enter expiry date (YYYY-MM-DD): ";
    cin >> expiry_date;

    medicines.emplace_back(name, type, price, quantity, expiry_date);

    cout << "Medicine added successfully!" << endl;
}

// Function to update stock of a medicine
void updateStock() {
    string name;
    cout << "Enter medicine name to update stock: ";
    cin >> name;

    bool found = false;
    for (auto &med : medicines) {
        if (med.name == name) {
            cout << "Current quantity: " << med.quantity << endl;
            int new_quantity;
            cout << "Enter new quantity: ";
            cin >> new_quantity;
            med.quantity = new_quantity;
            found = true;
            cout << "Stock updated successfully!" << endl;
            break;
        }
    }

    if (!found) {
        cout << "Medicine not found!" << endl;
    }
}

// Function to display the medicine inventory
void displayInventory() {
    cout << left << setw(15) << "Name" << setw(15) << "Type" << setw(10) << "Price" 
         << setw(10) << "Quantity" << setw(15) << "Expiry Date" << endl;
    cout << "-------------------------------------------------------------" << endl;

    for (const auto &med : medicines) {
        cout << left << setw(15) << med.name << setw(15) << med.med_type 
             << setw(10) << med.price << setw(10) << med.quantity << setw(15) << med.expiry_date << endl;
    }
}

// Function to show medicines that will expire soon
void displayExpiringMedicines() {
    cout << "\nMedicines expiring soon:" << endl;
    cout << left << setw(15) << "Name" << setw(15) << "Type" << setw(10) << "Price" 
         << setw(10) << "Quantity" << setw(15) << "Expiry Date" << endl;
    cout << "-------------------------------------------------------------" << endl;

    for (const auto &med : medicines) {
        if (isExpiringSoon(med)) {
            cout << left << setw(15) << med.name << setw(15) << med.med_type 
                 << setw(10) << med.price << setw(10) << med.quantity << setw(15) << med.expiry_date << endl;
        }
    }
}

int main() {
    const string inventory_file = "medicines.txt";
    const string expiring_file = "expiring_medicines.txt";
    const string short_stock_file = "short_stock_medicines.txt";
    const int stock_threshold = 10;

    // Load medicines from file at start
    loadMedicinesFromFile(inventory_file);

    int choice;
    do {
        cout << "\nPharmaceutical Medicine Management System\n";
        cout << "1. Add New Medicine\n";
        cout << "2. Update Medicine Stock\n";
        cout << "3. Display Medicine Inventory\n";
        cout << "4. Display Expiring Medicines\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                addMedicine();
                break;
            case 2:
                updateStock();
                break;
            case 3:
                displayInventory();
                break;
            case 4:
                displayExpiringMedicines();
                break;
            case 5:
                cout << "Exiting system...\n";
                break;
            default:
                cout << "Invalid choice, please try again.\n";
        }

        // Save updated medicines to file after every operation
        saveMedicinesToFile(inventory_file);

        // Update expiring and short stock files
        updateExpiringMedicines(expiring_file);
        updateShortStockMedicines(short_stock_file, stock_threshold);

    } while (choice != 5);

    return 0;
}
