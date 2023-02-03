#include "Box.h"

std::vector<Particle>Box::box_batch{};
std::map<uint16_t, Tags>Box::tags_list{};


Particle::Particle()
{
    _pos = glm::vec3(0);
    _size = glm::vec2(50.f, 50.f);
    _color = glm::vec4(0);
}

Particle::Particle(glm::vec3 pos, glm::vec2 s, glm::vec4 _col) :
    _pos(pos), _size(s), _color(_col)
{
}

Particle::Particle(std::string t, const SSS::TR::Format& fmt, 
    glm::vec3 pos, glm::vec2 s) :
    _pos(pos), _size(s), _color(glm::vec4(0))
{
    // Create text area & gl texture
    auto& area = SSS::TR::Area::create((int)_size.x, (int)_size.y);
    area.setFormat(fmt);
    area.parseString(t);

    auto& texture = SSS::GL::Texture::create(area);

    _sss_tex_id = texture.getID();
}

bool Particle::check_collision(glm::vec3 const& c_pos)
{
    //Check if a point is hovering the box
    //Point to Box Collision test
    if (((c_pos.x > _pos.x) && (c_pos.x < static_cast<double>(_pos.x) + static_cast<double>(_size.x))) &&
        ((c_pos.y < _pos.y) && (c_pos.y > static_cast<double>(_pos.y) - static_cast<double>(_size.y)))) {
        return true;
    }
    return false;
}

bool Particle::check_collision(Particle p)
{
    glm::vec2 delta = glm::vec3(glm::abs(p.center() - this->center()));
    glm::vec2 sum = (glm::abs(p._size) + this->_size) / 2.f;

    if (delta.x < sum.x && delta.y < sum.y) {
        return true;
    }
    return false;
}

glm::vec3 Particle::center()
{
    return _pos + glm::vec3(_size.x /2.0, -_size.y /2.0, 0);
}


Box::Box() { }

Box::Box(glm::vec3 pos, glm::vec2 s, std::string hex)
{
    _size = s;
    //Center the box around the cursor
    _pos = pos + glm::vec3{ -_size.x / 2.0f , _size.y / 2.0f, rand_float() };
    _color = hex_to_rgb(hex);
    _id = hex;

    create_box();
}

Box::~Box()
{
    link_to.clear();
    link_from.clear();
    model.clear();
    text_model.clear();
}


void Box::set_selected_col(std::string hex)
{
}

void Box::set_col(std::string hex)
{
}

void Box::create_box()
{
    //Brightning the color
    glm::vec4 factor = (glm::vec4(1.f) - _color) * glm::vec4(0.2f);

    //Create the model
    //Background
    model.emplace_back(_pos, _size, glm::vec4(_color));
    //ID Background
    model.emplace_back(_pos + glm::vec3(0.f, 0.f, epsilon), glm::vec2(_size.x - 2, _size.y / 3), glm::vec4(_color + factor));
    
    SSS::TR::Format fmt;
    fmt.charsize = (int)_size.y / 6;
    fmt.has_outline = true;
    fmt.outline_size = 2;

    text_model.emplace_back(_id, fmt, _pos + glm::vec3(0.f, 0.f, 2.f * epsilon), _size * glm::vec2(1.f, 0.3f));
    text_model.emplace_back("bsr", fmt, _pos + glm::vec3(0.f, -_size.y/3.f, 2.f * epsilon), _size * glm::vec2(1.f, 1.f - 0.3f));

    //Tags
    if (tags.size() > 0) {
        model.reserve(tags.size() * 2);
        for (size_t i = 0; i < tags.size(); ++i) {
            model.insert(model.end(), tags_list[tags[i]].model.begin(), tags_list[tags[i]].model.end());
        }
    }
}

int Box::check_text_selection(glm::vec3 const& c_pos)
{
    for (Particle p : text_model) {
        if (p.check_collision(c_pos) && p._sss_tex_id != UINT32_MAX) {
            std::cout << p._sss_tex_id << std::endl;
            SSS::GL::Window::getFirst()->getTextureMap().at(p._sss_tex_id)->getTextArea()->setFocus(true);
            return p._sss_tex_id;
        }
    }

    return INT32_MAX;
}

void Box::update_pos(glm::vec3 delta)
{
    //Numbers of particles for each box
    // Background, id background, numbers of tags, comment,  *2 + info particles

    for (size_t i = 0; i < model.size(); i++) {
        model[i]._pos = _pos + glm::vec3(0., 0., static_cast<float>(i) * epsilon);
    }
    for (size_t i = 0; i < text_model.size(); i++) {
        text_model[i]._pos = _pos + glm::vec3(0., 0., 2.0f * epsilon);
    }

}

void Box::update()
{
    for (size_t i = 0; i < model.size(); i++) {
        model[i]._pos = _pos + glm::vec3(0., 0., static_cast<float>(i) * epsilon);
    }
    for (size_t i = 0; i < text_model.size(); i++) {
        text_model[i]._pos = _pos + glm::vec3(0., 0., 2.0f * epsilon);
    }
}


