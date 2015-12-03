#include "assets.hpp"
#include "shader.hpp"
#include <glm/glm.hpp>

#include "dbg.h"

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

#include <stdlib.h>
#include <string.h>

void _LoadShaderCode(GLuint shader_id, const char* shader_path);


Shader Shader::FromAssets(std::string vertex_shader_asset,
                          std::string fragment_shader_asset){
    return Shader::FromAssets(vertex_shader_asset.c_str(),
                              fragment_shader_asset.c_str());
}

Shader Shader::FromAssets(const char * vertex_shader_asset,
                          const char * fragment_shader_asset){
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    _LoadShaderCode(vertex_shader_id, GetPathForAsset(vertex_shader_asset).c_str());
    _LoadShaderCode(fragment_shader_id, GetPathForAsset(fragment_shader_asset).c_str());

    GLuint pid = glCreateProgram();
    glAttachShader(pid, vertex_shader_id);
    glAttachShader(pid, fragment_shader_id);
    glLinkProgram(pid);

    // Check the program
    GLint result;
    GLint log_length;
    glGetProgramiv(pid, GL_LINK_STATUS, &result);
    glGetProgramiv(pid, GL_INFO_LOG_LENGTH, &log_length);
    if ( log_length > 0 ){
        std::vector<char> error_msgs(log_length+1);
        glGetProgramInfoLog(pid, log_length, NULL, &error_msgs[0]);
        log_warn("%s", &error_msgs[0]);
    }

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    return Shader(pid);
}




void Shader::BindUniform(const char* key, float value){
    auto location = glGetUniformLocation(this->_program_id, key);
    glUniform1f(location, value);
}

void Shader::BindUniform(const char* key, glm::vec3 value){
    auto location = glGetUniformLocation(this->_program_id, key);
    glUniform3f(location, value.x, value.y, value.z);
}

void Shader::BindUniform(const char* key, glm::mat4 value){
    auto location = glGetUniformLocation(this->_program_id, key);
    glUniformMatrix4fv(location, 1, GL_FALSE, &(value[0][0]));
}

void _LoadShaderCode(GLuint shader_id, const char* shader_code_path){
    ifstream shader_code_stream(shader_code_path, ios::in);
    string shader_code;
    if(shader_code_stream.is_open()){
        string line = "";
        while(getline(shader_code_stream, line)){
            shader_code += "\n" + line;}

        shader_code_stream.close();
    }
    else{
        log_err("Couldn't open %s.", shader_code_path);
    }
    // compile
    char const * shader_src_ptr = shader_code.c_str();
    glShaderSource(shader_id, 1, &shader_src_ptr , NULL);
    glCompileShader(shader_id);

    // check loaded shader
    GLint result = GL_FALSE;
    GLint log_length = -1;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
    if ( log_length > 0 ){
        std::vector<char> error_msg(log_length + 1);
        glGetShaderInfoLog(shader_id, log_length, NULL, &error_msg[0]);
        log_warn("%s", &error_msg[0]);
    }
}






// Shader::Shader(const char * vertex_file_path, const char * fragment_file_path){

//   // Create the shaders
//   GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
//   GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

//   // Read the Vertex Shader code from the file
//   std::string VertexShaderCode;
//   std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
//   if(VertexShaderStream.is_open()){
//     std::string Line = "";
//     while(getline(VertexShaderStream, Line))
//       VertexShaderCode += "\n" + Line;
//     VertexShaderStream.close();
//   }else{
//     log_err("Couldn't open %s.", vertex_file_path);
//     getchar();
//   }

//   // Read the Fragment Shader code from the file
//   std::string FragmentShaderCode;
//   std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
//   if(FragmentShaderStream.is_open()){
//     std::string Line = "";
//     while(getline(FragmentShaderStream, Line))
//       FragmentShaderCode += "\n" + Line;
//     FragmentShaderStream.close();
//   }

//   GLint Result = GL_FALSE;
//   int InfoLogLength;

//   // Compile Vertex Shader
//   // printf("Compiling shader : %s\n", vertex_file_path);
//   char const * shader_src_ptr = VertexShaderCode.c_str();
//   glShaderSource(VertexShaderID, 1, &shader_src_ptr , NULL);
//   glCompileShader(VertexShaderID);

//   // Check Vertex Shader
//   glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
//   glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//   if ( InfoLogLength > 0 ){
//     std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
//     glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
//     log_warn("%s", &VertexShaderErrorMessage[0]);
//   }

//   // Compile Fragment Shader
//   char const * FragmentSourcePointer = FragmentShaderCode.c_str();
//   glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
//   glCompileShader(FragmentShaderID);

//   // Check Fragment Shader
//   glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
//   glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//   if ( InfoLogLength > 0 ){
//     std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
//     glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
//     log_info("%s", &FragmentShaderErrorMessage[0]);
//   }

//   // Link the program
//   GLuint ProgramID = glCreateProgram();
//   glAttachShader(ProgramID, VertexShaderID);
//   glAttachShader(ProgramID, FragmentShaderID);
//   glLinkProgram(ProgramID);

//   // Check the program
//   glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
//   glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//   if ( InfoLogLength > 0 ){
//     std::vector<char> ProgramErrorMessage(InfoLogLength+1);
//     glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
//     log_warn("%s", &ProgramErrorMessage[0]);
//   }

//   glDeleteShader(VertexShaderID);
//   glDeleteShader(FragmentShaderID);

//   this->_program_id = ProgramID;
// }
