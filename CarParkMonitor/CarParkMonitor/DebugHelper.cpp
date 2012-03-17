#include "DebugHelper.h"
#include <assert.h>


DebugHelper::DebugHelper(void)
{
}


DebugHelper::~DebugHelper(void)
{
}

void DebugHelper::assertAllLabeled( const vector<blob*> vec )
{
	for_each(begin(vec), end(vec), [](const blob* b){
		assert(b->id != ID_UNDEFINED);
	});
}

void DebugHelper::assertAllUnlabeled( const vector<blob*> vec )
{
	for_each(begin(vec), end(vec), [](const blob* b){
		assert(b->id == ID_UNDEFINED);
	});
}
