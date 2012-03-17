#include "Helper.h"


Helper::Helper(void)
{
}

Helper::~Helper(void)
{
}

#pragma region storage

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

#pragma endregion storage

#pragma region converters

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

Mat_<int> Helper::intArrayToMat( int* arr, int len )
{
	Mat_<int> mat(1, len);
	for(int i = 0; i < len; i++)
	{
		mat[0][i] = arr[i]; 
	}

	return mat;
}

int* Helper::MatToIntArray( Mat_<int> mat, int* len )
{
	int size = mat.cols;
	*len = size;

	int* arr = new int[size];
	for(int i = 0; i < size; i++)
	{
		arr[i] = mat[0][i];
	}

	return arr;
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

cv::Mat Helper::concatImages( Mat img1, Mat img2 )
{
	Mat imgResult(std::max(img2.rows,img1.rows),img1.cols+img2.cols,img1.type()); // Your final image

	Mat roiImgResult_Left = imgResult(Rect(0,0,img1.cols,img1.rows)); //Img1 will be on the left part
	Mat roiImgResult_Right = imgResult(Rect(img1.cols,0,img2.cols,img2.rows)); //Img2 will be on the right part, we shift the roi of img1.cols on the right

	Mat roiImg1 = img1(Rect(0,0,img1.cols,img1.rows));
	Mat roiImg2 = img2(Rect(0,0,img2.cols,img2.rows));

	roiImg1.copyTo(roiImgResult_Left); //Img1 will be on the left of imgResult
	roiImg2.copyTo(roiImgResult_Right); //Img2 will be on the right of imgResult

	return imgResult;
}

void Helper::drawText( const char* text,const Point& origin, Mat& output )
{	
	putText( output, text, origin, CV_FONT_HERSHEY_PLAIN, 1.2, Scalar(0, 0, 255));
}

void Helper::drawBlob( const blob* b, Mat& output )
{
	Rect rect = b->rect;
	Scalar color = Scalar( 0, 0, 255 );
	rectangle( output, rect.tl(), rect.br(), color);
	
	unique_ptr<char> text = unique_ptr<char>(new char[50]);
	auto data = text.get();
	sprintf(data, "%d", b->id);
	drawText(data,b->rect.tl(), output);
}

#pragma endregion converters


