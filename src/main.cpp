#include <string>
#include <iostream>
#include <math.h>
#include <cstdlib>

#include "glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "shader.h"
#include "camera.h"
#include "perlin.h"
#include "main.h"
#include <limits>

#include "waterRenderer.hpp"
#include "waterFrameBuffer.cpp"


using namespace std;

GLint WIDTH = 1920, HEIGHT = 1080;

// Structs
struct plant {
    std::string type;
    float xpos;
    float ypos;
    float zpos;
    int xOffset;
    int yOffset;
    
    plant(std::string _type, float _xpos, float _ypos, float _zpos, int _xOffset, int _yOffset) {
        type = _type;
        xpos = _xpos;
        ypos = _ypos;
        zpos = _zpos;
        xOffset = _xOffset;
        yOffset = _yOffset;
    }
};

// Functions
int init();
void processInput(GLFWwindow *window, Shader &shader);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void render(std::vector<GLuint> &map_chunks, Shader &shader, glm::mat4 &view, glm::mat4 &model, glm::mat4 &projection, int &nIndices, std::vector<GLuint> &tree_chunks, std::vector<GLuint> &flower_chunks, glm::vec4 clip_plane);

std::vector<int> generate_indices();
std::vector<float> generate_noise_map(int xOffset, int yOffset);
std::vector<float> generate_vertices(const std::vector<float> &noise_map);
std::vector<float> generate_water_vertices(std::vector<float> &vertices);
std::vector<float> generate_normals(const std::vector<int> &indices, const std::vector<float> &vertices);
std::vector<float> generate_biome(const std::vector<float> &vertices, std::vector<plant> &plants, int xOffset, int yOffset);
void generate_map_chunk(GLuint &VAO, int xOffset, int yOffset, std::vector<plant> &plants);

void load_model(GLuint &VAO, std::string filename);
void setup_instancing(GLuint &VAO, std::vector<GLuint> &plant_chunk, std::string plant_type, std::vector<plant> &plants, std::string filename);

GLFWwindow *window;

// Global renderer
WaterRenderer *waterRenderer;

// Map params
float WATER_HEIGHT = 0.1;
int chunk_render_distance = 3;
const int xMapChunks = 10;
const int yMapChunks = 10;
int chunkWidth = 127;
int chunkHeight = 127;
int gridPosX = 0;
int gridPosY = 0;
float originX = (chunkWidth  * xMapChunks) / 2 - chunkWidth / 2;
float originY = (chunkHeight * yMapChunks) / 2 - chunkHeight / 2;

// Noise params
int octaves = 5;
float meshHeight = 32;  // Vertical scaling
float noiseScale = 64;  // Horizontal scaling
float persistence = 0.5;
float lacunarity = 2;
vector<int> p = get_permutation_vector();

float water_plane_height = 0.9 * WATER_HEIGHT * meshHeight;

// Model params
float MODEL_SCALE = 3;
float MODEL_BRIGHTNESS = 6;

// FPS
double lastTime = glfwGetTime();
int nbFrames = 0;

// Camera
Camera camera(glm::vec3(originX, 20.0f, originY));
bool firstMouse = true;
float lastX = WIDTH / 2;
float lastY = HEIGHT / 2;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float currentFrame;

vector<int> fogKeys;
vector<string> fogTypes;


// void generate_rectangle(GLuint &VAO) {
//     // Define vertices for a rectangle
//     float vertices[] = {
//         // Positions
//         -10, -10, 10, // Bottom Left
//         10, -10, 10,  // Bottom Right
//         10, 10, 10,   // Top Right
//         -10, 10, 10   // Top Left
//     };

//     GLuint VBO, EBO;

//     // Generate buffers and arrays
//     glGenBuffers(1, &VBO);
//     glGenBuffers(1, &EBO);
//     glGenVertexArrays(1, &VAO);

//     // Bind VAO
//     glBindVertexArray(VAO);

//     // Bind vertices to VBO
//     glBindBuffer(GL_ARRAY_BUFFER, VBO);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//     // Configure vertex position attribute
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//     glEnableVertexAttribArray(0);

//     // Unbind VBO and VAO
//     glBindBuffer(GL_ARRAY_BUFFER, 0);
//     glBindVertexArray(0);


// }

