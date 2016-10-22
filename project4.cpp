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
#include <vector>
#include <cmath>

using namespace std;

struct LairInfo{
    int ID;
    int total_money;
    string key;
    int alarm;
    string name;
    string decrypt_key;
    int x;
    int y;
};

struct Failure{
    int ID;
    string failure_reason;
    string incorrect_key;
    int decryption_time;
};\
bool cmp(const Failure& lhs, const Failure& rhs)
{
    return lhs.ID < rhs.ID;
}

bool removeID(Failure const &fail, int ID)
{
    return fail.ID == ID;
}

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

    //cout << encrypted << endl;
    // Remove all instances of the first three characters
    string temp = encrypted.substr(0,3);
    while(encrypted.find(temp) != encrypted.npos){
        encrypted.erase(encrypted.find(temp),3);
    }

    //cout << encrypted << endl;
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

int pathFind(int n, int x0, int y0, vector <LairInfo> map){
    int count = 0;
    for(int i = 0; i < n; i++){
        if(map[i].ID != 0){
            break;
        } else {
            count++;
        }
    }
    int minDist = dist(x0, y0, map[count].x, map[count].y);
    int minID = map[count].ID;
    for(int i = count + 1; i < n; i++){
        if(map[i].ID == 0){
            continue;
        } else if(minDist > dist(x0, y0, map[i].x, map[i].y)){
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
    double distance = 0.0;
    int bounty = 0;
    // Initialize a vector of struct lairInfo
    vector <LairInfo> info;
    vector <Failure> fail;

    fail.push_back(Failure());

    int i = 0;
    int p = 0;

    in_map >> num >> x_0 >> y_0 >> x_f >> y_f;
    //cout << num << x_0 << endl;

    while(in_lair >> ID){
        info.push_back(LairInfo());
        info[i].ID = ID;
        in_lair >> total_money;
        info[i].total_money = total_money;
        in_lair >> key;
        info[i].key = key;
        in_lair >> alarm;
        info[i].alarm = alarm;
        in_lair >> name;
        info[i].name = name;
        //cout << ID << name << endl;
        i++;
    }

    i = 0;
    while(in_rob >> decrypt_key){
        info[i].decrypt_key = decrypt_key;
        //cout << decrypt_key << endl;
        i++;
    }

    i = 0;
    while(in_map >> ID){
        int n=0;
        while(ID != info[n].ID){
            n++;
        }
        in_map >> x >> y;
        info[n].x = x;
        info[n].y = y;
        //cout << x << y << endl;
        i++;
    }
    //cout << i << endl;
    // Find the lairs that cannot be robbed
    for(int j=0; j < i; j++){
        decrypt_key = decrypt(info[j].decrypt_key);
        //cout << info[j].decrypt_key << "1" << endl;
        //cout << decrypt(info[j].decrypt_key) << "2" << endl;
        if(decrypt_key != info[j].key){
            fail.push_back(Failure());
            fail[p].ID = info[j].ID;
            fail[p].failure_reason = " incorrect decrypted key: ";
            fail[p].incorrect_key = decrypt_key;
            info[j].ID = 0;
            p++;
        } else if(info[j].decrypt_key.size() >= info[j].alarm){
            fail.push_back(Failure());
            fail[p].ID = info[j].ID;
            fail[p].failure_reason = " decryption tiem too long: ";
            fail[p].decryption_time = info[j].decrypt_key.size();
            info[j].ID = 0;
            p++;
        }
    }
    // Find the path for all avaiable lairs
    //cout << 1;
    x = x_0;
    y = y_0;
    //cout << x << y << endl;
    for(int j = 0; j < i; j++){
        if(info[j].ID != 0){
            int minID = pathFind(num, x, y, info);
            int n=0;
            while(minID != info[n].ID){
                // Find the ID's corresponding info
                n++;
            }
            //cout << info[n].ID << " " << info[n].x << endl;
            string lair_name = info[n].name;

            while(lair_name.find("_") != lair_name.npos){
                lair_name.replace(lair_name.find("_"), 1, " ");
            }

            out_bounty << "Moved from (" << x << "," << y << ") to (" << info[n].x << "," << info[n].y << ")" << endl;
            out_bounty << "Robbed lair " << lair_name << " and stole $" << info[n].total_money << endl;
            distance += sqrt(pow(x-info[n].x,2)+pow(y-info[n].y,2));
            x = info[n].x;
            y = info[n].y;
            bounty += info[n].total_money;
            n++;
            info[j].ID = 0;
        }
    }
    out_bounty << "Moved from (" << x << "," << y << ") to (" << x_f << "," << y_f << ")" << endl;
    out_bounty << "Made it to safety!" << endl;
    out_bounty << "Total distance traveled: " << distance + sqrt(pow(x_f-x,2)+pow(y_f-y,2)) << " miles" << endl;
    out_bounty << "Total bounty given to charity: $" << bounty << endl;

    for(int j = 0; j < p; j++) {
        int n = j;
        if(fail[j].ID != 0){
            int minID = fail[j].ID;
            for(int k = 0; k < p; k++){
                if(fail[j].ID != 0 && fail[j].ID < minID){
                    minID = fail[k].ID;
                    n = k;
                }
            }
            if(fail[n].failure_reason == " incorrect decrypted key: "){
                out_failure << fail[n].ID << " incorrect decrypted key: " << fail[n].incorrect_key << endl;
            } else {
                out_failure << fail[n].ID << " decryption time too long: " << fail[n].decryption_time << endl;
            }
        }
    }

}
