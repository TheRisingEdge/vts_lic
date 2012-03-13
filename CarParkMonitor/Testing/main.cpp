#include <stdio.h>
#include <vector>
#include <algorithm>
#include <memory>
#include <opencv2\opencv.hpp>
#include <conio.h>

using namespace std;
using namespace cv;

//bool operator()(const _Tp& a, const _Tp& b)

bool sameClass(const int& a,const int& b)
{
	return a-b == 0;
}

int main()
{
	vector<int> vec;
	vec.push_back(1);
	vec.push_back(2);
	vec.push_back(3);
	vec.push_back(5);
	vec.push_back(5);

	vector<int> labels;

	cv::partition(vec, labels, sameClass);

	for_each(labels.begin(), labels.end(), [](int val)		
	{
		cout << val << endl;
	});


	getch();
	return 0;
}
