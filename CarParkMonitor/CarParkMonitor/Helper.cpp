#include "Helper.h"


Helper::Helper(void)
{
}


Helper::~Helper(void)
{
}

float** Helper::MatToFloats( Mat_<float> m )
{
	int rows = m.rows;
	int cols = m.cols;

	float** fmat = new float*[rows];
	for(int i = 0; i < rows; i++)
	{
		fmat[i] = new float[cols];
		for(int j = 0; j < cols; j++)
		{	
			fmat[i][j] = m.at<float>(i,j);
		}		
	}

	return fmat;
}

Mat_<float> Helper::floatsToMat( float** data, int rows, int cols )
{
	Mat_<float> mat = Mat_<float>(rows, cols);

	for(int i = 0; i < rows; i++)
	{
		for(int j = 0; j < cols; j++)
		{
			mat[i][j] = data[i][j];
		}
	}

	return mat;
}



void Helper::writeKeypointCollectionVec( FileStorage f,const char* sizeKey, const char* baseKey, vector<vector<KeyPoint>> carKeypoints )
{
	assert(f.isOpened());

	int size = carKeypoints.size();
	f << sizeKey << size;

	for(int i = 0; i < size;i++)
	{
		vector<KeyPoint> keypoints = carKeypoints[i];

		char* key = new char[50];
		sprintf(key, "%s-%d", baseKey,i);

		write(f, key, keypoints);	
	}	
}

void Helper::readKeypointCollectionVec( FileStorage f,const char* sizeKey, const char* baseKey, vector<vector<KeyPoint>>* carKeypoints )
{
	assert(f.isOpened());

	int size = 0;
	f[sizeKey] >> size;

	vector<KeyPoint> keypoints;
	for(int i = 0; i < size;i++)
	{		
		char* key = new char[20];
		sprintf(key, "%s-%d", baseKey, i);

		read(f[key], keypoints);
		carKeypoints->push_back(keypoints);
	}	

	assert(carKeypoints->size() > 0);
}

void Helper::writeMatVec( FileStorage f, const char* baseKey, vector<Mat> carDescriptors )
{
	assert(f.isOpened());

	int size = carDescriptors.size();
	f << baseKey << "[";
	for(int i = 0; i < size ;i++){
		f << "{" << "item" << carDescriptors[i];
		f << "}";
	}
	f << "]";
}

void Helper::readMatVec( FileStorage f, const char* baseKey, vector<Mat>* carDescriptors )
{
	assert(f.isOpened());

	FileNode vec = f[baseKey];
	FileNodeIterator it = vec.begin(), it_end = vec.end();												
	for( ; it != it_end; ++it)
	{					
		Mat m;						 
		(*it)["item"] >> m;				
		carDescriptors->push_back(m);
	}
}

Mat_<float> Helper::filterColumns(Mat_<float> origin, int* columnsKept, int count)
{
	Mat_<float> dest = Mat_<float>(origin.rows, count);
	
	for(int i = 0; i < origin.rows; i++)
	{
		int insIndex = 0;
		for(int j = 0; j < count; j++)
		{
			int keptColumn = columnsKept[j];
			dest[i][insIndex++] = origin[i][keptColumn];			
		}
	}

	return dest;
}

vector<int> Helper::intArrayToVector( int* arr, int count )
{
	vector<int> vec;
	for(int i = 0; i < count; i++)
	{
		vec.push_back(arr[i]);
	}
	return vec;
}

int* Helper::vectorToIntArray( vector<int> vec, int* count /*= NULL*/ )
{
	int* arr = new int[vec.size()];
	*count = vec.size();

	for(int i = 0; i < vec.size(); i++)
	{
		arr[i] = vec[i];
	}

	return arr;
}

