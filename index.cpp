#include <bits/stdc++.h>
using namespace std;

class JsonParser {
public:
    map<string, map<string, string>> data;
    map<string, string> keys;
    
    void parseJSON(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Could not open " << filename << endl;
            exit(1);
        }
        string line, content = "";
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
        for (int i = 1; i <= 20; i++) {
            string searchKey = "\"" + to_string(i) + "\":";
            size_t pos = content.find(searchKey);
            if (pos != string::npos) {
                size_t start = pos + searchKey.length();
                size_t end = content.find("}", start) + 1;
                string pointData = content.substr(start, end - start);
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

long long convertToDecimal(const string& value, int base) {
    long long result = 0;
    long long power = 1;
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
            throw invalid_argument("Invalid character in number: " + string(1, digit));
        }
        if (digitValue >= base) {
            throw invalid_argument("Digit value " + to_string(digitValue) + " exceeds base " + to_string(base));
        }
        result += digitValue * power;
        power *= base;
    }
    return result;
}

long long lagrangeInterpolation(const vector<pair<long long, long long>>& points) {
    long double secret = 0.0; // Use long double for precision
    int k = points.size();
    for (int i = 0; i < k; i++) {
        long long x_i = points[i].first;
        long long y_i = points[i].second;
        long double li = 1.0;
        for (int j = 0; j < k; j++) {
            if (i != j) {
                long long x_j = points[j].first;
                if (x_i == x_j) {
                    cerr << "Error: Duplicate x-coordinates detected" << endl;
                    exit(1);
                }
                li *= (0.0 - x_j) / (long double)(x_i - x_j);
            }
        }
        secret += y_i * li;
    }
    return llround(secret); // Round to nearest integer
}

int main() {
    try {
        JsonParser parser;
        parser.parseJSON("input.json");
        int n = stoi(parser.keys["n"]);
        int k = stoi(parser.keys["k"]);
        if (n < k || k <= 0) {
            cerr << "Error: Invalid n=" << n << " or k=" << k << endl;
            return 1;
        }
        vector<pair<long long, long long>> points;
        for (const auto& entry : parser.data) {
            long long x = stoll(entry.first);
            int base = stoi(entry.second.at("base"));
            if (base < 2 || base > 36) {
                cerr << "Error: Invalid base " << base << " for point " << x << endl;
                continue;
            }
            string value = entry.second.at("value");
            long long y = convertToDecimal(value, base);
            points.push_back({x, y});
            cout << "Point: (" << x << ", " << y << ") - Base " << base << " value '" << value << "'" << endl;
        }
        if (points.size() < k) {
            cerr << "Error: Not enough points for reconstruction. Need " << k << ", got " << points.size() << endl;
            return 1;
        }
        if (points.size() > k) {
            points.resize(k);
        }
        cout << "\nUsing points for interpolation:" << endl;
        for (const auto& point : points) {
            cout << "(" << point.first << ", " << point.second << ")" << endl;
        }
        long long secret = lagrangeInterpolation(points);
        cout << "\nThe secret (constant term) is: " << secret << endl;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}
