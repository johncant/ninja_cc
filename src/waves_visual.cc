#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <stdio.h>
#include <boost/geometry/algorithms/distance.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometry.hpp>

#include "waves_visual.h"
#include "point3d.h"

#include "waves_visual.vs.h"
#include "waves_visual.fs.h"

using std::max;

class WavesVisualImpl {

  public:

  float unprojection_matrix[16];
  float center[3], z[3], y[3], show_radius, width, height;
  void build_unprojection_matrix();
  void build_waves_texture();
  Point3D unproject(float f1, float f2);
  float min_distance_visible_rectangle_to_point(const Point3D& pt);
  double time;


  friend class WavesVisual;

  WavesVisualImpl(SonicMedium& sm);
  ~WavesVisualImpl();

  SonicMedium& sonic_medium;
  GLuint program;
  GLuint vao, vbo;

  GLuint waves_texture;
  double waves_texture_pixel_height, waves_texture_scale;
  float point_sound_min_distance[64];
  float* wave_tex_data;
  double time_scale;

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


WavesVisual::WavesVisual(SonicMedium& sm) :
  pimpl(new WavesVisualImpl(sm))
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

  // Set up 2d drawing shape
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

//  glGenTextures(1, &pimpl->waves_texture);
  pimpl->build_waves_texture();
}

void WavesVisual::configure(double w, double h) {

  pimpl->width = w; pimpl->height = h;

  pimpl->build_unprojection_matrix();
  pimpl->build_waves_texture();
}

