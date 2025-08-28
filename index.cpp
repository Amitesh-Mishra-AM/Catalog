#include<bits/stdc++.h>
using namespace std;

long long base_to_decimal(string num, int base) {
    long long ans = 0;
    long long mult = 1;
    for(int i = num.size()-1; i >= 0; i--) {
        int digit;
        if(num[i] >= '0' && num[i] <= '9') 
            digit = num[i] - '0';
        else if(num[i] >= 'A' && num[i] <= 'F')
            digit = num[i] - 'A' + 10;
        else 
            digit = num[i] - 'a' + 10;
        
        ans += digit * mult;
        mult *= base;
    }
    return ans;
}

string clean_string(string s) {
    string result = "";
    for(char c : s) {
        if(c != '"' && c != ' ' && c != '\n' && c != '\t' && c != '\r') {
            result += c;
        }
    }
    return result;
}

int extract_number(string content, string key) {
    size_t pos = content.find(key);
    if(pos == string::npos) return -1;
    
    pos += key.length();
    string num = "";
    while(pos < content.length() && content[pos] >= '0' && content[pos] <= '9') {
        num += content[pos];
        pos++;
    }
    return num.empty() ? -1 : stoi(num);
}

int main() {
    ifstream file("input.json");
    if(!file.is_open()) {
        cout << "Error opening file" << endl;
        return 1;
    }
    
    string content = "";
    string line;
    while(getline(file, line)) {
        content += line;
    }
    file.close();
    
    content = clean_string(content);
    
    int n = extract_number(content, "n:");
    int k = extract_number(content, "k:");
    
    if(n == -1 || k == -1) {
        cout << "Error parsing n or k" << endl;
        return 1;
    }
    
    vector<pair<long long, long long>> points;
    
    for(int i = 1; i <= n; i++) {
        string search = to_string(i) + ":{";
        size_t pos = content.find(search);
        if(pos == string::npos) continue;
        
        size_t end_pos = content.find("}", pos);
        if(end_pos == string::npos) continue;
        
        string point_data = content.substr(pos, end_pos - pos + 1);
        
        int base = extract_number(point_data, "base:");
        
        size_t val_pos = point_data.find("value:");
        if(val_pos == string::npos) continue;
        val_pos += 6;
        
        string value = "";
        while(val_pos < point_data.length() && 
              ((point_data[val_pos] >= '0' && point_data[val_pos] <= '9') ||
               (point_data[val_pos] >= 'A' && point_data[val_pos] <= 'F') ||
               (point_data[val_pos] >= 'a' && point_data[val_pos] <= 'f'))) {
            value += point_data[val_pos];
            val_pos++;
        }
        
        if(base > 0 && !value.empty()) {
            long long y = base_to_decimal(value, base);
            points.push_back({i, y});
        }
    }
    
    if(points.size() < k) {
        cout << "Not enough points" << endl;
        return 1;
    }
    
    // use only first k points
    while(points.size() > k) {
        points.pop_back();
    }
    
    long long secret = 0;
    
    for(int i = 0; i < k; i++) {
        long long x_i = points[i].first;
        long long y_i = points[i].second;
        
        long long num = 1, den = 1;
        
        for(int j = 0; j < k; j++) {
            if(i != j) {
                long long x_j = points[j].first;
                num *= (0 - x_j);  
                den *= (x_i - x_j);
            }
        }
        
        secret += y_i * num / den;
    }
    
    cout << secret << endl;
    
    return 0;
}