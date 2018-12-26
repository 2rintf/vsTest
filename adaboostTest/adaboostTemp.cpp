#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>  
#include <string>  
#include <vector>  
#include <opencv2/opencv.hpp>
#include <cmath>

using namespace cv;
using namespace std;


struct DATA
{
	float num[2];
	float label;
};
vector<DATA> DATASET;
float alpha;
Mat expon;

void stumpClassifiy();
void buildStump(vector<DATA>& ds, Mat D);
void adaboost(vector<DATA>& ds);


