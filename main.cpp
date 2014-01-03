#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

SDL_Window* mWindow = NULL;
SDL_GLContext mContext = NULL;

bool init()
{
    bool success = true;
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        cout << "Error: Failed to load SDL!" << endl;
        success = false;
    }else{
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        
        mWindow = SDL_CreateWindow("OpenGL Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        
        if(mWindow == NULL)
        {
            cout << "Error: Failed to load Window!" << endl;
            success = false;
        }else{
            mContext = SDL_GL_CreateContext(mWindow);
            if(mContext == NULL)
            {
                cout << "Error Failed to create Context!" << endl;
                success = false;
            }else{
                glewExperimental = GL_TRUE;
                glewInit();
            }
        }
    }
    return success;
}

string loadShaderCode(const char* fileName)
{
    ifstream input(fileName);
    if(!input.good())
    {
        cout << "Failed to load: " << fileName << endl;
    }
    return std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
}

bool checkShaderStatus(GLuint shaderID)
{
    GLint compileStatus;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileStatus);
    if(compileStatus != GL_TRUE)
    {
        GLint infoLogLength;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
        
        GLchar* buffer = new GLchar[infoLogLength];
        GLsizei bufferSize;
        glGetShaderInfoLog(shaderID, infoLogLength, &bufferSize, buffer);
        
        cout << "Shader Compile Error: " << buffer << endl;
        
        delete buffer;
        return false;  
    }
    return true;
}

bool checkProgramStatus(GLuint programID)
{
    GLint linkStatus;
    glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);
    if(linkStatus != GL_TRUE)
    {
        GLint infoLogLength;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar* buffer = new GLchar[infoLogLength];
        GLsizei bufferSize;
        glGetProgramInfoLog(programID, infoLogLength, &bufferSize, buffer);
        
        cout << "Program Error: " << buffer << endl;
        
        delete buffer;  
        return false;
    }
    return true;
}

void genShaders(){
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    
    const char* adapter[1];
    string temp = loadShaderCode("./Shader/Vertex.vsh");
    adapter[0] = temp.c_str();
    glShaderSource(vertexShaderID, 1, adapter, 0);
    
    temp = loadShaderCode("./Shader/Fragment.fsh");
    adapter[0] = temp.c_str();
    glShaderSource(fragmentShaderID, 1, adapter, 0);
    
    glCompileShader(vertexShaderID);
    glCompileShader(fragmentShaderID);
    
    if(!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
    {
        return;
    }
    
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);
    if(!checkProgramStatus(programID))
    {
        return;
    }
    
    glUseProgram(programID);
}

void genObjects()
{
    const float TRI_Z = 0.5;
    GLfloat verts[] = {
        0.0f, 1.0f, TRI_Z,
        1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, TRI_Z,
        0.0f, 1.0f, 0.0f,
        1.0f, -1.0f, TRI_Z,
        0.0f, 0.0f, 1.0f
    };
    
    GLuint myBufferID;
    glGenBuffers(1, &myBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, myBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0 );
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (char*)(sizeof(float)*3));
}

void render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    int w, h;
    SDL_GetWindowSize(mWindow, &w, &h);
    
    glViewport(0, 0, w, h);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    SDL_GL_SwapWindow(mWindow);
}

void cleanup()
{
    SDL_DestroyWindow(mWindow);
    SDL_GL_DeleteContext(mContext);
    mWindow = NULL;
    mContext = NULL;
    SDL_Quit();
}

int main(int argc, char** argv)
{
    bool running = true;
    SDL_Event e;
    init();
    genObjects();
    genShaders();
    while(running)
    {
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT)
            {
                running = false;
            }
        }
        render();
    }
    cleanup();
    return 0;
}