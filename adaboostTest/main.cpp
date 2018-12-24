#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>  
#include <string>  
#include <vector>  
#include <opencv2/opencv.hpp>
#include <cmath>

using namespace cv;
using namespace std;



struct dataPic {
	int num;
	int classEst[3];
	int label;
};
float alpha;
Mat expon;

void readTypeFromXML(FileStorage& fs, vector<dataPic>& dpSet);
Mat getLabelArray(vector<dataPic>& dpSet);

vector<dataPic> dataSet;

int num = 0;


int main() {

	FileStorage fs("D:\\城管系统\\NewTestData\\abc.xml", FileStorage::READ);

	//Mat D = Mat::ones(Size(5, 1), CV_32F);
	Mat D(5, 1, CV_32F, Scalar(1 / 5.f));
	Mat F(5, 1, CV_32F, Scalar(1 / 5.f));


	//Mat help;
	//Mat result;

	//compare(D, F, help, CMP_EQ);
	//help = help / 255.f;
	//cout << help << endl;

	//reduce(help, result, 0, CV_REDUCE_SUM,CV_32F);

	//float k;

	//k = result.at<float>(0, 0);

	//
	//k = k / 10;

	////cout << help << endl;
	//cout << result << endl;
	//cout << k << endl;


	//int l = 5;
	//vector<float> x(l);
	//for (int i = 0; i < 5; i++)
	//{
	//	x[i] = 1;
	//}

	//Mat xx(x, CV_32F);

	//cout << xx << endl;

	readTypeFromXML(fs, dataSet);

	fs.release();
	
	//for (int j = 0; j < 5; j++) {
	//	for (int i = 0; i < 3; i++) {
	//		cout << dataSet[j].classEst[i] << " ";
	//	}
	//	cout << endl;
	//}
	int counter = 0;
	int l = dataSet.size();
	vector<float> label(l);
	for (auto iter = dataSet.cbegin(); iter != dataSet.cend(); iter++) {
		label[counter] = (*iter).classEst[1];
		counter++;
	}

	Mat labelArray(label, CV_32F);

	cout << labelArray.t() << endl;

	waitKey(0);

	return 0;
}








/**
 * 从XML中读取三种似物性检测算法的：
		①预测输出结果，存入dataPic的classEst[3]中；
		②人工标记结果，存入dataPic的label中
	最后并入dataSet.
 * !!循环边界值取决于XML节点数
*/
void readTypeFromXML(FileStorage& fs, vector<dataPic>& dpSet)
{
	string nodeName;
	dataPic dp;

	int xx = 0;

	printf("start read data from xml...\n");
	for (int i = 1; i <= 360; i++) {//此处边界值需调整

		nodeName = "data_" + to_string(i);
		dp.num = i;

		FileNode fn = fs[nodeName];
		if (fn.type() != FileNode::SEQ) {
			cerr << "read node error!" << endl;
		}
		FileNodeIterator it = fn.begin(), it_end = fn.end();


		/*用于获取label时的方法*/
		//cout << (int)*(++++it) << endl;

		int num = 0;
		for (; it != it_end; ++it) {
			if (num == 3) {
				//每一个节点的第4个值为人为标定标签
				dp.label = (int)*it;
			}
			else
			{
				//把原似物性判断结果分类为正类和负类
				cout << (int)*it << endl;
				if ((int)*it == 1 || (int)*it == 2) {
					dp.classEst[num] = -1;
				}
				if ((int)*it == 0) {
					dp.classEst[num] = 1;
				}
				//dp.classEst[num] = (int)*it;
			}
			
			num++;
		}
		xx++;
		dpSet.push_back(dp);
	}
	printf("finish read data from xml...\n");
	printf("total data num : %i\n", xx);

}


void adaBoost(vector<dataPic>& dpSet, float error)
{
	float m = dpSet.size();
	Mat D(m, 1, CV_32F, Scalar(1 / m));
	Mat aggClassEst = Mat::zeros(m, 1, CV_32F);
	Mat aggErrors;
	float errorRate;


	//获取标定标签矩阵
	Mat labelArrary = getLabelArray(dpSet);

	/*three classifier*/
	for (int i = 0; i < 3; i++) {

		//获取相应分类器的预测结果
		int counter = 0;
		int l = dpSet.size();
		vector<float> label(l);
		for (auto iter = dpSet.cbegin(); iter != dpSet.cend(); iter++) {
			label[counter] = (*iter).classEst[i];
			counter++;
		}
		Mat classEst(label, CV_32F);

		//cout << classEst << endl;




		//因为error在分母，所以要防止它等于0
		alpha = 0.5 * log((1.0 - error) / (error > 1e-16 ? error : 1e-16));
		expon = (-1 * alpha * labelArrary.t()) * classEst;

		Mat afterExp;
		exp(expon, afterExp);

		D = D * afterExp;
		Mat DSum;
		//求和函数，第三个变量置0时，求列向量的和；置1时，求行向量的和
		reduce(D, DSum, 0, CV_REDUCE_SUM, CV_32F);
		D = D / DSum;

		//多个弱分类器在此处加权叠加
		aggClassEst += alpha * classEst;
		//此值为adaboost输出的强分类器的分类结果（在结束循环后）
		Mat signOfaggClassEst = CV_SIGN(aggClassEst);

		Mat compareResult;
		//比较时，若符合要求（此处是CMP_NE，即不相等），则置255；反之置0
		compare(signOfaggClassEst, labelArrary.t(), compareResult, CMP_NE);
		compareResult = compareResult / 255;


		Mat errorNum;
		reduce(compareResult, errorNum, 0, CV_REDUCE_SUM, CV_32F);
		//errorRate = (errorNum / m);
		//结果矩阵中就只有一个元素，把它直接提出来作为float变量
		errorRate = errorNum.at<float>(0, 0);
		errorRate = errorRate / m;
		cout << "errorRate: " << errorRate << endl;


		/*未测试，再检查一遍逻辑*/

	}

}

/**
* 获取每一个数据的标定标签
*/
Mat getLabelArray(vector<dataPic>& dpSet)
{
	int counter = 0;
	int l = dpSet.size();
	vector<float> label(l);
	for (auto iter = dpSet.cbegin(); iter != dpSet.cend(); iter++) {
		label[counter] = (*iter).label;
		counter++;
	}

	Mat labelArray(label, CV_32F);
	return labelArray;
}
