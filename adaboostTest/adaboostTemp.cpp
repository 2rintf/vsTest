#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>  
#include <string>  
#include <vector>  
#include <opencv2/opencv.hpp>
#include <cmath>
#include <string>

using namespace cv;
using namespace std;

const int DIM = 2;//����ά��
const int NUM_OF_ITER = 40;//adaboost��������
/*����ӹ��ܣ������ʽ���ĳһֵʱֹͣ����*/

//װ����
struct DATA
{
	float num[DIM];
	float label;
};

//�����������ǿ�������е���������ֵ
struct StumpMesg
{
	int dim;//�ڼ������ݣ�ά����
	float thresh;
	int type;//0:С�ڣ�1������
	float alphaVal;
};

vector<DATA> DATASET;
StumpMesg sm;
vector<StumpMesg> SMS;
float alpha;


Mat getOneColData(vector<DATA>& ds, int col);
Mat getLabelArray(vector<DATA>& ds);
void stumpClassifiy(Mat dataMat, Mat& classify, int dimNow, float threshVal, int type);
void buildStump(vector<DATA>& ds, Mat& D, Mat& bestClassEst, float& errorToCal);
void adaboost(vector<DATA>& ds);

int main()
{
	DATA d;

	d.num[0] = 1.0f;
	d.num[1] = 2.1f;
	d.label = 1.0f;
	DATASET.push_back(d);

	d.num[0] = 2.0f;
	d.num[1] = 1.1f;
	d.label = 1.0f;
	DATASET.push_back(d);

	d.num[0] = 1.3f;
	d.num[1] = 1.0f;
	d.label = -1.0f;
	DATASET.push_back(d);

	d.num[0] = 1.0f;
	d.num[1] = 1.0f;
	d.label = -1.0f;
	DATASET.push_back(d);

	d.num[0] = 2.0f;
	d.num[1] = 1.0f;
	d.label = 1.0f;
	DATASET.push_back(d);


	Mat labelArr = getLabelArray(DATASET);
	//Mat dataArr1 = getOneColData(DATASET, 0);

	cout << "ground truth : " << labelArr.t() << endl;
	//cout << dataArr1 << endl;

	adaboost(DATASET);

	waitKey(0);
	return 0;
}

/**
@brief ���������������
@param dataMat ����
@param classify ���������������ࣩ
@param dimNow �����������еĵڼ���
@param threshVal ��ֵ
@param type ����������С�ڻ��Ǵ��ڡ� 0��С�ڣ�1������
*/
void stumpClassifiy(Mat dataMat, Mat& classify, int dimNow, float threshVal, int type)
{

	Mat result = Mat::ones(dataMat.rows, dataMat.cols, CV_32F);

	//С����ֵ
	if (type == 0)
	{
		for (int i = 0; i < dataMat.cols; i++) {
			for (int j = 0; j < dataMat.rows; j++) {
				if (dataMat.at<float>(j, i) <= threshVal) {
					result.at<float>(j, i) = -1.f;
				}
			}
		}
	}
	//������ֵ
	else
	{
		for (int i = 0; i < dataMat.cols; i++) {
			for (int j = 0; j < dataMat.rows; j++) {
				if (dataMat.at<float>(j, i) >= threshVal) {
					result.at<float>(j, i) = -1.f;
				}
			}
		}
	}

	result.copyTo(classify);
}


/**
@param col ��0��ʼ����
*/
Mat getOneColData(vector<DATA>& ds, int col)
{
	int counter = 0;
	int l = ds.size();
	vector<float> data(l);
	for (auto iter = ds.cbegin(); iter != ds.cend(); iter++) {
		data[counter] = (*iter).num[col];
		counter++;
	}
	Mat dataMat(data, CV_32F);
	return dataMat;
}

Mat getLabelArray(vector<DATA>& ds)
{
	int counter = 0;
	int l = ds.size();
	vector<float> label(l);
	for (auto iter = ds.cbegin(); iter != ds.cend(); iter++) {
		label[counter] = (*iter).label;
		counter++;
	}

	Mat labelArray(label, CV_32F);
	return labelArray;
}



void buildStump(vector<DATA>& ds, Mat& D, Mat& bestClassEst, float& errorToCal)
{
	//Mat dataMat;
	float minError = INFINITY;
	float numSteps = 10.f;//�������������ڼ��㲽��
	float threshVal;
	float weightedErrorRate;
	Mat labelMat = getLabelArray(ds);


	/*ȡ��ds��num[]�����ݵ�Mat�У�ѭ������ȡ����num[]�����С���������ж���ά*/
	for (int i = 0; i < DIM; i++) {
		/*---------��ȡÿһ�е�����ֵ,����Mat��---start-------*/
		//int counter = 0;
		//int l = ds.size();
		//vector<float> data(l);
		//for (auto iter = ds.cbegin(); iter != ds.cend(); iter++) {
		//	data[counter] = (*iter).num[i];
		//	counter++;
		//}
		//Mat dataMat(data, CV_32F);
		Mat dataMat = getOneColData(ds, i);
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
			//cout << "*************************" << endl;
			//cout << "thresh = " << threshVal << endl;
			for (int k = 0; k < 2; k++) {
				Mat classifyMat;
				stumpClassifiy(dataMat, classifyMat, i, threshVal, k);

				//cout << "classifyMat : " << classifyMat.t() << endl;

				/*��Ԥ�������ȷ����0��Ȼ����D��Ȩ���ü�Ȩ������*/
				Mat errorMat;
				compare(classifyMat, labelMat, errorMat, CMP_NE);
				errorMat = errorMat / 255;
				errorMat.convertTo(errorMat, CV_32F);
				//cout << errorMat << endl;
				//cout << classifyMat.t() << endl;

				//errorMat =  classifyMat.dot(errorMat);
				//cout << errorMat << endl;
				Mat weightedError;
				weightedError = (D.t()) * errorMat;
				weightedErrorRate = weightedError.at<float>(0, 0);

				//cout << "real-time D = " << D.t() << endl;
				//cout <<"real-time error = "<< weightedErrorRate <<endl;


				if (weightedErrorRate < minError)
				{
					minError = weightedErrorRate;
					errorToCal = minError;
					classifyMat.copyTo(bestClassEst);
					//cout << "update minError = " << weightedErrorRate << endl;
					sm.dim = i + 1;
					sm.thresh = threshVal;
					sm.type = k;

				}
				//cout << "*************************" << endl;
			}
		}


		//cout <<"thresh = "<< sm.thresh << endl;
	}

}

