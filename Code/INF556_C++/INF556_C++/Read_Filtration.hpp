//
//  Read_Filtration.hpp
//  INF556_C++
//
//  Created by apple on 7/13/18.
//  Copyright © 2018 UNC. All rights reserved.
//

#ifndef Read_Filtration_hpp
#define Read_Filtration_hpp

#include <stdio.h>
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

using namespace std;

typedef set<int> vertices;


struct simplex{
    int dim;
    float val;
    vertices vert;
};

vector<simplex> read_filtration(string name);


#endif /* Read_Filtration_hpp */
