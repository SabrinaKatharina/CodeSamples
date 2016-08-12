// No full source code !! 
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sceerror.h>

#include <gxm.h>
#include <kernel.h>
#include <ctrl.h>

#include <display.h>
#include <libdbg.h>
#include <math.h>
#include <vectormath.h>

#include <libdbgfont.h>
#include <vector>
#include <touch.h>

#include "VertexStructs.h"  // BasicVertex struct
#include "Face.h"           // Cube Face class
#include "Axis.h"			// Axis class


/* Defines ......*/

using namespace sce::Vectormath::Simd::Aos;

extern const SceGxmProgram binaryClearVGxpStart;
extern const SceGxmProgram binaryClearFGxpStart;




// Data related to rendering vertex  .....


// libgxm ...

/*	display data ... */


/*	shader data ... */

// Shader pactcher addded.

// Data added.
static SceGxmVertexProgram		*s_basicVertexProgram		= NULL;
static SceGxmFragmentProgram	*s_basicFragmentProgram		= NULL;
static BasicVertex				*s_basicVertices			= NULL;
static uint16_t					*s_basicIndices				= NULL;
static int32_t					s_basicVerticesUId;
static int32_t					s_basicIndiceUId;


// The program parameter for the transformation of the triangle
static float s_wvpData[16];		// world view projection -> in basic_vertex_shader float4x4, daher 16
static const SceGxmProgramParameter *s_wvpParam = NULL;
static const SceGxmProgramParameter *s_rotParam = NULL;
static const SceGxmProgramParameter *s_rotAxisParam = NULL;



/* Callback function  and helper function ..

/*	@brief Main entry point for the application
	@return Error code result of processing during execution: <c> SCE_OK </c> on success,
	or another code depending upon the error
*/
int main( void );
void Update(void);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Matrizen und Quaternion
static Quat m_orientationQuat(1.0f, 0.0f, 0.0f,0.0f); // Standard-Ausrichtung
static Matrix4 finalTransformation; // Transformations-Matrix
static Matrix4 finalRotation; // Cube-Rotations-Matrix;
static Matrix4 finalAxisRotation; // Axis-Rotations-Matrix für Animation
// für Animation
void setAxisRotationMatrix(CAxis *axis, float radians);
static void setRotating(CAxis * axis, bool rotating);

static float aspectRatio = (float)DISPLAY_WIDTH / (float)DISPLAY_HEIGHT;

// sortierte Reihenfolge der 6 Seiten
static CFace* sortedFaces = new CFace[9*6]; 

// 9 feste Achsen anlegen
static CAxis* axis;
// Den Achsen die jeweiligen Faces zuweisen
void addToCubeAxis(CAxis *axis, bool horizontal, unsigned int position, int order, unsigned int cubeSideIndex);
// falls eine Achse eine enclosedCubeSide hat, muss sich diese auch mitbewegen
void shiftEnclosedSide(CAxis *axis, bool direction);

// bool für buttonPressed zum rotieren einer Achse
static bool isRButtonPressed = false;
static bool isButtonPressed = false;
static bool isLButtonPressed = false;

// bool für isAxisRotating, falls eine Achse rotiert wird;
static bool isAxisRotating =false;
// speichere den Achsenindex, der gedreht wird
static unsigned int rotatingAxisIndex;
// speichere die Richtung, in die gedreht wird
static bool direction;

// float für radians zum animieren einer Achse
float degreeToRadians(float degrees);
static float tmpRadians;

// für Backtouch
// startposition von x und y des ersten Report.id speichern
static Vector2 btStartPos;
static bool btStarting = false;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*	@brief Initializes the graphics services and the libgxm graphics library
	@return Error code result of processing during execution: <c> SCE_OK </c> on success,
	or another code depending upon the error
*/
static int initGxm( void );

/*	 @brief Creates scenes with libgxm */
static void createGxmData( void );


/*	@brief Main rendering function to draw graphics to the display */
static void render( void );

/*	@brief render libgxm scenes */
static void renderGxm( void );



/*	@brief cycle display buffer */
static void cycleDisplayBuffers( void );

/*	@brief Destroy scenes with libgxm */
static void destroyGxmData( void );




/*	@brief Function to shut down libgxm and the graphics display services
	@return Error code result of processing during execution: <c> SCE_OK </c> on success,
	or another code depending upon the error
*/
static int shutdownGxm( void );



/*	@brief User main thread parameters */


/*	@brief libc parameters */


/* Main entry point of program */
int main( void )
{
	int returnCode = SCE_OK;

	/* initialize libdbgfont and libgxm */
	returnCode =initGxm();
	SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );

    SceDbgFontConfig config;
	memset( &config, 0, sizeof(SceDbgFontConfig) );
	config.fontSize = SCE_DBGFONT_FONTSIZE_LARGE;

	returnCode = sceDbgFontInit( &config );
	SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );

	/* Message for SDK sample auto test */
	printf( "## simple: INIT SUCCEEDED ##\n" );

	/* create gxm graphics data */
	createGxmData();

	/* start joystick */
	sceCtrlSetSamplingMode(SCE_CTRL_MODE_DIGITALANALOG_WIDE);

	/* start back touch*/

	sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_START);
	

	/* 6. main loop */
	while ( true)
	{
        Update();

		render();

		cycleDisplayBuffers();
	}

	sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_STOP);
}



