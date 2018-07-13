//
//  barcode2vector.cpp
//  INF556_C++
//
//  Created by apple on 7/13/18.
//  Copyright © 2018 UNC. All rights reserved.
//

#include <iostream>
#include <vector>
#include <cctype>
#include <limits>
#include <cmath>
#include <cstdlib>
#include <set>

using namespace std;


class Interval {
    
public:
    
    int id, d;
    double x,y;
    
    Interval(int id_, int d_, double x_, double y_) {
        id = id_;
        d = d_;
        x = x_;
        y = y_;
    }
    
    Interval(const Interval& i) {
        id = i.id;
        d = i.d;
        x = i.x;
        y = i.y;
    }
    
    bool operator< (Interval i) const {
        return d < i.d
        || (d == i.d && y-x > i.y-i.x)
        || (d == i.d && y-x == i.y-i.x && id < i.id);
    }
};

ostream& operator<< (ostream& os, const Interval& i) {
    os
    //    << i.id << " "
    << i.d << " " << i.x << " " << i.y;
    return os;
}

typedef set<Interval> Barcode;

int main (int argc, char** argv) {
    
    if (argc != 3) {
        cout << "Syntax: " << argv[0] << " d n\n\n"
        << "   - d = maximal dimension considered in the barcode (dimensions 0 to max_dim are cnosidered, entries completed with 0's if necessary)\n\n"
        << "   - n = number of points considered in each barcode (n points per dimension, plus n(n-1)/2 entries in the distance matrix per dimension, completed with 0's if necessary)\n\n"
        << "-> in total, the feature vector has dimension (d+1)n(n+1)/2"
        << endl;
        return 0;
    }
    
    
    int d, maxd;
    double x, y;
    char buf[256];
    
    Barcode b;
    int count = 0;
    maxd = 0;
    while ((cin>>d)) {
        maxd = max(d, maxd);
        cin >> x;
        cin >> std::ws;
        char c = cin.peek();
        if (c != 'i')
            cin >> y;
        else {
            y = numeric_limits<double>::max();
            cin.getline(buf, 255);
        }
        Interval i (count++, d, x, y);
        b.insert(i);
    }
    
    // cout << "number of intervals: " << b.size() << endl;
    
    // separate barcodes per dimension
    vector<Barcode> bs(maxd+1);
    for (Barcode::iterator bit = b.begin(); bit != b.end(); bit++)
        bs[bit->d].insert(*bit);
    
    // populate vector
    maxd = min(maxd, atoi(argv[1]));  // force maximal dimension
    int npoints = atoi(argv[2]);  // number of points kept per dimension
    int nentries = npoints*(npoints-1)/2;  // number of distance matrix entries kept per dimension
    
    for (d = 0; d<=maxd; d++) {
        // output first npoints distances to the diagonal
        int n=0;
        for (Barcode::iterator bit = bs[d].begin(); bit != bs[d].end() && n<npoints; bit++,n++)
            cout << (bit->y-bit->x)/2 << " ";
        // complete with 0's if necessary
        while (n++<npoints)
            cout << "0 ";
        
        // output first nentries distance matrix entries
        multiset<double> entries;
        for (Barcode::iterator bit = bs[d].begin(); bit != bs[d].end(); bit++)
            for (Barcode::iterator cit = bit; cit != bs[d].end(); cit++){
                if (cit == bit)
                    continue;
                entries.insert
                (min((bit->y-bit->x)/2,
                     min((cit->y-cit->x)/2,
                         max(abs(cit->y-bit->y), abs(cit->x-bit->x)))));
                if (entries.size() > nentries)  // remove smallest selected entry
                    entries.erase(entries.begin());
            }
        // output entries in decreasing order
        for (multiset<double>::reverse_iterator iit = entries.rbegin();
             iit != entries.rend(); iit++)
            cout << *iit << " ";
        // complete with 0's if necessary
        for (int j=entries.size(); j<nentries; j++)
            cout << "0 ";
    }    
    
    // finally complete with 0's for missing dimensions
    int desired_maxd = atoi(argv[1]);
    for (d = maxd+1; d<=desired_maxd; d++)
        for (int j=0; j<npoints+nentries; j++)
            cout << "0 ";
    
    cout << endl;
    
}
