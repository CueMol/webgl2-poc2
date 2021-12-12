#version 300 es
// -*-Mode: C++;-*-

layout (location=0) in vec3 vertexPosition;
layout (location=1) in vec4 color;
layout (location=2) in vec3 normal;

// layout (std140) uniform mvp_matrix {
//     mat4 model;
//     mat4 projection;
// };

uniform mat4 model;
uniform mat4 projection;

uniform bool enable_lighting;

out vec4 vColor;

void main() {
  vColor = color;
  gl_Position = projection * model * vec4(vertexPosition, 1.0);
}