void adaboost(vector<DATA>& ds)
{
	float m = ds.size();
	Mat D(m, 1, CV_32F, Scalar(1 / m));
	//Mat dataMat;
	Mat aggClassEst = Mat::zeros(m, 1, CV_32F);
	Mat classEst;
	Mat labelMat = getLabelArray(ds);
	Mat expon;

	Mat aggErrors;
	float weightedErrorRate;
	float errorRate;


	/**
	ʵ���Ϸŵ���Ŀ�У�����������������3�Σ���Ϊ�ܹ�����������������
	ֻ������θĽ�����ʹ��ÿ�����������ڲ����Ե�����ֱ���ҵ���������͵ķ�����ֵ
	��������adaboost�����ļ�Ȩ���ݲ���������
	*/
	for (int i = 0; i < 10; i++) {

		//cout << D << endl;
		buildStump(ds, D, classEst, weightedErrorRate);

		cout << " min error : " << weightedErrorRate << endl;
		alpha = 0.5 * log((1.0 - weightedErrorRate) / (weightedErrorRate > 1e-16 ? weightedErrorRate : 1e-16));
		sm.alphaVal = alpha;
		SMS.push_back(sm);
		//expon = ((-1 * alpha * labelMat).t()) * classEst;
		//�˴���Ҫ��mul����Ϊ����������Ҫ��ӣ�ֻ��Ҫ��˼��ɡ�exponӦ���Ǹ���������
		expon = ((-1 * alpha * labelMat)).mul(classEst);
		//cout << "classEst: " << classEst << endl;
		cout << expon << endl;


		Mat afterExp;
		exp(expon, afterExp);
		//cout << "afterExp = " << afterExp << endl;
		//�˴�ʹ��mul��ԭ��ͬ����expon��D�Ǹ���������
		//D = D * afterExp;
		D = D.mul(afterExp);
		//cout << "calculating D: " << D << endl;
		Mat DSum;
		//��ͺ�����������������0ʱ�����������ĺͣ���1ʱ�����������ĺ�
		reduce(D, DSum, 0, CV_REDUCE_SUM, CV_32F);
		//cout << "DSum = " << DSum << endl;

		D.convertTo(D, CV_64F);
		DSum.convertTo(DSum, CV_64F);

		//ʹ�ó���������CV_64F��ʽ
		divide(D, DSum, D);
		//D = (D.t()) / DSum;
		cout << "TEST " << D << endl;
		D.convertTo(D, CV_32F);

		/*float dsum_f = DSum.at<float>(0, 0);
		dsum_f = 1 / dsum_f;
		cout << "dsum_f = " << dsum_f << endl;
		D = D * dsum_f;*/
		cout << "new D:" << D.t() << endl;
		//������������ڴ˴���Ȩ����
		aggClassEst += alpha * classEst;
		cout << "alpha" << to_string(i) << " = " << alpha << endl;

		//ע�⣡ CV_SIGN������Ǵ���0����255��С��0����0������ͨsign������ͬ������Ҳ��Ҫת��type
		Mat signOfaggClassEst = CV_SIGN(aggClassEst);
		signOfaggClassEst = signOfaggClassEst / 255;
		signOfaggClassEst.convertTo(signOfaggClassEst, CV_32F);

		for (int k = 0; k < signOfaggClassEst.cols; k++) {
			for (int j = 0; j < signOfaggClassEst.rows; j++) {
				if (signOfaggClassEst.at<float>(j, k) == 0) {
					signOfaggClassEst.at<float>(j, k) = -1;
				}
			}
		}

		Mat compareResult;
		//�Ƚ�ʱ��������Ҫ�󣨴˴���CMP_NE��������ȣ�������255����֮��0
		compare(signOfaggClassEst, labelMat, compareResult, CMP_NE);
		compareResult = compareResult / 255;
		compareResult.convertTo(compareResult, CV_32F);


		Mat errorNum;
		reduce(compareResult, errorNum, 0, CV_REDUCE_SUM, CV_32F);
		//errorRate = (errorNum / m);
		//��������о�ֻ��һ��Ԫ�أ�����ֱ���������Ϊfloat����
		errorRate = errorNum.at<float>(0, 0);
		errorRate = errorRate / m;
		cout << "errorRate =  " << errorRate << endl;

		if (errorRate == 0.0)
		{
			int w = SMS.size();
			for (int v = 0; v < w; v++) {
				cout << "stump" << to_string(v + 1) << "={" << endl;
				cout << "       col:" << SMS[v].dim << endl;
				cout << "       thresh:" << SMS[v].thresh << endl;
				cout << "       type:" << SMS[v].type << "}" << endl;
				cout << "       alpha:" << SMS[v].alphaVal << "}" << endl;
			}

			break;
		}
	}



}