void Update (void)
{
	/* Back Touch Steuerung */
	SceTouchData btd; // back touch data
	int res = sceTouchRead(SCE_TOUCH_PORT_BACK, &btd, 1);
	int reportNum = btd.reportNum;

	
	// es liegt kein Finger auf dem Backtouch
	if(reportNum <= 0){
		btStarting = false; 
	}
			
	// sobald touched
	if(reportNum > 0){

		if(!btStarting){ // damit es nur einmal ausgeführt wird und somit nur die erste Touch-Position gespeichert wird
			btStartPos = Vector2(btd.report[0].x, btd.report[0].y);
		}
		btStarting = true;

		Vector2 btEndPos(btd.report[0].x, btd.report[0].y); // aktuelle x und y Position

		// Lerp: StartPos wird zu dem EndPos interpoliert
		btStartPos = lerp(1.1f, btStartPos, btEndPos);

		// berechne die Differenz von start x und end x , sowie von start y und end y

		float diffX = (btEndPos.getX() - btStartPos.getX()) / 3;
		float diffY = (btEndPos.getY() - btStartPos.getY()) / 3;

		// berechne die Geschwindigkeit
		Quat btRotationVelocity(diffY/30.f, -diffX/30.f, 0.f, 0.0f);
		m_orientationQuat += 2.5f * btRotationVelocity * m_orientationQuat;
		// normalisieren ; sonst wird es verzerrt
		m_orientationQuat = normalize(m_orientationQuat);

		// Cube-Rotations-Matrix
		finalRotation = Matrix4(m_orientationQuat, Vector3(0.0f, 0.0f, 0.0f)); 
		Matrix4 lookAt = Matrix4::lookAt(Point3(0.f, 0.f, -3.f), Point3(0.f, 0.f, 0.f), Vector3(0.f, -1.f, 0.f));

		Matrix4 perspective = Matrix4::perspective(3.141592f / 4.f,
			aspectRatio,
			0.1f,
			10.f);

		finalTransformation = perspective * lookAt * finalRotation;
	}



	/* Analogstick-Steuerung */
	SceCtrlData ct;
	sceCtrlReadBufferPositive(0, &ct, 1);
	
	// Für ein Frame wird alles berechnet!
	float x = (ct.ly / 128.0f) - 1.f;
	float y = (ct.lx / 128.0f) - 1.f;
	float z = (ct.rx / 128.0f) - 1.f;

	float s_accumulatedTurningAngle[3] = {0.f, 0.f, 0.f};
	// kleinere Werte => langsamere Drehung // große Werte => schnelle Drehung
	// 0, falls nicht gedrückt wird; Wert, falls x/y/z gesetzt ist
	s_accumulatedTurningAngle[0] += ((x < 0.04f && x > -0.04f) ? 0 : x/50.f);
	s_accumulatedTurningAngle[1] += ((y < 0.04f && y > -0.04f) ? 0 : y/50.f);
	s_accumulatedTurningAngle[2] += ((z < 0.04f && z > -0.04f) ? 0 : z/50.f);
	
	// Quaternion für Roations-geschwindigkeiten erzeugen
	Quat rotationVelocity(s_accumulatedTurningAngle[0], -s_accumulatedTurningAngle[1], s_accumulatedTurningAngle[2], 0.0f); // möglicherweise skalieren

	m_orientationQuat += 2.5f * rotationVelocity * m_orientationQuat;
	// normalisieren ; sonst wird es verzerrt
	m_orientationQuat = normalize(m_orientationQuat);

	// 4x4 Matrix mit unit-length quaternion und 3D Vektor erzeugen
	finalRotation = Matrix4(m_orientationQuat, Vector3(0.0f, 0.0f, 0.0f)); // da vllt quat (1,0,0,0)
	Matrix4 lookAt = Matrix4::lookAt(Point3(0.f, 0.f, -3.f), Point3(0.f, 0.f, 0.f), Vector3(0.f, -1.f, 0.f));

	Matrix4 perspective = Matrix4::perspective(3.141592f / 4.f,
		aspectRatio,
		0.1f,
		10.f);

	finalTransformation = perspective * lookAt * finalRotation;



	/* Steuerung mit Pfeiltasten */
	// prüfe nur ob Buttons gedrückt werden, wenn keine Achse momentan gedreht wird
	if(!isAxisRotating){

			// Achse X, Position 0 nach links shiften
			if((ct.buttons & SCE_CTRL_LEFT) && (ct.buttons & SCE_CTRL_L) != 0){
				
				if(!isButtonPressed && !isLButtonPressed)
				{
					setRotating(&axis[0], true);
					rotatingAxisIndex = 0;
					direction = false;
					isButtonPressed = true; //damit es nur einmal ausgeführt wird
					isLButtonPressed = true;
				}	
				
			} // Achse X, Position 0 nach rechts shiften
			else if((ct.buttons & SCE_CTRL_LEFT) && (ct.buttons & SCE_CTRL_R) != 0){

				if(!isButtonPressed && !isRButtonPressed)
				{

					setRotating(&axis[0], true);
					rotatingAxisIndex = 0;
					direction = true;
					isButtonPressed = true; //damit es nur einmal ausgeführt wird
					isRButtonPressed = true;
					
				}
				


			} // Achse X, Position 1 nach links shiften
			else if((ct.buttons & SCE_CTRL_UP) && (ct.buttons & SCE_CTRL_L) != 0){

				if(!isButtonPressed && !isLButtonPressed)
				{
					setRotating(&axis[1], true);
					rotatingAxisIndex = 1;
					direction = false;
					isButtonPressed = true; //damit es nur einmal ausgeführt wird
					isLButtonPressed = true;
				}
				


			} // Achse X, Position 1 nach rechts shiften
			else if((ct.buttons & SCE_CTRL_UP) && (ct.buttons & SCE_CTRL_R) != 0){

				if(!isButtonPressed && !isRButtonPressed)
				{
					setRotating(&axis[1], true);
					rotatingAxisIndex = 1;
					direction = true;
					isButtonPressed = true; //damit es nur einmal ausgeführt wird
					isRButtonPressed = true;

				}
				


			}// Achse X, Position 2 nach links shiften
			else if((ct.buttons & SCE_CTRL_RIGHT) && (ct.buttons & SCE_CTRL_L) != 0){

				if(!isButtonPressed && !isLButtonPressed)
				{
					setRotating(&axis[2], true);
					rotatingAxisIndex = 2;
					direction = false;
					isButtonPressed = true; //damit es nur einmal ausgeführt wird
					isLButtonPressed = true;
				}
				


			} // Achse X, Position 2 nach rechts shiften
			else if((ct.buttons & SCE_CTRL_RIGHT) && (ct.buttons & SCE_CTRL_R) != 0){

				if(!isButtonPressed && !isRButtonPressed)
				{

					setRotating(&axis[2], true);
					rotatingAxisIndex = 2;
					direction = true;
					isButtonPressed = true; //damit es nur einmal ausgeführt wird
					isRButtonPressed = true;

				}



			}
			
			// Achse Y, Position 0 nach links shiften
			else if((ct.buttons & SCE_CTRL_DOWN) && (ct.buttons & SCE_CTRL_L) != 0){

				if(!isButtonPressed && !isLButtonPressed)
				{
					setRotating(&axis[3], true);
					rotatingAxisIndex = 3;
					direction = false;
					isButtonPressed = true; //damit es nur einmal ausgeführt wird
					isLButtonPressed = true;
				}
				


			} // Achse Y, Position 0 nach rechts shiften
			else if((ct.buttons & SCE_CTRL_DOWN) && (ct.buttons & SCE_CTRL_R) != 0){

				if(!isButtonPressed && !isRButtonPressed)
				{

					setRotating(&axis[3], true);
					rotatingAxisIndex = 3;
					direction = true;
					isButtonPressed = true; //damit es nur einmal ausgeführt wird
					isLButtonPressed = true;

				}
			


			}

			// Achse Y, Position 1 nach links shiften
			else if((ct.buttons & SCE_CTRL_SQUARE) && (ct.buttons & SCE_CTRL_L) != 0){

				if(!isButtonPressed && !isLButtonPressed)
				{

					setRotating(&axis[4], true);
					rotatingAxisIndex = 4;
					direction = false;
					isButtonPressed = true; //damit es nur einmal ausgeführt wird
					isLButtonPressed = true;
				}
			

			} // Achse Y, Position 1 nach rechts shiften
			else if((ct.buttons & SCE_CTRL_SQUARE) && (ct.buttons & SCE_CTRL_R) != 0){

				if(!isButtonPressed && !isRButtonPressed)
				{

					setRotating(&axis[4], true);
					rotatingAxisIndex = 4;
					direction = true;
					isButtonPressed = true; //damit es nur einmal ausgeführt wird
					isRButtonPressed = true;

				}
				


			}
			// Achse Y, Position 2 nach links shiften
			else if((ct.buttons & SCE_CTRL_TRIANGLE) && (ct.buttons & SCE_CTRL_L) != 0){

				if(!isButtonPressed && !isLButtonPressed)
				{

					setRotating(&axis[5], true);
					rotatingAxisIndex = 5;
					direction = false;
					isButtonPressed = true; //damit es nur einmal ausgeführt wird
					isLButtonPressed = true;
				}
				


			} // Achse Y, Position 2 nach rechts shiften
			else if((ct.buttons & SCE_CTRL_TRIANGLE) && (ct.buttons & SCE_CTRL_R) != 0){

				if(!isButtonPressed && !isRButtonPressed)
				{

					setRotating(&axis[5], true);
					rotatingAxisIndex = 5;
					direction = true;
					isButtonPressed = true; //damit es nur einmal ausgeführt wird
					isRButtonPressed = true;

				}
				


			}
			// Achse Z, Position 0 nach links shiften
			else if((ct.buttons & SCE_CTRL_CIRCLE) && (ct.buttons & SCE_CTRL_L) != 0){

				if(!isButtonPressed && !isLButtonPressed)
				{

					setRotating(&axis[6], true);
					rotatingAxisIndex = 6;
					direction = false;
					isButtonPressed = true; //damit es nur einmal ausgeführt wird
					isLButtonPressed = true;
				}
				


			} // Achse Z, Position 0 nach rechts shiften
			else if((ct.buttons & SCE_CTRL_CIRCLE) && (ct.buttons & SCE_CTRL_R) != 0){

				if(!isButtonPressed && !isRButtonPressed)
				{

					setRotating(&axis[6], true);
					rotatingAxisIndex = 6;
					direction = true;
					isButtonPressed = true; //damit es nur einmal ausgeführt wird
					isRButtonPressed = true;

				}
				


			}
			// Achse Z, Position 1 nach links shiften
			else if((ct.buttons & SCE_CTRL_CROSS) && (ct.buttons & SCE_CTRL_L) != 0){

				if(!isButtonPressed && !isLButtonPressed)
				{

					setRotating(&axis[7], true);
					rotatingAxisIndex = 7;
					direction = false;
					isButtonPressed = true; //damit es nur einmal ausgeführt wird
					isLButtonPressed = true;
				}
				


			} // Achse Z, Position 1 nach rechts shiften
			else if((ct.buttons & SCE_CTRL_CROSS) && (ct.buttons & SCE_CTRL_R) != 0){

				if(!isButtonPressed && !isRButtonPressed)
				{
					setRotating(&axis[7], true);
					rotatingAxisIndex = 7;
					direction = true;
					isButtonPressed = true; //damit es nur einmal ausgeführt wird
					isRButtonPressed = true;

				}
				


			}
			// Achse Z, Position 2 nach links shiften
			else if((ct.buttons & SCE_CTRL_START) && (ct.buttons & SCE_CTRL_L) != 0){

				if(!isButtonPressed && !isLButtonPressed)
				{

					setRotating(&axis[8], true);
					rotatingAxisIndex = 8;
					direction = false;
					isButtonPressed = true; //damit es nur einmal ausgeführt wird
					isLButtonPressed = true;
				}
				


			} // Achse Z, Position 2 nach rechts shiften
			else if((ct.buttons & SCE_CTRL_START) && (ct.buttons & SCE_CTRL_R) != 0){

				if(!isButtonPressed && !isRButtonPressed)
				{

					setRotating(&axis[8], true);
					rotatingAxisIndex = 8;
					direction = true;
					isButtonPressed = true; //damit es nur einmal ausgeführt wird
					isRButtonPressed = true;

				}
				
			}// alles wird wieder auf false gesetzt, da kein Button gedrückt wird
			else {
				isButtonPressed = false;
				isLButtonPressed = false;
				isRButtonPressed = false;
			}
		}
	else{
		// inkrementiere bzw. dekrementiere den Winkel
		if(direction)
			tmpRadians+= 4;
		else
			tmpRadians-=4;
		// berechne die aktuelle Rotations-Matrix
		setAxisRotationMatrix(&axis[rotatingAxisIndex], degreeToRadians(tmpRadians));

		// sobald die Achse 90 grad bzw. -90 grad rotiert wurde
		if((tmpRadians >= 90 )|| (tmpRadians <= -90)){
			// Rotation der Achse wird zurückgesetzt, damit der Geometrie-Würfel erhalten bleibt
			tmpRadians = 0.0f;
			setAxisRotationMatrix(&axis[rotatingAxisIndex], degreeToRadians(tmpRadians));
			setRotating(&axis[rotatingAxisIndex], false);
			
			// nur die Farben werden geändert! 
			axis[rotatingAxisIndex].shift(direction);	
	
			// falls es keine mittlere Achse ist, drehe auch die EnclosedCubeSide
			if(axis[rotatingAxisIndex].getAxisIndex() != 1) 
			{
				if(rotatingAxisIndex == 0 || rotatingAxisIndex == 5 || rotatingAxisIndex == 8) // hier direction invertieren
					shiftEnclosedSide(&axis[rotatingAxisIndex], !direction);
				else
					shiftEnclosedSide(&axis[rotatingAxisIndex], direction);
			}			
		}			
	}
}



