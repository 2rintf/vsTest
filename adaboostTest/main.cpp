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

	

	waitKey(0);

	return 0;
}








/**
 * 从XML中读取三种似物性检测算法的输出结果，存入dataPic的classEst[3]中，然后
 * 并入dataSet.
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
		int num = 0;
		for (; it != it_end; ++it) {

			cout << (int)*it << endl;
			dp.classEst[num] = (int)*it;
			num++;
		}
		xx++;
		dpSet.push_back(dp);
	}
	printf("finish read data from xml...\n");
	printf("total data num : %i\n", xx);

}


void adaBoost(Mat classEst, vector<dataPic>& dpSet, float error)
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
		//因为error在分母，所以要防止它等于0
		alpha = 0.5 * log((1.0 - error) / (error > 1e-16 ? error : 1e-16));
		expon = (-1 * alpha * labelArrary.t()) * classEst.t();

		Mat afterExp;
		exp(expon, afterExp);

		D = D * afterExp;
		Mat DSum;
		reduce(D, DSum, 0, CV_REDUCE_SUM);
		D = D / DSum;

		aggClassEst += alpha * classEst.t();

		Mat signOfaggClassEst = CV_SIGN(aggClassEst);

		Mat compareResult;
		compare(signOfaggClassEst, labelArrary.t(), compareResult, CMP_NE);


		//aggErrors  errorRate



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
