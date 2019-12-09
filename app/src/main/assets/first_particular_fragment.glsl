#version 300 es
precision mediump float;

uniform vec4 u_Color;
in float v_lifetime;
layout(location =0) out vec4 fragColor;
uniform sampler2D s_texture;
void main() {
    vec4 texColor;
    texColor=texture(s_texture, gl_PointCoord);
    fragColor=vec4(u_Color)*texColor;
    fragColor.a *= v_lifetime;
}
