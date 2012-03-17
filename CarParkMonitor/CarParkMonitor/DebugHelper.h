#pragma once
#include "Blob.h"

class DebugHelper
{
public:
	DebugHelper(void);
	~DebugHelper(void);

	static void assertAllLabeled(const vector<blob*> vec);
	static void assertAllUnlabeled(const vector<blob*> vec);
};

