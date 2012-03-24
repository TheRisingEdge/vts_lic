#pragma once
#include "Blob.h"

class DebugHelper
{
public:
	DebugHelper(void);
	~DebugHelper(void);

	static void assertAllLabeled(const vector<shared_ptr<blob>> vec);
	static void assertAllUnlabeled(const vector<shared_ptr<blob>> vec);
};

