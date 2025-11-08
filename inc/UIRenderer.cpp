//#include <SSS/GL/Objects/Models/UIRenderer.hpp>
#include "UIRenderer.hpp"
#include <SSS/GL/Window.hpp>

SSS_GL_BEGIN;

UIRenderer::UIRenderer()
{
    //setShaders(Window::getPresetShaders(static_cast<uint32_t>(Shaders::Preset::Line)));

    setShaders(SSS::GL::Shaders::create("glsl/ui.vert", "glsl/ui.frag"));

    std::vector<float> vertices = {
    0.0f, 1.0f, 0.0f,  // top
    0.0f, 0.0f, 0.0f,  // left
    1.0f, 0.0f, 0.0f,   // right

    1.0f, 1.0f, 0.0f,  // top
    1.0f, 0.0f, 0.0f,  // left
    0.0f, 1.0f, 0.0f   // right
    };

    _vbo.edit(vertices, GL_STATIC_DRAW);

    _vao.setup([this]() {



        //glGenBuffers(1, &_vbo);

        //// bind + upload
        //glBindBuffer(GL_ARRAY_BUFFER, VBO);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);




        //_ibo.bind();
        //Coordinates
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3,
            GL_FLOAT, GL_FALSE,
            3 * sizeof(float), (void*)0);
    });

    //GLuint ssbo = 0;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

    // allocate initial storage (reserve for N elements)
    size_t maxElements = 1024;
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        maxElements * sizeof(UIPrimitive),
        nullptr,
        GL_DYNAMIC_DRAW);



    _vao.unbind();
}

void UIRenderer::render()
{
    static size_t size;

    

    


    // bind to binding point 0 (must match GLSL 'binding = 0')
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);


    UIPrimitive p;
    p.shapeId = SDF_Shapes::sdCircle;
    p.pos = glm::vec2(0);
    p.size.r = 250;
    p.color = glm::vec4(1);

    std::vector<UIPrimitive> test;
    test.push_back(p);

    Shaders::Shared shader = getShaders();
    if (!shader) {
        LOG_METHOD_WRN("No shaders bound");
        return;
    }

    //glm::mat4 const mvp = camera ? camera->getVP() : glm::mat4(1);

    glm::mat4 ortho = glm::ortho(
        0.0f, (float)800,     // left, right
        (float)600, 0.0f,    // bottom, top (Y down)
        -1.0f, 1.0f                   // near, far
    );


    shader->use();
    shader->setVec2("uFrameRes", glm::vec2(800, 600));
    shader->setFloat("uProgress", 0.f);
    shader->setInt("uPrimSize", test.size());
    shader->setMat4("uProj", ortho);
    shader->setVec2("uSize", glm::vec2(800, 600));
    shader->setVec3("uPos", glm::vec3(0, 0, 0));

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
        test.size() * sizeof(UIPrimitive),
        test.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


    //for (const auto& node : _nodes) 
    //{
    //    Node_UI* n = reinterpret_cast<Node_UI*>(_sg->at(node));
    //    std::vector<UIPrimitive> prims = n->renderUI();
    //    //glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
    //    //    prims.size() * sizeof(UIPrimitive),
    //    //    prims.data());
    //    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    //    //shader->setInt("u_PrimsSize", prims.size());


    //}
    _vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);


    test.clear();
    _vao.unbind();
}

SSS_GL_END;