int main() {
    // Initalize variables
    glm::mat4 view;
    glm::mat4 model;
    glm::mat4 projection;
    std::vector<plant> plants;

    // Initialize GLFW and GLAD
    if (init() != 0)
        return -1;
    
    // Shader fogShader("../resources/shaders/objectShader.vert", "../resources/shaders/objectShader.frag");
    Shader fogShader("../resources/shaders/fogShader.vert", "../resources/shaders/fogShader.frag");
    WaterShader waterShader("../resources/shaders/waterShader.vert", "../resources/shaders/waterShader.frag");  // WaterShader has additional functionality on top of Shader class

    // Default to coloring to flat mode
    fogShader.use();
    fogShader.setBool("isFlat", false);
    
    // Lighting intensities and direction
    fogShader.setVec3("light.ambient", 0.2, 0.2, 0.2);
    fogShader.setVec3("light.diffuse", 0.3, 0.3, 0.3);
    fogShader.setVec3("light.specular", 1.0, 1.0, 1.0);
    fogShader.setVec3("light.direction", -1.0f, 1.0f, 1.0f);
    fogShader.setFloat("distance", max(chunkWidth, chunkHeight) * chunk_render_distance);
    fogKeys.push_back(GLFW_KEY_0);
    fogKeys.push_back(GLFW_KEY_1);
    fogKeys.push_back(GLFW_KEY_2);

    fogTypes.push_back("isFogLinear");
    fogTypes.push_back("isFogExponential");
    fogTypes.push_back("isFogExponentialSquared");
    
    std::vector<GLuint> map_chunks(xMapChunks * yMapChunks);

    // Set up water resources
    Loader loader = Loader();   
    waterRenderer = new WaterRenderer(loader, waterShader, glm::mat4(1.0));  
    WaterFrameBuffers* buffers = new WaterFrameBuffers();

    // std::vector<Water> waters;
    // waters.push_back(Water(originX, originY, 0.1 * meshHeight, chunkWidth/2, chunkHeight/2));

    // GLuint rectangleVAO;
    // generate_rectangle(rectangleVAO);
    
    for (int y = 0; y < yMapChunks; y++)
        for (int x = 0; x < xMapChunks; x++) {
            generate_map_chunk(map_chunks[x + y*xMapChunks], x, y, plants);
        }
    
    int nIndices = chunkWidth * chunkHeight * 6;

    // Generate water tiles here
    GLuint treeVAO, flowerVAO;
    std::vector<GLuint> tree_chunks(xMapChunks * yMapChunks);
    std::vector<GLuint> flower_chunks(xMapChunks * yMapChunks);
    
    setup_instancing(treeVAO, tree_chunks, "tree", plants, "../resources/obj/CommonTree_1.obj");
    setup_instancing(flowerVAO, flower_chunks, "flower", plants, "../resources/obj/Flowers.obj");
    
    while (!glfwWindowShouldClose(window)) {
        glEnable(GL_CLIP_DISTANCE0);  

        fogShader.use();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.2f, (float)chunkWidth * (chunk_render_distance - 1.2f));
        view = camera.GetViewMatrix();
        fogShader.setMat4("u_projection", projection);
        fogShader.setMat4("u_view", view);
        fogShader.setVec3("u_viewPos", camera.Position);
        
        // Render reflection
        buffers->bindReflectionFrameBuffer();
        render(map_chunks, fogShader, view, model, projection, nIndices, tree_chunks, flower_chunks, glm::vec4(0, 1, 0, -water_plane_height));
        buffers->unbindCurrentFrameBuffer();

        // Render refraction
        buffers->bindRefractionFrameBuffer();
        render(map_chunks, fogShader, view, model, projection, nIndices, tree_chunks, flower_chunks, glm::vec4(0, -1, 0, water_plane_height));
        buffers->unbindCurrentFrameBuffer();

        // Render onto the actual display's FBO
        glDisable(GL_CLIP_DISTANCE0); 
        render(map_chunks, fogShader, view, model, projection, nIndices, tree_chunks, flower_chunks, glm::vec4(0, -1, 0, water_plane_height));  // The last argument for this call is just a dummy value

        waterShader.use();
        // Render water tiles
        waterRenderer->loadProjectionMatrix(projection);  // Must update for every loop
        waterRenderer->render(camera);

        // Measure speed in ms per frame
        double currentTime = glfwGetTime();
        nbFrames++;
        // If last prinf() was more than 1 sec ago printf and reset timer
        if (currentTime - lastTime >= 1.0 ) {
            printf("%f ms/frame\n", 1000.0/double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;
        }
    
        // Use double buffer
        // Only swap old frame with new when it is completed
        glfwPollEvents();
        glfwSwapBuffers(window);

    }
    
    for (int i = 0; i < map_chunks.size(); i++) {
        glDeleteVertexArrays(1, &map_chunks[i]);
        glDeleteVertexArrays(1, &tree_chunks[i]);
        glDeleteVertexArrays(1, &flower_chunks[i]);
    }
    
    // TODO VBOs and EBOs aren't being deleted
//     glDeleteBuffers(3, VBO);
//     glDeleteBuffers(1, &EBO);

    glfwTerminate();
    
    return 0;
}