/* Initialize libgxm */
int initGxm( void )
{
	int returnCode = SCE_OK;

	/* set up parameters */

	/* start libgxm */

	/* allocate ring buffer memory using default sizes */

	/* create a rendering context */

	/* set buffer sizes for this sample */

	/* allocate memory for buffers and USSE code */

	/* create a shader patcher */

	/* create a render target */

	/* allocate memory and sync objects for display buffers */

	/* compute the memory footprint of the depth buffer */
	
	/* allocate it */
	
	/* create the SceGxmDepthStencilSurface structure */
	return returnCode;
}

/* Create libgxm scenes */
void createGxmData( void )
{
	/* register programs with the patcher */

	/* get attributes by name to create vertex format bindings */

	/* create clear vertex format */

	/* create clear programs */

	/* create the clear triangle vertex/index data */
    /* get attributes by name to create vertex format bindings */
	/* first retrieve the underlying program to extract binding information */
	const SceGxmProgram *basicProgram = sceGxmShaderPatcherGetProgramFromId( s_basicVertexProgramId );
	SCE_DBG_ALWAYS_ASSERT( basicProgram );
	const SceGxmProgramParameter *paramBasicPositionAttribute = sceGxmProgramFindParameterByName( basicProgram, "aPosition" );
	SCE_DBG_ALWAYS_ASSERT( paramBasicPositionAttribute && ( sceGxmProgramParameterGetCategory(paramBasicPositionAttribute) == SCE_GXM_PARAMETER_CATEGORY_ATTRIBUTE ) );
	const SceGxmProgramParameter *paramBasicNormalAttribute = sceGxmProgramFindParameterByName( basicProgram, "aNormal" );
	SCE_DBG_ALWAYS_ASSERT( paramBasicNormalAttribute && ( sceGxmProgramParameterGetCategory(paramBasicNormalAttribute) == SCE_GXM_PARAMETER_CATEGORY_ATTRIBUTE ) );
	const SceGxmProgramParameter *paramBasicColorAttribute = sceGxmProgramFindParameterByName( basicProgram, "aColor" );
	SCE_DBG_ALWAYS_ASSERT( paramBasicColorAttribute && ( sceGxmProgramParameterGetCategory(paramBasicColorAttribute) == SCE_GXM_PARAMETER_CATEGORY_ATTRIBUTE ) );
	const SceGxmProgramParameter *paramBasicRotatingAttribute = sceGxmProgramFindParameterByName( basicProgram, "aRotating" );
	SCE_DBG_ALWAYS_ASSERT( paramBasicRotatingAttribute && ( sceGxmProgramParameterGetCategory(paramBasicRotatingAttribute) == SCE_GXM_PARAMETER_CATEGORY_ATTRIBUTE ) );

	/* create shaded triangle vertex format */
	SceGxmVertexAttribute basicVertexAttributes[4];
	SceGxmVertexStream basicVertexStreams[1];
	basicVertexAttributes[0].streamIndex = 0;
	basicVertexAttributes[0].offset = 0;
	basicVertexAttributes[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
	basicVertexAttributes[0].componentCount = 3;
	basicVertexAttributes[0].regIndex = sceGxmProgramParameterGetResourceIndex( paramBasicPositionAttribute );
	basicVertexAttributes[1].streamIndex = 0;
	basicVertexAttributes[1].offset = 12;
	basicVertexAttributes[1].format = SCE_GXM_ATTRIBUTE_FORMAT_F32; 
	basicVertexAttributes[1].componentCount = 3;
	basicVertexAttributes[1].regIndex = sceGxmProgramParameterGetResourceIndex( paramBasicNormalAttribute );
	basicVertexAttributes[2].streamIndex = 0;
	basicVertexAttributes[2].offset = 24;
	basicVertexAttributes[2].format = SCE_GXM_ATTRIBUTE_FORMAT_U8N; // Mapping relation clarified.
	basicVertexAttributes[2].componentCount = 4;
	basicVertexAttributes[2].regIndex = sceGxmProgramParameterGetResourceIndex( paramBasicColorAttribute );
	basicVertexAttributes[3].streamIndex = 0;
	basicVertexAttributes[3].offset = 28;
	basicVertexAttributes[3].format = SCE_GXM_ATTRIBUTE_FORMAT_U8N;
	basicVertexAttributes[3].componentCount = 1;
	basicVertexAttributes[3].regIndex = sceGxmProgramParameterGetResourceIndex( paramBasicRotatingAttribute );
	basicVertexStreams[0].stride = sizeof(BasicVertex);
	basicVertexStreams[0].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;

	/* create shaded triangle shaders */
	returnCode = sceGxmShaderPatcherCreateVertexProgram( s_shaderPatcher, s_basicVertexProgramId, basicVertexAttributes, 4,
														 basicVertexStreams, 1, &s_basicVertexProgram );
	SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );

	returnCode = sceGxmShaderPatcherCreateFragmentProgram( s_shaderPatcher, s_basicFragmentProgramId,
														   SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4, SCE_GXM_MULTISAMPLE_NONE, NULL,
														   sceGxmShaderPatcherGetProgramFromId(s_basicVertexProgramId), &s_basicFragmentProgram );
	SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );

	/* find vertex uniforms by name and cache parameter information */
	s_wvpParam = sceGxmProgramFindParameterByName( basicProgram, "wvp" );
	SCE_DBG_ALWAYS_ASSERT( s_wvpParam && ( sceGxmProgramParameterGetCategory( s_wvpParam ) == SCE_GXM_PARAMETER_CATEGORY_UNIFORM ) );
	s_rotParam = sceGxmProgramFindParameterByName( basicProgram, "rot" );
	SCE_DBG_ALWAYS_ASSERT( s_rotParam && ( sceGxmProgramParameterGetCategory( s_rotParam ) == SCE_GXM_PARAMETER_CATEGORY_UNIFORM ) );
	s_rotAxisParam = sceGxmProgramFindParameterByName( basicProgram, "rotAxis" );
	SCE_DBG_ALWAYS_ASSERT( s_rotAxisParam && ( sceGxmProgramParameterGetCategory( s_rotAxisParam ) == SCE_GXM_PARAMETER_CATEGORY_UNIFORM ) );



	/* create shaded triangle vertex/index data */
	s_basicVertices = (BasicVertex *)graphicsAlloc( SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE, 6*9*4*sizeof(BasicVertex), 4, SCE_GXM_MEMORY_ATTRIB_READ, &s_basicVerticesUId );
	s_basicIndices = (uint16_t *)graphicsAlloc( SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE, 6*9*6*sizeof(uint16_t), 2, SCE_GXM_MEMORY_ATTRIB_READ, &s_basicIndiceUId );

	/* Würfel erzeugen*/
	int faceCount = 0; // Zähler für die Face-id
	int vertexCount = 0; // Zähler für die Vertices
	CFace createdFaces[9*6]; // Array, der 9 Faces * 6 Seiten speichert

	int axisType = 2; // Z-Achse
	//Side 0 = front
	for(int i = 0; i < 9; ++i){
			BasicVertex* v1 = &(s_basicVertices[vertexCount++]);
			BasicVertex* v2 = &(s_basicVertices[vertexCount++]);
			BasicVertex* v3 = &(s_basicVertices[vertexCount++]);
			BasicVertex* v4 = &(s_basicVertices[vertexCount++]);
			createdFaces[faceCount]= CFace(v1,v2,v3,v4, axisType,1, faceCount);
			faceCount++;
		
	}

	axisType = 0; // X-Achse

	//Side 1 = rechts
	for(int i = 0; i < 9; ++i){
			BasicVertex* v1 = &(s_basicVertices[vertexCount++]);
			BasicVertex* v2 = &(s_basicVertices[vertexCount++]);
			BasicVertex* v3 = &(s_basicVertices[vertexCount++]);
			BasicVertex* v4 = &(s_basicVertices[vertexCount++]);
			createdFaces[faceCount]= CFace(v1,v2,v3,v4, axisType,1, faceCount);
			faceCount++;
		
	}

	axisType = 2; // Z-Achse
	//Side 2 = back
	for(int i = 0; i < 9; ++i){
			BasicVertex* v1 = &(s_basicVertices[vertexCount++]);
			BasicVertex* v2 = &(s_basicVertices[vertexCount++]);
			BasicVertex* v3 = &(s_basicVertices[vertexCount++]);
			BasicVertex* v4 = &(s_basicVertices[vertexCount++]);
			createdFaces[faceCount]= CFace(v1,v2,v3,v4, axisType,-1, faceCount);
			faceCount++;
	}

	axisType = 0; // X-Achse
	//Side 3  = links
	for(int i = 0; i < 9; ++i){
			BasicVertex* v1 = &(s_basicVertices[vertexCount++]);
			BasicVertex* v2 = &(s_basicVertices[vertexCount++]);
			BasicVertex* v3 = &(s_basicVertices[vertexCount++]);
			BasicVertex* v4 = &(s_basicVertices[vertexCount++]);
			createdFaces[faceCount]= CFace(v1,v2,v3,v4, axisType,-1, faceCount);
			faceCount++;
		
	}
	axisType = 1; // Y-Achse
	//Side 4 = oben
	for(int i = 0; i < 9; ++i){
			BasicVertex* v1 = &(s_basicVertices[vertexCount++]);
			BasicVertex* v2 = &(s_basicVertices[vertexCount++]);
			BasicVertex* v3 = &(s_basicVertices[vertexCount++]);
			BasicVertex* v4 = &(s_basicVertices[vertexCount++]);
			createdFaces[faceCount]= CFace(v1,v2,v3,v4, axisType,1, faceCount);
			faceCount++;
		
	}
	
	//Side 5 = unten
	for(int i = 0; i < 9; ++i){
			BasicVertex* v1 = &(s_basicVertices[vertexCount++]);
			BasicVertex* v2 = &(s_basicVertices[vertexCount++]);
			BasicVertex* v3 = &(s_basicVertices[vertexCount++]);
			BasicVertex* v4 = &(s_basicVertices[vertexCount++]);
			createdFaces[faceCount]= CFace(v1,v2,v3,v4, axisType,-1, faceCount);
			faceCount++;
	}
	
	
	// Nun wird ein Geometrie-Würfel erstellt, bei der die Indexe an der gleichen Stelle stehen pro Seite (bei createdFaces sind die Indexe von der Achse abhängig)
	// Geometrie-Würfel wird bei der Bestimmung der Achsen helfen

	int count = 0;	// Zähler von 0 bis 53
	int cubeSideIndex = 0;

	// sortierte Front Seite
	sortedFaces[count++] = (createdFaces[cubeSideIndex * 9 + 6]);
	sortedFaces[count++] = (createdFaces[cubeSideIndex * 9 + 7]);
	sortedFaces[count++] = (createdFaces[cubeSideIndex * 9 + 8]);
	sortedFaces[count++] = (createdFaces[cubeSideIndex * 9 + 3]);
	sortedFaces[count++] = (createdFaces[cubeSideIndex * 9 + 4]);
	sortedFaces[count++] = (createdFaces[cubeSideIndex * 9 + 5]);
	sortedFaces[count++] = (createdFaces[cubeSideIndex * 9 + 0]);
	sortedFaces[count++] = (createdFaces[cubeSideIndex * 9 + 1]);
	sortedFaces[count++] = (createdFaces[cubeSideIndex * 9 + 2]);

	cubeSideIndex ++;

	// sortierte Rechte Seite
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 8];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 5];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 2];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 7];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 4];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 1];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 6];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 3];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 0];

	cubeSideIndex ++;

	// sortierte Back Seite
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 8];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 7];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 6];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 5];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 4];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 3];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 2];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 1];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 0];

	cubeSideIndex ++;

	// sortierte Linke Seite
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 2];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 5];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 8];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 1];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 4];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 7];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 0];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 3];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 6];

	cubeSideIndex ++;

	// sortierte Obere Seite
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 0];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 3];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 6];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 1];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 4];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 7];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 2];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 5];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 8];

	cubeSideIndex ++;

	// sortierte Untere Seite
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 2];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 5];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 8];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 1];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 4];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 7];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 0];
	sortedFaces[count++] = createdFaces[cubeSideIndex * 9 + 3];
	sortedFaces[count  ] = createdFaces[cubeSideIndex * 9 + 6];

	// die sortierten Faces den richtigen Index zuweisen (der von den sortedFaces)
	for(int i = 0; i < 9*6; i++){
		sortedFaces[i].setFaceIndex(i);
	}
	// Indices werden erstellt für 9 Faces * 6 Seiten
	count = 0;	
	for(int i = 0; i < 9*6; ++i)
	{
		int baseIndex = i*4;
		s_basicIndices[count++] = baseIndex;
		s_basicIndices[count++] = baseIndex+1;
		s_basicIndices[count++] = baseIndex+2;

		s_basicIndices[count++] = baseIndex;
		s_basicIndices[count++] = baseIndex+3;
		s_basicIndices[count++] = baseIndex+2;
	}
	
	// die 9 Achsen erzeugen
	axis = new CAxis[9];
	count = 0;
	//X-Achsen
	axis[count++] = CAxis(0, 0, 4);
	axis[count++] = CAxis(1, 0, NULL);
	axis[count++] = CAxis(2, 0, 5);
	// Y-Achsen
	axis[count++] = CAxis(0, 1, 3);
	axis[count++] = CAxis(1, 1, NULL);
	axis[count++] = CAxis(2, 1, 1);
	// Z-Achsen
	axis[count++] = CAxis(0, 2, 0);
	axis[count++] = CAxis(1, 2, NULL);
	axis[count]	  = CAxis(2, 2, 2);
	
	// Faces den Achsen zuweisen
			// add(axis, horizontal, position, order, cubeSide)
	count = 0;
	// CubeAchse 0
		addToCubeAxis(&axis[count  ], true, 0, 1,	0);
		addToCubeAxis(&axis[count  ], true, 0, 1,	1);
		addToCubeAxis(&axis[count  ], true, 0, 1,	2);
		addToCubeAxis(&axis[count++], true, 0, 1,	3);
	// CubeAchse 1	   
		addToCubeAxis(&axis[count  ], true, 1, 1,	0);
		addToCubeAxis(&axis[count  ], true, 1, 1,	1);
		addToCubeAxis(&axis[count  ], true, 1, 1,	2);
		addToCubeAxis(&axis[count++], true, 1, 1,	3);
	// CubeAchse 2		  
		addToCubeAxis(&axis[count  ], true, 2, 1,	0);
		addToCubeAxis(&axis[count  ], true, 2, 1,	1);
		addToCubeAxis(&axis[count  ], true, 2, 1,	2);
		addToCubeAxis(&axis[count++], true, 2, 1,	3);
	// CubeAchse 3	   
		addToCubeAxis(&axis[count  ], false, 0, 1,  0);
		addToCubeAxis(&axis[count  ], false, 0, 1,  5);
		addToCubeAxis(&axis[count  ], false, 2, -1, 2);
		addToCubeAxis(&axis[count++], false, 0, 1, 4);
	// CubeAchse 4	   
		addToCubeAxis(&axis[count  ], false, 1, 1,  0);
		addToCubeAxis(&axis[count  ], false, 1, 1,  5);
		addToCubeAxis(&axis[count  ], false, 1, -1, 2);
		addToCubeAxis(&axis[count++], false, 1, 1, 4);
	// CubeAchse 5		  
		addToCubeAxis(&axis[count  ], false, 2, 1,  0);
		addToCubeAxis(&axis[count  ], false, 2, 1,  5);
		addToCubeAxis(&axis[count  ], false, 0, -1, 2);
		addToCubeAxis(&axis[count++], false, 2, 1, 4);
	// CubeAchse 6		  
		addToCubeAxis(&axis[count  ], false, 0, 1 , 1);
		addToCubeAxis(&axis[count  ], true,  0, -1, 5);
		addToCubeAxis(&axis[count  ], false, 2, -1, 3);
		addToCubeAxis(&axis[count++], true,  2, 1 , 4);
	// CubeAchse 7		  
		addToCubeAxis(&axis[count  ], false, 1, 1 , 1);
		addToCubeAxis(&axis[count  ], true,  1, -1, 5);
		addToCubeAxis(&axis[count  ], false, 1, -1, 3);
		addToCubeAxis(&axis[count++], true,  1, 1 , 4);
	// CubeAchse 8		  
		addToCubeAxis(&axis[count  ], false, 2, 1 , 1);
		addToCubeAxis(&axis[count  ], true,  2, -1, 5);
		addToCubeAxis(&axis[count  ], false, 0, -1, 3);
		addToCubeAxis(&axis[count  ], true,  0, 1 , 4);

	
}


