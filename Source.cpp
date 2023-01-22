#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <GLM/glm/glm.hpp>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include <GLM/glm/gtc/type_ptr.hpp>

#include "shader_s.h"
#include "camera.h"
#include "model.h"

#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// game settings
float playerOneY;
float playerTwoY;

float ballx;
float bally;
float ballSpeedX = 10.0f;
float ballSpeedY = 10.0f;

// screen settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f; // time between current frame and last
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Pong", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);
    
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("shader.vs", "shader.fs");

    // load models
    // -------------
    Model ball("res/objects/pong/ball.obj");
    Model paddle("res/objects/pong/paddle.obj");
    
    
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // -----
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame; 

        // input
        // -----
        processInput(window);
        
        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
        ourShader.use();

        // render the ball
        glm::mat4 BallMatrix = glm::mat4(1.0f);
        BallMatrix = glm::translate(BallMatrix, glm::vec3(ballx += ballSpeedX * deltaTime, bally += ballSpeedY * deltaTime, 0.0f));
        BallMatrix = glm::scale(BallMatrix, glm::vec3(.2f, .2f, .2f));	
        ourShader.setMat4("model", BallMatrix);
        ball.Draw(ourShader);
        
        // ball collision (against walls)
        
        if (ballx > 10.0f)
        {
            ballx = 10.0f;
            ballSpeedX *= -1;
        }
        if (ballx < -10.0f)
        {
            ballx = -10.0f;
            ballSpeedX *= -1;
        }
        

        // top and bottom collision 
        if (bally > 8.0f)
        {
            bally = 8.0f;
            ballSpeedY *= -1;
        }
        if (bally < -8.0f)
        {
            bally = -8.0f;
            ballSpeedY *= -1;
        }

        // render the 1st paddle 
        glm::mat4 Paddle1 = glm::mat4(1.0f);
        Paddle1 = glm::translate(Paddle1, glm::vec3(-10.0f,playerOneY,0.0f)); 
        //Paddle1 = glm::rotate(Paddle1, (float)glfwGetTime(), glm::vec3(0.0f, 5.0f, 0.0f));
        Paddle1 = glm::scale(Paddle1, glm::vec3(0.5f, 0.5f, 0.5f));	
        ourShader.setMat4("model", Paddle1);
        paddle.Draw(ourShader);

        // render the 2nd paddle
        glm::mat4 Paddle2 = glm::mat4(1.0f);
        Paddle2 = glm::translate(Paddle2, glm::vec3(10.0f, playerTwoY, 0.0f)); 
        //Paddle2 = glm::rotate(Paddle1, (float)glfwGetTime(), glm::vec3(0.0f, 5.0f, 0.0f));
        Paddle2 = glm::scale(Paddle2, glm::vec3(0.5f, 0.5f, 0.5f));	
        ourShader.setMat4("model", Paddle2);
        paddle.Draw(ourShader);

        /// view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // player movement input
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        playerOneY += 0.25f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        playerOneY -= 0.25f;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        playerTwoY += 0.25f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        playerTwoY -= 0.25f;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);

    
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
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}