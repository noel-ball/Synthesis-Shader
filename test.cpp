#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib> // For rand() and RAND_MAX

const int NUM_PARTICLES = 1000;

struct Particle {
    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec3 color;
};

// Utility function to load shader code from file
std::string loadShaderSource(const char* filepath) {
    std::ifstream file(filepath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Compile shaders and link into a shader program
GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath) {
    std::string vertexSource = loadShaderSource(vertexPath);
    const char* vertexCode = vertexSource.c_str();
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexCode, nullptr);
    glCompileShader(vertexShader);

    std::string fragmentSource = loadShaderSource(fragmentPath);
    const char* fragmentCode = fragmentSource.c_str();
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentCode, nullptr);
    glCompileShader(fragmentShader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

// Update particle positions and wrap them within dynamic screen bounds
void updateParticles(std::vector<Particle>& particles, float deltaTime, float screenWidth, float screenHeight) {
    for (auto& particle : particles) {
        // Update position
        particle.position += particle.velocity * deltaTime;

        // Wrap particles around screen bounds
        if (particle.position.x > screenWidth / 2.0f) particle.position.x = -screenWidth / 2.0f;
        if (particle.position.x < -screenWidth / 2.0f) particle.position.x = screenWidth / 2.0f;
        if (particle.position.y > screenHeight / 2.0f) particle.position.y = -screenHeight / 2.0f;
        if (particle.position.y < -screenHeight / 2.0f) particle.position.y = screenHeight / 2.0f;
    }
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set initial window dimensions
    float screenWidth = 800.0f;
    float screenHeight = 600.0f;

    // Create an OpenGL window
    GLFWwindow* window = glfwCreateWindow((int)screenWidth, (int)screenHeight, "Random Moving Particles", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    GLuint shaderProgram = createShaderProgram("vertex_shader.glsl", "fragment_shader.glsl");

    // Initialize particles with random positions, velocities, and colors
    std::vector<Particle> particles(NUM_PARTICLES);
    for (auto& particle : particles) {
        particle.position = glm::vec2(
            (rand() / (float)RAND_MAX) * screenWidth - screenWidth / 2.0f, // Random x in full screen width
            (rand() / (float)RAND_MAX) * screenHeight - screenHeight / 2.0f // Random y in full screen height
        );
        particle.velocity = glm::vec2(
            ((rand() / (float)RAND_MAX) - 0.5f) * 100.0f, // Random x velocity between -50 and 50
            ((rand() / (float)RAND_MAX) - 0.5f) * 100.0f  // Random y velocity between -50 and 50
        );
        particle.color = glm::vec3(
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX
        );
    }

    // Set up buffers
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0); // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(sizeof(glm::vec2))); // Color
    glEnableVertexAttribArray(1);

    auto previousTime = std::chrono::high_resolution_clock::now();

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        // Get updated framebuffer size (in pixels)
        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        screenWidth = static_cast<float>(fbWidth);
        screenHeight = static_cast<float>(fbHeight);

        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - previousTime).count();
        previousTime = currentTime;

        // Update particle positions with dynamic screen size
        updateParticles(particles, deltaTime, screenWidth, screenHeight);

        // Update buffer data for particle positions
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(Particle), particles.data());

        // Clear the screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render particles
        glUseProgram(shaderProgram);
        glUniform2f(glGetUniformLocation(shaderProgram, "screenSize"), screenWidth, screenHeight);
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
