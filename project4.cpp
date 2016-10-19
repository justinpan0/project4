// ENGR 151
// Project 4
// Filename: project4.cpp
//
// Authors: Zimeng Pan (lab section: 101)
// Uniqnames: zmpan
//
// Date Created: Oct. 16th
// Date Submitted: 
//
// Program Description:
// Decryption of pixel info by functions specified in orders

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

struct lairInfo{
    int ID;
    int total_money;
    string key;
    int alarm;
    string name;
    string decrypt_key;
    int x;
    int y;
};

struct failure{
    int ID;
    string failure_reason;
    string incorrect_key;
    int decryption_time;
};
// Decryption

string decrypt(string encrypted){
    // Remove all adjacent duplicate characters
    for(int i = 0; i < encrypted.size(); i++){
        if(encrypted[i] == encrypted[i+1]){
            int count = 0;
            for(int j = i+1; j < encrypted.size(); j++){
                if(encrypted[i] == encrypted[j]){
                    count++;
                } else {
                    break;
                }
            }
            encrypted.erase(i+1, count);
        }
    }

    // cout << encrypted << endl;
    // Remove all instances of the first three characters
    string temp = encrypted.substr(0,3);
    while(encrypted.find(temp) != encrypted.npos){
        encrypted.erase(encrypted.find(temp),3);
    }

    // cout << encrypted << endl;
    // If the fist and last two characters match, shift all characters up by 3 in the ACSII map; otherwise, shift by 5
    if(encrypted.substr(0,2) == encrypted.substr(encrypted.size()-2,2)){
        for(int i = 0; i < encrypted.size(); i++){
            encrypted[i] = encrypted[i] + 3;
        }
    } else {
        for(int j = 0; j < encrypted.size(); j++){
            encrypted[j] = encrypted[j] + 5;
        }
    }
    encrypted.erase(0,2);
    encrypted.erase(encrypted.size()-2,2);
    return encrypted;
}

// Path Finding
int dist(int x1,int y1, int x2, int y2){
    int dist = sqrt(pow(x2-x1,2)+pow(y2-y1,2));
    return dist;
}

int pathFind(int n, int x0, int y0, vector <lairInfo> map){
    int minDist = dist(x0, y0, map[0].x, map[0].y);
    int minID = map[0].ID;
    for(int i=1; i < n; i++){
        if(minDist > dist(x0, y0, map[i].x, map[i].y)){
            minDist = dist(x0, y0, map[i].x, map[i].y);
            minID = map[i].ID;
        } else if(minDist == dist(x0, y0, map[i].x, map[i].y) || minID > map[i].ID){
            minID = map[i].ID;
        }
    }
    return minID;
}


int main(){
    ifstream in_lair("lair.txt");
    ifstream in_rob("robbery.txt");
    ifstream in_map("map.txt");
    ofstream out_bounty("bounty.txt");
    ofstream out_failure("failure.txt");

    string key, name,decrypt_key,init_map;
    int ID, total_money, alarm, num, x_0, y_0, x_f, y_f;
    int x,y;
    // Initialize a vector of struct lairInfo
    vector <lairInfo> info;
    vector <failure> failure;
    int i = 0;
    int p = 0;

    in_map >> num >> x_0 >> y_0 >> x_f >> y_f;
    while(in_lair >> ID){
        info[i].ID = ID;
        in_lair >> total_money;
        info[i].total_money = total_money;
        in_lair >> key;
        info[i].key = key;
        in_lair >> alarm;
        info[i].alarm = alarm;
        in_lair >> name;
        info[i].name = name;
        in_rob >> decrypt_key;
        info[i].decrypt_key = decrypt_key;
        in_map >> ID >> x >> y;
        info[i].x = x;
        info[i].y = y;
        i++;
    }

    for(int j=0; j < num; j++){
        decrypt_key = decrypt(info[j].key);
        if(decrypt_key != info[j].decrypt_key){
            failure[p].ID = info[j].ID;
            failure[p].failure_reason = " incorrect decrypted key: ";
            failure[p].incorrect_key = decrypt_key;
            info[j].ID = 0;
            j++;
        } else if(info[j].key.size() >= info[j].alarm){
            failure[p].ID = info[j].ID;
            failure[p].failure_reason = " decryption tiem too long: ";
            failure[p].decryption_time = info[j].key.size();
            info[j].ID = 0;
            j++;
        } else {
            continue;
        }
    }

    //out_failure << info[i].ID << " incorrect decrypted key: " << endl;
    //out_failure << info.[i].ID << " decryption time too long: " << info[i].key.size();
}
