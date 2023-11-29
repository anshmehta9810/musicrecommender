#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>

using namespace std;

struct UserData {
    int age;
    string gender;
    string prof;
    string location;
    string genre;
};

// Trim leading and trailing spaces from a string
string trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// Convert a string to lowercase
string toLowercase(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

void read_data(const UserData& userInput, vector<string>& matchage) {
    ifstream fin("uu.csv");
    if (!fin.is_open()) {
        cerr << "Error opening file.\n";
        exit(EXIT_FAILURE);
    }

    vector<string> row;
    string line, word;
    bool skipFirstRow = true;

    // Define age range parameters
    int ageLowerBound = userInput.age - 5;  // Adjust as needed
    int ageUpperBound = userInput.age + 5;  // Adjust as needed

    while (getline(fin, line)) {
        row.clear();
        stringstream s(line);

        while (getline(s, word, ',')) {
            row.push_back(trim(word));  // Trim spaces from each field
        }

        if (skipFirstRow) {
            skipFirstRow = false;
            continue;
        }

        if (row.size() > 3) {
            // Check if the user's gender, profession, location, and age range match
            if (toLowercase(row[3]) == toLowercase(userInput.gender) &&
                row[4] == userInput.prof &&
                row[5] == userInput.location &&
                stoi(row[2]) >= ageLowerBound && stoi(row[2]) <= ageUpperBound) {
                matchage.push_back(row[0]);
            }
        }
    }

    fin.close();
}


void calculate_similarity(const UserData& userInput, const vector<string>& matchingSerialIDs) {
    ifstream fin("uu.csv");
    if (!fin.is_open()) {
        cerr << "Error opening file.\n";
        exit(EXIT_FAILURE);
    }

    vector<string> row;
    string line, word;
    bool skipFirstRow = true;

    double dgen = 0.1;
    double dage = 0.3;
    double dprof = 0.3;
    double dlocation = 0.3;

    double totalWeight = dgen + dprof + dlocation + dage;

    map<string, double> similarityScores;  // Store SerialID -> Similarity Score

    while (getline(fin, line)) {
        row.clear();
        stringstream s(line);

        while (getline(s, word, ',')) {
            row.push_back(trim(word));  // Trim spaces from each field
        }

        if (skipFirstRow) {
            skipFirstRow = false;
            continue;
        }

        // Check if the SerialID is in the matchingSerialIDs array
        if (row.size() >= 7 && find(matchingSerialIDs.begin(), matchingSerialIDs.end(), row[0]) != matchingSerialIDs.end()) {
            // Assuming the order of fields is consistent, directly access the fields by index
            string dbGen = row[3];
            string dbage = row[2];
            string dbProf = row[4];
            string dbLocation = row[5];

            // Check the order of fields in your dataset and adjust the comparisons accordingly
            double genderSimilarity = (toLowercase(userInput.gender) == toLowercase(dbGen)) ? dgen : 0.0;
            double ageSimilarity = dage * (1.0 - abs(userInput.age - stoi(dbage))) / (80-13);
            double profSimilarity = (userInput.prof == dbProf) ? dprof : 0.0;
            double locationSimilarity = (userInput.location == dbLocation) ? dlocation : 0.0;

            // Calculate the total similarity for this row
            double rowSimilarity = (genderSimilarity + profSimilarity + locationSimilarity + ageSimilarity) / totalWeight;

            // Store the similarity score in the map with the SerialID as the key
            similarityScores[row[0]] = rowSimilarity;
        }
    }
    fin.close();

    // Print similarity scores for the matching SerialIDs
    cout << "Matching SerialIDs and their Similarity Scores:\n";
    for (const auto& pair : similarityScores) {
        cout << "SerialID: " << pair.first << ", Similarity Score: " << pair.second << endl;
    }
}

int main() {
    UserData userInput;

    cout << "Enter your gender: ";
    cin >> userInput.gender;

    cout << "Enter your Age : ";
    cin >> userInput.age;
    cout << "Enter your Profession: ";
    cin >> userInput.prof;

    cout << "Enter your location: ";
    cin >> userInput.location;

    vector<string> matchage;
    read_data(userInput, matchage);

    if (!matchage.empty()) {
        cout << "Matching Serial IDs:\n";
        for (const string& serialID : matchage) {
            cout << serialID << endl;
        }

        // Calculate similarity for the matching PIDs
        calculate_similarity(userInput, matchage);
    } else {
        cout << "No data matched.\n";
    }

    return 0;
}