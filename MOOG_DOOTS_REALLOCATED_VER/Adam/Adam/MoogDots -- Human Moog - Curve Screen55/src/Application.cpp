#include "Application.hpp"

extern OculusVR g_oculusVR;


CameraDirector g_cameraDirector;

Application::~Application()
{
	if (glewInit())
	{
		if (m_vertexBuffer != NULL)
		{
			if (glIsBuffer(m_vertexBuffer))
				glDeleteBuffers(1, &m_vertexBuffer);
		}

		if (m_colorBuffer != NULL)
		{
			if (glIsBuffer(m_colorBuffer))
				glDeleteBuffers(1, &m_colorBuffer);
		}

		if (m_texcoordBuffer != NULL)
		{
			if (glIsBuffer(m_texcoordBuffer))
				glDeleteBuffers(1, &m_texcoordBuffer);
		}

		if (m_vertexArray != NULL)
		{
			if (glIsVertexArray(m_vertexArray))
				glDeleteVertexArrays(1, &m_vertexArray);
		}
	}
}

void Application::OnStart(GLfloat* vertexArray, int numOfVertexes)
{
	glEnable(GL_MULTISAMPLE);

	g_cameraDirector.AddCamera(0.0f, 0.0f, 0.0f);

	//m_texture = TextureManager::GetInstance()->LoadTexture("block_blue.png");

	// create quad VAO and VBOs
	glGenVertexArrays(1, &m_vertexArray);
	glBindVertexArray(m_vertexArray);
	
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, numOfVertexes * 3, vertexArray, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &m_colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);


}

void Application::OnRender(int numOfVertexes, GLfloat* vertexArray, int numOfTriangles,
	float directionX, float directionY, float directionZ,
	float targetPosX, float targetPosY, float targetPosZ,
	float upPosX, float upPosY, float upPosZ,
	float starsCenterX, float starsCenterY, float starsCenterZ,
	float fixationPointX, float fixationPointY, float fixationPointZ,
	bool drawFixationPoint,
	int eyeIndex,
	int zDistanceFromScreen,
	bool drawStaticSensorCube,
	bool drawFlashingSquare)

