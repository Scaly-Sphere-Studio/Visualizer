#include "Box.h"

#define FLAG_ID                 0
#define FLAG_TEXT               1

std::vector<Particle>Box::box_batch{};
std::map<uint16_t, Tags>Box::tags_list{};
std::map<std::string, GUI_Layout> Box::layout_map{};
glm::vec2 Box::minsize = glm::vec2{ 150,75 };

Particle::Particle()
{
    _pos = glm::vec3(0);
    _size = glm::vec2(50.f, 50.f);
    _color = glm::vec4(0);

    translation = glm::vec3(0,0,0);
}

Particle::Particle(glm::vec3 pos, glm::vec2 s, glm::vec4 _col) :
    _pos(pos), _size(s), _color(_col)
{
    translation = glm::vec3(0, 0, 0);
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
    _pos = pos;
    _color = hex_to_rgb(hex);
    _id = hex;

    create_box();
}

Box::~Box()
{
    link_to.clear();
    link_from.clear();
    model.clear();
}


void Box::set_selected_col(std::string hex)
{
}

void Box::set_col(std::string hex)
{
}

void Box::set_text_data(const Text_data& td)
{
    _td = td;
    model.clear();
    curs_pos = glm::vec2(0, 0);
    create_box();
}

void Box::create_box()
{
    _size = glm::vec2(_size.x,0);

    //Brightning the color
    glm::vec4 factor = (glm::vec4(1.f) - _color) * glm::vec4(0.2f);

    //Create the model
    // reverse order, background last.
    // First ID text and background
    text_frame(this->_td.text_ID, Box::layout_map["ID"], * this, FLAG_ID);
    // Text
    text_frame(this->_td.text, Box::layout_map["TEXT"], *this, FLAG_TEXT);
    // Comment text
    if(!this->_td.comment.empty())  
        text_frame("commentaire", Box::layout_map["TEXT"], *this, FLAG_TEXT);
    // tags
    if(!this->tags.empty())
        text_frame("tags", Box::layout_map["TEXT"], *this, FLAG_TEXT);
    // background
    background_frame(*this);
        
    //Tags
    if (tags.size() > 0) {
        model.reserve(tags.size() * 2);
        for (size_t i = 0; i < tags.size(); ++i) {
            model.insert(model.end(), tags_list[tags[i]]._model.begin(), tags_list[tags[i]]._model.end());
        }
    }

    this->update();
}

SSS::GL::Texture::Shared Box::check_text_selection(glm::vec3 const& c_pos)
{
    //for (Particle p : text_model) {
    //    if (p.check_collision(c_pos) && p._sss_texture) {
    //        p._sss_texture->getTextArea()->setFocus(true);
    //        return p._sss_texture;
    //    }
    //}

    return nullptr;
}

void Box::update()
{
    for (size_t i = 0; i < model.size(); i++) {
        model[i].translation = _pos;
        model[i].translation.z += model[i]._pos.z;
    }
}

#define PARTICLE_VERTICES       0
#define PARTICLE_SIZE           1
#define PARTICLE_COLOR          2
#define PARTICLE_POSITION       3
#define PARTICLE_UV             4
#define ID_TEXTURE              5
//Transformations
#define PARTICLE_TRANSLATE      6
#define PARTICLE_SCALE          7
#define PARTICLE_ROTATE         8


BoxRenderer::BoxRenderer()
    : Renderer<BoxRenderer>(), vao(), billboard_vbo(), billboard_ibo(), particles_vbo()
{

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
        glEnableVertexAttribArray(PARTICLE_VERTICES);
        glVertexAttribPointer(PARTICLE_VERTICES, 3, GL_FLOAT, GL_FALSE,
            sizeof(float) * 5, (void*)0);
        
        glEnableVertexAttribArray(PARTICLE_UV);
        glVertexAttribPointer(PARTICLE_UV, 2, GL_FLOAT, GL_FALSE,
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
        glEnableVertexAttribArray(PARTICLE_SIZE);
        glVertexAttribPointer(PARTICLE_SIZE, 2, GL_FLOAT, GL_FALSE,
            sizeof(Particle), (void*)(sizeof(glm::vec3)));
        glVertexAttribDivisor(PARTICLE_SIZE, 1);

        // Color
        glEnableVertexAttribArray(PARTICLE_COLOR);
        glVertexAttribPointer(PARTICLE_COLOR, 4, GL_FLOAT, GL_FALSE,
            sizeof(Particle), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2))
        );
        glVertexAttribDivisor(PARTICLE_COLOR, 1);

        // Position
        glEnableVertexAttribArray(PARTICLE_POSITION);
        glVertexAttribPointer(PARTICLE_POSITION, 3, GL_FLOAT, GL_FALSE,
            sizeof(Particle), (void*)0);
        glVertexAttribDivisor(PARTICLE_POSITION, 1);

        // Texture unit
        glEnableVertexAttribArray(ID_TEXTURE);
        glVertexAttribIPointer(ID_TEXTURE, 1, GL_UNSIGNED_INT,
            sizeof(Particle), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2) + sizeof(glm::vec4)));
        glVertexAttribDivisor(ID_TEXTURE, 1);
        
        //TODO
        // Transformations
        //// Translation
        glEnableVertexAttribArray(PARTICLE_TRANSLATE);
        glVertexAttribPointer(PARTICLE_TRANSLATE, 3, GL_FLOAT, GL_FALSE,
            sizeof(Particle), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2) + sizeof(glm::vec4) + sizeof(GLuint)));
        glVertexAttribDivisor(PARTICLE_TRANSLATE, 1);

        //// Rotate
        //glEnableVertexAttribArray(ID_TEXTURE);
        //glVertexAttribIPointer(ID_TEXTURE, 1, GL_FLOAT,
        //    sizeof(Particle), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2) + sizeof(glm::vec4)+ sizeof(GL_UNSIGNED_INT) + sizeof(GL_FLOAT)));
        //glVertexAttribDivisor(ID_TEXTURE, 1);

        //// Scale
        //glEnableVertexAttribArray(PARTICLE_SCALE);
        //glVertexAttribIPointer(PARTICLE_SCALE, 1, GL_FLOAT,
        //    sizeof(Particle), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2) + sizeof(glm::vec4)));
        //glVertexAttribDivisor(PARTICLE_SCALE, 1);
    }
    vao.unbind();
}

