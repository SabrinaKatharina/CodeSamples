#pragma once

#include "VertexStructs.h"
#include <gxm.h>

class CFace
{
private:
	unsigned int faceIndex;
	uint32_t faceColor;
	BasicVertex* vertices[4];
	
public:
	CFace();
	CFace(BasicVertex* v1,BasicVertex* v2,BasicVertex* v3, BasicVertex* v4, int axisType, int direction, unsigned int id);
	~CFace();
	unsigned int getFaceIndex();
	uint32_t getFaceColor();
	bool getVerticesRotating();
	void setFaceIndex(unsigned int newIndex);
	void setFaceColor(uint32_t newColor);
	void setVerticesRotating(bool rotating);
};

