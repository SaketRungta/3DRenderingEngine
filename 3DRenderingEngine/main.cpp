#include <stdio.h>
#include <string.h>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

const GLint WIDTH = 1300, HEIGHT = 1300;

GLuint VAO, VBO, shaderProgram, uniformModel;

bool translateDirection = true;
float translateOffset = 0.f;
float translateMaxOffset = 0.7f;
float translateIncrement = 0.005f;

static const char* vShader = "															\n\
#version 330																			\n\
																						\n\
layout(location = 0) in vec3 pos;														\n\
																						\n\
uniform mat4 model;																						\n\
																						\n\
void main()																				\n\
{																						\n\
	gl_Position = model * vec4(0.4 * pos, 1.f);														\n\
}";

static const char* fShader = "															\n\
#version 330																			\n\
																						\n\
out vec4 colour;																		\n\
																						\n\
void main()																				\n\
{																						\n\
	colour = vec4(1.f, 1.f, 0.f, 1.f);													\n\
}";

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		printf("Error comiling %d shader '%s'", shaderType, eLog);
		return;
	}

	glAttachShader(theProgram, theShader);
}

void CompileShader()
{
	shaderProgram = glCreateProgram();

	if (!shaderProgram)
	{
		printf("Failed to create shader program");
		return;
	}

	AddShader(shaderProgram, vShader, GL_VERTEX_SHADER);
	AddShader(shaderProgram, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderProgram, sizeof(eLog), NULL, eLog);
		printf("Error comiling shader program '%s'", eLog);
		return;
	}

	uniformModel = glGetUniformLocation(shaderProgram, "model");
}

void CreateTriangle()
{
	GLfloat bottomTriangleVertices[] = {
		-1.f, -1.f, 0.f,
		1.f, -1.f, 0.f,
		0.f, 1.f, 0.f,
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bottomTriangleVertices), bottomTriangleVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int main()
{
	if (!glfwInit())
	{
		printf("GLFW Initialization failed");
		glfwTerminate();
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Triangles Window", NULL, NULL);
	if (!mainWindow)
	{
		printf("Failed to create window");
		glfwTerminate();
		return 1;
	}

	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);
	glfwMakeContextCurrent(mainWindow);

	glewExperimental = GL_TRUE;

	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		printf("Error Initializing Glew '%s'", glewGetErrorString(error));
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glViewport(0, 0, bufferWidth, bufferHeight);

	CreateTriangle();
	CompileShader();

	while (!glfwWindowShouldClose(mainWindow))
	{
		glfwPollEvents();

		if (translateDirection)translateOffset += translateIncrement;
		else translateOffset -= translateIncrement;

		if (abs(translateOffset) >= translateMaxOffset)translateDirection = !translateDirection;

		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);

		glm::mat4 model(1.f);
		model = glm::translate(model, glm::vec3(translateOffset, 0.f, 0.f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}

	return 0;
}