#pragma once
#include "shader.h" 
#include "camera.h" 

class WaterShader : public Shader {
    private:
        static const std::string VERTEX_FILE;
        static const std::string FRAGMENT_FILE;

        // GLuint location_modelMatrix;
        // GLuint location_viewMatrix;
        // GLuint location_projectionMatrix;

    // protected:
    //     void bindAttributes() {
    //         bindAttribute(0, "position");
    //     }

    //     virtual void getAllUniformLocations() override {
    //         location_projectionMatrix = glGetUniformLocation("projectionMatrix");
    //         location_viewMatrix = glGetUniformLocation("viewMatrix");
    //         location_modelMatrix = glGetUniformLocation("modelMatrix");
    //     }

    public:
        unsigned int ID;
        unsigned int vertexShaderID;
        unsigned int fragmentShaderID;

        WaterShader(const char* vertexFilePath, const char* fragmentFilePath) : Shader(vertexFilePath, fragmentFilePath) {
            // location_projectionMatrix = glad_glGetUniformLocation(this->ID, "projectionMatrix");
		    // location_viewMatrix = glad_glGetUniformLocation(this->ID, "viewMatrix");
		    // location_modelMatrix = glad_glGetUniformLocation(this->ID, "modelMatrix");
        }
        
        void connectTextureUnits() {
            setInt("reflectionTexture", 0);
            setInt("refractionTexture", 1);
            setInt("dudvMap", 2);
            setInt("normal_map", 3);
        }

        void loadMoveFactor(float factor) {
            this->setFloat("move_factor", factor);
        }

        // void loadMatrix(GLint location, glm::mat4 matrix) {
        //     glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
        // }

        void loadProjectionMatrix(const glm::mat4& projection) {
            // loadMatrix(location_projectionMatrix, projection);
            this->setMat4("u_projection", projection);
        }

        // glm::mat4 createViewMatrix(const Camera& camera) {
        //     glm::mat4 viewMatrix = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
        //     return viewMatrix;
        // }

        void loadViewMatrix(Camera& camera) {
            glm::mat4 viewMatrix = camera.GetViewMatrix();
            // loadMatrix(location_viewMatrix, viewMatrix);
            this->setMat4("u_view", viewMatrix);
            this->setVec3("camera_position", camera.Position);
        }

        void loadModelMatrix(const glm::mat4& modelMatrix) {
            // loadMatrix(location_modelMatrix, modelMatrix);
            this->setMat4("u_model", modelMatrix);
        }
};
