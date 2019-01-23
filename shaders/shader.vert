#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 vertColor;
layout(location = 2) in vec2 texCoord;

uniform mat4 u_MVP;

out vec2 v_TexCoord;
out vec4 v_Color;

void main(){

    gl_Position = u_MVP * position;
    v_TexCoord = texCoord;
    v_Color = vertColor;
}
