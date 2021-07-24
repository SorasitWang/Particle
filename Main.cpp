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


#include "header/imgui/imgui.h"
#include "header/imgui/imgui_impl_glfw.h"
#include "header/imgui/imgui_impl_opengl3.h"
#include <stdio.h>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>            // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>            // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>          // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <glad/gl.h>            // Initialize with gladLoadGL(...) or gladLoaderLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h>  // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h>// Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>
#include "header/imgui/imgui_impl_glfw.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window,float deltatIme,Shader boxShader);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

ballProperty ballProp;
//mouse
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float xPos = 0.0f;
float yPos = 0.0f;
bool firstMouse = true;
float collectTime = 0.15f;
float countTime = 0.0f;
bool threeD = false;
std::vector<float> oldPos = { 0.0f,0.0f };
glm::vec3 direction = glm::vec3(0.0f,0.0f,0.0f);

bool edit = false;
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

bool adding = false;
Box box = Box(0);
Camera cam = Camera();

int numBalls = 8;
std::vector<Ball> balls;
int main()
{
    const char* glsl_version = "#version 130";
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

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
        balls[i].init(ballShader,ballProp,threeD,balls);
    }
    box = Box(sizeWall);

   
    box.init(boxShader,threeD);


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        countTime += deltaTime;
        // input
        // -----
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        processInput(window,deltaTime,boxShader);

       
        if(show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Settings!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("Movement Property");   
            ImGui::Text("TAB to change Dimension Mode");
            ImGui::Text("SPACE to change Edit Mode");
            // Display some text (you can use a format strings too)
            ImGui::Checkbox("Open 3D mode", &threeD);
            ImGui::Checkbox("Open Edit mode", &edit);// Edit bools storing our window open/close state
            //ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("Friction", &ballProp.friction, 0.0f, 1.0f);
            ImGui::SliderFloat("LifeTime", &ballProp.lifeTime, 3.0f, 60.0f);    
            ImGui::SliderFloat("G", &ballProp.g, -1.0f, 1.0f); 
            ImGui::SliderFloat("VelocityLostWall", &ballProp.velocityLostWall, 0.0f, 0.99f); 
            ImGui::SliderFloat("VelocityLostBump", &ballProp.velocityLostBump, 0.0f, 0.99f);

           ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
       
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        glm::mat4 projection = glm::perspective(glm::radians(cam.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = cam.GetViewMatrix();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        box.draw(boxShader,projection,view);
        for (int i = 0; i < numBalls; i++) {
            balls[i].draw(ballShader, deltaTime, box.sizeX, box.sizeY,box.sizeZ, balls, i, projection, view, ballProp);
            //std::cout <<i<< "   "<< balls[i].direction.x << " " << balls[i].velocity.y << std::endl;
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

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

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        box.changeSize(deltaTime, 4, boxShader);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        box.changeSize(deltaTime, 5, boxShader);
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) 
        cam.ProcessKeyboard(FORWARD, deltaTime);
        
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) 
        cam.ProcessKeyboard(BACKWARD, deltaTime);
        
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        cam.ProcessKeyboard(LEFT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)     
        cam.ProcessKeyboard(RIGHT, deltaTime);

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
    {
       
        threeD = !threeD;
        std::cout << threeD;
        Shader ballShader = Shader("ball.vs", "ball.fs");
        Shader boxShader = Shader("box.vs", "box.fs");
        box.init(boxShader, threeD);
        for (int i = 0; i < numBalls; i++) {
            balls.push_back(Ball());
            balls[i].init(ballShader, ballProp, threeD,balls);
        }

    }


    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {

        edit = !edit;
        if (!edit) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }


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
    cam.ProcessMouseMovement(xoffset, yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        if (adding == false && threeD == false) {
            Shader boxShader = Shader("box.vs", "box.fs");
            balls.push_back(Ball());
            balls[balls.size() - 1].init(boxShader, ballProp,threeD,balls);
            balls[balls.size() - 1].position = glm::vec3(2 * xPos / SCR_WIDTH - 1, 2 * (-yPos / SCR_HEIGHT + 0.5), 0.5f);
            balls[balls.size() - 1].velocity.y = 0;
            balls[balls.size() - 1].isIn = false;
            balls[balls.size() - 1].move = false;
           
            numBalls += 1;
            adding = true;
        }
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
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



// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cam.ProcessMouseScroll(yoffset);
}