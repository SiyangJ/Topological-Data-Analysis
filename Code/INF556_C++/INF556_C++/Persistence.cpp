//
//  Persistence.cpp
//  INF556_C++
//
//  Created by apple on 7/13/18.
//  Copyright © 2018 UNC. All rights reserved.
//

// TODO
// Seems like many of the codes can be accomplished by external library.

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <limits>
#include <assert.h>

using namespace std;

typedef vector<int> boundary;
typedef set<int> vertices;
typedef vector<boundary> boundary_matrix;
typedef vector<pair<int, pair<float, float> > > barcode;

// val: the first time it appears.
struct simplex{
    int dim;
    float val;
    vertices vert;
};

// Ordering function for the simplices.
bool comp_filter (simplex i, simplex j) {
    if(i.val<j.val){return true;}
    else{
        if(i.val==j.val){return (i.dim<j.dim);}
        else{return false;}
    }
}


bool mycomp (int i, int j){return (i>j);}


boundary add_in_Z2(boundary L1, boundary L2){
    boundary T; T.clear(); int l = L1.size();
    for(int i = 0; i < l; i++){
        if (L1[i] == L2[i])
            T.push_back(0);
        else
            T.push_back(1);
    }
    return T;
}

int lowest_row(boundary L){
    int low = -1; int l = L.size();
    for(int i = 0; i < l; i++)
        if(L[i] != 0)
            low = i;
    return low;
}

boundary sparse_add_in_Z2(boundary L1, boundary L2){
    int i = 0; int j = 0; int l1 = L1.size(); int l2 = L2.size();
    boundary T; T.clear();
    while( i < l1 and j < l2){
        if(L1[i] > L2[j]){T.push_back(L1[i]); i += 1;}
        else if(L2[j] > L1[i]){T.push_back(L2[j]); j += 1;}
        else{i += 1; j += 1;}  // L1[j] == L2[i]
    }
    if(i == l1){
        while(j < l2){T.push_back(L2[j]); j += 1;}
    }
    if(j == l2){
        while(i < l1){T.push_back(L1[i]); i += 1;}
    }
    return T;
}


vector<simplex> read_filtration(string name){
    
    vector<simplex> F; F.clear();
    char input_file[100];
    sprintf(input_file, "%s", (char*) name.c_str());
    ifstream input(input_file);
    
    if (input){
        string line;
        while(getline(input,line)){
            stringstream stream(line);
            simplex s; s.vert.clear();
            s.dim = -1; stream >> s.val; stream >> s.dim; int i = 0;
            while(i <= s.dim){
                int f; stream >> f;
                s.vert.insert(f); i++;
            }
            if(s.dim != -1)
                F.push_back(s);
        }
    }
    else{cout << "Failed to read file " << name << endl;}
    return F;
    
};


std::ostream& operator<<(std::ostream& os, const vertices& v) {
    os << "(";
    for(vertices::iterator vit = v.begin(); vit != v.end(); vit++) {
        os << *vit;
        vertices::iterator vitt = vit;
        vitt++;
        if (vitt != v.end())
            os << ", ";
    }
    os << ")";
    return os;
}

//***********************************************************************
// Core Algorithm.


boundary_matrix compute_boundary_matrix(vector<simplex>* F, bool sparse){
    
    boundary_matrix B; B.clear();
    int num_simplex = (*F).size();
    
    // sort filtration by function values and dimensions
    sort((*F).begin(), (*F).end(), comp_filter);
    
    // re-sort now by lexicographical order
    // TODO: Very inefficient
    map<vertices,int> lexico;
    for (int i=0; i<F->size(); i++)
        lexico.insert(pair<vertices,int> ((*F)[i].vert, i));
    
    // create sparse matrix
    if(sparse){
        for (int i = 0; i < num_simplex; ++i ){
            boundary h;
            vertices b = (*F)[i].vert;
            int num_simplex_bound = b.size();
            // compute simplices of codimension 1
            for(int j = 0; j < num_simplex_bound; j++){
                vertices bb = b;
                vertices::iterator vit = bb.begin();
                for (int k=0; k<j; k++) vit++;
                bb.erase(vit);
                // bb.erase(bb.begin()+j);
                // find them in the filtration
                map<vertices,int>::iterator sit = lexico.find(bb);
                if(sit != lexico.end())
                    h.push_back(sit->second);
                else if (b.size() > 1){
                    std::cerr << "Error: " << b << " has no boundary " << bb << " at time of insertion" << std::endl;
                    assert(false);
                }
            }
            // simplices in the boundary are assumed to be sorted in decreasing order
            sort(h.begin(), h.end(), mycomp);
            B.push_back(h);
        }
    }
    // create full matrix
    else{
        for (int i = 0; i < num_simplex; ++i ){
            boundary h;
            vertices b = (*F)[i].vert;
            int num_simplex_bound = b.size();
            boundary b2(num_simplex, 0);
            // compute simplices of codimension 1
            for(int j = 0; j < num_simplex_bound; j++){
                vertices bb = b;
                vertices::iterator vit = bb.begin();
                for (int k=0; k<j; k++) vit++;
                bb.erase(vit);
                // bb.erase(bb.begin()+j);
                // find them in the filtration
                for(int k = 0; k < num_simplex; k++){
                    vertices bbb = (*F)[k].vert;
                    if(bbb==bb){
                        b2[k]=1; break;
                    }
                }
            }
            B.push_back(b2);
        }
    }
    
    return B;
    
};