// weist 3 Faces der Axis zu, die auf der Seite cubeSideIndex sind und horizontal (true) oder vertikal (false) in der Reihe position (0 bis 2) liegen, 
//die order gibt an, in welcher Reihenfolge die Faces in der Reihe hinzugefügt werden
void addToCubeAxis(CAxis *axis, bool horizontal, unsigned int position, int order, unsigned int cubeSideIndex)
{
	unsigned int firstFaceofSide = cubeSideIndex * 9;
	if(position == 0){ 
			if(horizontal){ // Reihe oben
				if(order == 1){
					axis->addFace(&sortedFaces[firstFaceofSide+0]);
					axis->addFace(&sortedFaces[firstFaceofSide+1]);
					axis->addFace(&sortedFaces[firstFaceofSide+2]);
				}
				else{
					axis->addFace(&sortedFaces[firstFaceofSide+2]);
					axis->addFace(&sortedFaces[firstFaceofSide+1]);
					axis->addFace(&sortedFaces[firstFaceofSide+0]);
				}
			}
			else{			//Reihe links
				if(order == 1){
					axis->addFace(&sortedFaces[firstFaceofSide+0]);
					axis->addFace(&sortedFaces[firstFaceofSide+3]);
					axis->addFace(&sortedFaces[firstFaceofSide+6]);
				}
				else{
					axis->addFace(&sortedFaces[firstFaceofSide+6]);
					axis->addFace(&sortedFaces[firstFaceofSide+3]);
					axis->addFace(&sortedFaces[firstFaceofSide+0]);
				}

			}
	}
	else if(position == 1){ 
			if(horizontal){ //Reihe mittlere
				if(order == 1){
					axis->addFace(&sortedFaces[firstFaceofSide+3]);
					axis->addFace(&sortedFaces[firstFaceofSide+4]);
					axis->addFace(&sortedFaces[firstFaceofSide+5]);
				}
				else{
					axis->addFace(&sortedFaces[firstFaceofSide+5]);
					axis->addFace(&sortedFaces[firstFaceofSide+4]);
					axis->addFace(&sortedFaces[firstFaceofSide+3]);
				}

			}
			else{			//Reihe zentrum
				if(order == 1){
					axis->addFace(&sortedFaces[firstFaceofSide+1]);
					axis->addFace(&sortedFaces[firstFaceofSide+4]);
					axis->addFace(&sortedFaces[firstFaceofSide+7]);
				}
				else{
					axis->addFace(&sortedFaces[firstFaceofSide+7]);
					axis->addFace(&sortedFaces[firstFaceofSide+4]);
					axis->addFace(&sortedFaces[firstFaceofSide+1]);
				}
			
			}
	}

	else if(position == 2){ 
			if(horizontal){ // Reihe unten
				if(order == 1){
					axis->addFace(&sortedFaces[firstFaceofSide+6]);
					axis->addFace(&sortedFaces[firstFaceofSide+7]);
					axis->addFace(&sortedFaces[firstFaceofSide+8]);
					}
				else{
					axis->addFace(&sortedFaces[firstFaceofSide+8]);
					axis->addFace(&sortedFaces[firstFaceofSide+7]);
					axis->addFace(&sortedFaces[firstFaceofSide+6]);
					}

			}
			else{			// Reihe rechts
				if(order == 1){
					axis->addFace(&sortedFaces[firstFaceofSide+2]);
					axis->addFace(&sortedFaces[firstFaceofSide+5]);
					axis->addFace(&sortedFaces[firstFaceofSide+8]);
					}
				else{
					axis->addFace(&sortedFaces[firstFaceofSide+8]);
					axis->addFace(&sortedFaces[firstFaceofSide+5]);
					axis->addFace(&sortedFaces[firstFaceofSide+2]);
					}
			}
	}
}
// alle 9 Faces auf der enclosedSide der axis werden mit der selben direction geshiftet (+3 oder -3, außer das mittlere Face);
void shiftEnclosedSide(CAxis *axis, bool direction){

	int enclosedSideIndex = axis->getEnclosedSideIndex();
	uint32_t oldColorOrder[9];
	// aktuelle Farben holen
	for(int i = 0; i < 9; ++i){
		oldColorOrder[i] = sortedFaces[enclosedSideIndex*9 + i].getFaceColor();
	}
	// shiften
	if(!direction){ //shift: - 3
		int newIndex;
		// Faces 0, 1, 2
		for(int i = 0; i < 3; ++i){
			newIndex = i + 2 * (i+1);
			sortedFaces[enclosedSideIndex*9 + i].setFaceColor(oldColorOrder[newIndex]);
		}	
		// Faces 3, 4, 5
		int count = -1;
		for(int i = 3; i < 6; ++i){
			newIndex = i + 2 * count;
			sortedFaces[enclosedSideIndex*9 + i].setFaceColor(oldColorOrder[newIndex]);
			count++;
		}	
		// Faces 6, 7, 8
		count = 3;
		for(int i = 6; i < 9; ++i){
			newIndex = i + -2 * count;
			sortedFaces[enclosedSideIndex*9 + i].setFaceColor(oldColorOrder[newIndex]);
			count--;
		}
	}
	else{ // shift: +3
		int newIndex;
		// Faces 0, 1, 2
		int count = 6;
		for(int i = 0; i < 3; ++i){
			newIndex = i + count;
			sortedFaces[enclosedSideIndex*9 + i].setFaceColor(oldColorOrder[newIndex]);
			count -= 4;
		}
		// Faces 3, 4, 5
		count = 1;
		for(int i = 3; i < 6; ++i){
			newIndex = i + 4 * count;
			sortedFaces[enclosedSideIndex*9 + i].setFaceColor(oldColorOrder[newIndex]);
			count--;
		}	
		// Faces 6, 7, 8
		count = 2;
		for(int i = 6; i < 9; ++i){
			newIndex = i + count;
			sortedFaces[enclosedSideIndex*9 + i].setFaceColor(oldColorOrder[newIndex]);
			count -= 4;
		}
	}
	

}
// berechne die jeweilige Rotationsmatrix
void setAxisRotationMatrix(CAxis *axis, float radians){

	unsigned int type = axis->getAxisType();
	if(type == 0){ // falls X-Achse

		finalAxisRotation = Matrix4::rotationY(radians);
		
	} else if(type == 1){ // falls Y-Achse

		finalAxisRotation = Matrix4::rotationX(radians);
	}else if(type == 2){ // falls Z-Achse

		finalAxisRotation = Matrix4::rotationZ(-radians);
	}

}
// Setze die Achse in den Rotation-Zustand und falls die Achse eine EnclosedCubeSide hat, auch die Vertices auf der EnclosedCubeSide rotating
void setRotating(CAxis *axis, bool rotating){

	axis->setRotating(rotating);
	isAxisRotating = rotating;
	if(axis->getAxisIndex() != 1){ // falls keine mittlere Achse
		unsigned int enclosedSideIndex = axis->getEnclosedSideIndex();
		int count = enclosedSideIndex *9; // erhalte den ersten FaceIndex der EnclosedCubeSide
		for(int i = 0; i < 9; ++i)
			sortedFaces[count++].setVerticesRotating(rotating);	
	}
}


