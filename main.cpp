#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>

using namespace std;

SDL_Window* mWindow = NULL;
SDL_GLContext mContext = NULL;

const float X_DELTA = 0.1f;
const unsigned int NUM_OF_VERTICES_PER_TRI = 3;
const unsigned int NUM_FLOATS_PER_VERTICE = 6;
const unsigned int TRIANGLE_BYTE_SIZE = NUM_OF_VERTICES_PER_TRI * NUM_FLOATS_PER_VERTICE * sizeof(float);
unsigned int numTris = 0;

bool init()//init OpenGL
{
    bool success = true;
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        cout << "Error Failed to Load SDL!" << endl;
        success = false;
    }else{
        //set Version to 3.3
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        
        mWindow = SDL_CreateWindow("OpenGL Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        if(mWindow == NULL)
        {
            cout << "Failed to init Window!" << endl;
            success = false;
        }else{
            mContext = SDL_GL_CreateContext(mWindow);
            if(mContext == NULL)
            {
                cout << "Failed to init context!" << endl;
                success = false;
            }else{
                //init Glew
                glewExperimental = GL_TRUE;
                glewInit();
            }
        }
    }
    
}

string loadShaderCode(const char* fileName)
{
    ifstream input(fileName);
    if(!input.good())
    {
        cout << "FileFailed to load... " << fileName << endl;
    }
    return std::string(std::istreambuf_iterator<char>(input),
                       std::istreambuf_iterator<char>());
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
        
        cout << "Shader Error: " << buffer << endl;
        
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
        cout << "Program error: " << buffer << endl;
        
        delete buffer;
        
        return false;
    }
    return true;
}

void genShaders(){
   GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER); //position
   GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER); // color
   
   const char* adapter[1];
   string temp = loadShaderCode("./Shader/Vertex.vsh");
   adapter[0] = temp.c_str();
   glShaderSource(vertexShaderID, 1, adapter, 0 );
   
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
   
   if(!checkProgramStatus(programID));
   
   glUseProgram(programID);
}

void genObjects()
{
    const float RED_TRIANGLE_Z = 0.5;
    const float BLUE_TRIANGLE_Z = -0.5f;
    GLfloat verts[] =
    {
        0.0f, 1.0f, -1.0, //0
        1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, RED_TRIANGLE_Z,//1
        0.0f, 1.0f, 0.0f,
        1.0f, -1.0f, RED_TRIANGLE_Z,//2
        0.0f, 0.0f, 1.0f,
        
        -1.0f, 1.0f, BLUE_TRIANGLE_Z,//3
        0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, BLUE_TRIANGLE_Z, //4
        0.0f, 1.0f, 0.0f,
        0.0f, -1.0f, BLUE_TRIANGLE_Z, //5
        1.0f, 0.0f, 0.0f    
    };
    
    GLuint myBufferID; //Buffer Object
    glGenBuffers(1, &myBufferID);//generate the buffer
    glBindBuffer(GL_ARRAY_BUFFER, myBufferID);//bind the attributes to the buffer, specify this is an array
    glBufferData(GL_ARRAY_BUFFER, 10000, NULL, GL_STATIC_DRAW);//pass the vertex data to the buffer
    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, 0);//parse for position
    glEnableVertexAttribArray(1); // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (char*)(sizeof(float)*3));//parse for color
    
    GLushort indices[] = {0,1,2, 5,3,4};//specifies which row of vertices to use
    GLuint indexBufferID;
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
     
}

void genNewTriangle()
{
    const GLfloat THIS_TRI_X = -1 + numTris * X_DELTA;
    
    GLfloat thisTri[] = {
        THIS_TRI_X, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        
        THIS_TRI_X + X_DELTA, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        
        THIS_TRI_X, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f
    };
    glBufferSubData(GL_ARRAY_BUFFER, numTris * TRIANGLE_BYTE_SIZE, TRIANGLE_BYTE_SIZE, thisTri);
    numTris++;
}

void render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glEnable(GL_DEPTH_TEST);
    //render code here
    int x, y;
    SDL_GetWindowSize(mWindow, &x, &y);
    glViewport(0, 0, x, y);//specify how we handle the viewport.
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    genNewTriangle();
    
    glDrawArrays(GL_TRIANGLES, (numTris-1) * NUM_OF_VERTICES_PER_TRI, NUM_OF_VERTICES_PER_TRI);
    
    SDL_GL_SwapWindow(mWindow);
}

void cleanup()
{
    SDL_GL_DeleteContext(mContext);
    SDL_DestroyWindow(mWindow);
    mWindow = NULL;
    mContext = NULL;
    SDL_Quit;
}

int main(int argc, char** argv) {

    bool isRunning = true;
    SDL_Event event;
    
    init();
    genObjects();
    genShaders();
    while(isRunning)
    {
        //handle input
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                isRunning = false;
            }
        }
        //handle draw
        render();
    }
    
    cleanup();
    
    return 0;
}

