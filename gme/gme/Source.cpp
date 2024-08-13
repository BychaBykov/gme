#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL/SOIL.h>
#include "camera.h"
#include "mesh.h"
#include "interface.h"
#include "functoptions.h"

const GLfloat WIDTH = 1580;
const GLfloat HEIGHT = 900;

const glm::vec3 baseLoc = glm::vec3(0.0f, 0.0f, 0.0f);

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void Do_Movement();
bool gen_fun(vector<Vertex>* vert, vector<unsigned int>* ind);
glm::vec3 normals(float x, float y, float step, Func2arg<float>* fun);
// Camera
camera pov(glm::vec3(20.0f,20.0f,20.0f),-90.0f, 0.0f);
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
bool noerror = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

//Grid
GLfloat gridVertices[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,

    -1.0f, -1.0f, -.0f,
    -1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f};

glm::vec3 gridPos = glm::vec3(0.0f, 1.0f, 0.0f);

// The MAIN function, from here we start the application and run the game loop
int main()
{

    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;

    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();

    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);

    // Setup OpenGL options
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    //setup dearimgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    //open shaders
    Shader ourShader("shaders/shader.vertex", "shaders/shaderSM.frag");
    ourShader.Use();
    //make vao and ebo for grid
    GLuint gridVBO, gridVAO;
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(gridVAO);

    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gridVertices), gridVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    //glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

    glBindVertexArray(0);

    ifstream objectfile;
    objectfile.open("input.txt");
    int vert_num, ind_num;
    objectfile >> vert_num >> ind_num;
    vector<Vertex> formvert;
    formvert.resize(vert_num);
    for (int i = 0; i < vert_num; i++) {
        glm::vec3 pos;
        GLfloat x, y, z;
        objectfile >> x >> y >> z;
        pos.x = x;
        pos.y = y;
        pos.z = z;
        formvert[i].Position = pos;
    }
    vector<unsigned int> indices;
    indices.resize(ind_num*3);
    for (int i = 0; i < ind_num * 3; i++) {
        int vert;
        objectfile >> vert;
        indices[i] = vert;
    }
    Mesh form;
    bool ex = false;
    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //ImGui::ShowDemoWindow();
        render();
        //Do_Movement();
        glClearColor(0.941f, 0.929f, 0.933f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        //make grid

        glBindVertexArray(gridVAO);
        glm::mat4 view = pov.view;
        glm::mat4 projection = glm::perspective(pov.zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

        //set axis, borders and grid
        {
            //axis y green
            GLint colorLoc = glGetUniformLocation(ourShader.Program, "colour");
            glUniform3f(colorLoc, 0.337f, 0.871f, 0.306f);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(15.0f, 0.1f, 0.1f));
            GLint pvmLoc = glGetUniformLocation(ourShader.Program, "pvmMat");
            glm::mat4 pvm = projection * view * model;
            glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            //axis z blue
            glUniform3f(colorLoc, 0.498f, 0.69f, 0.922f);
            model = glm::mat4(1.0f);
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(15.0f, 0.1f, 0.1f));
            pvm = projection * view * model;
            glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            //axis x red
            glUniform3f(colorLoc, 0.839f, 0.208f, 0.208f);
            model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(15.0f, 0.1f, 0.1f));
            pvm = projection * view * model;
            glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
            glDrawArrays(GL_TRIANGLES, 0, 6);

            //set borders
            glUniform3f(colorLoc, 0.98f, 0.98f, 0.973f);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(15.0f, 15.0f, 0.0f));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(15.0f, 0.1f, 0.1f));
            pvm = projection * view * model;
            glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(15.0f, -15.0f, 0.0f));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(15.0f, 0.1f, 0.1f));
            pvm = projection * view * model;
            glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-15.0f, 15.0f, 0.0f));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(15.0f, 0.1f, 0.1f));
            pvm = projection * view * model;
            glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-15.0f, -15.0f, 0.0f));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(15.0f, 0.1f, 0.1f));
            pvm = projection * view * model;
            glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(15.0f, 0.0f, 15.0f));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(15.0f, 0.1f, 0.1f));
            pvm = projection * view * model;
            glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(15.0f, 0.0f, -15.0f));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(15.0f, 0.1f, 0.1f));
            pvm = projection * view * model;
            glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-15.0f, 0.0f, 15.0f));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(15.0f, 0.1f, 0.1f));
            pvm = projection * view * model;
            glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-15.0f, 0.0f, -15.0f));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(15.0f, 0.1f, 0.1f));
            pvm = projection * view * model;
            glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 15.0f, 15.0f));
            model = glm::scale(model, glm::vec3(15.0f, 0.1f, 0.1f));
            pvm = projection * view * model;
            glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 15.0f, -15.0f));
            model = glm::scale(model, glm::vec3(15.0f, 0.1f, 0.1f));
            pvm = projection * view * model;
            glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, -15.0f, 15.0f));
            model = glm::scale(model, glm::vec3(15.0f, 0.1f, 0.1f));
            pvm = projection * view * model;
            glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, -15.0f, -15.0f));
            model = glm::scale(model, glm::vec3(15.0f, 0.1f, 0.1f));
            pvm = projection * view * model;
            glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
            glDrawArrays(GL_TRIANGLES, 0, 6);

            //set grid
            for (int i = 0; i < 15; i++) {
                GLint colorLoc = glGetUniformLocation(ourShader.Program, "colour");
                glUniform3f(colorLoc, 0.5f, 0.5f, 0.5f);
                const glm::vec3 addVec = glm::vec3(0.0f, 1.0f*i, 0.0f);
                model = glm::mat4(1.0f);
                model = glm::translate(model, gridPos + addVec);
                model = glm::scale(model, glm::vec3(15.0f, 0.05f, 0.05f));
                pvm = projection * view * model;
                glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
                glDrawArrays(GL_TRIANGLES, 0, 6);

                model = glm::mat4(1.0f);
                model = glm::translate(model, -(gridPos + addVec));
                model = glm::scale(model, glm::vec3(15.0f, 0.05f, 0.05f));
                pvm = projection * view * model;
                glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
                glDrawArrays(GL_TRIANGLES, 0, 6);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(gridPos.y+ addVec.y, gridPos.x + addVec.x, gridPos.z));
                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                model = glm::scale(model, glm::vec3(15.0f, 0.05f, 0.05f));
                pvm = projection * view * model;
                glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
                glDrawArrays(GL_TRIANGLES, 0, 6);

                model = glm::mat4(1.0f);
                model = glm::translate(model, -glm::vec3(gridPos.y + addVec.y, gridPos.x + addVec.x, gridPos.z));
                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                model = glm::scale(model, glm::vec3(15.0f, 0.05f, 0.05f));
                pvm = projection * view * model;
                glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
                glDrawArrays(GL_TRIANGLES, 0, 6);

            }
            glBindVertexArray(0);
        }
        if (setfunc) {
            ex = gen_fun(&formvert, &indices);
            setfunc = false;
            form.reinit(formvert, indices);
        }
        if (ex) {
            GLint colorLoc = glGetUniformLocation(ourShader.Program, "colour");
            glUniform3f(colorLoc, 0.5f, 0.5f, 0.5f);
            GLint lightposloc = glGetUniformLocation(ourShader.Program, "lightpos");
            glUniform3f(lightposloc, pov.cameraPOSITION.x, pov.cameraPOSITION.y, pov.cameraPOSITION.z);
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 pvm = projection * view * model;
            GLint pvmLoc = glGetUniformLocation(ourShader.Program, "pvmMat");
            glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
            form.Draw(ourShader);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

void Do_Movement()
{
    // Camera controls
    if (keys[GLFW_KEY_W])
        pov.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        pov.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        pov.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        pov.ProcessKeyboard(RIGHT, deltaTime);
    if (keys[GLFW_MOUSE_BUTTON_1]) std::cout << 1;
}


// Is called whenever a key is pressed/released via GLFW
bool cameraCan = false;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    //cout << key << endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    //if (key >= 0 && key < 1024)
    //{
    //    if (action == GLFW_PRESS)
    //        keys[key] = true;
    //    else if (action == GLFW_RELEASE)
    //        keys[key] = false;
    //}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {

        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

    lastX = xpos;
    lastY = ypos;

    if(cameraCan) pov.mouseMovement(xoffset, yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            cameraCan = true;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        cameraCan = false;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    
}

bool gen_fun(vector<Vertex>* vert, vector<unsigned int>* ind) {
    Func2arg<float> f(FuncFromInput);
    if (f.fexist) {
        int i = 0; 
        float step = float(30.0f / (number - 1));
        vert->clear();
        ind->clear();
        for (float x = -15.0f+step/2.0f; x <= 15.0f-step/2.0f; x += step) {
            for (float y = -15.0f + step / 2.0f; y <= 15.0f - step / 2.0f; y += step) {
                float z = f.value(x - step / 2.0f, y - step / 2.0f);
                vert->push_back({glm::vec3(x - step / 2.0f, y - step / 2.0f, z) ,normals(x - step / 2.0f, y - step / 2.0f,step/2.0f,&f)});
                ind->push_back(i);
                z = f.value(x - step / 2.0f, y + step / 2.0f);
                vert->push_back({ glm::vec3(x - step / 2.0f, y + step / 2.0f, z) ,normals(x - step / 2.0f, y - step / 2.0f,step / 2.0f,&f) });
                ind->push_back(i + 1);
                z = f.value(x + step / 2.0f, y + step / 2.0f);
                vert->push_back({ glm::vec3(x + step / 2.0f, y + step / 2.0f, z) ,normals(x - step / 2.0f, y - step / 2.0f,step / 2.0f,&f)});
                ind->push_back(i + 2);
                z = f.value(x + step / 2.0f, y - step / 2.0f);
                vert->push_back({ glm::vec3(x + step / 2.0f, y - step / 2.0f, z) ,normals(x - step / 2.0f, y - step / 2.0f,step / 2.0f,&f) });
                ind->push_back(i + 3);
                ind->push_back(i);
                ind->push_back(i + 2);
                i += 4;
            }
        }
        return true;
    }
    else return false;
}
glm::vec3 normals(float x, float y, float step, Func2arg<float>* fun) {
    float dfx = (fun->value(x, y) - fun->value(x + step, y))/step;
    float dfy = (fun->value(x, y) - fun->value(x, y+step))/step;
    float norms = sqrt(pow(dfx, 2) + pow(dfy, 2) + 1);
    return glm::vec3(dfx / norms, dfy / norms, 1.0f / norms);
}