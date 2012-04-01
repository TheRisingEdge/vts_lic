#include "Tool.h"


Tool::Tool(void)
{
}


Tool::~Tool(void)
{
}

void Tool::rectToCenter(const Point& center, Rect& r )
{
	int width = r.width;
	int height = r.height;

	Point tl = Point(center.x - width/2, center.y - height/2);
	r = Rect(tl.x, tl.y, width, height);	
}

Point Tool::rectCenter( const Rect& r )
{
	return Point(r.x + r.width/2, r.y + r.height/2);
}

bool Tool::rectInside( const Rect& r, const Rect& container )
{
	auto tl = r.tl();
	auto br = r.br();

	return container.contains(tl) && container.contains(br);
}


