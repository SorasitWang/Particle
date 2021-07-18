#version 330 core
layout (location = 0) in vec3 aPos;


uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform bool threeD;
void main()
{
    if (threeD){
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
    else{
        gl_Position = model * vec4(aPos, 1.0);
    }
}