void setup_instancing(GLuint &VAO, std::vector<GLuint> &plant_chunk, std::string plant_type, std::vector<plant> &plants, std::string filename) {
    std::vector<std::vector<float>> chunkInstances;
    chunkInstances.resize(xMapChunks * yMapChunks);
    
    // Instancing prep
    for (int i = 0; i < plants.size(); i++) {
        float xPos = plants[i].xpos / MODEL_SCALE;
        float yPos = plants[i].ypos / MODEL_SCALE;
        float zPos = plants[i].zpos / MODEL_SCALE;
        int pos = plants[i].xOffset + plants[i].yOffset*xMapChunks;
        
        if (plants[i].type == plant_type) {
            chunkInstances[pos].push_back(xPos);
            chunkInstances[pos].push_back(yPos);
            chunkInstances[pos].push_back(zPos);
        }
    }
    
    GLuint instancesVBO[xMapChunks * yMapChunks];
    glGenBuffers(xMapChunks * yMapChunks, instancesVBO);
    
    for (int y = 0; y < yMapChunks; y++) {
        for (int x = 0; x < xMapChunks; x++) {
            int pos = x + y*xMapChunks;
            load_model(plant_chunk[pos], filename);
            
            glBindVertexArray(plant_chunk[pos]);
            glBindBuffer(GL_ARRAY_BUFFER, instancesVBO[pos]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * chunkInstances[pos].size(), &chunkInstances[pos][0], GL_STATIC_DRAW);
            
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            
            // Instanced array
            // Move to next vertex attrib on next instance of object
            glVertexAttribDivisor(3, 1);
        }
    }
}

void render(std::vector<GLuint> &map_chunks, Shader &shader, glm::mat4 &view, glm::mat4 &model, glm::mat4 &projection, int &nIndices, std::vector<GLuint> &tree_chunks, std::vector<GLuint> &flower_chunks, glm::vec4 clip_plane) {
    // Per-frame time logic
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    
    processInput(window, shader);
    
    glClearColor(0.53, 0.81, 0.92, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Measures number of map chunks away from origin map chunk the camera is
    gridPosX = (int)(camera.Position.x - originX) / chunkWidth + xMapChunks / 2;
    gridPosY = (int)(camera.Position.z - originY) / chunkHeight + yMapChunks / 2;
    
    shader.setVec4("plane", clip_plane);
    // Render map chunks
    for (int y = 0; y < yMapChunks; y++)
        for (int x = 0; x < xMapChunks; x++) {
            // Only render chunk if it's within render distance
            if (std::abs(gridPosX - x) <= chunk_render_distance && (y - gridPosY) <= chunk_render_distance) {
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(-chunkWidth / 2.0 + (chunkWidth - 1) * x, 0.0, -chunkHeight / 2.0 + (chunkHeight - 1) * y));
                shader.setMat4("u_model", model);

                // Terrain chunk
                glBindVertexArray(map_chunks[x + y*xMapChunks]);
                glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
                
                // Plant chunks
                // model = glm::mat4(1.0f);
                // model = glm::translate(model, glm::vec3(-chunkWidth / 2.0 + (chunkWidth - 1) * x, 0.0, -chunkHeight / 2.0 + (chunkHeight - 1) * y));
                // model = glm::scale(model, glm::vec3(MODEL_SCALE));
                // shader.setMat4("u_model", model);

                // glEnable(GL_CULL_FACE);
                // glBindVertexArray(tree_chunks[x + y*xMapChunks]);
                // glDrawArraysInstanced(GL_TRIANGLES, 0, 10192, 8);

                // glBindVertexArray(flower_chunks[x + y*xMapChunks]);
                // glDrawArraysInstanced(GL_TRIANGLES, 0, 1300, 16);
                // glDisable(GL_CULL_FACE);
            }
        }
}

