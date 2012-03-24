#include "DebugHelper.h"
#include <assert.h>


DebugHelper::DebugHelper(void)
{
}


DebugHelper::~DebugHelper(void)
{
}

void DebugHelper::assertAllLabeled( const vector<shared_ptr<blob>> vec )
{
	for_each(begin(vec), end(vec), [](shared_ptr<blob> b){
		assert(b->id != ID_UNDEFINED);
	});
}

void DebugHelper::assertAllUnlabeled( const vector<shared_ptr<blob>> vec )
{
	for_each(begin(vec), end(vec), [](shared_ptr<blob> b){
		assert(b->id == ID_UNDEFINED);
	});
}
