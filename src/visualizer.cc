//#define GLFW_INCLUDE_GL_3
#define GLFW_INCLUDE_GLCOREARB
//#define __gl_h_
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "visualizer.h"

class VisualizerImpl {
  friend class Visualizer;
  GLFWwindow* window;
};

static void error_callback(int error, const char* desc) {
  std::cerr << desc << std::endl;
};

VisualizerImpl* Visualizer::mkimpl() {
  return new VisualizerImpl;
}

void Visualizer::mainloop() {
  initialize();
  // Never return
  while(!glfwWindowShouldClose(pimpl->window)) {
    draw();
    glfwWaitEvents();
  }
}

void Visualizer::initialize() {
  int argc = 0;
  char* argv0 = NULL;

  glfwSetErrorCallback(error_callback);
  glfwInit();
  //glewInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  pimpl->window = glfwCreateWindow(800, 600, "foo", NULL, NULL);

  glfwSetErrorCallback(NULL);

  if (!pimpl->window) {
    glfwTerminate();
  }

  glfwMakeContextCurrent(pimpl->window);
}

void Visualizer::draw() {
  glfwMakeContextCurrent(pimpl->window);

  float ratio;
  int width, height;

  glfwGetFramebufferSize(pimpl->window, &width, &height);

  ratio = width / (float) height;
  glViewport(0, 0, width, height);
  glClearColor(1.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glfwSwapBuffers(pimpl->window);
}

Visualizer::~Visualizer() {
  if (pimpl->window) {
    glfwDestroyWindow(pimpl->window);
    glfwTerminate();
  }
  delete visual;
}

void Visualizer::destroy() {
  glfwDestroyWindow(pimpl->window);
  glfwTerminate();
}

