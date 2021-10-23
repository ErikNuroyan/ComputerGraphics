#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <unistd.h>
#include <fstream>
#include <sstream>

#define PI 3.14
#define TO_RADIAN(x) ((x)*(PI))/180

struct ShaderInfo{
    unsigned int type;
    std::string shsource;
};


static ShaderInfo parseShader(const std::string& f_path, unsigned int type){
    std::fstream f_stream = std::fstream(f_path);
    std::stringstream m;
    
    if(!f_stream.is_open()){
        std::cout<<"Not Open";
    }
    
    std::string line;
    while(std::getline(f_stream,line)){
        m<<line<<"\n";
    }
    
    return {type,m.str()};
}

static unsigned int compileMyShader(const ShaderInfo& sh){
    unsigned int shader = glCreateShader(sh.type);
    const char* src =sh.shsource.c_str();
    glShaderSource(shader, 1, &src , NULL);
    glCompileShader(shader);
    
    //TODO: Error Handling here, compilation succesfull
    int result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE){
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char message[length];
        glGetShaderInfoLog(shader, length, &length, message);
        std::cout<<message<<std::endl;
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

    return program;
}

int main(){
    GLFWwindow* wind;
    if(!glfwInit()){
        return -1;
    }
    
    wind = glfwCreateWindow(450, 450, "MyTitle", NULL, NULL);
    if(!wind){
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(wind);
    
    unsigned int buffer_id;
    glGenBuffers(1,&buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER,buffer_id);
    
    float vec_positions[8] = {
        -0.5f,0.0f,
        0.5f,0.0f,
        0.0f,0.5f,
        1.0f,0.5f
    };
    
    
    glBufferData(GL_ARRAY_BUFFER,8*sizeof(float),vec_positions,GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,0);
    
    unsigned int indices[6] = {
        0,1,2,
        2,1,3
    };
    
    unsigned int buffer2Id;
    glGenBuffers(1, &buffer2Id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer2Id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    
    
    
    unsigned int shaderProgram = createShader("res/shaders/vshader.vsh", "res/shaders/fshader.fsh");
    glUseProgram(shaderProgram);
    
    int u_ColorLocation = glGetUniformLocation(shaderProgram, "u_Color");
    int u_Position = glGetUniformLocation(shaderProgram, "u_Rotate");
    
    float alpha = 0.0;
    float v[4] = {static_cast<float>(cos(0.418879)), static_cast<float>(-sin(0.418879)), static_cast<float>(sin(0.418879)), static_cast<float>(cos(0.418879))};
    while(!glfwWindowShouldClose(wind)){
        glClear(GL_COLOR_BUFFER_BIT);
        glUniform4f(u_ColorLocation, v[0], v[1], v[2], v[3]);
        v[0] = static_cast<float>(cosl(TO_RADIAN(alpha)));
        v[1] = static_cast<float>(-sinl(TO_RADIAN(alpha)));
        v[2] = static_cast<float>(sinl(TO_RADIAN(alpha)));
        v[3] = static_cast<float>(cosl(TO_RADIAN(alpha)));
        glUniformMatrix2fv(u_Position, 1, GL_FALSE, v);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        alpha += 1.0;
        glfwSwapBuffers(wind);
        glfwPollEvents();
    }
    
    glfwTerminate();
    
    return 0;
}
