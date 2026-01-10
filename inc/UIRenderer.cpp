//#include <SSS/GL/Objects/Models/UIRenderer.hpp>
#include "UIRenderer.hpp"
#include <SSS/GL/Window.hpp>

SSS_GL_BEGIN;

UIRenderer::UIRenderer()
{
    //setShaders(Window::getPresetShaders(static_cast<uint32_t>(Shaders::Preset::Line)));

    setShaders(SSS::GL::Shaders::create("glsl/ui.vert", "glsl/ui.frag"));

    //_resolution = glm::vec2(800, 600);

    _proj = glm::ortho(
        0.0f, (float)1440.f,     // left, right
        (float)810.f, 0.0f,    // bottom, top (Y down)
        -1.0f, 1.0f                   // near, far
    );


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

    init();

}

void UIRenderer::render()
{


    Shaders::Shared shader = getShaders();
    if (!shader) {
        LOG_METHOD_WRN("No shaders bound");
        return;
    }
    
    shader->use();
    shader->setVec2("uFrameRes", _resolution);
    shader->setFloat("uProgress", 0.f);


    shader->setMat4("uProj", _proj);
    //shader->setMat4("uProj", _cam->getProjection());

    //Size and pos of the bounding box
    shader->setVec2("uSize", _resolution);
    shader->setVec3("uPos", glm::vec3(0, 0, 0));



    _vao.bind();
    // bind to binding point 0 (must match GLSL 'binding = 0')
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
    //if (_sg == nullptr)
    //    return;
    glClear(GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LEQUAL);
    int offset = 0;
    for (const auto& node : list)
    {
        Node_UI* n = reinterpret_cast<Node_UI*>(at(node));
        if (n->isHidden()|| n->prims.empty())
            continue;

        shader->setInt("uPrimSize", n->prims.size());

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
            n->prims.size() * sizeof(UIPrimitive),
            n->prims.data());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    _vao.unbind();
}
void UIRenderer::updateResolution(const float _w, const float _h)
{
     _resolution = glm::vec2(_w, _h);


     _proj = glm::ortho(
         0.0f, _w,     // left, right
         _h, 0.0f,    // bottom, top (Y down)
         -20.f, 20.f                  // near, far
     );

     glm::vec3 camPos = _rd->camera->getPosition();
     _rd->camera->setPosition(glm::vec3(_w/2.0f, -_h/2.0f, camPos.z));
}
void UIRenderer::push(Node_UI* n)
{
    emplace(n);
    list.push_back(n->_key);
    n->observe(_window);
}
SSS_GL_END;
