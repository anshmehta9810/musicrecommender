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

void read_data(const UserData& userInput, vector<string>& matchage) {
    ifstream fin("uu.csv");
    if (!fin.is_open()) {
        cerr << "Error opening file.\n";
        exit(EXIT_FAILURE);
    }

    vector<string> row;
    string line, word;
    bool skipFirstRow = true;

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

        if (row.size() > 2) {
            if (row[6] == userInput.genre && row[4] == userInput.prof && row[5] == userInput.location) {
                matchage.push_back(row[0]);
            }
        }
    }

    fin.close();
}

void calculate_similarity(const UserData& userInput, const vector<string>& matchingPIDs) {
    ifstream fin("uu.csv");
    if (!fin.is_open()) {
        cerr << "Error opening file.\n";
        exit(EXIT_FAILURE);
    }

    vector<string> row;
    string line, word;
    bool skipFirstRow = true;

    double dgenre = 0.4;
    double dprof = 0.3;
    double dlocation = 0.3;

    double totalWeight = dgenre + dprof + dlocation;

    map<string, double> similarityScores;  // Store PID -> Similarity Score

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

        // Check if the PID is in the matchingPIDs array
        if (row.size() >= 8 && find(matchingPIDs.begin(), matchingPIDs.end(), row[0]) != matchingPIDs.end()) {
            string dbGenre = row[6];
            string dbProf = row[4];
            string dbLocation = row[5];

            // Check the order of fields in your dataset and adjust the comparisons accordingly
            double genreSimilarity = (userInput.genre == dbGenre) ? dgenre : 0.0;
            double profSimilarity = (userInput.prof == dbProf) ? dprof : 0.0;
            double locationSimilarity = (userInput.location == dbLocation) ? dlocation : 0.0;

            // Calculate the total similarity for this row
            double rowSimilarity = (genreSimilarity + profSimilarity + locationSimilarity) / totalWeight;

            // Store the similarity score in the map with the PID as the key
            similarityScores[row[0]] = rowSimilarity;
        }
    }
    fin.close();

    // Print similarity scores for the matching PIDs
    cout << "Matching PIDs and their Similarity Scores:\n";
    for (const auto& pair : similarityScores) {
        cout << "PID: " << pair.first << ", Similarity Score: " << pair.second << endl;
    }
}

int main() {
    UserData userInput;

    cout << "Enter your favorite genre: ";
    cin >> userInput.genre;

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