void load_model(GLuint &VAO, std::string filename) {
    std::vector<float> vertices;
    std::vector<int> indices;
    
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;

    tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str(), "../resources/obj/");

    if (!warn.empty()) {
        std::cout << warn << std::endl;
    } else if (!err.empty()) {
        std::cerr << err << std::endl;
    }
    
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                vertices.push_back(attrib.vertices[3*idx.vertex_index+0]);
                vertices.push_back(attrib.vertices[3*idx.vertex_index+1]);
                vertices.push_back(attrib.vertices[3*idx.vertex_index+2]);
                vertices.push_back(attrib.normals[3*idx.normal_index+0]);
                vertices.push_back(attrib.normals[3*idx.normal_index+1]);
                vertices.push_back(attrib.normals[3*idx.normal_index+2]);
                vertices.push_back(materials[shapes[s].mesh.material_ids[f]].diffuse[0] * MODEL_BRIGHTNESS);
                vertices.push_back(materials[shapes[s].mesh.material_ids[f]].diffuse[1] * MODEL_BRIGHTNESS);
                vertices.push_back(materials[shapes[s].mesh.material_ids[f]].diffuse[2] * MODEL_BRIGHTNESS);
            }
            index_offset += fv;
        }
    }
    
    GLuint VBO, EBO;
    
    // Create buffers and arrays
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    
    // Bind vertices to VBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    
    // Configure vertex position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Configure vertex normals attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Configure vertex color attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void generate_map_chunk(GLuint &VAO, int xOffset, int yOffset, std::vector<plant> &plants) {
    std::vector<int> indices;
    std::vector<float> noise_map;
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> colors;
    std::vector<float> water_vertices;
    
    // Generate map
    indices = generate_indices();
    noise_map = generate_noise_map(xOffset, yOffset);
    vertices = generate_vertices(noise_map);
    water_vertices = generate_water_vertices(vertices);
    normals = generate_normals(indices, vertices);
    colors = generate_biome(vertices, plants, xOffset, yOffset);
    
    int idx = xOffset + yOffset * xMapChunks;
    waterRenderer->setUpVAO(water_vertices, idx, -chunkWidth / 2.0 + (chunkWidth - 1) * xOffset,
                             water_plane_height , -chunkWidth / 2.0 + (chunkWidth - 1) * yOffset);


    GLuint VBO[3], EBO;
    
    // Create buffers and arrays
    glGenBuffers(3, VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    
    // Bind vertices to VBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    
    // Create element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);
    
    // Configure vertex position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Bind vertices to VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_STATIC_DRAW);
    
    // Configure vertex normals attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    
    // Bind vertices to VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), &colors[0], GL_STATIC_DRAW);
    
    // Configure vertex colors attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
}

glm::vec3 get_color(int r, int g, int b) {
    return glm::vec3(r/255.0, g/255.0, b/255.0);
}

std::vector<float> generate_noise_map(int offsetX, int offsetY) {
    std::vector<float> noiseValues;
    std::vector<float> normalizedNoiseValues;
    
    float amp  = 1;
    float freq = 1;
    float maxPossibleHeight = 0;
    
    for (int i = 0; i < octaves; i++) {
        maxPossibleHeight += amp;
        amp *= persistence;
    }
    
    for (int y = 0; y < chunkHeight; y++) {
        for (int x = 0; x < chunkWidth; x++) {
            amp  = 1;
            freq = 1;
            float noiseHeight = 0;
            for (int i = 0; i < octaves; i++) {
                float xSample = (x + offsetX * (chunkWidth-1))  / noiseScale * freq;
                float ySample = (y + offsetY * (chunkHeight-1)) / noiseScale * freq;
                
                float perlinValue = perlin_noise(xSample, ySample, p);
                noiseHeight += perlinValue * amp;
                
                // Lacunarity  --> Increase in frequency of octaves
                // Persistence --> Decrease in amplitude of octaves
                amp  *= persistence;
                freq *= lacunarity;
            }
            
            noiseValues.push_back(noiseHeight);
        }
    }
    
    for (int y = 0; y < chunkHeight; y++) {
        for (int x = 0; x < chunkWidth; x++) {
            // Inverse lerp and scale values to range from 0 to 1
            normalizedNoiseValues.push_back((noiseValues[x + y*chunkWidth] + 1) / maxPossibleHeight);
        }
    }

    return normalizedNoiseValues;
}

