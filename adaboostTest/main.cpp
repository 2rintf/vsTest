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

	FileStorage fs("D:\\�ǹ�ϵͳ\\NewTestData\\abc.xml", FileStorage::READ);

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
 * ��XML�ж�ȡ���������Լ���㷨�ģ�
		��Ԥ��������������dataPic��classEst[3]�У�
		���˹���ǽ��������dataPic��label��
	�����dataSet.
 * !!ѭ���߽�ֵȡ����XML�ڵ���
*/
void readTypeFromXML(FileStorage& fs, vector<dataPic>& dpSet)
{
	string nodeName;
	dataPic dp;

	int xx = 0;

	printf("start read data from xml...\n");
	for (int i = 1; i <= 360; i++) {//�˴��߽�ֵ�����

		nodeName = "data_" + to_string(i);
		dp.num = i;

		FileNode fn = fs[nodeName];
		if (fn.type() != FileNode::SEQ) {
			cerr << "read node error!" << endl;
		}
		FileNodeIterator it = fn.begin(), it_end = fn.end();


		/*���ڻ�ȡlabelʱ�ķ���*/
		//cout << (int)*(++++it) << endl;

		int num = 0;
		for (; it != it_end; ++it) {
			if (num == 3) {
				//ÿһ���ڵ�ĵ�4��ֵΪ��Ϊ�궨��ǩ
				dp.label = (int)*it;
			}
			else
			{
				//��ԭ�������жϽ������Ϊ����͸���
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


	//��ȡ�궨��ǩ����
	Mat labelArrary = getLabelArray(dpSet);

	/*three classifier*/
	for (int i = 0; i < 3; i++) {

		//��ȡ��Ӧ��������Ԥ����
		int counter = 0;
		int l = dpSet.size();
		vector<float> label(l);
		for (auto iter = dpSet.cbegin(); iter != dpSet.cend(); iter++) {
			label[counter] = (*iter).classEst[i];
			counter++;
		}
		Mat classEst(label, CV_32F);

		//cout << classEst << endl;




		//��Ϊerror�ڷ�ĸ������Ҫ��ֹ������0
		alpha = 0.5 * log((1.0 - error) / (error > 1e-16 ? error : 1e-16));
		expon = (-1 * alpha * labelArrary.t()) * classEst;

		Mat afterExp;
		exp(expon, afterExp);

		D = D * afterExp;
		Mat DSum;
		//��ͺ�����������������0ʱ�����������ĺͣ���1ʱ�����������ĺ�
		reduce(D, DSum, 0, CV_REDUCE_SUM, CV_32F);
		D = D / DSum;

		//������������ڴ˴���Ȩ����
		aggClassEst += alpha * classEst;
		//��ֵΪadaboost�����ǿ�������ķ��������ڽ���ѭ����
		Mat signOfaggClassEst = CV_SIGN(aggClassEst);

		Mat compareResult;
		//�Ƚ�ʱ��������Ҫ�󣨴˴���CMP_NE��������ȣ�������255����֮��0
		compare(signOfaggClassEst, labelArrary.t(), compareResult, CMP_NE);
		compareResult = compareResult / 255;


		Mat errorNum;
		reduce(compareResult, errorNum, 0, CV_REDUCE_SUM, CV_32F);
		//errorRate = (errorNum / m);
		//��������о�ֻ��һ��Ԫ�أ�����ֱ���������Ϊfloat����
		errorRate = errorNum.at<float>(0, 0);
		errorRate = errorRate / m;
		cout << "errorRate: " << errorRate << endl;


		/*δ���ԣ��ټ��һ���߼�*/

	}

}

/**
* ��ȡÿһ�����ݵı궨��ǩ
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
