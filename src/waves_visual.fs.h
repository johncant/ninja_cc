const char waves_visual_shader_fs_src[] = "\n\
#version 330\n\
\n\
out vec4 wtfFragColor;\n\
\n\
in vec3 pos3;\n\
in vec2 pos_uv; // For testing only \n\
\n\
uniform vec3 point_sound_origins[64];\n\
uniform float point_sound_min_distance[64];\n\
uniform float sound_speed, value_scale;\n\
uniform float waves_texture_scale;\n\
\n\
uniform sampler1DArray waves_sampler;\n\
\n\
void main() {\n\
  float value = 0.0;\n\
  for (int i=0; i<64;i++) {\n\
    float dist = distance(pos3, point_sound_origins[i]);\n\
    float broadcast_time = (dist-point_sound_min_distance[i])*waves_texture_scale;\n\
    //float sound_value = (-1.0 + 2.0*texture(waves_sampler, vec2(broadcast_time, i))[0])/(dist*dist);\n\
    float sound_value = texture(waves_sampler, vec2(broadcast_time, i))[0]/(dist*dist);\n\
    value = value + sound_value;\n\
  }\n\
    //value = -1.0+2.0*texture(waves_sampler, vec2(2*dist*waves_texture_scale, waves_texture_pixel_height*(1+2*3)))[0];\n\
  \n\
  value = value*value_scale;\n\
  \n\
  \n\
  value = clamp(value, -1.0, 1.0);\n\
  \n\
  //value = broadcast_time;\n\
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
  //if (abs(dist-16.0) < 0.1) { wtfFragColor = vec4(0.0, 0.0, 0.0, 1.0); }\n\
  //if (abs(broadcast_time-0.25) < 0.003) { wtfFragColor = vec4(1.0, 1.0, 1.0, 1.0); }\n\
  //wtfFragColor = vec4(1-value*0.5, 0.0, 0.0, 1.0);\n\
  //wtfFragColor = texture(waves_sampler, vec2(pos_uv[0], pos_uv[1]*16));\n\
}\n\
\n\
";
