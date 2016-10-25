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
// According to the input data, decide on the nodes that can be reached, and plan the best 
// path through these nodes based on Greedy Algorithm

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

struct LairInfo{
    // Define structure lairinfo as all the information read from lair, map and robbery txt 
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
    // Define structure failure as the failure reasons of specific failed cases from LairInfo
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

    // Remove all instances of the first three characters
    string temp = encrypted.substr(0,3);
    while(encrypted.find(temp) != encrypted.npos){
        encrypted.erase(encrypted.find(temp),3);
    }

    // If the fist and last two characters match, shift all characters up by 3 in 
    // the ACSII map; otherwise, shift by 5
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
double dist(int x1,int y1, int x2, int y2){
    // Distance between two coordinates
    double dist = sqrt(pow(x2-x1,2)+pow(y2-y1,2)); 
    return dist;
}

int pathFind(int x0, int y0, vector <LairInfo> &map){
    // Find the cloest point of (x0,y0) from map
    int j=0;
    while(map[j].ID == 0) {
	// Find the first ID that is not used before
        j++;
    }
    double minDist = dist(x0, y0, map[j].x, map[j].y);
    int minID = map[j].ID;

    for(int i = 0; i < map.size(); i++){
        //cout << map[i].ID << endl; //For Testing
        if(map[i].ID != 0){
            //cout << i << " " << map[i].ID << " "; //For Testing
            if(minDist > dist(x0, y0, map[i].x, map[i].y)){
		// If finded a dist smaller than previous min dist
                minDist = dist(x0, y0, map[i].x, map[i].y);
                minID = map[i].ID;
                //cout << "minDist:" << minDist << endl; //For Testing
            } else if(minDist == dist(x0, y0, map[i].x, map[i].y) && minID > map[i].ID){
		// If finded a dist equal to previous min dist with a smaller ID #
                minID = map[i].ID;
                //cout << "minID " << minID << endl; //For Testing
            }
        }
    }
    //cout << minID << endl; //For Testing
    return minID;
}


int main(){
    // Read all the files
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
    int p = 0; // The total number of failed cases
	
    in_map >> num >> x_0 >> y_0 >> x_f >> y_f;

    while(in_lair >> ID){
	// Read from lair txt
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
        i++;
    }

    i = 0;
    while(in_rob >> decrypt_key){
	// Read from robbery txt
        info[i].decrypt_key = decrypt_key;
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
        i++;
    }

    for(int j=0; j < i; j++){
	// Find all failed cases    
        decrypt_key = decrypt(info[j].decrypt_key);
        if(decrypt_key != info[j].key){
	    // Failed b/c wrong key 	
            fail.push_back(Failure());
            fail[p].ID = info[j].ID;
            fail[p].failure_reason = " incorrect decrypted key: "; // Failed reason
            fail[p].incorrect_key = decrypt_key;
            info[j].ID = 0; // Set the ID in LairInfo to 0 indicating it is a failed case
            p++;
        } else if(info[j].decrypt_key.size() > info[j].alarm){
	    // Failed b/c right key but exceeding time limit	
            fail.push_back(Failure());
            fail[p].ID = info[j].ID;
            fail[p].failure_reason = " decryption tiem too long: ";
            fail[p].decryption_time = info[j].decrypt_key.size();
            info[j].ID = 0;
            p++;
        }
    }
    // p is the total # of failed cases
	
    // Find the path for all remaining lairs
    x = x_0;
    y = y_0;
    int n;
    for(int j = 0; j < info.size()-fail.size()+1; j++) {
        n = 0;
        int minID = pathFind(x, y, info); // Find the closest coordinates
        while (minID != info[n].ID) {
            // Find the ID's corresponding info
            n++;
        }

        string lair_name = info[n].name;
        while (lair_name.find("_") != lair_name.npos) {
	    // Replace all the "_" with blank space
            lair_name.replace(lair_name.find("_"), 1, " ");
        }

        out_bounty << "Moved from (" << x << "," << y << ") to (" << info[n].x << "," << info[n].y << ")" << endl;
        out_bounty << "Robbed lair " << lair_name << " and stole $" << info[n].total_money << endl;

        distance += sqrt(pow(x - info[n].x, 2) + pow(y - info[n].y, 2)); // Add up the total distance
	bounty += info[n].total_money; // Add up the total bounty

        x = info[n].x; // Assign the new point to the x,y
        y = info[n].y;

        info[n].ID = 0; // Set the ID in LairInfo to 0 indicating it is a used case
    }
    out_bounty << "Moved from (" << x << "," << y << ") to (" << x_f << "," << y_f << ")" << endl;
    out_bounty << "Made it to safety!" << endl;
    out_bounty << "Total distance traveled: " << distance + sqrt(pow(x_f-x,2)+pow(y_f-y,2)) << " miles" << endl;
    out_bounty << "Total bounty given to charity: $" << bounty << endl;
    
    // Output the failure txt by ascending ID #
    for(int j = 0; j < p; j++) {
        int n = j;
        if(fail[j].ID != 0){
            int minID = fail[j].ID;
            for(int k = 0; k < p; k++){
                if(fail[j].ID != 0 && fail[j].ID < minID){
		    // Find the smallest ID # thats not 0
                    minID = fail[k].ID;
                    n = k;
                }
            }
            if(fail[n].failure_reason == " incorrect decrypted key: "){
                out_failure << fail[n].ID << "	incorrect decrypted key: " << fail[n].incorrect_key << endl;
            } else {
                out_failure << fail[n].ID << "	decryption time too long: " << fail[n].decryption_time << endl;
            }
        }
    }
}
