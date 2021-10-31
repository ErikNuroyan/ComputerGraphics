#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <iostream>
#include <math.h>
#include <fstream>
#include <sstream>
#include <vector>

#define EPSILLON 0.00001

struct ShaderInfo{
    unsigned int type;
    std::string shsource;
};

struct WindowContext {
    glm::mat4* view;
    int viewLocation;
    int viewDirectionLocation;
    glm::vec3* cameraPos;
    glm::vec3* cameraFront;
    glm::vec3* cameraUp;
};

void CheckOpenGLError(const char* stmt, const char* fname, int line)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        printf("OpenGL error %i, at %s:%i - for %s\n", err, fname, line, stmt);
        abort();
    }
}

#define GL_CHECK(stmt) do { \
        stmt; \
        CheckOpenGLError(#stmt, __FILE__, __LINE__); \
    } while (0)



static ShaderInfo parseShader(const std::string& f_path, unsigned int type) {
    std::fstream f_stream = std::fstream(f_path);
    std::stringstream m;
    
    if(!f_stream.is_open()){
        std::cout << "Not Open";
    }
    
    std::string line;
    while(std::getline(f_stream,line)){
        m<< line << "\n";
    }
    
    return {type,m.str()};
}

static unsigned int compileMyShader(const ShaderInfo& sh){
    unsigned int shader = glCreateShader(sh.type);
    const char* src =sh.shsource.c_str();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    
    int result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE){
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char message[length];
        glGetShaderInfoLog(shader, length, &length, message);
        std::cout << message << std::endl;
        glDeleteShader(shader);
    }
    
    return shader;
}
static unsigned int createShader(const std::string& vshPath, const std::string& fshPath){
    unsigned int program = glCreateProgram();
    
    ShaderInfo vsh = parseShader(vshPath, GL_VERTEX_SHADER);
    ShaderInfo fsh = parseShader(fshPath, GL_FRAGMENT_SHADER);
    
    unsigned int vshId = compileMyShader(vsh);
    unsigned int fshId = compileMyShader(fsh);
    
    glAttachShader(program, vshId);
    glAttachShader(program, fshId);
    glLinkProgram(program);
    glValidateProgram(program);
    
    glDeleteShader(vshId);
    glDeleteShader(fshId);
    
    return program;
}

std::vector<float> getCylinderCoordinates(float height, int nSectors) {
    std::vector<float> coords;
    
    //The center coordinates of the first circle
    coords.push_back(0.0f); //x
    coords.push_back(-1.0f);//y
    coords.push_back(0.0f); //z
    
    
    //The center coordinates of the second circle
    coords.push_back(0.0f); //x
    coords.push_back(1.0f); //y
    coords.push_back(0.0f); //z

    
    const float deltaAngle = 2 * M_PI / nSectors;
    const float deltaHeight = 2.0f / height;
    
    for (float h = -1.0f; h < 1.0f + EPSILLON; h += deltaHeight) {
        for (float phi = 0.0f; phi < 2 * M_PI - deltaAngle + EPSILLON; phi += deltaAngle) {
            const float x = cos(phi);
            const float z = sin(phi);
            coords.push_back(x);
            coords.push_back(h);
            coords.push_back(z);
        }
    }
    
    
    return coords;
}

std::vector<std::vector<unsigned int>> getIndexBuffers(int nSectors, int nSegments) {
    std::vector<std::vector<unsigned int>> indexBuffers;
    
    //Creating the bottom fan
    indexBuffers.emplace_back();
    indexBuffers[0].push_back(0);
    int start = 2;
    for (int i = start; i < start + nSectors; ++i) {
        indexBuffers[0].push_back(i);
    }
    indexBuffers[0].push_back(start);
    
    //Creating the middle strip
    indexBuffers.emplace_back();
    for (int i = 0; i < nSegments; ++i) {
        for (int j = 0; j <= nSectors; ++j) {
            indexBuffers[1].push_back(i * nSectors + j % nSectors + start);
            indexBuffers[1].push_back((i + 1) * nSectors + j % nSectors + start);
        }
        if (i != nSegments - 1) {
            indexBuffers[1].push_back((i + 1) * nSectors + start);
            indexBuffers[1].push_back((i + 1) * nSectors + start);
        }
    }
    
    //Creating the top fan
    indexBuffers.emplace_back();
    indexBuffers[2].push_back(1);
    start = start + nSectors * nSegments;
    for (int i = start; i < start + nSectors; ++i) {
        indexBuffers[2].push_back(i);
    }
    indexBuffers[2].push_back(start);
    
    
    return indexBuffers;
}

