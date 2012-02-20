#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <memory.h>
#include "Helper.h"

#include "opencv2/opencv.hpp"

using namespace cv;

typedef struct plsaParam
{
	float** data;
	int documentCount;
	int wordCount;
	int topicCount;

	int documentTopicRows;
	int topicWordRows;
	int topicWordCols;

	int maxIterations;
	int keptFeatureCount;

	static struct plsaParam FromCvMat(Mat_<float> m, int topics, int maxIterations = 1000)
	{
		struct plsaParam param;

		param.data = Helper::MatToFloats(m);
		param.wordCount = m.cols;
		param.documentCount = m.rows;
		param.topicCount = topics;
		param.maxIterations = maxIterations;

		return param;
	}

}PlsaParam;

class PlsaComponent
{
private:		
	float** data;
	float** documentTopicMat;
	float** topicWordMat;
	int maxIterations;

	int* relevantIndexes;
	int relevantIndexesCount;

	void printVector(char* title, float* Vector, int n);

	void printMatrix(char* title, float** Matrix, int nRows, int nCols);

	void normalizeMatrix(float** Matrix, int nRows, int nCols);

	void initializeMatrix(float** Matrix, int nRows, int nCols);

	void initializeZ(float* Z, int nSize);

	float computeLikelihood(float** D, float** W, int nRows, int nCols, int nCats);
	

	void getNewN(float** N, float** D, float** W, float* Z, int nRows, int nCols, int nCats);

	void getNewD(float** D2, float** D1, float** W2, float** W1, float** N, float* Z, int nRows, int nCols, int nCats);

	void getNewW(float** W2, float** W1, float** D2, float** D1, float** N, float* Z, int nRows, int nCols, int nCats);

	void getNewZ(float* Z, float** D, int nRows, int nCols, int nCats);

	bool makeApproximationStepPLSA(float** D1, float** D2, float** W1, float** W2, float** N, float* Z, int nRows, int nCols, int nCats);

	void performPlsa();

public:
	int numberOfRows;
	int numberOfColumns;
	int numberOfTopics;

	PlsaParam param;
	PlsaComponent();
	PlsaComponent(float** data, int rows, int cols, int topics);;
	~PlsaComponent(void);

	float** getDocumentTopicMatrix();

	float** getTopicWordMatrix();

	void testPLSA();

	void performPlsa(PlsaParam param);
	int* computeRelevantFeatures(PlsaParam param);

	void save();
	void load();
};