void WavesVisual::draw() {

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(pimpl->program);

  pimpl->build_waves_texture();

  GLfloat sound_source_point[64*3];
  GLfloat sound_source_a_w0_phi[64*3];

  for (int i=0; i<64; i++) {
    if (i < pimpl->sonic_medium.sound_sources.size()) {
      sound_source_point[3*i+0] = pimpl->sonic_medium.sound_sources[i]->position().get<0>();
      sound_source_point[3*i+1] = pimpl->sonic_medium.sound_sources[i]->position().get<1>();
      sound_source_point[3*i+2] = pimpl->sonic_medium.sound_sources[i]->position().get<2>();
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

  glUniform3fv(glGetUniformLocation(pimpl->program, "point_sound_origins"), 64, sound_source_point);
  glUniform3fv(glGetUniformLocation(pimpl->program, "point_sound_a_w0_phi"), 64, sound_source_a_w0_phi);
  glUniform1fv(glGetUniformLocation(pimpl->program, "point_sound_min_distance"), 64, pimpl->point_sound_min_distance);
  glUniform1f(glGetUniformLocation(pimpl->program, "sound_speed"), 342.0);
  glUniform1f(glGetUniformLocation(pimpl->program, "value_scale"), 10.0);
  glUniform1f(glGetUniformLocation(pimpl->program, "waves_texture_scale"), pimpl->waves_texture_scale);
  glUniform1f(glGetUniformLocation(pimpl->program, "waves_texture_pixel_height"), pimpl->waves_texture_pixel_height);

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

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_1D_ARRAY, pimpl->waves_texture);
  glUniform1i(glGetUniformLocation(pimpl->program, "waves_sampler"), 0);

  glDrawArrays(GL_TRIANGLES, 0, 6);

}

void WavesVisual::show_radius(double radius) { pimpl->show_radius = radius; pimpl->build_unprojection_matrix(); }

void WavesVisual::time_scale(double ts) {
  pimpl->time_scale = ts;
}

void WavesVisual::time(double t) {
  pimpl->time = t*pimpl->time_scale;
//  std::cout << " timer: " << pimpl->time << std::endl;
}
void WavesVisual::time_advance(double t) { pimpl->time += t; }

WavesVisualImpl::WavesVisualImpl(SonicMedium& sm) :
  sonic_medium(sm),
  wave_tex_data(NULL),
  time_scale(1)
  {
  center[0] = 0; center[1] = 0; center[2] = 0;
       y[0] = 0;      y[1] = 1;      y[2] = 0;
       z[0] = 0;      z[1] = 0;      z[2] = 1;
  show_radius = 1;
  build_unprojection_matrix();
}

WavesVisualImpl::~WavesVisualImpl() {
  if (wave_tex_data) delete[] wave_tex_data;
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

class _TextureIndex {
  unsigned int w,h,c;
  public:
  _TextureIndex(unsigned int _w, unsigned int _h, unsigned int _c) : w(_w), h(_h), c(_c) {}
  unsigned int inline operator()(unsigned int x, unsigned int y, unsigned int ci) {
    return y*w*c+x*c+ci;
  }
  unsigned int inline size() { return w*h*c; }
  //unsigned int inline size() { return w*h*c; }
};

void WavesVisualImpl::build_waves_texture() {
  unsigned int tex_width;
  double scale;


  tex_width = nearbyint(sqrt(pow(width, 2) + pow(height, 2)))*2;
  scale = 2*show_radius/double((width > height) ? height : width); // Distance units per pixel

  waves_texture_scale = 1/(scale*0.5*double(tex_width)); // texels per distance unit

  _TextureIndex t(tex_width, 64, 1);

  if (wave_tex_data) delete[] wave_tex_data;
  wave_tex_data = new float[t.size()];


  for (unsigned int si=0; si < 64 ; si++) {
    // Find shortest distance to sound source

    if (si < sonic_medium.sound_sources.size()) {
      point_sound_min_distance[si] =
      min_distance_visible_rectangle_to_point(sonic_medium.sound_sources[si]->position());
    }

    for (unsigned int ti=0; ti < tex_width; ti++) {
      double result;

      if (si < sonic_medium.sound_sources.size()) {
        result = sonic_medium.sound_sources[si]->evaluate_raw(time - ((ti+0.5)*0.5*scale + point_sound_min_distance[si])/342.0);
      } else {
        result = 0;
      }
//      double result = 0.6*sin(ti*si*440*scale/342);
      result = (result > -1) ? result : -1;
      result = (result <  1) ? result :  1;
      unsigned char int_result = nearbyint(0.5+result*0.5);
//      printf("%d, %d, %d, \n", t.size(), t(ti, si*2+1 ,0), int_result);
      wave_tex_data[t(ti, si  , 0)] = result;
    }
  }

//  std::cerr << "Supplying texture" << std::endl;
//  std::cerr << tex_width << std::endl;
//  std::cerr << tex_height << std::endl;

  // Preallocate evaluation texture

  glDeleteTextures(1, &waves_texture);
  glGenTextures(1, &waves_texture);

  glBindTexture(GL_TEXTURE_1D_ARRAY, waves_texture);

  glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_1D_ARRAY, 0, GL_RED, tex_width, 64, 0, GL_RED, GL_FLOAT, wave_tex_data);

}

Point3D WavesVisualImpl::unproject(float f1, float f2) {
  const float* m[4] = {
    &unprojection_matrix[0],
    &unprojection_matrix[4],
    &unprojection_matrix[8],
    &unprojection_matrix[12]
  };

  return Point3D(
    m[0][0]*f1 + m[0][1]*f2 + m[0][2]*0 + m[0][3]*1,
    m[1][0]*f1 + m[1][1]*f2 + m[1][2]*0 + m[1][3]*1,
    m[2][0]*f1 + m[2][1]*f2 + m[2][2]*0 + m[2][3]*1
  );
};

float WavesVisualImpl::min_distance_visible_rectangle_to_point(const Point3D& pt) {
  typedef boost::geometry::model::polygon<Point3D> Polygon3D;

  // Boost only finds distance to box extrema lines and points.
  //   Find if minimum in plane is inside box

  //   (p - c).u = s
  //   (p - c).v = t
  //
  //   If s and t are viable, then distance = (p - c).n
  //
  //   Otherwise
  //   Fix s = rx or -rx. Define q as closest point
  //     (q - c).u = +-rx
  //     (q - c).v = t
  //     (p - q).v = 0
  //     => q = c + s u + (p - c).v v
  //
  //     If t is viable
  //       (p - q).u = dist
  //
  //     Otherwise
  //     Fix t = ry or -ry. Define q as closest point
  //       (q - c).u = s
  //       (q - c).v = +-ry
  //       (p - q).u = 0
  //       => q = c + (p - c).u u + t v
  //
  //       If s is viable
  //         (p - q).v = dist
  //
  //         Otherwise
  //           Use preferred min/max s and min/max t
  float p_minus_c[3] = {
    pt.get<0>() - center[0],
    pt.get<1>() - center[1],
    pt.get<2>() - center[2]
  };

  float u[3] = { y[1]*z[2]-y[2]*z[1], y[2]*z[0]-y[0]*z[2], y[0]*z[1]-y[1]*z[0] };
  const float *v = y;
  const float *n = z;

//  std::cout << p_minus_c[0] << " " << p_minus_c[1] << " " << p_minus_c[2] << std::endl;
//  std::cout << std::endl;
//  std::cout << u[0] << " " << u[1] << " " << u[2] << std::endl;
//  std::cout << v[0] << " " << v[1] << " " << v[2] << std::endl;
//  std::cout << n[0] << " " << n[1] << " " << n[2] << std::endl;

  float s = p_minus_c[0]*u[0]
          + p_minus_c[1]*u[1]
          + p_minus_c[2]*u[2];

  float t = p_minus_c[0]*v[0]
          + p_minus_c[1]*v[1]
          + p_minus_c[2]*v[2];

//  std::cout << "s = " << s << std::endl;
//  std::cout << "t = " << t << std::endl;

  float srx, sry;

  if (width > height) {
    sry = show_radius;
    srx = show_radius*width/height;
  } else {
    srx = show_radius;
    sry = show_radius*height/width;
  }

  float p_minus_c_dot_n =
  { p_minus_c[0]*z[0]
  + p_minus_c[1]*z[1]
  + p_minus_c[2]*z[2]
  };


  if  ( s >= -srx
     && s <=  srx
     && t >= -sry
     && t <=  sry) {

    //std::cout << "CANTS DISTANCE ";

    // Closest point in-plane to pt is inside our viewing rectangle.

    return p_minus_c_dot_n;

  } else {
    // Try fixing s
    //     => q = c + s u + (p - c).v v

    // Why is not important. They just happen to always be the same
    float p_minus_c_dot_u = s;
    float p_minus_c_dot_v = t;

    float s_fix_s = fmin(srx, fmax(-srx, s));
    float t_fix_t = fmin(sry, fmax(-sry, t));

    float q_minus_center_fix_s[3] = {
      s_fix_s * u[0] + p_minus_c_dot_v * v[0],
      s_fix_s * u[1] + p_minus_c_dot_v * v[1],
      s_fix_s * u[2] + p_minus_c_dot_v * v[2]
    };

    float t_fix_s = q_minus_center_fix_s[0] * v[0]
                  + q_minus_center_fix_s[1] * v[1]
                  + q_minus_center_fix_s[2] * v[2];

    float q_minus_center_fix_t[3] = {
      p_minus_c_dot_u * u[0] + t_fix_t * v[0],
      p_minus_c_dot_u * u[1] + t_fix_t * v[1],
      p_minus_c_dot_u * u[2] + t_fix_t * v[2]
    };

    float s_fix_t = q_minus_center_fix_s[0] * u[0]
                  + q_minus_center_fix_s[1] * u[1]
                  + q_minus_center_fix_s[2] * u[2];

    if (t_fix_s >= -sry
     && t_fix_s <= sry) {
      return sqrt(pow(p_minus_c_dot_u-s_fix_s, 2) + pow(p_minus_c_dot_v-t_fix_s, 2) + pow(p_minus_c_dot_n, 2));
    } else if ( s_fix_t >= -srx
           && s_fix_t <= srx) {
      return sqrt(pow(p_minus_c_dot_u-s_fix_t, 2) + pow(p_minus_c_dot_v-t_fix_t, 2) + pow(p_minus_c_dot_n, 2));
    } else {
      // Fix both s and t
      return sqrt(pow(p_minus_c_dot_u-s_fix_s, 2) + pow(p_minus_c_dot_v-t_fix_t, 2) + pow(p_minus_c_dot_n, 2));
    }
  }

  // This didn't work:
//    Point3D pt_00(unproject(-1, -1)),
//            pt_10(unproject(1, -1)),
//            pt_11(unproject(1, 1)),
//            pt_01(unproject(-1, 1));
//    Polygon3D visible_rect;
//
//    boost::geometry::append(visible_rect, pt_00);
//    boost::geometry::append(visible_rect, pt_10);
//    boost::geometry::append(visible_rect, pt_11);
//    boost::geometry::append(visible_rect, pt_01);
//  //  boost::geometry::correct(visible_rect);
//
//    std::cout << "BOOST DISTANCE " << boost::geometry::distance(visible_rect, pt) << " " << pt.get<0>() << " " << pt.get<1>() << " " << pt.get<2>() <<
//    pt_00.get<0>() << " " << pt_00.get<1>() << " " << pt_00.get<2>() << std::endl;
//    return boost::geometry::distance(visible_rect, pt);
//
//  }
}

