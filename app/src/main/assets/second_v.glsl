#version 300 es
layout (location=0) in vec4 vPosition;
layout (location=1) in vec4 aColor;
out vec4 v_Color;
void main(){
    v_Color=aColor;
    gl_Position=vPosition;
}