struct terrainColor {
    terrainColor(float _height, glm::vec3 _color) {
        height = _height;
        color = _color;
    };
    float height;
    glm::vec3 color;
};

std::vector<float> generate_biome(const std::vector<float> &vertices, std::vector<plant> &plants, int xOffset, int yOffset) {
    std::vector<float> colors;
    std::vector<terrainColor> biomeColors;
    glm::vec3 color = get_color(255, 255, 255);
    
    // NOTE: Terrain color height is a value between 0 and 1
    biomeColors.push_back(terrainColor(WATER_HEIGHT * 0.5, get_color(60,  95, 190)));   // Deep water
    biomeColors.push_back(terrainColor(WATER_HEIGHT,        get_color(60, 100, 190)));  // Shallow water
    biomeColors.push_back(terrainColor(0.25, get_color(210, 215, 130)));                // Sand
    biomeColors.push_back(terrainColor(0.40, get_color( 95, 165,  30)));                // Grass 1
    biomeColors.push_back(terrainColor(0.50, get_color( 65, 115,  20)));                // Grass 2              // Grass 2
    biomeColors.push_back(terrainColor(0.65, get_color( 90,  65,  60)));                // Rock 1
    biomeColors.push_back(terrainColor(0.80, get_color( 75,  60,  55)));                // Rock 2
    biomeColors.push_back(terrainColor(1.2, get_color(220, 220,  220)));                // Snow 1
    biomeColors.push_back(terrainColor(1.45, get_color(255, 255, 255)));                // Snow 2
    
    std::string plantType;
    
    // Determine which color to assign each vertex by its y-coord
    // Iterate through vertex y values
    for (int i = 1; i < vertices.size(); i += 3) {  // x, y, z, so look at every y value
        int k = 0;
        for (int j = 0; j < biomeColors.size(); j++) {
            // NOTE: The max height of a vertex is "meshHeight"
            if (vertices[i] <= biomeColors[j].height * meshHeight) {
                color = biomeColors[j].color;
                if (j == 3 || j == 4) {
                    if (rand() % 1000 < 5) {
                        if (rand() % 100 < 70) {
                            plantType = "flower";
                        } else {
                            plantType = "tree";
                        }
                        plants.push_back(plant{plantType, vertices[i-1], vertices[i], vertices[i+1], xOffset, yOffset});
                    }
                }
                k = j;
                break;
            }
        }

        if (k > 0) {
            glm::vec3 prevColor = biomeColors[k - 1].color;
            glm::vec3 currColor = biomeColors[k].color;

            float prevHeight = biomeColors[k - 1].height * meshHeight;
            float currHeight = biomeColors[k].height * meshHeight;

            float t = (vertices[i] - prevHeight) / (currHeight - prevHeight);

            float r = ((1 - t) * prevColor.r + t * currColor.r) * 255;
            float g = ((1 - t) * prevColor.g + t * currColor.g) * 255;
            float b = ((1 - t) * prevColor.b + t * currColor.b) * 255;

            color = get_color(r, g, b);
        }
        colors.push_back(color.r);
        colors.push_back(color.g);
        colors.push_back(color.b);
    }
    return colors;
}

std::vector<float> generate_normals(const std::vector<int> &indices, const std::vector<float> &vertices) {
    int pos;
    glm::vec3 normal;
    std::vector<float> normals;
    std::vector<glm::vec3> verts;
    
    // Get the vertices of each triangle in mesh
    // For each group of indices
    for (int i = 0; i < indices.size(); i += 3) {
        
        // Get the vertices (point) for each index
        for (int j = 0; j < 3; j++) {
            pos = indices[i+j]*3;
            verts.push_back(glm::vec3(vertices[pos], vertices[pos+1], vertices[pos+2]));
        }
        
        // Get vectors of two edges of triangle
        glm::vec3 U = verts[i+1] - verts[i];
        glm::vec3 V = verts[i+2] - verts[i];
        
        // Calculate normal
        normal = glm::normalize(-glm::cross(U, V));
        normals.push_back(normal.x);
        normals.push_back(normal.y);
        normals.push_back(normal.z);
    }
    
    return normals;
}