{
	const ShaderProgram &shader = ShaderManager::GetInstance()->UseShaderProgram(ShaderManager::BasicShader);

	GLuint vertexPosition_modelspaceID = glGetAttribLocation(shader.id, "inVertex");

	// setup quad data
	glBindVertexArray(m_vertexArray);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, numOfVertexes * 3 * 4, vertexArray, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(vertexPosition_modelspaceID);

	glVertexAttribPointer(vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// draw the quad!
	glDrawArrays(GL_TRIANGLES, vertexPosition_modelspaceID, 3 * numOfTriangles);

	glDisableVertexAttribArray(vertexPosition_modelspaceID);
	
	GLfloat* temp = new GLfloat[3];

	if (CENTER_POINT_DRAWING)
	{
		//draw the fixation point.
		const ShaderProgram &shader2 = ShaderManager::GetInstance()->UseShaderProgram(ShaderManager::FontShader);
		GLuint vertexPosition_modelspaceID2 = glGetAttribLocation(shader2.id, "inVertex");
		numOfVertexes = 1;


		temp[0] = starsCenterX;
		temp[1] = starsCenterY;
		temp[2] = starsCenterZ;

		vertexArray = temp;

		glBufferData(GL_ARRAY_BUFFER, numOfVertexes * 3, vertexArray, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(vertexPosition_modelspaceID2);
		glVertexAttribPointer(vertexPosition_modelspaceID2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		numOfTriangles = 1;
		glPointSize(8.0);
		glEnable(GL_POINT_SMOOTH);
		glDrawArrays(GL_POINTS, vertexPosition_modelspaceID2, 3 * numOfTriangles);
		glDisable(GL_POINT_SMOOTH);
		glDisableVertexAttribArray(vertexPosition_modelspaceID2);
	}

	if (drawFixationPoint)
	{
		//With no any Rotation or translation draw the fixation point as is.
		const ShaderProgram &shader3 = ShaderManager::GetInstance()->UseShaderProgram(ShaderManager::BasicShaderNoTex);
		GLuint vertexPosition_modelspaceID3 = glGetAttribLocation(shader3.id, "inVertex");


		//change the shader print to be blue color.
		//todo:make the color as parameter.
		float trianglesColor[3];
		trianglesColor[0] = (float)(0);
		trianglesColor[1] = (float)(1);
		trianglesColor[2] = (float)(0);
		ShaderManager::GetInstance()->ShaderColor(trianglesColor, ShaderManager::BasicShaderNoTex);


		temp[0] = fixationPointX;
		temp[1] = fixationPointY;
		temp[2] = fixationPointZ;

		OVR::Matrix4f MVPMatrix = g_oculusVR.GetProjectionMatrix(eyeIndex, zDistanceFromScreen, temp);
		glUniformMatrix4fv(shader3.uniforms[ModelViewProjectionMatrix], 1, GL_FALSE, &MVPMatrix.Transposed().M[0][0]);

		vertexArray = temp;
		numOfVertexes = 1;
		glBufferData(GL_ARRAY_BUFFER, numOfVertexes * 3, vertexArray, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(vertexPosition_modelspaceID3);
		glVertexAttribPointer(vertexPosition_modelspaceID3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		numOfTriangles = 1;
		glPointSize(8.0);
		glEnable(GL_POINT_SMOOTH);
		glDrawArrays(GL_POINTS, vertexPosition_modelspaceID3, 3 * numOfTriangles);
		glDisable(GL_POINT_SMOOTH);
		glDisableVertexAttribArray(vertexPosition_modelspaceID3);
	}

	//draw a red FP if needed at each frame.
	if(drawFlashingSquare)
	{
		//With no any Rotation or translation draw the fixation point as is.
		const ShaderProgram &shader3 = ShaderManager::GetInstance()->UseShaderProgram(ShaderManager::BasicShaderNoTex);
		GLuint vertexPosition_modelspaceID3 = glGetAttribLocation(shader3.id, "inVertex");


		//change the shader print to be blue color.
		//todo:make the color as parameter.
		float trianglesColor[3];
		trianglesColor[0] = (float)(0);
		trianglesColor[1] = (float)(0);
		trianglesColor[2] = (float)(1);
		ShaderManager::GetInstance()->ShaderColor(trianglesColor, ShaderManager::BasicShaderNoTex);


		temp[0] = fixationPointX;
		temp[1] = fixationPointY;
		temp[2] = fixationPointZ;

		OVR::Matrix4f MVPMatrix = g_oculusVR.GetProjectionMatrix(eyeIndex, zDistanceFromScreen, temp);
		glUniformMatrix4fv(shader3.uniforms[ModelViewProjectionMatrix], 1, GL_FALSE, &MVPMatrix.Transposed().M[0][0]);

		vertexArray = temp;
		numOfVertexes = 1;
		glBufferData(GL_ARRAY_BUFFER, numOfVertexes * 3, vertexArray, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(vertexPosition_modelspaceID3);
		glVertexAttribPointer(vertexPosition_modelspaceID3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		numOfTriangles = 1;
		glPointSize(8.0);
		glEnable(GL_POINT_SMOOTH);
		glDrawArrays(GL_POINTS, vertexPosition_modelspaceID3, 3 * numOfTriangles);
		glDisable(GL_POINT_SMOOTH);
		glDisableVertexAttribArray(vertexPosition_modelspaceID3);
	}


	if (drawStaticSensorCube)
	{
		//change the shader print to be blue color.
		//todo:make the color as parameter.
		float trianglesColor[3];
		trianglesColor[0] = (float)(0);
		trianglesColor[1] = (float)(0);
		trianglesColor[2] = (float)(1.0);
		ShaderManager::GetInstance()->ShaderColor(trianglesColor, ShaderManager::BasicShaderNoTex);

		//draw the fixation point.
		const ShaderProgram &shader2 = ShaderManager::GetInstance()->UseShaderProgram(ShaderManager::BasicShaderNoTex);
		const GLuint vertexPosition_modelspaceID2 = glGetAttribLocation(shader2.id, "inVertex");

		numOfVertexes = 1;

		//todo:Fix it to be a variable and not a static numm - this should be by trigonometric.
		temp[0] = -45;
		temp[1] = -45;
		temp[2] = 0;

		OVR::Matrix4f MVPMatrix = g_oculusVR.GetProjectionMatrixWithoutOrientation(eyeIndex, zDistanceFromScreen, temp);;
		glUniformMatrix4fv(shader2.uniforms[ModelViewProjectionMatrix], 1, GL_FALSE, &MVPMatrix.Transposed().M[0][0]);

		vertexArray = temp;

		glBufferData(GL_ARRAY_BUFFER, numOfVertexes * 3, vertexArray, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(vertexPosition_modelspaceID2);
		glVertexAttribPointer(vertexPosition_modelspaceID2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		numOfTriangles = 1;
		glPointSize(30);
		glEnable(GL_POINT_SMOOTH);
		glDrawArrays(GL_POINTS, vertexPosition_modelspaceID2, 3 * numOfTriangles);
		glDisable(GL_POINT_SMOOTH);
		glDisableVertexAttribArray(vertexPosition_modelspaceID2);
	}



	delete[] temp;
}

void Application::OnRenderSphereField(GLfloat* fsphereFieldArray, int numOfVertexes, int drawMode, int pointSize)
{
	glBindVertexArray(m_vertexArray);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, numOfVertexes * 3, fsphereFieldArray, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// draw the quad!
	if (drawMode == 1)
		glDrawArrays(GL_TRIANGLES, 0, numOfVertexes);
	else if (drawMode == 2)
	{
		glPointSize(pointSize);
		glDrawArrays(GL_POINTS, 0, numOfVertexes);
	}
	glDisableVertexAttribArray(0);
}

void Application::OnKeyPress(KeyCode key)
{
	switch (key)
	{
	case KEY_ESC:
		Terminate();
		break;
	default:
		break;
	}
}
