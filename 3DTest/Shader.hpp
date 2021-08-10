#pragma once
#include <map>
#include <GL/glew.h>

namespace util {
	class Shader { 
	public:
		static std::map<const char*, GLuint> compiled;

	public:
		static void Init(void);
		static GLuint CompileShader(const char* name);
		static GLuint GetDefaultShader(void);
	};
}