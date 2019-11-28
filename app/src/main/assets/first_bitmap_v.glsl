#version 300 es
layout(location =0) in vec4 a_position;
layout(location =1) in vec2 a_textCoord;

out vec2 v_textCoord;

void main() {
    gl_Position = a_position;
    v_textCoord = a_textCoord;
}
