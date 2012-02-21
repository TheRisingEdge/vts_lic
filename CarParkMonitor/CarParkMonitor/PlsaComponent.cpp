#include "PlsaComponent.h"
#include "Helper.h"
#include "opencv2/opencv.hpp"
#include "Content.h"

using namespace cv;

PlsaComponent::~PlsaComponent(void)
{
}

void PlsaComponent::printVector( char* title, float* Vector, int n )
{
	printf("%s\n\n", title);
	for (int i=0; i<n; ++i) {
		printf("%10.4f", Vector[i]);
	}
	printf("\n");
}

void PlsaComponent::printMatrix( char* title, float** Matrix, int nRows, int nCols )
{
	printf("%s\n\n", title);
	for (int i=0; i<nRows; ++i) {
		for (int j=0; j<nCols; ++j) {
			printf("%10.4f", Matrix[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void PlsaComponent::normalizeMatrix( float** Matrix, int nRows, int nCols )
{
	for (int i=0; i<nRows; ++i) {
		float min = Matrix[i][0];
		for (int j=0; j<nCols; ++j) {
			if (min > Matrix[i][j]) min = Matrix[i][j];
		}
		if (min < 0.0) {
			for (int j=0; j<nCols; ++j) {
				Matrix[i][j] -= min;
			}
		}
		float s = 0.0;
		for (int j=0; j<nCols; ++j) {
			s += Matrix[i][j];
		}
		if (s > 0.0) {
			for (int j=0; j<nCols; ++j) {
				Matrix[i][j] /= s;
			}
		}
	}
}

void PlsaComponent::initializeMatrix( float** Matrix, int nRows, int nCols )
{
	srand((unsigned)(time(0))); 
	for (int i=0; i<nRows; ++i) {
		for (int j=0; j<nCols; ++j) {
			Matrix[i][j] = (float)(rand() % 100) / 100.0;
		}
	}
}

void PlsaComponent::initializeZ( float* Z, int nSize )
{
	for (int i=0; i<nSize; ++i) Z[i] = 1.0/(float)(nSize);
}

float PlsaComponent::computeLikelihood( float** D, float** W, int nRows, int nCols, int nCats )
{
	float likelihood = 0.0;
	for (int i=0; i<nRows; ++i) {
		for (int j=0; j<nCols; ++j) {
			if (data[i][j] > 0.0) {
				float s = 0.0;
				for (int k = 0; k<nCats; ++k) {
					s += D[i][k] * W[k][j];
				}
				if (s > 0.0) {
					likelihood += data[i][j] * log(s);
				}
			}
		}
	}
	return likelihood;
}

void PlsaComponent::getNewN( float** N, float** D, float** W, float* Z, int nRows, int nCols, int nCats )
{
	for (int i=0; i<nRows; ++i) {
		for (int j=0; j<nCols; ++j) {
			if (data[i][j] > 0.0) {
				float s = 0.0;
				for (int k = 0; k<nCats; ++k) {
					s += D[i][k] * W[k][j] * Z[k];
				}
				if (s > 0.0) {
					N[i][j] = data[i][j] / s;
				}
				else {
					N[i][j] = 0.0;
				}
			}
			else {
				N[i][j] = 0.0;
			}
		}
	}
}

void PlsaComponent::getNewD( float** D2, float** D1, float** W2, float** W1, float** N, float* Z, int nRows, int nCols, int nCats )
{
	//it is D = N * WT multiplication
	for (int nWhich=0; nWhich<nCats; ++nWhich) {
		for (int i=0; i<nRows; ++i) {
			D2[i][nWhich] = 0.0;
			for (int j=0; j<nCols; ++j) {
				if (data[i][j] > 0.0) {
					D2[i][nWhich] += N[i][j] * W1[nWhich][j]; 
				}
			}
		}
	}
	for (int i=0; i<nRows; ++i) {
		for (int nWhich=0; nWhich<nCats; ++nWhich) {
			D2[i][nWhich] *= D1[i][nWhich];
		}
	}
}

void PlsaComponent::getNewW( float** W2, float** W1, float** D2, float** D1, float** N, float* Z, int nRows, int nCols, int nCats )
{
	//it is W = DT * N multiplication
	for(int nWhich=0; nWhich<nCats; ++nWhich) {
		for (int j=0; j<nCols; ++j) {
			W2[nWhich][j] = 0.0;
			for (int i=0; i<nRows; ++i) {
				if (data[i][j] > 0.0) {
					W2[nWhich][j] += N[i][j] * D1[i][nWhich]; 
				}
			}
		}
	}
	for(int nWhich=0; nWhich<nCats; ++nWhich) {
		for (int j=0; j<nCols; ++j) {
			W2[nWhich][j] *= W1[nWhich][j]; 
		}
	}
}

void PlsaComponent::getNewZ( float* Z, float** D, int nRows, int nCols, int nCats )
{
	for(int nWhich=0; nWhich<nCats; ++nWhich) {
		Z[nWhich] = 0.0;
		for (int j=0; j<nCols; ++j) {
			for (int i=0; i<nRows; ++i) {
				if (data[i][j] > 0.0) {
					Z[nWhich] += data[i][j] * D[i][nWhich]; 
				}
			}
		}
	}
	float s = 0.0;
	for (int i=0; i<nCats; ++i) {
		s += Z[i];
	}
	for (int i=0; i<nCats; ++i) {
		Z[i] /= s;
	}
}

bool PlsaComponent::makeApproximationStepPLSA( float** D1, float** D2, float** W1, float** W2, float** N, float* Z, int nRows, int nCols, int nCats )
{
	//This is E-step
	getNewN(N, D1, W1, Z, nRows, nCols, nCats); 
	//Next is M-step
	getNewD(D2, D1, W2, W1, N, Z, nRows, nCols, nCats);
	getNewW(W2, W1, D2, D1, N, Z, nRows, nCols, nCats);
	getNewZ(Z, D1, nRows, nCols, nCats);

	normalizeMatrix(D2, nRows, nCats);
	normalizeMatrix(W2, nCats, nCols);

	float difference = 0.0;
	for (int i=0; i<nRows; ++i) {
		for (int j=0; j<nCats; ++j) {
			difference += fabs(D1[i][j] - D2[i][j]);
			D1[i][j] = D2[i][j];
		}
	}
	for (int i=0; i<nCats; ++i) {
		for (int j=0; j<nCols; ++j) {
			difference += fabs(W1[i][j] - W2[i][j]);
			W1[i][j] = W2[i][j];
		}
	}
	difference /= (float)(nCats);
	difference /= (float)(nCats);
	#pragma region decideStopConstant

	//0.001
	if (difference < 0.008) return true;
	return false;

	#pragma endregion decideStopConstant
	
}

void PlsaComponent::performPlsa()
{
	int nRows = numberOfRows;
	int nCols = numberOfColumns;
	int nCats = numberOfTopics;

	//Allocate memory
	float** D1 = (float**)malloc(nRows * sizeof(float*));
	for (int i=0; i<nRows; ++i) {
		D1[i] = (float*)malloc(nCats * sizeof(float));
	}

	float** D2 = (float**)malloc(nRows * sizeof(float*));
	for (int i=0; i<nRows; ++i) {
		D2[i] = (float*)malloc(nCats * sizeof(float));
	}

	float** W1 = (float**)malloc(nCats * sizeof(float*));
	for (int i=0; i<nCats; ++i) {
		W1[i] = (float*)malloc(nCols * sizeof(float));
	}

	float** W2 = (float**)malloc(nCats * sizeof(float*));
	for (int i=0; i<nCats; ++i) {
		W2[i] = (float*)malloc(nCols * sizeof(float));
	}

	float** N = (float**)malloc(nRows * sizeof(float*));
	for (int i=0; i<nRows; ++i) {
		N[i] = (float*)malloc(nCols * sizeof(float));
	}

	float* Z = (float*)malloc(nCats * sizeof(float));
	//end

	/////////////////////////////Start pLSA/////////////////////////////
	printf("Start PLSA\n");

	initializeMatrix(D1, nRows, nCats);
	initializeMatrix(W1, nCats, nCols);

	normalizeMatrix(D1, nRows, nCats);
	normalizeMatrix(W1, nCats, nCols);

	initializeZ(Z, nCats);
	int nCounter = 0;
	while (!makeApproximationStepPLSA(D1, D2, W1, W2, N, Z, nRows, nCols, nCats)) {
		float currentLikelihood = computeLikelihood(D2, W2, nRows, nCols, nCats);
		printf("current likelihood %f\n", currentLikelihood);
		++nCounter;
		if (nCounter >= this->maxIterations) break;
	}	
	
	normalizeMatrix(D2, nRows, nCats);
	normalizeMatrix(W2, nCats, nCols);

	if(param.verbose)
	{
		printf("\n\n");
		printMatrix("Document-category", D2, numberOfRows, nCats);
		printMatrix("Word-category", W2, nCats, numberOfColumns);
		printf("End PLSA\n\n");	
	}

	documentTopicMat = D2;
	topicWordMat = W2;

	if (Z) free(Z);

	if (N) {
		for (int i=0; i<nRows; ++i) {
			if (N[i]) free(N[i]);
		}
		free(N);
	}

	if (W1) {
		for (int i=0; i<nCats; i++) {
			if (W1[i]) free(W1[i]);
		}
		free(W1);
	}

	if (D1) {
		for (int i=0; i<nRows; i++) {
			if (D1[i]) free(D1[i]);
		}
		free(D1);
	}
}

float** PlsaComponent::getDocumentTopicMatrix()
{
	return documentTopicMat;
}

float** PlsaComponent::getTopicWordMatrix()
{
	return topicWordMat;
}

PlsaComponent::PlsaComponent( float** data, int rows, int cols, int topics )
{
	numberOfTopics = topics; 
	numberOfRows       = rows;  
	numberOfColumns    = cols; 
	this->data = data;
}

PlsaComponent::PlsaComponent()
{

}

void PlsaComponent::testPLSA()
{
	int rows = 5;
	int cols = 6;

	float data[5][6] = {
		{0.1, 0.1, 0.1, 0.1, 0, 0},   
		{0.1, 0.2, 0.1, 0.1, 0, 0},   
		{0.2, 0.1, 0.2, 0.1, 0, 0.1},
		{0.1, 0.1, 0.0, 0.2, 0.4, 0.7},
		{0.1, 0.3, 0.6, 0.5, 0.7, 0.6},   
	};

	float** bows = new float*[5];
	for(int i = 0; i < 5; i++)
	{
		bows[i] = new float[6];
		for(int j = 0; j < 6; j++)
		{
			bows[i][j] = data[i][j];
		}
	}

	this->data = bows;
	this->numberOfRows = 5;
	this->numberOfColumns = 6;
	this->numberOfTopics = 2;
	
	this->performPlsa();
}

void PlsaComponent::performPlsa( PlsaParam param )
{
	numberOfTopics = param.topicCount; 
	numberOfRows       = param.documentCount;  
	numberOfColumns    = param.wordCount; 
	data = param.data;
	this->maxIterations = param.maxIterations;
	this->param = param;

	performPlsa();
}

void PlsaComponent::save()
{
	Mat topicWord = Helper::floatsToMat(topicWordMat, numberOfTopics, numberOfColumns);

	FileStorage f = FileStorage(Content::ymlFile("plsa"), FileStorage::WRITE);

	f << "topicCount" << numberOfTopics;
	f << "documentCount" << numberOfRows;
	f << "wordCount" << numberOfColumns;
	f << "maxIterations" << maxIterations;

	f << "topicWordMat" << topicWord;
		 	
	Mat_<int> indexes = Helper::intArrayToMat(relevantIndexes, relevantIndexesCount);
	f << "relevantIndexes" << indexes;

	f.release();
}

void PlsaComponent::load()
{
	FileStorage f = FileStorage(Content::ymlFile("plsa"), FileStorage::READ);
	Mat topicWord;
		
	f["topicCount"] >> numberOfTopics;
	f["documentCount"] >> numberOfRows;
	f["wordCount"] >> numberOfColumns;
	f["maxIterations"] >> maxIterations;

	f["topicWordMat"] >> topicWord;

	Mat_<int> relevant;
	f["relevantIndexes"] >> relevant;

	int count = 0;
	this->relevantIndexes = Helper::MatToIntArray(relevant, &count);
	this->relevantIndexesCount = count;

	this->topicWordMat = Helper::MatToFloats(topicWord);
	
	f.release();
}

int* PlsaComponent::computeRelevantFeatures( PlsaParam param )
{
	assert(param.topicCount == 1);
	assert(param.keptFeatureCount > 0);

	performPlsa(param);

	float** wordProb = this->getTopicWordMatrix();
	
	int* positions = new int[param.wordCount];
	for(int i = 0; i < param.wordCount; i++)
	{
		positions[i] = i;
	}
	
	float* fake = new float[param.wordCount];	
	for(int i = 0; i < param.wordCount; i++)
	{
		fake[i] = wordProb[0][i];
	}


	for(int i = 0; i < param.wordCount - 1; i++)
	{
		for(int j = i+1; j < param.wordCount; j++)
		{
			if(fake[i] < fake[j])
			{
				float aux = fake[i];
				fake[i] = fake[j];
				fake[j] = aux;

				int auxx = positions[i];
				positions[i] = positions[j];
				positions[j] = auxx;
			}
		}
	}

	int toKeep = param.keptFeatureCount;	
	int* relevantIndexes = new int[toKeep];

	for(int i = 0 ; i < toKeep; i++)
	{
		relevantIndexes[i] = positions[i];
	}

	this->relevantIndexes = relevantIndexes;
	this->relevantIndexesCount = param.keptFeatureCount;

	return relevantIndexes;
}




