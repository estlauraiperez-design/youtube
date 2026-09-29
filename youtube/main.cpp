#include <iostream>
#include <vector>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Vertex Shader
const char* vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"
"out vec2 TexCoord;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"    TexCoord = aTexCoord;\n"
"}\0";

// Fragment Shader
const char* fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D texture1;\n"
"void main()\n"
"{\n"
"    FragColor = texture(texture1, TexCoord);\n"
"}\n\0";

// Función para generar una textura procedural de madera en memoria
GLuint createProceduralWoodTexture() {
    const int width = 256;
    const int height = 256;
    std::vector<unsigned char> data(width * height * 3);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float u = (float)x / width;
            float v = (float)y / height;

            // Anillos de madera usando sin/cos
            float ring = sin(sqrt(u * u + v * v) * 40.0f);
            float grain = (ring + 1.0f) * 0.5f;

            // Colores base de madera (Marrón claro y oscuro)
            unsigned char r = (unsigned char)(139 * (0.6f + 0.4f * grain));
            unsigned char g = (unsigned char)(69 * (0.6f + 0.4f * grain));
            unsigned char b = (unsigned char)(19 * (0.6f + 0.4f * grain));

            int index = (y * width + x) * 3;
            data[index] = r;
            data[index + 1] = g;
            data[index + 2] = b;
        }
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Piramide 3D con Textura de Madera", NULL, NULL);
    if (window == NULL) {
        std::cout << "Error al crear la ventana de GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, 800, 800);
    glEnable(GL_DEPTH_TEST);

    // Vértices de una Pirámide 3D (Posición X,Y,Z y Coordenadas de textura U,V)
    float pyramidVertices[] = {
        // Cara frontal
         0.0f,  0.8f,  0.0f,   0.5f, 1.0f,
        -0.6f, -0.5f,  0.6f,   0.0f, 0.0f,
         0.6f, -0.5f,  0.6f,   1.0f, 0.0f,

         // Cara derecha
          0.0f,  0.8f,  0.0f,   0.5f, 1.0f,
          0.6f, -0.5f,  0.6f,   0.0f, 0.0f,
          0.6f, -0.5f, -0.6f,   1.0f, 0.0f,

          // Cara trasera
           0.0f,  0.8f,  0.0f,   0.5f, 1.0f,
           0.6f, -0.5f, -0.6f,   0.0f, 0.0f,
          -0.6f, -0.5f, -0.6f,   1.0f, 0.0f,

          // Cara izquierda
           0.0f,  0.8f,  0.0f,   0.5f, 1.0f,
          -0.6f, -0.5f, -0.6f,   0.0f, 0.0f,
          -0.6f, -0.5f,  0.6f,   1.0f, 0.0f,

          // Base
          -0.6f, -0.5f,  0.6f,   0.0f, 1.0f,
           0.6f, -0.5f,  0.6f,   1.0f, 1.0f,
          -0.6f, -0.5f, -0.6f,   0.0f, 0.0f,

           0.6f, -0.5f,  0.6f,   1.0f, 1.0f,
           0.6f, -0.5f, -0.6f,   1.0f, 0.0f,
          -0.6f, -0.5f, -0.6f,   0.0f, 0.0f
    };

    // Shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // VBO y VAO
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Generar textura de madera interna
    GLuint woodTexture = createProceduralWoodTexture();

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Proyección y Cámara
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 800.0f, 0.1f, 100.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.2f, -3.5f));

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Matriz de Modelo (Rotación continua en 3D)
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime() * 0.8f, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Dibujar pirámide
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 18);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &woodTexture);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}