float degreeToRadians(float degrees){

	return degrees * (PI/180);
}

/* Main render function */
void render( void )
{
	/* render libgxm scenes */
	renderGxm();
}

/* render gxm scenes */
void renderGxm( void )
{
	/* start rendering to the render target */
	sceGxmBeginScene( s_context, 0, s_renderTarget, NULL, NULL, s_displayBufferSync[s_displayBackBufferIndex], &s_displaySurface[s_displayBackBufferIndex], &s_depthSurface );

	/* set clear shaders */
	sceGxmSetVertexProgram( s_context, s_clearVertexProgram );
	sceGxmSetFragmentProgram( s_context, s_clearFragmentProgram );

	/* draw ther clear triangle */
	sceGxmSetVertexStream( s_context, 0, s_clearVertices );
	sceGxmDraw( s_context, SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U16, s_clearIndices, 3 );


    // Speciality for rendering the cube.

    /* render the cube*/
	sceGxmSetVertexProgram( s_context, s_basicVertexProgram );
	sceGxmSetFragmentProgram( s_context, s_basicFragmentProgram );

	/* set the vertex program constants */
	void *vertexDefaultBuffer;
	sceGxmReserveVertexDefaultUniformBuffer( s_context, &vertexDefaultBuffer );
	sceGxmSetUniformDataF( vertexDefaultBuffer, s_wvpParam, 0, 16, (float*)&finalTransformation ); 
	sceGxmSetUniformDataF( vertexDefaultBuffer, s_rotParam, 0, 16, (float*)&finalRotation ); 
	sceGxmSetUniformDataF( vertexDefaultBuffer, s_rotAxisParam, 0, 16, (float*)&finalAxisRotation ); 

	/* draw the cube*/
	sceGxmSetVertexStream( s_context, 0, s_basicVertices );								
	sceGxmDraw( s_context, SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U16, s_basicIndices, 6*9*6 );

	/* stop rendering to the render target */
	sceGxmEndScene( s_context, NULL, NULL );
}



/* queue a display swap and cycle our buffers */
void cycleDisplayBuffers( void )
{
	/* PA heartbeat to notify end of frame */
	
	/* queue the display swap for this frame */

	/* front buffer is OLD buffer, back buffer is NEW buffer */
	
	/* update buffer indices */
	
}