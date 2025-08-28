#include<bits/stdc++.h>
using namespace std;
class JsonParser {
public:
    map<string, map<string, string>> data;
    map<string, string> keys;
    
    void parseJSON(const string& filename) {
        ifstream file(filename);
        string line;
        string content = "";

        while (getline(file, line)) {
            content += line;
        }
        file.close();
        content.erase(remove_if(content.begin(), content.end(), ::isspace), content.end());
        size_t ks = content.find("\"keys\":{");
        if (ks != string::npos) {
            size_t keysEnd = content.find("}", ks);
            string keysSection = content.substr(ks + 8, keysEnd - ks - 8);
            parseKeysSection(keysSection);
        }
        parseDataPoints(content);
    }
    
private:
    void parseKeysSection(const string& keysSection) {
        // Parse "n":value and "k":value
        size_t nPos = keysSection.find("\"n\":");
        size_t kPos = keysSection.find("\"k\":");
        
        if (nPos != string::npos) {
            size_t start = nPos + 4;
            size_t end = keysSection.find(",", start);
            if (end == string::npos) end = keysSection.length();
            keys["n"] = keysSection.substr(start, end - start);
            keys["n"].erase(remove(keys["n"].begin(), keys["n"].end(), '"'), keys["n"].end());
        }
        
        if (kPos != string::npos) {
            size_t start = kPos + 4;
            size_t end = keysSection.find(",", start);
            if (end == string::npos) end = keysSection.length();
            keys["k"] = keysSection.substr(start, end - start);
            keys["k"].erase(remove(keys["k"].begin(), keys["k"].end(), '"'), keys["k"].end());
        }
    }
    void parseDataPoints(const string& content) {
        // Find all data points like "1":{"base":"10","value":"4"}
        for (int i = 1; i <= 20; i++) { // Check up to 20 points
            string searchKey = "\"" + to_string(i) + "\":";
            size_t pos = content.find(searchKey);
            
            if (pos != string::npos) {
                size_t start = pos + searchKey.length();
                size_t end = content.find("}", start) + 1;
                string pointData = content.substr(start, end - start);
                
                // Extract base and value
                map<string, string> point;
                
                size_t basePos = pointData.find("\"base\":");
                if (basePos != string::npos) {
                    size_t baseStart = pointData.find("\"", basePos + 7) + 1;
                    size_t baseEnd = pointData.find("\"", baseStart);
                    point["base"] = pointData.substr(baseStart, baseEnd - baseStart);
                }
                
                size_t valuePos = pointData.find("\"value\":");
                if (valuePos != string::npos) {
                    size_t valueStart = pointData.find("\"", valuePos + 8) + 1;
                    size_t valueEnd = pointData.find("\"", valueStart);
                    point["value"] = pointData.substr(valueStart, valueEnd - valueStart);
                }
                
                if (!point["base"].empty() && !point["value"].empty()) {
                    data[to_string(i)] = point;
                }
            }
        }
    }
};

// Convert from any base to decimal
long long convertToDecimal(const string& value, int base) {
    long long result = 0;
    long long power = 1;
    
    // Process digits from right to left
    for (int i = value.length() - 1; i >= 0; i--) {
        char digit = value[i];
        int digitValue;
        
        if (digit >= '0' && digit <= '9') {
            digitValue = digit - '0';
        } else if (digit >= 'A' && digit <= 'Z') {
            digitValue = digit - 'A' + 10;
        } else if (digit >= 'a' && digit <= 'z') {
            digitValue = digit - 'a' + 10;
        } else {
            throw invalid_argument("Invalid character in number");
        }
        
        if (digitValue >= base) {
            throw invalid_argument("Digit value exceeds base");
        }
        
        result += digitValue * power;
        power *= base;
    }
    
    return result;
}

// Lagrange interpolation to find f(0) = secret
long long lagrangeInterpolation(const vector<pair<long long, long long>>& points) {
    long long secret = 0;
    int n = points.size();
    
    for (int i = 0; i < n; i++) {
        long long xi = points[i].first;
        long long yi = points[i].second;
        
        // Calculate Li(0) = product of (0 - xj) / (xi - xj) for all j != i
        long long numerator = 1;
        long long denominator = 1;
        
        for (int j = 0; j < n; j++) {
            if (i != j) {
                long long xj = points[j].first;
                numerator *= (0 - xj);  // This is just -xj
                denominator *= (xi - xj);
            }
        }
        
        // Add yi * Li(0) to the secret
        secret += yi * numerator / denominator;
    }
    
    return secret;
}

int main() {
    try {
        // Parse JSON file
        JsonParser parser;
        parser.parseJSON("input.json");
        
        // Get k value (minimum number of points needed)
        int k = stoi(parser.keys["k"]);
        
        // Decode points and store them
        vector<pair<long long, long long>> points;
        
        for (const auto& entry : parser.data) {
            long long x = stoll(entry.first);  // Key as x-coordinate
            int base = stoi(entry.second.at("base"));
            string value = entry.second.at("value");
            
            // Convert y-coordinate from given base to decimal
            long long y = convertToDecimal(value, base);
            
            points.push_back({x, y});
            
            cout << "Point: (" << x << ", " << y << ") - Base " << base 
                 << " value '" << value << "'" << endl;
        }
        
        // We need at least k points for reconstruction
        if (points.size() < k) {
            cerr << "Not enough points for reconstruction. Need " << k 
                 << ", got " << points.size() << endl;
            return 1;
        }
        
        // Use only the first k points (or you could use any k points)
        if (points.size() > k) {
            points.resize(k);
        }
        
        // Find the secret using Lagrange interpolation
        long long secret = lagrangeInterpolation(points);
        
        cout << "\nUsing points for interpolation:" << endl;
        for (const auto& point : points) {
            cout << "(" << point.first << ", " << point.second << ")" << endl;
        }
        
        cout << "\nThe secret (constant term) is: " << secret << endl;
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}