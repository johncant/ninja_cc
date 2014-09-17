#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "waves_visual.h"

#include "waves_visual.vs.h"
#include "waves_visual.fs.h"

class WavesVisualImpl {

  float unprojection_matrix[16];
  float center[3], z[3], y[3], show_radius, width, height;
  void build_unprojection_matrix();

  public:

  friend class WavesVisual;

  WavesVisualImpl();
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


WavesVisual::WavesVisual(const SonicMedium& sm) :
  pimpl(new WavesVisualImpl()),
  sonic_medium(sm)
{
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
       -1.0f,  1.0f, 0.0f,
       -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
       -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f, 0.0f
  };

  glGenBuffers(1, &pimpl->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, pimpl->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(test_data), test_data, GL_STATIC_DRAW);
}

void WavesVisual::configure(double w, double h) {
  pimpl->width = w; pimpl->height = h;
  pimpl->build_unprojection_matrix();
}

void WavesVisual::draw() {

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(pimpl->program);


  GLfloat sound_source_point[16*3];
  GLfloat sound_source_a_w0_phi[16*3];

  for (int i=0; i<16; i++) {
    if (i < sonic_medium.sound_sources.size()) {
      sound_source_point[3*i+0] = sonic_medium.sound_sources[i]->position().get<0>();
      sound_source_point[3*i+1] = sonic_medium.sound_sources[i]->position().get<1>();
      sound_source_point[3*i+2] = sonic_medium.sound_sources[i]->position().get<2>();
      sound_source_a_w0_phi[3*i+0] = 0.6;
      sound_source_a_w0_phi[3*i+1] = 4400.0;
      sound_source_a_w0_phi[3*i+2] = 0.6;
    } else {
      sound_source_point[3*i+0] = 0;
      sound_source_point[3*i+1] = 0;
      sound_source_point[3*i+2] = 0;
      sound_source_a_w0_phi[3*i+0] = 0;
      sound_source_a_w0_phi[3*i+1] = 0;
      sound_source_a_w0_phi[3*i+2] = 0;
    }
  }

  glUniform3fv(glGetUniformLocation(pimpl->program, "point_sound_origins"), 16, sound_source_point);
  glUniform3fv(glGetUniformLocation(pimpl->program, "point_sound_a_w0_phi"), 16, sound_source_a_w0_phi);
  glUniform1f(glGetUniformLocation(pimpl->program, "sound_speed"), 342.0);
  glUniform1f(glGetUniformLocation(pimpl->program, "value_scale"), 1.0);

  glUniformMatrix4fv(glGetUniformLocation(pimpl->program, "unprojection_matrix"), 1, false, pimpl->unprojection_matrix);

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

  glDrawArrays(GL_TRIANGLES, 0, 6);

}

WavesVisualImpl::WavesVisualImpl() {
  center[0] = 0; center[1] = 0; center[2] = 0;
       y[0] = 0;      y[1] = 1;      y[2] = 0;
       z[0] = 0;      z[1] = 0;      z[2] = 1;
  show_radius = 1;
  build_unprojection_matrix();
}

void WavesVisualImpl::build_unprojection_matrix() {
  GLfloat x[3] = { y[1]*z[2]-y[2]*z[1], y[2]*z[0]-y[0]*z[2], y[0]*z[1]-y[1]*z[0] };
  GLfloat *p = unprojection_matrix;

  GLfloat sx, sy;
  if (width > height) {
    sx = show_radius*width/height;
    sy = show_radius;
  } else {
    sx = show_radius;
    sy = show_radius*height/width;
  }

  *p++ = x[0]*sx ; *p++ = x[1]*sx ; *p++ = x[2]*sx ; *p++ = center[0];
  *p++ = y[0]*sy ; *p++ = y[1]*sy ; *p++ = y[2]*sy ; *p++ = center[1];
  *p++ = z[0]    ; *p++ = z[1]    ; *p++ = z[2] ; *p++ = center[2];
  *p++ = 0.0     ; *p++ = 0.0     ; *p++ = 0.0  ; *p++ = 1.0;
}

