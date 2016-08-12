#include "Face.h"
#include "CColor.h" // all available cube colors (red, green, white, yellow, blue, orange)
#include "VertexStructs.h"  //BasicVertex struct
#include <iostream>

// A Cube Face has 4 vertices, an axisType (x,y,z), a positive or negative direction and an unique id
CFace::CFace(){
}

CFace::CFace(BasicVertex* v1,BasicVertex* v2,BasicVertex* v3, BasicVertex* v4, int axisType, int direction, unsigned int id) 
{
	faceIndex = id;
	//vertices initalisieren
	vertices[0] = v1;
	vertices[1] = v2;
	vertices[2] = v3;
	vertices[3] = v4;
	// vertices isRotating setzen
	v1->rotating = false;
	v2->rotating = false;
	v3->rotating = false;
	v4->rotating = false;
	
	// zuerst den Bezugspunkt berechnen
	v1->position[axisType] = direction*0.63f;
	v2->position[axisType] = direction*0.63f;
	v3->position[axisType] = direction*0.63f;
	v4->position[axisType] = direction*0.63f;
	

	// dann seine lokale x und y Dimension berechnen (=> alle 3 Achsen berechnet)
	int localXDim = (axisType + 1) % 3;
	int localYDim = (axisType + 2) % 3;

	// X-offset für jedes Face berechnen, das in einer CubeSide liegt
	//switch-case für alle 0-9 Indices
	float offX = 0.0f, offY =0.0f;
	switch(id%9){
	
	case 0: case 3 : case 6: offX = -0.6 - 0.02; //untere Reihe
		break;
	case 1: case 4: case 7: offX = -0.2; // mittlere Reihe
		break;
	case 2: case 5: case 8: offX = 0.2+ 0.02; // obere Reihe
		break;
	}
	// Y-offset für jedes Face berechnen, das in einer CubeSide liegt
	switch(id%9){
	
	case 0: case 1 : case 2: offY = -0.6 -0.02 ; //untere Reihe
		break;
	case 3: case 4: case 5: offY = -0.2; // mittlere Reihe
		break;
	case 6: case 7: case 8: offY = 0.2 + 0.02; // obere Reihe
		break;
	}

	// lokale x-Achse: 1 und 2 Vertex (2 Vertex auf einer Linie)
	v1->position[localXDim] =  offX ;
	v2->position[localXDim] =  offX ;
	v3->position[localXDim] =  offX +0.4f;
	v4->position[localXDim] =  offX + 0.4f;
											
	// lokale y-Achse: 3 und 4 Vertex (2 Vertex auf einer Linie)
	v1->position[localYDim] = offY ;
	v2->position[localYDim] = offY+0.4f;
	v3->position[localYDim] = offY+0.4f;
	v4->position[localYDim] = offY  ;

	// Farbe bestimmen und speichern
	int cubeSideIndex = faceIndex / 9 ;
	if(cubeSideIndex == 0){
		faceColor = 0xff00ffff;
	}
	else if(cubeSideIndex == ORANGE){
		faceColor = 0xff00a5ff;
	}
	else if(cubeSideIndex == WEISS){
		faceColor = 0xffffffff;
	}
	else if(cubeSideIndex == ROT){
		faceColor = 0xff0000ff;
	}
	else if(cubeSideIndex == BLAU){
		faceColor = 0xffff0000;
	}
	else if(cubeSideIndex == GRUEN){
		faceColor = 0xff008800;
	}

	// color setzen
	v1->color = faceColor;
	v2->color = faceColor;
	v3->color = faceColor;
	v4->color = faceColor;

	// normalen berechnen
	float normal[3];
	normal[0] = normal[1] = normal[2] = 0.f;
	normal[axisType] = direction;

	for(int i = 0; i < 3; i++){
		v1->normal[i] = normal[i];
		v2->normal[i] = normal[i];
		v3->normal[i] = normal[i];
		v4->normal[i] = normal[i];
	}
}

CFace::~CFace()
{
	delete [] vertices;
}

unsigned int CFace::getFaceIndex(){
	return faceIndex;

}

uint32_t CFace::getFaceColor(){

	return faceColor;
}

bool CFace::getVerticesRotating(){

	return vertices[0]->getRotating();
}

void CFace::setFaceIndex(unsigned int newIndex){

	faceIndex = newIndex;

}

void CFace::setFaceColor(uint32_t newColor){

	faceColor = newColor;
	for(int i = 0; i < 4; ++i){
	
		vertices[i]->setColor(newColor);
		
	}
}

void CFace::setVerticesRotating(bool rotating){

	for(int i = 0; i < 4; ++i){
		vertices[i]->setRotating(rotating);

	}
}

