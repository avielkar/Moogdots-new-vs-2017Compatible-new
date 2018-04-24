#ifndef APPLICATION_INCLUDED
#define APPLICATION_INCLUDED

#include "InputHandlers.hpp"
#include "OpenGL.hpp"
#include "Texture.hpp"
#include "libxl.h"

#include <SDL.h>
#include "CameraDirector.hpp"
#include "ShaderManager.hpp"
#include "TextureManager.hpp"
#include "OVR_Math.h"
#include "StdAfx.h"
#include "GlobalDefs.h"

/*
 * main application 
 */
class Application
{
public:
    Application::Application() : m_running(true)
    {
		book = xlCreateXMLBook();
		if (book)
		{
			sheet = book->addSheet("Points");
			rowNum = 0;
		}
    }

	~Application();

	void OnStart(GLfloat* vertexArray, int numOfVertexes);

	/***	Function : OnRender - to render all field stars.
	****	numOfVetexes - the num of the vertexex in the array (the numOfTriangles*3).
	****	vertexArray - the vertexes of the triangles to be rendered as the stars field.
	****	numOfTriangles - the num of triangles in the vertexArray pointer.
	****	direction - the point vector of the origin of the camera.
	****	targetPos - the point vector target to look at.\
	****	starsCenter - the point vector of the center of the star field(may be always (0,0,0) at the begin).
	****	fixationPoint - the fixation point of the camera (the one that is between the camera place and the camera target).
	****	drawFixationPoint - input from the matlab assigned to draw the FP or not to draw the FP.
	****	eyeRender - the eye to render the strs to it.
	****	zDistanceFromScreen - the distance of the camera from the screen.
	****/
	void OnRender(int numOfVertexes, GLfloat* vertexArray, int numOfTriangles,
		float directionX, float directionY, float directionZ,
		float targetPosX, float targetPosY, float targetPosZ,
		float upPosX, float upPosY, float upPosZ,
		float starsCenterX, float starsCenterY, float starsCenterZ,
		float fixationPointX, float fixationPointY, float fixationPointZ,
		bool drawFixationPoint,
		int eyeRender,
		int zDistanceFromScreen);

	/***
	****	Function : OnRenderSphereField - render the sphere field of the noise one that moves vertically.
	****	fsphereFieldArray - the sphere noise field array.
	****	numOfVertexes - the num of vertexes in the sphere field array.
	****	drawMode - '0' for triangles, '1' for points.
	****	pointSize - the point size if the drawMode is '1'.
	***/
	void OnRenderSphereField(GLfloat* fsphereFieldArray, int numOfVertexes, int drawMode, int pointSize = 0);

    inline bool Running() const  { return m_running; }
    inline void Terminate()      { m_running = false; }

    void OnKeyPress(KeyCode key);
private:
    bool m_running;

    // rendered quad data
    GLuint m_vertexBuffer;
    GLuint m_colorBuffer;
    GLuint m_texcoordBuffer;
    GLuint m_vertexArray;
    Texture *m_texture;
	float m_x;
	libxl::Book* book;
	libxl::Sheet* sheet;
	int rowNum;
};

#endif
