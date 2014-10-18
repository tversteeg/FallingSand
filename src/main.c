#include <stdio.h>

#include <ccore/file.h>
#include <ccore/display.h>
#include <ccore/window.h>
#include <ccore/opengl.h>
#include <ccore/event.h>
#include <ccore/string.h>
#include <ccore/error.h>

#include <GL/glew.h>

#include "tga.h"

GLuint loadShader(char *file, GLenum type)
{
	FILE *fp;
	char *buf;
	GLuint shader;
	GLint status, logLength;
	ccFileInfo fi;

	fi = ccFileInfoGet(file);
	if(!fi.size){
		return 0;
	}
	buf = malloc(fi.size);

	fp = fopen(file, "rb");
	fread(buf, 1, fi.size, fp);
	fclose(fp);

	buf[fi.size] = '\0';

	shader = glCreateShader(type);
	glShaderSource(shader, 1, (const char**)&buf, NULL);
	glCompileShader(shader);

	free(buf);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if(status == GL_FALSE){
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

		buf = malloc(logLength + 1);
		glGetShaderInfoLog(shader, logLength, NULL, buf);

		printf("%s shader error:\n\t%s", (type == GL_VERTEX_SHADER) ? "Vertex" : "Fragment", buf);

		free(buf);	
	}

	return shader;
}

GLuint loadProgram()
{
	GLuint program, vertex, fragment;

	fragment = loadShader(ccStringConcatenate(2, ccFileDataDirGet(), "fragment.glsl"), GL_FRAGMENT_SHADER);
	vertex = loadShader(ccStringConcatenate(2, ccFileDataDirGet(), "vertex.glsl"), GL_VERTEX_SHADER);

	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);

	glBindAttribLocation(program, 0, "position");
	glLinkProgram(program);

	return program;
}

void loadScreenTriangles(GLuint *vao, GLuint *vbo)
{
	GLfloat vertices[] = {
		-1,-1, 0,
		1,-1, 0,
		-1, 1, 0,
		1,-1, 0,
		-1, 1, 0,
		1, 1, 0,
	};

	glGenVertexArrays(1, vao);
	glBindVertexArray(*vao);

	glGenBuffers(1, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int main(int argc, char** argv)
{
	bool loop, mouseDown;
	ccEvent event;
	ccError error;
	GLuint program, vao, vbo, tex;
	GLint imageLocation, mouseLocation;

	loop = true;
	mouseDown = false;

	if(ccDisplayInitialize()){
		goto cc_error;
	}
	if(ccWindowCreate((ccRect){.x = 0, .y = 0, .width = 800, .height = 600}, "OpenGL Falling sand, made with ccore", 0)){
		goto cc_error;
	}
	if(ccGLContextBind()){
		goto cc_error;
	}

	glewInit();

	printf("OpenGL version: %s\n", glGetString(GL_VERSION));
	printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));

	program = loadProgram();

	tex = loadTGATexture(ccStringConcatenate(2, ccFileDataDirGet(), "texture.tga"));
	imageLocation = glGetUniformLocation(program, "texUnit");
	mouseLocation = glGetUniformLocation(program, "mouse");
	glUseProgram(program);

	loadScreenTriangles(&vao, &vbo);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glProgramUniform1i(program, imageLocation, 0);

	while(loop){
		while(ccWindowEventPoll()){
			event = ccWindowEventGet();
			switch(event.type){
				case CC_EVENT_WINDOW_QUIT:
					loop = false;
					break;
				case CC_EVENT_MOUSE_DOWN:
					mouseDown = true;
					break;
				case CC_EVENT_MOUSE_UP:
					mouseDown = false;
					break;
				default:
					break;
			}
		}

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		if(mouseDown){
			glProgramUniform2f(program, mouseLocation, (float)ccWindowGetMouse().x / 800, (600 - (float)ccWindowGetMouse().y) / 600);
		}else{
			glProgramUniform2f(program, mouseLocation, 0, 0);
		}

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, 800, 600, 0);

		if(ccGLBuffersSwap()){
			goto cc_error;
		}
	}

	ccFree();

	return 0;

cc_error:
	while((error = ccErrorPop())){
		printf("Error: %s", ccErrorString(error));
	}

	ccFree();

	return -1;
}
