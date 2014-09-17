const char waves_visual_shader_vs_src[] = "\
#version 330\n\
\n\
in vec2 pos2;\n\
uniform mat4 unprojection_matrix;\n\
out vec3 pos3;\n\
\n\
void main() {\n\
  gl_Position = vec4(pos2[0], pos2[1], 0, 1);\n\
  vec4 pos4 = unprojection_matrix*gl_Position;\n\
  pos3 = vec3(pos4[0], pos4[1], pos4[2]);\n\
}\n\
\n\
\n\
";