std::vector<float> generate_vertices(const std::vector<float> &noise_map) {
    std::vector<float> v;
    
    for (int y = 0; y < chunkHeight + 1; y++)
        for (int x = 0; x < chunkWidth; x++) {
            v.push_back(x);
            // Apply cubic easing to the noise
            float easedNoise = std::pow(noise_map[x + y*chunkWidth] * 1.1, 3);
            // Scale noise to match meshHeight
            // Pervent vertex height from being below WATER_HEIGHT
            v.push_back(easedNoise * meshHeight);
            // v.push_back(std::fmax(easedNoise * meshHeight, WATER_HEIGHT * 0.5 * meshHeight));
            v.push_back(y);
        }
    
    return v;
}

std::vector<int> generate_indices() {
    std::vector<int> indices;
    
    for (int y = 0; y < chunkHeight; y++)
        for (int x = 0; x < chunkWidth; x++) {
            int pos = x + y*chunkWidth;
            
            if (x == chunkWidth - 1 || y == chunkHeight - 1) {
                // Don't create indices for right or top edge
                continue;
            } else {
                // Top left triangle of square
                indices.push_back(pos + chunkWidth);
                indices.push_back(pos);
                indices.push_back(pos + chunkWidth + 1);
                // Bottom right triangle of square
                indices.push_back(pos + 1);
                indices.push_back(pos + 1 + chunkWidth);
                indices.push_back(pos);
            }
        }

    return indices;
}

std::vector<float> generate_water_vertices(std::vector<float> &vertices) {
    float max_x = 0;
    float min_x = 127;
    float max_z = 0;
    float min_z = 127;

    // Bug: currently just getting the bounding water box for a chunk, rather than per island within a chunk.
    for (int i = 1; i < vertices.size(); i += 3) {
        if (vertices[i] <= WATER_HEIGHT * meshHeight) {
            // max_x = std::max(vertices[i-1], max_x);
            // min_x = std::min(vertices[i-1], min_x);
            // max_z = std::max(vertices[i+1], max_z);
            // min_z = std::min(vertices[i+1], min_z); 
            vertices[i] = vertices[i] * 0.7; // To prevent z-fighting with the water plane
        } else {
            vertices[i] += 0.05; // To prevent z-fighting with the water plane
        }
    }

    // std::cout << min_x << " " << max_x << " " << min_z << " " << max_z << " ";
    
    std::vector<float> water_vertices = {min_x, min_z, min_x, max_z,
                                            max_x, min_z, max_x, min_z,
                                            min_x, max_z, max_x, max_z};
    
    return water_vertices;
}

// Initialize GLFW and GLAD
int init() {
    glfwInit();
    
    // Set OpenGL window to version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // macOS compatibility
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    window = glfwCreateWindow(WIDTH, HEIGHT, "Terrain Generator", nullptr, nullptr);
    
    glfwWindowHint(GLFW_DEPTH_BITS, 64);
    // Account for macOS retina resolution
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, screenWidth, screenHeight);

    // Enable z-buffer
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    
    // Enable mouse input
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    return 0;
}

void processInput(GLFWwindow *window, Shader &shader) {
    if ((glfwGetKey(window, GLFW_KEY_LEFT_SUPER) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
    
    // Enable wireframe mode
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    // Enable flat mode
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        shader.use();
        shader.setBool("isFlat", true);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
      if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        shader.use();
        shader.setBool("isFlat", false);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

    for (int i = 0; i < fogKeys.size(); i++) {
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && glfwGetKey(window, fogKeys[i]) == GLFW_PRESS) {
            shader.use();
            for (int j = 0; j < fogTypes.size(); j++) {
                if (i == j) {
                    shader.setBool(fogTypes[j], true);
                } else {
                    shader.setBool(fogTypes[j], false);
                }
            }
        }
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        shader.use();
        for (int i = 0; i < fogTypes.size(); i++) {
            shader.setBool(fogTypes[i], false);
        }
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    // Prevent camera jumping when mouse first enters screen
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    // yoffset is reversed since y-coords go from bottom to top
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}