void BoxRenderer::render()
{
    std::queue<Batch> queue;
    Batch* batch = &queue.emplace();

    // Process batch queue
    for (GLuint i = 0; i < Box::box_batch.size(); ++i) {
        Particle& box = Box::box_batch[i];
        // Skip if no texture needed
        if (!box._sss_texture) {
            batch->count++;
            continue;
        }
        // Check if texture ID is already in batch
        auto const it = std::find(
            batch->textures.cbegin(),
            batch->textures.cend(),
            box._sss_texture
        );
        // If not found, push texture ID in batch
        if (it == batch->textures.cend()) {
            // Max number of texture units in the fragment shader
            static uint32_t const max_texture_units = []() {
                int i;
                glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &i);
                return static_cast<uint32_t>(i);
            }();
            // If texture IDs are full, create new batch
            if (batch->textures.size() >= max_texture_units) {
                batch = &queue.emplace();
                batch->offset = i;
            }
            // Add texture ID to batch and batch ID to particle
            batch->textures.push_back(box._sss_texture);
            box._glsl_tex_unit = static_cast<GLint>(batch->textures.size() - 1);
        }
        // Else just add corresponding batch ID
        else {
            box._glsl_tex_unit =
                static_cast<GLint>(std::distance(batch->textures.cbegin(), it));
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
        for (uint32_t i = 0; i < batch.textures.size(); ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            batch.textures[i]->bind();
        }
        // Draw batch
        glDrawElementsInstancedBaseInstance(GL_TRIANGLES, 6, GL_UNSIGNED_INT,
            nullptr, batch.count, batch.offset);
    }

    vao.unbind();
}

Tags::Tags()
{
    _weight = 1;
}

Tags::Tags(std::string _name, std::string hex, uint32_t weight)
{
    int char_size = 12;
    _size = { char_size * _name.size() + 5, char_size * 1.5f};
    //Center the box around the cursor
    _pos = { 0.0f, 0.0f, 0.0f };
    _color = hex_to_rgb(hex);
    _weight = weight;


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

    //Create the model
    _model.emplace_back(_pos, _size, glm::vec4(_color));
    _model.emplace_back(_pos + glm::vec3{1,2,0}, _size, glm::vec4(0))
        ._sss_texture = SSS::GL::Texture::create(area);
}

Tags::~Tags()
{
    _model.clear();
}

void text_frame(std::string s, const GUI_Layout& layout, Box& b, int flag)
{
    glm::vec3 pos = glm::vec3(b.curs_pos, BACKGROUND_COLOR_LAYER);
    SSS::TR::Format fmt = layout._fmt;
    glm::vec4 c = rgb_to_hsl(b._color);
    c.b = 0.3;
    c = hsl_to_rgb(c);

    if (flag == FLAG_ID) {
        fmt.text_color = rgb_to_int32t(c);
    }

    // Create text area & gl texture
    auto& area = SSS::TR::Area::create(s);
    area.setFormat(fmt);
    area.setMarginH(layout._marginh);
    area.setMarginV(layout._marginv);
    int x, y;
    area.getDimensions(x, y);
    glm::vec2 size = glm::vec2{x,y};

    
    //Create the model
    Particle p;
    p._pos = pos;
    p._size = size;
    p.translation = b._pos;
    p._pos.z = TXT_LAYER;
    b.model.emplace_back(p)
        ._sss_texture = SSS::GL::Texture::create(area);

    b._size.x = std::max(b._size.x, size.x);
    b._size.y += size.y;
    b.curs_pos -= glm::vec2{0, size.y};
}

void background_frame(Box& b)
{
    // Create text area & gl texture
    auto& area = SSS::TR::Area::create("0");
    //auto fmt = area.getFormat();
    area.setFormat(Box::layout_map["ID"]._fmt);
    area.setMarginH(Box::layout_map["ID"]._marginh);
    area.setMarginV(Box::layout_map["ID"]._marginv);
    int x, y;
    area.getDimensions(x, y);
    glm::vec2 size = glm::vec2{ x,y };
    
    b.model.emplace_back(glm::vec3(0, 0, BACKGROUND_LAYER), b._size, b._color).translation = b._pos;

    glm::vec4 scol = rgb_to_hsl(b._color);
    scol.b -= 0.15;
    b.model.emplace_back(glm::vec3(0, 0, BACKGROUND_COLOR_LAYER)
        , glm::vec2(b._size.x, static_cast<float>(y - 3))
        , hsl_to_rgb(scol)).translation = b._pos;
}
