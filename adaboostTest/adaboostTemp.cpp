#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>  
#include <string>  
#include <vector>  
#include <opencv2/opencv.hpp>
#include <cmath>

using namespace cv;
using namespace std;

const int DIM = 2;

struct DATA
{
	float num[DIM];
	float label;
};

struct WeakClassifier
{


};

vector<DATA> DATASET;
float alpha;
Mat expon;

void stumpClassifiy(Mat dataMat, Mat& classify, int dimNow, float threshVal, int type);
void buildStump(vector<DATA>& ds, Mat& D);
void adaboost(vector<DATA>& ds);

int main()
{
	waitKey(0);
	return 0;
}

/**
*@param dimNow 现在是数据中的第几列
*@param type 迭代种类是小于还是大于。 0：小于，1：大于
*/
void stumpClassifiy(Mat dataMat, Mat& classify, int dimNow, float threshVal, int type)
{

}

void buildStump(vector<DATA>& ds, Mat& D)
{
	//Mat dataMat;
	float minError = INFINITY;
	float numSteps = 10.f;//迭代步数，用于计算步长
	float threshVal;

	/*取出ds中num[]的数据到Mat中，循环次数取决于num[]数组大小，即数据有多少维*/
	for (int i = 0; i < DIM; i++) {
		/*---------获取每一列的数据值,放入Mat中---start-------*/
		int counter = 0;
		int l = ds.size();
		vector<float> data(l);
		for (auto iter = ds.cbegin(); iter != ds.cend(); iter++) {
			data[counter] = (*iter).num[i];
			counter++;
		}
		Mat dataMat(data, CV_32F);
		/*---------获取每一列的数据值,放入Mat中-----end-------*/


		double minVal, maxVal;
		double* pminVal = &minVal;
		double* pmaxVal = &maxVal;
		int minValXY[2];
		int maxValXY[2];
		/*获取这一列数据的最大最小值，用于计算迭代步长*/
		minMaxIdx(dataMat, pminVal, pmaxVal, minValXY, maxValXY);
		float stepSize = (maxVal - minVal) / numSteps;//迭代步长

		for (int j = -1; j < numSteps + 2; j++) {

			threshVal = minVal + (float)j * stepSize;//分类用阈值

			for (int k = 0; k < 2; k++) {
				Mat classifyMat;
				stumpClassifiy(dataMat, classifyMat, i, threshVal, k);

			}
		}



	}

}

void adaboost(vector<DATA>& ds)
{
	float m = ds.size();
	Mat D(m, 1, CV_32F, Scalar(1 / m));
	Mat dataMat;
	Mat aggClassEst = Mat::zeros(m, 1, CV_32F);

	Mat aggErrors;
	float weightedErrorRate;
	float errorRate;




	/**实际上放到项目中，这个大迭代次数仍是3次，因为总共就是三个弱分类器
	   只不过这次改进可以使得每个弱分类器内部可以迭代，直至找到错误率最低的分类阈值
	   这样子与adaboost交互的加权数据才真正有用
	*/
	for (int i = 0; i < 40; i++) {

	}



}
