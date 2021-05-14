#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <sstream>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;

//take as input a vector of
double euclid_dist(vector< vector<string> > train, vector<double> test, int n){
  double x, y, d;
  string strX;
  d = 0;

  for(int j = 1; j < 4; j++){
    strX = train[n][j];
    x = atof(strX.c_str());
    y = test[j];
    d += pow(x - y, 2);
  }
  d = sqrt(d);
  return d;
}

int main(int argc, char**arv){
  ifstream file("Default_Fin.csv");

  vector< vector<string> > default_Fin;
  vector<string> data;
  string line, cell;

  //read in each line and parse each line by every comma
  while (getline(file, line)){
    data.clear();
    stringstream w(line);

    //store each line in a vector of vectors of strings for each line
    while(getline(w, cell, ',')){
      data.push_back(cell);
    }
    default_Fin.push_back(data);
  }

  //create a test case
  vector<double> test_data{0,1,12537.01,103292.52};

  int nthreads, tid;
  vector<double> nearest, index;
  double dist, smallest_dist, c;
  int k = 7;
  string str;
  int i;

  //parallelize the search for the smallest distance
  #pragma omp parallel private(nthreads, tid, dist, smallest_dist, c, str, i) shared(default_Fin, test_data, nearest, k, index)
  {
    tid = omp_get_thread_num();
    nthreads = omp_get_num_threads();
    smallest_dist = 10000000;
    c = 0;

    //for each data point, find the distance between the test case and save the index in variable c
    #pragma omp for
      for (i = 1; i < 10001; i++){
        dist = euclid_dist(default_Fin, test_data, i);
        if(smallest_dist > dist){
          smallest_dist = dist;
          str = default_Fin[i][0];
          c = atof(str.c_str());
        }
      }

	//have each thread record theri shortest distance
    #pragma omp critical
      nearest.push_back(smallest_dist);
      index.push_back(c);

    //have thread 0 output the closest datapoint
    if (tid == 0){
      for(int x = 0; x < nearest.size(); x++){
        cout << "Distance: " << nearest[x] << " Index: " << index[x] << endl;;
      }
    }
  }

  //TODO:
    //save the closest k values 
    //determine whether those k closest datapoints have defaulted or not and count how many of each
    //return the majority value and display the prediction for the target data point

    //or take a random data point and make it the target value to see if the prediction is correct

  return 0;
}
