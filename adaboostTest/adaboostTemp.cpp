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
*@param dimNow �����������еĵڼ���
*@param type ����������С�ڻ��Ǵ��ڡ� 0��С�ڣ�1������
*/
void stumpClassifiy(Mat dataMat, Mat& classify, int dimNow, float threshVal, int type)
{

}

void buildStump(vector<DATA>& ds, Mat& D)
{
	//Mat dataMat;
	float minError = INFINITY;
	float numSteps = 10.f;//�������������ڼ��㲽��
	float threshVal;

	/*ȡ��ds��num[]�����ݵ�Mat�У�ѭ������ȡ����num[]�����С���������ж���ά*/
	for (int i = 0; i < DIM; i++) {
		/*---------��ȡÿһ�е�����ֵ,����Mat��---start-------*/
		int counter = 0;
		int l = ds.size();
		vector<float> data(l);
		for (auto iter = ds.cbegin(); iter != ds.cend(); iter++) {
			data[counter] = (*iter).num[i];
			counter++;
		}
		Mat dataMat(data, CV_32F);
		/*---------��ȡÿһ�е�����ֵ,����Mat��-----end-------*/


		double minVal, maxVal;
		double* pminVal = &minVal;
		double* pmaxVal = &maxVal;
		int minValXY[2];
		int maxValXY[2];
		/*��ȡ��һ�����ݵ������Сֵ�����ڼ����������*/
		minMaxIdx(dataMat, pminVal, pmaxVal, minValXY, maxValXY);
		float stepSize = (maxVal - minVal) / numSteps;//��������

		for (int j = -1; j < numSteps + 2; j++) {

			threshVal = minVal + (float)j * stepSize;//��������ֵ

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




	/**ʵ���Ϸŵ���Ŀ�У�����������������3�Σ���Ϊ�ܹ�����������������
	   ֻ������θĽ�����ʹ��ÿ�����������ڲ����Ե�����ֱ���ҵ���������͵ķ�����ֵ
	   ��������adaboost�����ļ�Ȩ���ݲ���������
	*/
	for (int i = 0; i < 40; i++) {

	}



}
