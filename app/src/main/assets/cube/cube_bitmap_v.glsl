#version 300 es
layout(location = 0)  in vec4 a_position;
layout(location = 1)  in vec2 a_textCoord;
layout(location = 2) in mat4 a_mvpMatrix;
out vec2 v_textCoord;
void main() {
    v_textCoord=a_textCoord;
    gl_Position = a_mvpMatrix * a_position;
}
