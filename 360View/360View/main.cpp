#include <iostream>
#include "LoadShader.h"
#include "generateSphere.h"
#include <GLFW\glfw3.h>
#include "SOIL.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


#pragma comment(lib,"glfw3.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"OpenGL32.Lib")
#pragma comment(lib,"SOIL.lib")

const int windowWidth  = 800;
const int windowHeight = 600;

#define OSSphereSliceNum 300
#define OSSphereRadius 1.0   // ����ģ�Ͱ뾶
GLint  _numIndices;          //��������
GLuint  matixMVP;
GLfloat xRot = 0.0f;         //��ת�ĽǶ�
GLfloat yRot = 0.0f;

float speed = 1.0f;

GLuint VAO, VBO,VEO;
GLuint textureOne;
GLuint program;

int buttonState;
int ox;
int oy;
GLfloat xRotLength = 0.0f;
GLfloat yRotLength = 0.0f;



//Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double x, double y);
void init();

int main(int argc, char** argv)
{
	std::cout << "Starting GLFW context, OpenGL 4.3" << std::endl;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);                 //GL�汾��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //�ɱ�̹���
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);                      //�������촰��

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();  //�ͷ�GLFW����Դ
		return -1;
	}
	glfwMakeContextCurrent(window);  //��ǰ������Ϊ�豸������
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		glfwTerminate();
		return -1;
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	init();

	while (!glfwWindowShouldClose(window)) //���GLFW�Ƿ��˳�
	{
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.3f, 1.0f); //������ɫ��������ɫ
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glm::mat4 Projection = glm::perspective(45.0f, (GLfloat)windowWidth / windowHeight, 0.1f, 100.f);
		glm::mat4 View = glm::lookAt(
			glm::vec3(0, 0, 0), // Camera is at (4,3,3), in World Space  
			glm::vec3(0, 0, -1), // and looks at the origin  
			glm::vec3(0, -1, 0) // Head is up (set to 0,-1,0 to look upside-down)        
			);

		if (buttonState == 1)
		{
			xRot += (xRotLength - xRot)*0.0001f;
			yRot += (yRotLength - yRot)*0.0001f;
		}

		glm::mat4 Model = glm::mat4(1.0f);
		Model = glm::rotate(Model, xRot, glm::vec3(1.0f, 0.0f, 0.0f));
		Model = glm::rotate(Model, yRot, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 MVP = Projection * View * Model;

		glUniformMatrix4fv(matixMVP, 1, GL_FALSE, &MVP/*rotationMatrix*/[0][0]);

		glBindTexture(GL_TEXTURE_2D, textureOne);
		glUseProgram(program);  //����ʹ��  �����λ��Ǻ�ɫ
		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, _numIndices, GL_UNSIGNED_SHORT, 0);

		glBindVertexArray(0);
		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}

void  init()
{
	//set vertex data
	GLuint _vertexBuffer;
	GLuint _textureCoordBuffer;
	GLuint _indexBuffer;

	GLfloat  *_vertices;
	GLfloat  *_texCoords;
	GLushort *_indices;

	int numVertices = 0;  // ����ĸ���
	int strideNum   = 3;    // ���ݵĲ����� ���綥������Ϊ(1,1)�������Ϊ2

	_numIndices = generateSphere(OSSphereSliceNum, OSSphereRadius, &_vertices, &_texCoords, &_indices, &numVertices);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// ���ض�������
	glGenBuffers(1, &_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, numVertices*strideNum*sizeof(GLfloat), _vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &_indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _numIndices*sizeof(GLushort), _indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, strideNum, GL_FLOAT, GL_FALSE, strideNum*sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(0);

	//������������
	glGenBuffers(1, &_textureCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _textureCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * numVertices, _texCoords, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

	//texture
	glGenBuffers(1, &textureOne);
	glBindBuffer(GL_TEXTURE_2D, textureOne);
	
	// Set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height;
	unsigned char* image = SOIL_load_image("city.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);


	// �ͷ��ڴ�
	free(_vertices);
	free(_indices);
	free(_texCoords);

	
	//unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	ShaderInfo shaderInfo[] = 
	{
		{ GL_VERTEX_SHADER,   "shader/triangles.vert" },
		{ GL_FRAGMENT_SHADER, "shader/triangles.frag" },
		{ GL_NONE,NULL}
	};

	program = LoadShader(shaderInfo);
	matixMVP = glGetUniformLocation(program, "RotationMatrix");
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
	{
		if (action == GLFW_PRESS)
		{
			std::cout << "left mouse button press" << std::endl;
			buttonState = 1;
		}
		else if (action == GLFW_RELEASE)
		{
			std::cout << "left mouse button release" << std::endl;
			buttonState = 0;
		}
	}
	break;

	default:
		break;
	}

	return;
}

void cursor_position_callback(GLFWwindow* window, double x, double y)
{
	float dx, dy;
	dx = (float)(x - ox);
	dy = (float)(y - oy);

	if (buttonState == 1)
	{
		xRotLength += dy / 10.0f;
		yRotLength += dx / 10.0f;
	}
	return;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	else if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}