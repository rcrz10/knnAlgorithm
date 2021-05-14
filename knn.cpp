#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;

double euclid_dist(vector< vector<string> > train, vector<double> test, int n){// function to acquire eucildean distance
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
  ifstream file("Default_Fin.csv"); //name of file

  vector< vector<string> > default_Fin;//matrix created with vectors
  vector<string> data;
  string line, cell;

  while (getline(file, line)){// loop to read in each line of the csv file
    data.clear();
    stringstream w(line);
    while(getline(w, cell, ',')){// knows to enter into new column as it is separated by comma ','
      data.push_back(cell);
    }
    default_Fin.push_back(data);// enter data vector into default_Fin vector
  }

  vector<double> test_data{0,1,12537.01,103292.52};// fized test data (can be changed) to find its classification / if it will default

  int tid; //thread id
  double distance[10000];
  double dist;
  //string check[10001], index[10001]; //used for testing
  int i, k = 7; //fixed k (can be changed)
  vector<pair <double, double> > neighbor;// vector pair used later to enter distance for each data and its original index in default_Fin vector

  //start of parallelization
  #pragma omp parallel private(tid, dist, i) shared(default_Fin, test_data, distance, neighbor)
  {
    tid = omp_get_thread_num(); //gets thread id

    #pragma omp for
      for (i = 0; i < 10000; i++){ //
        dist = euclid_dist(default_Fin, test_data, i+1);
        distance[i] = dist;
        //index[i] = default_Fin[i+1][0]; //used for testing
        //check[i] = default_Fin[i+1][3];
      }

    if (tid == 0){//master thread
      for(int j = 0; j < 10000; j++){
        neighbor.push_back(make_pair(distance[j], j));
      }
      //cout << neighbor[0].first << " " << neighbor[0].second << " " << nearest[0] << " " << index[0] << " " << check[0] << endl; //used for testing
      //cout << neighbor[9999].first << " " << neighbor[9999].second << " " << nearest[9999] << " " << index[9999] << " " << check[9999] << endl;
      sort(neighbor.begin(), neighbor.end());//sort neighbor vector pair that contains all of the distances
      int idx, defaulted, notDefault;// idx used to help find row number of default_Fin vector, defaulted & and notDefault used to keep count of each class
      string classification; //store string class of default_Fin vector given its row number
      for(int x = 0; x < k; x++){// looks through the first k values of the sorted neighbor class
        cout << "Distance: " << neighbor[x].first << " Original index: " << neighbor[x].second << endl;
        idx = 1 + neighbor[x].second; //gets original store index and increments it by 1 to match up with default_Fin vector
        classification = default_Fin[idx][4];
        if(classification[0] == '0'){
          notDefault++;
        }
        else if(classification[0] == '1'){
          defaulted++;
        }
      }
      cout << "Defaulted neighbor count: " << defaulted << endl;
      cout << "Not Defaulted neighbor count: " << notDefault << endl;

      if(notDefault > defaulted){//based on what class has a higher count will determine its classification
        cout << "The new data is under the classification of 0 or not defaulted" << endl;
      }else{
        cout << "The new data is under the classification of 1 or defaulted" << endl;
      }
    }
  }

  return 0;
}