BoxRenderer::BoxRenderer(std::weak_ptr<SSS::GL::Window> win, uint32_t id)
    : Renderer(win, id), vao(win), billboard_vbo(win), billboard_ibo(win), particles_vbo(win)
{
    SSS::GL::Context const& context(_window);

    vao.bind();

    // Static vertices
    {
        constexpr GLfloat vertices[] = {
            // Position         // Texture UV
            0.f,  0.f, 0.f,     0.f, 1.f - 1.f, // Top left
            0.f, -1.f, 0.f,     0.f, 1.f - 0.f, // Bottom left
            1.f, -1.f, 0.f,     1.f, 1.f - 0.f, // Bottom right
            1.f,  0.f, 0.f,     1.f, 1.f - 1.f  // Top right
        };
        billboard_vbo.edit(sizeof(vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
            sizeof(float) * 5, (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE,
            sizeof(float) * 5, (void*)(sizeof(float) * 3));

        constexpr GLuint indices[] = {
            0, 1, 2,    // First triangle
            2, 3, 0     // Second triangle
        };
        billboard_ibo.edit(sizeof(indices), indices, GL_STATIC_DRAW);
    }

    // Particles
    {
        particles_vbo.bind();

        // Size (width / height)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
            sizeof(Particle), (void*)(sizeof(glm::vec3)));
        glVertexAttribDivisor(1, 1);

        // Color
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE,
            sizeof(Particle), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2))
        );
        glVertexAttribDivisor(2, 1);

        // Position
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
            sizeof(Particle), (void*)0);
        glVertexAttribDivisor(3, 1);

        // Texture unit
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 1, GL_UNSIGNED_INT,
            sizeof(Particle), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2) + sizeof(glm::vec4)));
        glVertexAttribDivisor(5, 1);
    }

    vao.unbind();
}

void BoxRenderer::render()
{
    SSS::GL::Context const& context(_window);

    std::queue<Batch> queue;
    Batch* batch = &queue.emplace();

    auto const& textures = _window.lock()->getTextureMap();
    // Process batch queue
    for (GLuint i = 0; i < Box::box_batch.size(); ++i) {
        Particle& box = Box::box_batch[i];
        uint32_t const tex_id = box._sss_tex_id;
        // Skip if no texture needed
        if (textures.count(tex_id) == 0) {
            batch->count++;
            continue;
        }
        // Check if texture ID is already in batch
        auto const it = std::find(
            batch->tex_ids.cbegin(),
            batch->tex_ids.cend(),
            box._sss_tex_id
        );
        // If not found, push texture ID in batch
        if (it == batch->tex_ids.cend()) {
            // Max number of texture units in the fragment shader
            static uint32_t const max_texture_units = []() {
                int i;
                glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &i);
                return static_cast<uint32_t>(i);
            }();
            // If texture IDs are full, create new batch
            if (batch->tex_ids.size() >= max_texture_units) {
                batch = &queue.emplace();
                batch->offset = i;
            }
            // Add texture ID to batch and batch ID to particle
            batch->tex_ids.push_back(box._sss_tex_id);
            box._glsl_tex_unit = static_cast<GLint>(batch->tex_ids.size() - 1);
        }
        // Else just add corresponding batch ID
        else {
            box._glsl_tex_unit =
                static_cast<GLint>(std::distance(batch->tex_ids.cbegin(), it));
        }
        // Increment elements count
        batch->count++;
    }

    // Setup VAO
    vao.bind();
    if (Box::box_batch.size() > 0) {
        particles_vbo.edit(sizeof(Particle) * Box::box_batch.size(),
            Box::box_batch.data(), GL_DYNAMIC_DRAW);
    }

    // Setup shader
    auto const shader = getShaders();

    auto mvp = camera->getVP();

    shader->use();
    shader->setUniformMat4fv("u_MVP", 1, GL_FALSE, &mvp[0][0]);

    // Draw all particles
    static constexpr std::array<GLint, 128> texture_IDs = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
        16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
        32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
        48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
        64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
        80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
        96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
        112, 113, 114, 115, 116, 117, 118, 119, 110, 121, 122, 123, 124, 125, 126, 127
    };
    for (; !queue.empty(); queue.pop()) {
        Batch const& batch = queue.front();
        shader->setUniform1iv("u_Textures", batch.count, &texture_IDs[0]);
        // Bind needed textures
        for (uint32_t i = 0; i < batch.tex_ids.size(); ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            textures.at(batch.tex_ids[i])->bind();
        }
        // Draw batch
        glDrawElementsInstancedBaseInstance(GL_TRIANGLES, 6, GL_UNSIGNED_INT,
            nullptr, batch.count, batch.offset);
    }

    vao.unbind();
}

Tags::Tags()
{
}

Tags::Tags(std::string _name, std::string hex, uint32_t weight)
{
    int char_size = 12;
    _size = { char_size * _name.size() + 5, char_size * 1.5f};
    //Center the box around the cursor
    _pos = { 0.0f, 0.0f, 0.0f };
    _color = hex_to_rgb(hex);


    // Create text area & gl texture
    auto& area = SSS::TR::Area::create((int)_size.x, (int)_size.y);
    auto fmt = area.getFormat();
    //fmt.charsize = (int)_size.y / 3;
    fmt.charsize = char_size;
    fmt.has_outline = false;
    fmt.outline_size = 2;
    fmt.text_color = 0x000000;
    area.setFormat(fmt);
    area.parseString(_name);

    auto const& texture = SSS::GL::Texture::create(area);

    //Create the model
    model.emplace_back(_pos, _size, glm::vec4(_color));
    model.emplace_back(_pos + glm::vec3{1,2,0}, _size, glm::vec4(0))._sss_tex_id = texture.getID();
}

Tags::~Tags()
{
    model.clear();
}

Text_particle::Text_particle(glm::vec3 _pos, glm::vec2 _s, const std::string text, const SSS::TR::Format& fmt)
{

}
