/* MAC PROTOCOLS
 * 
 * Author: Thessalonika Magadia
 * Notes: assumed one time slot is 10ms
 * 
 */

#include <stdio.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <iomanip> 

#define ONE_TIME_SLOT 10

using namespace std;

// counts probe slots of recursive MAC protocol from root
int idle = 0;
int collisions = 0;
int success = 0;

// converts integer i to binary (string) with k-bits specified
string int_to_binary (int i, int k) {
    string binary;

    while (i > 0) {
        binary.insert(0, to_string(i%2));
        i /= 2;
    }
    // pads the beginning of binary with 0s if needed
    while (binary.length() < k) {
        binary.insert (0, "0");
    }

    return binary;
}

// recursive function (from the root) to do MAC Protocol to scan basket
void scan_basket_root (vector<string> b, int level) {
    
    // checks if basket is empty
    if (b.empty()) {
        idle++;
        return;
    } 
    //cout << "\nlevel " << level << endl;
    // base case
    if (level > b.at(0).length()) {
        return;
    }

    int collision_check0 = 0; 
    int collision_check1 = 0;
    int success_check0 = 0;
    int success_check1 = 0;
    int idle_check0 = 0;
    int idle_check1 = 0;
    string success_value0;
    string success_value1;

    // checks for cases at level zero
    if (level == 0 && b.size() > 1) {
        // more than on item in basket
        collisions++;
        scan_basket_root (b, level+1);
    }
    else if (level == 0 && b.size() == 1){
        // only one item in basket
        success++;
        return;
    }
    else {
        // checks for cases at level 1+

        // these nested loops will look if there have been any collisions 
        // from the left subtree (0)
        for (int i = 0; i < b.size() && collision_check0 == 0; i++) {
            

            for (int j = 0; j < b.size(); j++) {
                
                // there was a match of 0 at bit level _ (ex if level 2 checks bit 2)
                if (b.at(j)[level-1] == '0') {

                    success_check0 = 1; // success flag
                    success_value0 = b.at(j); // binary value being checked, that can possibly be a success
                    idle_check0 = 0; // clear idle flag
                    
                    // if there has been a NEW collision on left side node
                    if (b.at(i)[level-1] == '0' && b.at(i) != b.at(j)) {
                        collision_check0 = 1; // collision flag
                        success_check0 = 0; // clears success flag
                        collisions++;
                        break;
                    }
                }
                else{
                   
                    idle_check0 = 1;
                } 
            }

        } // end of nested for loops for left subtree

        // these nested loops will look if there have been any collisions 
        // from the right subtree (1)
        for (int i = 0; i < b.size() && collision_check1 == 0; i++) {

            for (int j = 0; j < b.size(); j++) {
                
                // checks if there was a match of 1 at bit level
                if (b.at(j)[level-1] == '1') {
                    
                    success_check1 = 1; // success flag
                    success_value1 = b.at(j); // binary value being checked, that can possibly be success
                    idle_check1 = 0; // clear idle flag

                    // checks to see if there is NEW collision on right side node
                    if (b.at(i)[level-1] == '1' && b.at(i) != b.at(j)) {
                        collision_check1 = 1; // collision flag
                        success_check1 = 0; // clear success flag
                        collisions++;
                        break;
                    }
                }
                else{
                    idle_check1 = 1;
                } 

            }

        } // end of nested for loops for right subtree

        // add idle to count
        if (idle_check0 && !collision_check0 && !success_check0) 
            idle++;
        if (idle_check1 && !collision_check1 && !success_check1) 
            idle++;

        // add success to count
        if (success_check0) 
            success++;
        if (success_check1)
            success++;

        // if there has been a collision... continue to probe
        vector <string> left_subtree;
        vector <string> right_subtree;

        if (collision_check0 == 1) { // scan left subtree

            // get left subtree
            for (int i = 0; i < b.size(); i++) {
                if (b.at(i)[level-1] == '0') {
                    left_subtree.push_back(b.at(i));
                }
            }
            scan_basket_root (left_subtree, level+1);

        }
        if (collision_check1 == 1) {  // scan right subtree
           
            // get right subtree
            for (int i = 0; i < b.size(); i++) {
                if (b.at(i)[level-1] == '1') {
                    right_subtree.push_back(b.at(i));
                }
            }
            scan_basket_root (right_subtree, level+1);

        }

    }
    
    return;
} 

int main (int argc, char *argv[]) {

    int k_bits; // bits specified by user
    vector<int> basket;
    vector<string> basket_binary;

    if (argc != 3) {
        fprintf(stderr, "usage: <input file> <integer K-bits>\n");
        exit(1);
    }

    sscanf (argv[2], "%d", &k_bits);

    // check that integer 
    if (k_bits < 1) {
        fprintf (stderr, "not a valid value for K-bits\n");
        exit(1);
    }

    // opening and reading file
    ifstream myFile;
    myFile.open (argv[1]);

    // check that file can be opened
    if (!myFile.is_open()) {
        fprintf (stderr, "File could not be found!\n");
        exit(1);
    }

    // saving file into list (basket)
    string buffer;
    //cout << "\nContents of Text File: \n";
    while (getline(myFile, buffer)) {
        basket.push_back(stoi(buffer));
    }

    
    // convert basket to binary
    for (int i = 0; i < basket.size() && !basket.empty(); i++) 
        basket_binary.push_back(int_to_binary(basket.at(i), k_bits));
    

    // print basket in binary for debugging
    /* cout << "\nContents of Basket in Binary: \n";
    if (basket_binary.empty()) {
        cout << "[empty]" << endl;
    }
    else {
        for (int i = 0; i < basket_binary.size(); i++) 
            cout << basket_binary.at(i) << endl;
        cout << endl;
    } */

    // ================= ROOT LEVEL ================= //
    scan_basket_root(basket_binary, 0);
    int total_root = success + collisions + idle;
    cout << "\n===== root scan results =====" << endl;
    cout << "successses\t" << success << endl;
    cout << "collisions\t" << collisions << endl;
    cout << "idle\t\t" << idle << endl;
    cout << "total\t\t" << total_root << endl;
    cout << "efficiency\t" << setprecision(2) << fixed << (double)success/total_root*100 << "%" << endl;
    cout << "time\t\t" << total_root*ONE_TIME_SLOT << endl;

    // ================= LEAF LEVEL ================= //
    int total_leaf = pow(2, k_bits);
    int idle_leaf = total_leaf - basket.size();
    int success_leaf = basket.size();

    cout << "\n===== leaf scan results =====" << endl;
    cout << "successes\t" << success_leaf << endl;
    cout << "collisions\t" << 0 << endl;
    cout << "idle\t\t" << idle_leaf << endl;
    cout << "total\t\t" << total_leaf << endl;
    cout <<  "efficiency\t" << setprecision(2) << fixed << (double)success_leaf/total_leaf*100 << "%" << endl;
    cout << "time\t\t" << total_leaf*ONE_TIME_SLOT << endl;

    return 0;
}
    