//***********************************************************************
// Core Algorithm.

barcode compute_barcode(boundary_matrix B, vector<simplex>* F){
    
    // initialization
    barcode bc; bc.clear();
    vector<int> low; low.clear(); low.push_back(-1);
    int num_simplex = B.size();
    
    
    // algorithm
    for(int j = 1; j < num_simplex; j++){
        int lowj = lowest_row(B[j]);
        int token = 0;
        while(token != j){
            token = 0;
            for (int j0 = 0; j0 < j; j0++){
                if (low[j0] == lowj and lowj != -1){
                    B[j] = add_in_Z2(B[j], B[j0]);
                    lowj = lowest_row(B[j]);
                    break;
                }
                token += 1;
            }
        }
        low.push_back(lowj);
        
    }
    
    // function values & dimensions
    for(int j = 0; j < num_simplex; j++){
        if(low[j] != -1 and (*F)[low[j]].val != (*F)[j].val){ // remove intervals of length zero
            //    if(low[j] != -1){
            pair<float, float> p((*F)[low[j]].val, (*F)[j].val);
            int d = (*F)[low[j]].dim;
            pair<int, pair<float, float> > q(d,p);
            bc.push_back(q);
        }
        if(low[j] == -1){
            int token = 0;
            for(int k = 0; k < num_simplex; k++){ if(low[k] == j){token = 1; break;} }
            if(token == 0){
                pair<float, float> p((*F)[j].val, numeric_limits<float>::infinity());
                int d = (*F)[j].dim;
                pair<int, pair<float, float> > q(d,p);
                bc.push_back(q);
            }
        }
    }
    
    return bc;
    
};


barcode compute_barcode_sparse(boundary_matrix B, vector<simplex>* F){
    
    // initialization
    boundary_matrix R = B;
    boundary L; L.clear();
    int num_simplex = B.size();
    vector< pair<int, int> > bc_int;
    barcode bc; bc.clear();
    
    // algorithm
    for(int j = 0; j < num_simplex; j++){
        // cout << "j=" << j << endl;
        pair<int, int> p(j,j); bc_int.push_back(p);
        L = B[j]; R[j].clear();
        while( L.size() > 0 and R[L[0]].size() > 0 ){
            // cout << "   L[0] = " << L[0] << "...";
            // cout.flush();
            L[0];
            L = sparse_add_in_Z2(L,R[L[0]]);
            // cout << " done" << endl;
        }
        if( L.size() > 0 ){
            R[L[0]] = L;
            bc_int[L[0]].second = j;
        }
        else
            bc_int[j].second = -1;
    }
    
    // function values & dimensions
    for(int i = 0; i < bc_int.size(); i++){
        if(bc_int[i].second != -1 and (*F)[bc_int[i].first].val != (*F)[bc_int[i].second].val){ // remove intervals of length zero
            //    if(bc_int[i].second != -1){
            pair<float, float> p((*F)[bc_int[i].first].val, (*F)[bc_int[i].second].val);
            int d = (*F)[bc_int[i].first].dim;
            pair<int, pair<float, float> > q(d,p);
            bc.push_back(q);
        }
        if(bc_int[i].second == -1){
            pair<float, float> p((*F)[bc_int[i].first].val, numeric_limits<float>::infinity());
            int d = (*F)[bc_int[i].first].dim;
            pair<int, pair<float, float> > q(d,p);
            bc.push_back(q);
        }
    }
    
    return bc;
    
};



int main(int argc, char** argv) {
    
    if (argc != 3) {
        cout << "Syntax: " << argv[0] << " <filtration_file> <0|1>" << endl
        << "   0: dense boundary matrix representation" << endl
        << "   1: sparse boundary matrix representation + O(m^3) algorithm"
        << endl;
        return 0;
    }
    
    
    string name = argv[1];
    bool sparse = atoi(argv[2]);
    
    // cout << "Reading filtration..." << endl;
    vector<simplex> F = read_filtration(name);
    // cout << "Done." << endl;
    
    // cout << "Computing boundary matrix..." << endl;
    boundary_matrix B = compute_boundary_matrix(&F, sparse);
    // cout << "Done." << endl;
    
    if(sparse){
        
        // cout << "Computing barcode (sparse)..." << endl;
        barcode b = compute_barcode_sparse(B,&F);
        
        // cout << "Efficient computation:" << endl;
        for(int i = 0 ; i < b.size(); i++)
            cout << b[i].first << " " << b[i].second.first << " " << b[i].second.second << endl;
    }
    else{
        
        // cout << "Computing barcode (naive)..." << endl;
        barcode b = compute_barcode(B,&F);
        
        // cout << "Naive computation:" << endl;
        for(int i = 0 ; i < b.size(); i++)
            cout << b[i].first << " " << b[i].second.first << " " << b[i].second.second << endl;
    }
    
    return 0;
}