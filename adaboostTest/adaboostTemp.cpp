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

const int DIM = 2;//数据维度
const int NUM_OF_ITER = 40;//adaboost迭代次数
/*可添加功能：错误率降到某一值时停止迭代*/

//装数据
struct DATA
{
	float num[DIM];
	float label;
};

//用于输出最终强分类器中的弱分类数值
struct StumpMesg
{
	int dim;//第几列数据（维数）
	float thresh;
	int type;//0:小于；1：大于
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
@brief 弱分类器分类过程
@param dataMat 输入
@param classify 分类结果（正、负类）
@param dimNow 现在是数据中的第几列
@param threshVal 阈值
@param type 迭代种类是小于还是大于。 0：小于，1：大于
*/
void stumpClassifiy(Mat dataMat, Mat& classify, int dimNow, float threshVal, int type)
{

	Mat result = Mat::ones(dataMat.rows, dataMat.cols, CV_32F);

	//小于阈值
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
	//大于阈值
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
@param col 从0开始计数
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
	float numSteps = 10.f;//迭代步数，用于计算步长
	float threshVal;
	float weightedErrorRate;
	Mat labelMat = getLabelArray(ds);


	/*取出ds中num[]的数据到Mat中，循环次数取决于num[]数组大小，即数据有多少维*/
	for (int i = 0; i < DIM; i++) {
		/*---------获取每一列的数据值,放入Mat中---start-------*/
		//int counter = 0;
		//int l = ds.size();
		//vector<float> data(l);
		//for (auto iter = ds.cbegin(); iter != ds.cend(); iter++) {
		//	data[counter] = (*iter).num[i];
		//	counter++;
		//}
		//Mat dataMat(data, CV_32F);
		Mat dataMat = getOneColData(ds, i);
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
			//cout << "*************************" << endl;
			//cout << "thresh = " << threshVal << endl;
			for (int k = 0; k < 2; k++) {
				Mat classifyMat;
				stumpClassifiy(dataMat, classifyMat, i, threshVal, k);

				//cout << "classifyMat : " << classifyMat.t() << endl;

				/*将预测分类正确项置0，然后用D加权，得加权错误率*/
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
	实际上放到项目中，这个大迭代次数仍是3次，因为总共就是三个弱分类器
	只不过这次改进可以使得每个弱分类器内部可以迭代，直至找到错误率最低的分类阈值
	这样子与adaboost交互的加权数据才真正有用
	*/
	for (int i = 0; i < 10; i++) {

		//cout << D << endl;
		buildStump(ds, D, classEst, weightedErrorRate);

		cout << " min error : " << weightedErrorRate << endl;
		alpha = 0.5 * log((1.0 - weightedErrorRate) / (weightedErrorRate > 1e-16 ? weightedErrorRate : 1e-16));
		sm.alphaVal = alpha;
		SMS.push_back(sm);
		//expon = ((-1 * alpha * labelMat).t()) * classEst;
		//此处需要用mul，因为计算结果不需要相加，只需要相乘即可。expon应该是个列向量。
		expon = ((-1 * alpha * labelMat)).mul(classEst);
		//cout << "classEst: " << classEst << endl;
		cout << expon << endl;


		Mat afterExp;
		exp(expon, afterExp);
		//cout << "afterExp = " << afterExp << endl;
		//此处使用mul的原因同计算expon。D是个列向量。
		//D = D * afterExp;
		D = D.mul(afterExp);
		//cout << "calculating D: " << D << endl;
		Mat DSum;
		//求和函数，第三个变量置0时，求列向量的和；置1时，求行向量的和
		reduce(D, DSum, 0, CV_REDUCE_SUM, CV_32F);
		//cout << "DSum = " << DSum << endl;

		D.convertTo(D, CV_64F);
		DSum.convertTo(DSum, CV_64F);

		//使用除法必须是CV_64F格式
		divide(D, DSum, D);
		//D = (D.t()) / DSum;
		cout << "TEST " << D << endl;
		D.convertTo(D, CV_32F);

		/*float dsum_f = DSum.at<float>(0, 0);
		dsum_f = 1 / dsum_f;
		cout << "dsum_f = " << dsum_f << endl;
		D = D * dsum_f;*/
		cout << "new D:" << D.t() << endl;
		//多个弱分类器在此处加权叠加
		aggClassEst += alpha * classEst;
		cout << "alpha" << to_string(i) << " = " << alpha << endl;

		//注意！ CV_SIGN的输出是大于0的置255，小于0的置0，与普通sign函数不同，而且也需要转换type
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
		//比较时，若符合要求（此处是CMP_NE，即不相等），则置255；反之置0
		compare(signOfaggClassEst, labelMat, compareResult, CMP_NE);
		compareResult = compareResult / 255;
		compareResult.convertTo(compareResult, CV_32F);


		Mat errorNum;
		reduce(compareResult, errorNum, 0, CV_REDUCE_SUM, CV_32F);
		//errorRate = (errorNum / m);
		//结果矩阵中就只有一个元素，把它直接提出来作为float变量
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
