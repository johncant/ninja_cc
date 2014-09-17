const char waves_visual_shader_fs_src[] = "\n\
#version 330\n\
\n\
out vec4 wtfFragColor;\n\
\n\
in vec3 pos3;\n\
\n\
uniform vec3 point_sound_origins[16];\n\
uniform vec3 point_sound_a_w0_phi[16];\n\
uniform float sound_speed, value_scale;\n\
\n\
\n\
void main() {\n\
  float value = 0.0;\n\
  for (int i=0; i<16;i++) {\n\
    float dist = distance(pos3, point_sound_origins[i]);\n\
    value = value + point_sound_a_w0_phi[i][0]*cos(point_sound_a_w0_phi[i][2] + point_sound_a_w0_phi[i][1]*(dist/sound_speed))/(dist*dist);\n\
  }\n\
  \n\
  value = value*value_scale;\n\
  \n\
  \n\
  value = clamp(value, -1.0, 1.0);\n\
  \n\
  if (value < -0.33333) {\n\
    // Blue to green\n\
    wtfFragColor = vec4(0.0, 1.5 + value*1.5, -0.5 -value*1.5, 1.0);\n\
  } else if (value < 0.33333) {\n\
    // Green to yellow\n\
    wtfFragColor = vec4(0.5+value*1.5, 1, 0, 1.0);\n\
  } else {\n\
    // Yellow to red\n\
    wtfFragColor = vec4(1, 1.5 - 1.5*value, 0, 1.0);\n\
  }\n\
  //wtfFragColor = vec4(1-value*0.5, 0.0, 0.0, 1.0);\n\
}\n\
\n\
";
