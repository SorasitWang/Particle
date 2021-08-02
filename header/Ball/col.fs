#version 330 core
out vec4 FragColor;
in vec3 pos;
uniform vec3 color;
uniform vec3 center;
uniform int axis;
uniform float rad;
uniform float time;
void main()
{
    vec3 tmp = pos;
    float w;
    vec3 colorFade = color;
    if (axis == 0){ //x
        colorFade *= max( sqrt(pow(pos.y-center.y,2) + pow(pos.z-center.z,2)),0.04)/rad;
        w = sqrt(pow(pos.y-center.y,2) + pow(pos.z-center.z,2))/rad;
    }
    else if (axis==1){//y
        colorFade *= max(sqrt(pow(pos.x-center.x,2) + pow(pos.z-center.z,2)),0.04 )/rad;
        w = sqrt(pow(pos.x-center.x,2) + pow(pos.z-center.z,2)) / rad;
        }
    else {//z
        colorFade *= max(sqrt(pow(pos.y-center.y,2) + pow(pos.x-center.x,2)),0.04 )/rad;
        w = sqrt(pow(pos.y-center.y,2) + pow(pos.x-center.x,2)) / rad;
         }
    FragColor =vec4(colorFade, 0.2/w*time);
    
    }