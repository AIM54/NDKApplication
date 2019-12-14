#version 300 es
precision mediump float;
in vec2 v_textCoord;
layout(location =0) out vec4 out_color;
uniform sampler2D a_texture;
void main() {
    out_color=texture(a_texture, v_textCoord);
}
