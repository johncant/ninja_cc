#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#include "waves_visual.h"

WavesVisual::WavesVisual(const SonicMedium& sm) {
}

void WavesVisual::initialize() {
}

void WavesVisual::configure(double w, double h) {
}

void WavesVisual::draw() {

  glClearColor(1.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

}

