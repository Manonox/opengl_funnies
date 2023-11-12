#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <vector>
#include <cmath>
#include <SFML/Graphics.hpp>

#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1
// #include <SFML/OpenGL.hpp>
#include "glad/glad.h"


float currentTime;
void load();
void draw();

int main() {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 4;
    settings.minorVersion = 6;

    auto window = sf::RenderWindow{ { 512u, 512u }, "OpenGL", sf::Style::Default, settings };
    window.setFramerateLimit(144);
    window.setActive(true);
    gladLoadGL();

    bool loaded = false;
    auto startTime = std::chrono::high_resolution_clock::now();
    while (window.isOpen()) {
        currentTime = std::chrono::duration_cast<std::chrono::microseconds>(startTime - std::chrono::high_resolution_clock::now()).count() / 1000000.0;

        for (auto event = sf::Event{}; window.pollEvent(event);) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                
                case sf::Event::Resized:
                    glViewport(0, 0, event.size.width, event.size.height);
                    break;
            
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::R) {
                        printf("Reloading...\n");
                        load();
                    }
                    break;
            }
        }

        if (!loaded) {
            load();
            loaded = true;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        draw();

        window.display();
    }
}


typedef struct ShaderCompilationResult_ {
    bool success = true;
    GLuint shader;
    std::string error;
} ShaderCompilationResult;

ShaderCompilationResult compileShader(GLenum type, char *source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    int success;
    char infoLog[513]; infoLog[513] = '\0';
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        ShaderCompilationResult result;
        result.success = false;
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        result.error = std::string(infoLog);
        return result;
    }

    ShaderCompilationResult result;
    result.shader = shader;
    return result;
}


GLuint compileShaderFile(GLenum type, std::string path) {
    std::ifstream file(path);
    file.seekg(0, std::ios_base::end);
    const unsigned int length = file.tellg();
    file.seekg(0);
    char *content = new char[length + 1];
    file.read(content, length);
    content[length] = '\0';

    auto result = compileShader(type, content);
    delete content;

    if (!result.success) {
        printf("Shader (%s) compile error.\n%s\n", path.c_str(), result.error.c_str());
        exit(0);
    }

    return result.shader;
}

GLuint VAO, VBO, EBO;
GLuint vertexShader, fragmentShader;
GLuint shaderProgram;

void load() {
    // Compile shaders
    vertexShader = compileShaderFile(GL_VERTEX_SHADER, std::string("resources/vertex.glsl"));
    fragmentShader = compileShaderFile(GL_FRAGMENT_SHADER, std::string("resources/fragment.glsl"));

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char error[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, error);
        std::cout << "Program linking error: " << error << std::endl;
        return;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
}

void draw() {
    glUseProgram(shaderProgram);
    // uniform float time;
    glUniform1f(0, currentTime);

    
    std::vector<GLfloat> vertices;
    vertices.push_back(-0.5);
    vertices.push_back(-0.5);
    vertices.push_back(0.0);
    
    vertices.push_back(0.3);
    vertices.push_back(0.3);
    vertices.push_back(1.0);
    vertices.push_back(1.0);

    for (int i = 0; i < 5; i++) {
        auto f = i / 5.0;
        auto rad = f * 6.283 + currentTime;
        auto x = cos(rad) * 0.2 - 0.5;
        auto y = -sin(rad) * 0.2 - 0.5;
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0);
        
        vertices.push_back(cosf(f * 2.0) * 0.5 + 0.5);
        vertices.push_back(f);
        vertices.push_back(0.0);
        vertices.push_back(1.0);
    }

    std::vector<GLuint> indices = { 0, 1, 2, 3, 4, 5, 1 };

    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLE_FAN, indices.size(), GL_UNSIGNED_INT, 0);
}
 