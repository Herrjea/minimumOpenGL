#version 330 core

layout(location = 0) out vec4 color;

uniform vec4 u_BackgroundColor;
uniform sampler2D u_Texture;

in vec2 v_TexCoord;
in vec4 v_Color;

float amount;


void main(){

    amount = texture( u_Texture, v_TexCoord ).r;
    color = v_Color;
    color = mix( v_Color, u_BackgroundColor, ( 1.0f - amount ) * 2.0f );
}
