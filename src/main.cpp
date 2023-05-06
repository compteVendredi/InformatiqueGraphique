#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>
#include <courbeBezier.h>
#include <surfaceBezier.h>

#include <iostream>
#include <fenetre.h>
#include <define.h>


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader shader_3d("shader/3d.vs", "shader/3d.fs");
    Shader shader_3d_simple("shader/3d_simple.vs", "shader/3d_simple.fs");
    Shader shader_3d_normale("shader/3d.vs", "shader/3d_normale.fs");
    Shader shader_3d_micro_facette("shader/3d.vs", "shader/3d_microfacette.fs");
    Shader shader_3d_micro_facette_texture("shader/3d_texture.vs", "shader/3d_microfacette_texture.fs");
    Shader shader_3d_micro_facette_texture_lumiere("shader/3d_texture.vs", "shader/3d_microfacette_texture_3_lumiere.fs");


    
    std::vector<glm::vec3> pointsControles = {
        glm::vec3(-0.25f, 0.0f, 0.0f),
        glm::vec3(-0.25f, 0.5f, 0.0f),
        glm::vec3(0.25f, -0.5f, 0.0f),
        glm::vec3(0.5f, 0.0f, 0.0f)
    };
    CourbeBezier curve(pointsControles, 0.001f);
    unsigned int VBO[2], VAO[2];
    curve.charger(VBO, VAO);


    std::vector<std::vector<glm::vec3>> ctrlpoints = {
      {
        glm::vec3(-0.25f, 0.0f, 0.0f),
        glm::vec3(-0.25f, 0.5f, 0.0f),
        glm::vec3(0.25f, -0.5f, 0.0f),
        glm::vec3(0.5f, 0.0f, 0.0f)},
      {
        glm::vec3(-0.25f, 0.25f, 0.2f),
        glm::vec3(-0.25f, 0.75f, 0.2f),
        glm::vec3(0.25f, -0.25f, 0.2f),
        glm::vec3(0.5f, 0.25f, 0.2f)},
      {
        glm::vec3(-0.25f, 0.25f, 0.4f),
        glm::vec3(-0.25f, 0.75f, 0.4f),
        glm::vec3(0.25f, -0.25f, 0.4f),
        glm::vec3(0.5f, 0.25f, 0.4f)},
      {
        glm::vec3(-0.25f, 0.0f, 0.6f),
        glm::vec3(-0.25f, 0.5f, 0.6f),
        glm::vec3(0.25f, -0.5f, 0.6f),
        glm::vec3(0.5f, 0.0f, 0.6f)}
    };


    glm::vec3 positions[] = {
            glm::vec3(2.0f, 2.0f, -2.0f),//courbe
            glm::vec3(0.0f,  0.0f, -2.0f),//surface
            glm::vec3(-1.0f,  0.0f, 0.0f),//surface normale
            glm::vec3(1.0f,  0.0f, 0.0f),//surface microfacette
            glm::vec3(1.0f,  -1.0f, -1.0f),//surface microfacette texture
            glm::vec3(-1.0f,  -1.0f, -1.0f),//surface microfacette texture 3 lumières
    };

    SurfaceBezier surface(ctrlpoints, 0.01f, 0.01f);
    unsigned int VBO_surface[2], VAO_surface[2], EBO[2];
    surface.charger(VBO_surface, VAO_surface, EBO);

    // lighting
    glm::vec3 lightPos = positions[0];
    std::vector<glm::vec3> multipleLightPos;
    multipleLightPos.push_back(positions[0]);
    multipleLightPos.push_back(positions[1]);
    multipleLightPos.push_back(positions[2]);


    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int tex_width, tex_height, nrChannels;
    unsigned char *tex_data = stbi_load("res/texture_microfacette.jpg", &tex_width, &tex_height, &nrChannels, 0);
    if (tex_data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        //Affichage


        shader_3d.use();
        shader_3d.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        shader_3d.setVec3("lightPos", lightPos);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shader_3d.setMat4("projection", projection);

        glm::mat4 view = camera.GetViewMatrix();
        shader_3d.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, positions[1]);
        shader_3d.setMat4("model", model);
        shader_3d.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
        surface.afficher();


        shader_3d_simple.use();
        shader_3d_simple.setMat4("projection", projection);
        shader_3d_simple.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, positions[0]);
        shader_3d_simple.setMat4("model", model);
        shader_3d_simple.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
        curve.afficherCourbe();
        shader_3d_simple.setVec3("objectColor", 1.0f, 0.0f, 0.0f);
        curve.afficherPolygoneControle();
        model = glm::mat4(1.0f);
        model = glm::translate(model, positions[1]);
        shader_3d_simple.setMat4("model", model);
        shader_3d_simple.setVec3("objectColor", 1.0f, 0.0f, 0.0f);
        surface.afficherPolygoneControle();

        model = glm::mat4(1.0f);
        model = glm::translate(model, positions[2]);
        shader_3d_simple.setMat4("model", model);
        shader_3d_simple.setVec3("objectColor", 1.0f, 0.0f, 0.0f);
        surface.afficherPolygoneControle();


        model = glm::mat4(1.0f);
        model = glm::translate(model, positions[3]);
        shader_3d_simple.setMat4("model", model);
        shader_3d_simple.setVec3("objectColor", 1.0f, 0.0f, 0.0f);
        surface.afficherPolygoneControle();


        model = glm::mat4(1.0f);
        model = glm::translate(model, positions[4]);
        shader_3d_simple.setMat4("model", model);
        shader_3d_simple.setVec3("objectColor", 1.0f, 0.0f, 0.0f);
        surface.afficherPolygoneControle();

        model = glm::mat4(1.0f);
        model = glm::translate(model, positions[5]);
        shader_3d_simple.setMat4("model", model);
        shader_3d_simple.setVec3("objectColor", 1.0f, 0.0f, 0.0f);
        surface.afficherPolygoneControle();



        shader_3d_normale.use();
        shader_3d_normale.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        shader_3d_normale.setVec3("lightPos", lightPos);  
        shader_3d_normale.setMat4("projection", projection);     
        shader_3d_normale.setMat4("view", view); 
        shader_3d_normale.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, positions[2]);
        shader_3d_normale.setMat4("model", model);
        surface.afficher();






        shader_3d_micro_facette.use();
        shader_3d_micro_facette.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        shader_3d_micro_facette.setVec3("lightPos", lightPos);  
        shader_3d_micro_facette.setMat4("projection", projection);     
        shader_3d_micro_facette.setMat4("view", view); 
        shader_3d_micro_facette.setVec3("objectColor", 0.0f, 1.0f, 0.0f);
        shader_3d_micro_facette.setVec3("viewPos", camera.Position);
        shader_3d_micro_facette.setFloat("rougness", 0.5f); 
        shader_3d_micro_facette.setFloat("metallic", 0.0f); 
        model = glm::mat4(1.0f);
        model = glm::translate(model, positions[3]);
        shader_3d_micro_facette.setMat4("model", model);
        surface.afficher();


        shader_3d_micro_facette_texture.use();
        shader_3d_micro_facette_texture.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        shader_3d_micro_facette_texture.setVec3("lightPos", lightPos);  
        shader_3d_micro_facette_texture.setMat4("projection", projection);     
        shader_3d_micro_facette_texture.setMat4("view", view); 
        shader_3d_micro_facette_texture.setVec3("objectColor", 0.0f, 1.0f, 0.0f);
        shader_3d_micro_facette_texture.setVec3("viewPos", camera.Position);
        model = glm::mat4(1.0f);
        model = glm::translate(model, positions[4]);
        shader_3d_micro_facette_texture.setMat4("model", model);
        surface.afficher();


        shader_3d_micro_facette_texture_lumiere.use();
        shader_3d_micro_facette_texture_lumiere.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        shader_3d_micro_facette_texture_lumiere.setVec3("lightPos[0]", multipleLightPos[0]);  
        shader_3d_micro_facette_texture_lumiere.setVec3("lightPos[1]", multipleLightPos[1]);  
        shader_3d_micro_facette_texture_lumiere.setVec3("lightPos[2]", multipleLightPos[2]);  
        shader_3d_micro_facette_texture_lumiere.setMat4("projection", projection);     
        shader_3d_micro_facette_texture_lumiere.setMat4("view", view); 
        shader_3d_micro_facette_texture_lumiere.setVec3("objectColor", 0.0f, 1.0f, 0.0f);
        shader_3d_micro_facette_texture_lumiere.setVec3("viewPos", camera.Position);
        model = glm::mat4(1.0f);
        model = glm::translate(model, positions[5]);
        shader_3d_micro_facette_texture_lumiere.setMat4("model", model);
        surface.afficher();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(2, VAO);
    glDeleteBuffers(2, VBO);    
    glDeleteVertexArrays(2, VAO_surface);
    glDeleteBuffers(2, VBO_surface); 
    glDeleteBuffers(2, EBO);

    stbi_image_free(tex_data);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


