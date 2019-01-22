#version 330 core

layout(location = 0) out vec4 color;
uniform vec4 u_Color;
uniform sampler2D u_Texture;
vec4 texColor;

in vec2 v_TexCoord;
in vec4 v_Color;

void main(){

    texColor = texture( u_Texture, v_TexCoord );
    color = v_Color;
    color = v_Color * texColor;
}
