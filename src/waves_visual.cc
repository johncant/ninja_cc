#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "waves_visual.h"

#include "waves_visual.vs.h"
#include "waves_visual.fs.h"

class WavesVisualImpl {
  friend class WavesVisual;
  GLuint program;
  GLuint vao, vbo;
};

static GLuint compile_shader(GLenum shader_type, const char* source) {
  GLuint shader = glCreateShader(shader_type);

  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  GLint info_log_length, compile_status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);

  if (!compile_status) {
    char error_log[info_log_length];
    glGetShaderInfoLog(shader, info_log_length, NULL, error_log);

    std::cout << error_log << std::endl;
  }

  return shader;
}

static GLuint compile_shader_program(
  const char* vertex_shader_source,
  const char* fragment_shader_source
 ) {
  GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader_source);
  GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_source);

  GLuint program = glCreateProgram();

  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  GLint link_status, info_log_length;

  glGetProgramiv(program, GL_LINK_STATUS, &link_status);
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);

  if (!link_status) {

    char error_log[info_log_length];
    glGetProgramInfoLog(program, info_log_length, NULL, error_log);

    std::cout << error_log << std::endl;
  }

  return program;
}


WavesVisual::WavesVisual(const SonicMedium& sm) : pimpl(new WavesVisualImpl()) {
}

WavesVisual::~WavesVisual() {
  delete pimpl;
}

void WavesVisual::initialize() {

  pimpl->program = compile_shader_program(
    waves_visual_shader_vs_src,
    waves_visual_shader_fs_src
  );
  std::cout << "Successfully compiled and linked shaders!" << std::endl;

  glGenVertexArrays(1, &pimpl->vao);
  glBindVertexArray(pimpl->vao);

  static const GLfloat test_data[] = {
       -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f
  };

  glGenBuffers(1, &pimpl->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, pimpl->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(test_data), test_data, GL_STATIC_DRAW);
}

void WavesVisual::configure(double w, double h) {
}

void WavesVisual::draw() {

  glClearColor(1.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(pimpl->program);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, pimpl->vbo);
  glVertexAttribPointer(
    0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );

  glDrawArrays(GL_TRIANGLES, 0, 3);
}

