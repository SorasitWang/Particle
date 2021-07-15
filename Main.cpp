#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include "./header/shader_m.h"
#include "./header/stb_image.h"
#include "./header/camera.h"
#include <iostream>
#include "./header/Box/Box.h"
#include "./header/Ball.h"
#include "Ball.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window,float deltatIme,Shader boxShader);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;


//mouse
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float xPos = 0.0f;
float yPos = 0.0f;
bool firstMouse = true;
float collectTime = 0.15f;
float slope = 0.0f;
float oldSlope = 0.0f;
float countTime = 0.0f;
std::vector<float> oldPos = { 0.0f,0.0f };
glm::vec3 direction = glm::vec3(0.0f,0.0f,0.0f);

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

bool adding = false;
Box box = Box(0);

int numBalls = 10;
std::vector<Ball> balls;
int main()
{

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif


    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader boxShader = Shader("box.vs", "box.fs");
    float sizeWall = 0.5;
    srand(time(0));

    Shader ballShader = Shader("ball.vs", "ball.fs");
    for (int i = 0; i < numBalls; i++) {
        balls.push_back(Ball());
        balls[i].init(ballShader);
    }
    box = Box(sizeWall);

   
    box.init(boxShader);


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        countTime += deltaTime;
        // input
        // -----
        processInput(window,deltaTime,boxShader);

       
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      
        box.draw(boxShader);
        for (int i = 0; i < numBalls; i++) {
            balls[i].draw(ballShader, deltaTime, box.sizeX,box.sizeY,balls,i);
            //std::cout <<i<< "   "<< balls[i].direction.x << " " << balls[i].velocity.y << std::endl;
        }
        //std::cout <<"------------"<< std::endl;

        // camera/view transformation



        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window,float deltaTime,Shader boxShader)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        box.changeSize(deltaTime, 2, boxShader);
       
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        box.changeSize(deltaTime, 3, boxShader);
       
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        box.changeSize(deltaTime, 1, boxShader);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        box.changeSize(deltaTime, 0, boxShader);
    }


}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
    {
       
       
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)

{
   
    xPos = xpos; yPos = ypos;
    if (adding == true ) {
        if (oldPos[0] == -1) {
            oldPos[0] = xPos;
            oldPos[1] = yPos;
        }
        else if(countTime > collectTime){
            oldPos[0] = xPos;
            oldPos[1] = yPos;
            countTime = 0.0f;
        }
        countTime += deltaTime;
    }
    if (adding == false) {
        //std::cout << xPos << " " << oldPos[0] << "---" << yPos << " " << oldPos[1] << std::endl;
        direction.y = yPos - oldPos[1];
        direction.x = xPos - oldPos[0];
        oldPos[0] = -1;
        oldPos[1] = -1;
    }
        

        /*if (countTime > collectTime) {
            //std::cout << countTime << std::endl;
            
            countTime = 0.0f;
            //if (abs(slope - oldSlope) > 0.3) {
                //oldSlope = slope;
            
            //}
        }
    }*/
    if (adding) balls[balls.size() - 1].position = glm::vec3(2 * xPos / SCR_WIDTH - 1, 2 * (-yPos / SCR_HEIGHT + 0.5), 0.0f);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
   
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (adding == false) {
            Shader boxShader = Shader("box.vs", "box.fs");
            balls.push_back(Ball());
            balls[balls.size() - 1].init(boxShader);
            balls[balls.size() - 1].position = glm::vec3(2 * xPos / SCR_WIDTH - 1, 2 * (-yPos / SCR_HEIGHT + 0.5), 0.0f);
            balls[balls.size() - 1].velocity.y = 0;
            balls[balls.size() - 1].isIn = false;
            balls[balls.size() - 1].move = false;
           
            numBalls += 1;
            adding = true;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        adding = false;
        balls[balls.size() - 1].velocity.y = -1;
        balls[balls.size() - 1].move = true;
//std::cout << xPos << " " << oldPos[0] << "////" << yPos << " " << oldPos[1] << std::endl;
       
        
        direction.x = xPos - oldPos[0];
        direction.y = yPos - oldPos[1];
        if (oldPos[0] == -1)
            direction.x = 0.0;
        if (oldPos[1] == -1)
            direction.y = 0.0;
        //std::cout << direction.x << " " << SCR_WIDTH << " " << direction.y << " " << SCR_HEIGHT << std::endl;
        balls[balls.size() - 1].direction.x = 6*direction.x / SCR_WIDTH;
        balls[balls.size() - 1].velocity.y = -6*direction.y / SCR_HEIGHT;
    }
}