int main(){
    GLFWwindow* wind;
    if (!glfwInit()) {
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    const int width = 700;
    const int height = 700;
    
    
    wind = glfwCreateWindow(width, height, "MyTitle", NULL, NULL);
    if (!wind) {
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(wind);
    
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    
    GLuint vertexArray;
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    
    const int nSectors = 127;
    const int nSegments = 127;
    std::vector<float> coords = getCylinderCoordinates(nSegments, nSectors);
    unsigned int buffer_id;
    GL_CHECK(glGenBuffers(1, &buffer_id));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, buffer_id));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, coords.size() * sizeof(float), coords.data(), GL_STATIC_DRAW));
    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0));
    GL_CHECK(glEnableVertexAttribArray(0));
    
    
    std::vector<std::vector<unsigned int>> indexBuffers = getIndexBuffers(nSectors, nSegments);
    unsigned int indexBufferBottom;
    GL_CHECK(glGenBuffers(1, &indexBufferBottom));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferBottom));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffers[0].size() * sizeof(unsigned int), indexBuffers[0].data(), GL_STATIC_DRAW));
    
    unsigned int indexBufferMiddle;
    GL_CHECK(glGenBuffers(1, &indexBufferMiddle));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferMiddle));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffers[1].size() * sizeof(unsigned int), indexBuffers[1].data(), GL_STATIC_DRAW));

    unsigned int indexBufferTop;
    GL_CHECK(glGenBuffers(1, &indexBufferTop));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferTop));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffers[2].size() * sizeof(unsigned int), indexBuffers[2].data(), GL_STATIC_DRAW));
    
    unsigned int shaderProgram = createShader("res/shaders/vshader.vsh", "res/shaders/fshader.fsh");
    GL_CHECK(glUseProgram(shaderProgram));
    
    //Setting some of the uniforms
    glm::mat4 model = glm::mat4(1.0f);
    int modelLocation = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
    
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 150.0f);
    int projectionLocation = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
    
    int kALocation = glGetUniformLocation(shaderProgram, "Ka");
    glUniform1f(kALocation, 0.1);
    
    int kDLocation = glGetUniformLocation(shaderProgram, "Kd");
    glUniform1f(kDLocation, 1.0);
    
    int kSLocation = glGetUniformLocation(shaderProgram, "Ks");
    glUniform1f(kSLocation, 1.0);
    
    int ambientColorLocation = glGetUniformLocation(shaderProgram, "ambientColor");
    glUniform3f(ambientColorLocation, 1.0, 1.0, 1.0);
    
    int diffuseColorLocation = glGetUniformLocation(shaderProgram, "diffuseColor");
    glUniform3f(diffuseColorLocation, 1.0, 1.0, 1.0);
    
    int specularColorLocation = glGetUniformLocation(shaderProgram, "specularColor");
    glUniform3f(specularColorLocation, 1.0, 1.0, 1.0);
    
    int lightPositionLocation = glGetUniformLocation(shaderProgram, "lightPosition");
    glUniform3f(lightPositionLocation, -3.0, 3.0, 5.0);
    
    //Setting the camera
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    
    glm::mat4 view = glm::lookAt(cameraPos, cameraFront, cameraUp);
    int viewLocation = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
    
    int viewDirectionLocation = glGetUniformLocation(shaderProgram, "viewDirection");
    glUniform3f(viewDirectionLocation, cameraPos[0], cameraPos[1], cameraPos[2]);
    
    WindowContext ctx{&view,
                      viewLocation,
                      viewDirectionLocation,
                      &cameraPos,
                      &cameraFront,
                      &cameraUp};
    glfwSetWindowUserPointer(wind, &ctx);
    
    auto cursorPosCallback = [](GLFWwindow* window, double xpos, double ypos) {
        static float curPosX = xpos, curPosY = ypos;
        if (xpos >= 0 && xpos < width && ypos >= 0 && ypos < height) {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                WindowContext* winCont = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
                static float phi = 0.0;
                static float theta = 0.0;
                float dx = xpos - curPosX;
                float dy = ypos - curPosY;
                
                theta -= glm::radians(dx);
                phi += glm::radians(dy);
                if (phi > M_PI / 2) {
                    phi = glm::radians(90.0f);
                }
                else if (phi < -M_PI / 2) {
                    phi = glm::radians(-90.0f);
                }
                
                glm::vec3& cameraPos = *(winCont->cameraPos);
                cameraPos[0] = 5.0 * cos(phi) * sin(theta);
                cameraPos[1] = 5.0 * sin(phi);
                cameraPos[2] = 5.0 * cos(phi) * cos(theta);
                
                glm::mat4& view = *(winCont->view);
                view = glm::lookAt(cameraPos, *(winCont->cameraFront), *(winCont->cameraUp));
                glUniformMatrix4fv(winCont->viewLocation, 1, GL_FALSE, &view[0][0]);
                
                glUniform3f(winCont->viewDirectionLocation, cameraPos[0], cameraPos[1], cameraPos[2]);
            }
            
            curPosX = xpos;
            curPosY = ypos;
        }
    };
    glfwSetCursorPosCallback(wind, cursorPosCallback);
    
    glEnable(GL_DEPTH_TEST);
    
    int objectColorLocation = glGetUniformLocation(shaderProgram, "objectColor");
    int partLocation = glGetUniformLocation(shaderProgram, "part");
    
    while (!glfwWindowShouldClose(wind)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //Bottom circle
        glUniform4f(objectColorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferBottom);
        glUniform1i(partLocation, 1);
        glDrawElements(GL_TRIANGLE_FAN, indexBuffers[0].size(), GL_UNSIGNED_INT, nullptr);
        
        //Middle part
        glUniform4f(objectColorLocation, 0.0f, 1.0f, 0.0f, 1.0f);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferMiddle);
        glUniform1i(partLocation, 2);
        glDrawElements(GL_TRIANGLE_STRIP, indexBuffers[1].size(), GL_UNSIGNED_INT, nullptr);

        //Top circle
        glUniform4f(objectColorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferTop);
        glUniform1i(partLocation, 3);
        glDrawElements(GL_TRIANGLE_FAN, indexBuffers[2].size(), GL_UNSIGNED_INT, nullptr);
        
        glfwSwapBuffers(wind);
        glfwPollEvents();
    }
    
    glDeleteProgram(shaderProgram);
    
    glfwTerminate();
    
    return 0;
}
