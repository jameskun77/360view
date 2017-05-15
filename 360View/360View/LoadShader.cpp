//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Shader Progame ���
// Shader��������C������
// Program��������C������
//
// 1. ����Shader
//1����дVertex Shader�� Fragment ShaderԴ��
//2����������shaderʵ�� glCreateShader(GLenum type)
//3����Shaderʵ��ָ��Դ�� glShaderSource
//4�����߱���shaerԴ��  glCompileShader(GLuint shader)
//
//����Program
//1������program glCreateProgram(void)
//2����shader��program glAttachShader(GLuint program, GLuint shader) ÿ��program�����һ��Vertex Shader��һ��Fragment Shader
//3������program glLinkProgram(GLuint program)
//4��ʹ��porgram glUseProgram(GLuint program)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LoadShader.h"
#include <iostream>


#ifdef __cplusplus
extern "C"{
#endif     //__cplusplus

	static const GLchar* ReadShader(const char* fileName)
	{
#ifdef WIN32
		FILE* infile;
		fopen_s(&infile, fileName, "rb");
#else
		FILE* infile = fopen(fileName, "rb");
#endif //WIN32

		if (!infile)
		{
#ifdef _DEBUG
			std::cerr << "Unable to open file..." << std::endl;
#endif //_DEBUG
			return NULL;
		}

		fseek(infile, 0, SEEK_END); //SEEK_END = 2
		int lenFile = ftell(infile);
		fseek(infile, 0, SEEK_SET);

		GLchar* source = new GLchar[lenFile + 1];
		fread(source, 1, lenFile, infile);
		fclose(infile);

		source[lenFile] = 0;

		return const_cast<const GLchar*>(source);
	}

	GLuint LoadShader(ShaderInfo* shaders)
	{
		if (shaders == NULL)
		{
			return 0;
		}

		GLuint program = glCreateProgram(); //program ����C������
		ShaderInfo* entry = shaders;

		while (entry->type != GL_NONE)
		{
			GLuint shader = glCreateShader(entry->type); //shader����C������
			entry->shader = shader;

			const GLchar* soruce = ReadShader(entry->fileName); //��ȡԴ��
			if (soruce == NULL)
			{
				for (entry = shaders; entry->type != GL_NONE; ++entry)
				{
					glDeleteShader(entry->shader);
					entry->shader = 0;
				}
				return 0;
			}

			glShaderSource(shader, 1, &soruce, NULL); //��shaderָ��Դ��
			delete[]soruce;
			glCompileShader(shader);                  //����

			GLint complied; //��ȡshader�ı������
			glGetShaderiv(shader, GL_COMPILE_STATUS, &complied);
			if (!complied)
			{
#ifdef _DEBUG
				GLsizei len;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

				GLchar* log = new GLchar[len + 1];
				glGetShaderInfoLog(shader, len, &len, log);
				std::cerr << "Shader compilation failed: " << log << std::endl;
				delete[] log;
#endif // _DEBUG
				return 0;
			}

			glAttachShader(program, shader); //shader�󶨵�program
			++entry;
		}

		glLinkProgram(program);  //����program
		GLint linked;			 //��ȡ�������
		glGetProgramiv(program, GL_LINK_STATUS, &linked);

		if (!linked)
		{
#ifdef _DEBUG
			GLsizei len;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

			GLchar* log = new GLchar[len + 1];
			glGetProgramInfoLog(program, len, &len, log);
			std::cerr << "Shader linking failed: " << log << std::endl;
			delete[] log;
#endif // _DEBUG

			for (entry = shaders; entry->type != GL_NONE; ++entry)
			{
				glDeleteShader(entry->shader);
				entry->shader = 0;
			}
			return 0;
		}

		return program;
	}


#ifdef __cplusplus
};
#endif // __cplusplus