#version 420 core

in vec2 TexCoord;
in vec3 ourColor;

out vec4 color;
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

void main()
{
  color = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), 0.2);
    //color = vec4(1,0,0,1);
}
