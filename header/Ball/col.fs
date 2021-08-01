#version 330 core
out vec4 FragColor;
in vec3 pos;
uniform vec3 color;
uniform vec3 center;
uniform float rad;
void main()
{
    vec3 tmp = pos;

    vec3 colorFade = color * sqrt(pow(pos.x-center.x,2) + pow(pos.y-center.y,2) + pow(pos.z-center.z,2) )/2;
    FragColor =vec4(colorFade, 1.0f);
    
    }