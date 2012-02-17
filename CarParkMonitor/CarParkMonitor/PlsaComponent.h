#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <memory.h>

class PlsaComponent
{
private:
	int numberOfCategories;
	int numberOfRows;
	int numberOfColumns;
	float** data;
	float** documentTopicMat;
	float** wordTopicMat;

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

public:
	PlsaComponent(float** data, int rows, int cols, int topics);;
	~PlsaComponent(void);

	float** getDocumentTopicMatrix();

	float** getWordTopicMatrix();

	void performPLSA();
};