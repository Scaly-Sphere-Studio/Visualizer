#include "visualizer.h"

/* [MISC] */
static std::array<float, 4> BezierCoeffs(float P0, float P1, float P2, float P3)
{
    std::array<float, 4> Z;
    Z[0] = -P0 + 3.0f * P1 + -3.0f * P2 + P3;
    Z[1] = 3.0f * P0 - 6.0f * P1 + 3.0f * P2;
    Z[2] = -3.0f * P0 + 3.0f * P1;
    Z[3] = P0;

    return Z;
}

static std::array<float, 3> CubicRoots(float a, float b, float c, float d)
{

    float A = b / a;
    float B = c / a;
    float C = d / a;

    float Im;

    float Q = (3.f * B - std::pow(A, 2.f)) / 9.f;
    float R = (9.f * A * B - 27.f * C - 2.f * std::pow(A, 3.f)) / 54.f;
    float D = std::pow(Q, 3.f) + std::pow(R, 2.f);    // polynomial discriminant

    std::array<float, 3U> t;

    if (D >= 0)                                 // complex or duplicate roots POI
    {
        float S = signum(R + std::sqrt(D)) * std::pow(std::abs(R + std::sqrt(D)), (1.0f / 3.0f));
        float T = signum(R - std::sqrt(D)) * std::pow(std::abs(R - std::sqrt(D)), (1.0f / 3.0f));

        t[0] = -A / 3.0f + (S + T);                         // real root
        t[1] = -A / 3.0f - (S + T) / 2.0f;                  // real part of complex root
        t[2] = -A / 3.0f - (S + T) / 2.0f;                  // real part of complex root
        Im = std::abs(std::sqrt(3.0f) * (S - T) / 2.0f);    // complex part of root pair   

        //discard complex roots//
        if (Im != 0) {
            t[1] = -1.f;
            t[2] = -1.f;
        }

    }
    else                                          // distinct real roots
    {
        float th = std::acos(R / std::sqrt(-std::pow(Q, 3.f)));

        t[0] = 2.0f * std::sqrt(-Q) * std::cos(th / 3.0f) - A / 3.0f;
        t[1] = 2.0f * std::sqrt(-Q) * std::cos((th + 2.0f * glm::pi<float>()) / 3.0f) - A / 3.0f;
        t[2] = 2.0f * std::sqrt(-Q) * std::cos((th + 4.0f * glm::pi<float>()) / 3.0f) - A / 3.0f;
        Im = 0.0f;
    }

    /*discard out of spec roots*/
    for (size_t i = 0; i < t.size(); i++) {
        if (t[i] < 0 || t[i] > 1.0) {
            t[i] = -1;
        }
    }


    return t;
}

//px and py are the coordinates of the start, first tangent, second tangent, end in that order. length = 4
//lx and ly are the start then end coordinates of the stright line. length = 2
static bool cubic_bezier_segment_intersection(glm::vec3 b_a, glm::vec3 b_b, glm::vec3 b_c, glm::vec3 b_d,
    glm::vec3 s_a, glm::vec3 s_b) {

    glm::vec2 X;


    float A = s_b.y - s_a.y;      //A=y2-y1
    float B = s_a.x - s_b.x;      //B=x1-x2
    float C = s_a.x * (s_a.y - s_b.y) + s_a.y * (s_b.x - s_a.x);  //C=x1*(y1-y2)+y1*(x2-x1)

    std::array<float, 4> bx = BezierCoeffs(b_a.x, b_b.x, b_c.x, b_d.x);
    std::array<float, 4> by = BezierCoeffs(b_a.y, b_b.y, b_c.y, b_d.y);

    std::array<float, 4> P;
    P[0] = A * bx[0] + B * by[0];       /*t^3*/
    P[1] = A * bx[1] + B * by[1];       /*t^2*/
    P[2] = A * bx[2] + B * by[2];       /*t*/
    P[3] = A * bx[3] + B * by[3] + C;   /*1*/

    std::array<float, 3> r = CubicRoots(P[0], P[1], P[2], P[3]);

    /*verify the roots are in bounds of the linear segment*/
    for (size_t i = 0; i < r.size(); i++) {
        float t = r[i];

        X[0] = bx[0] * t * t * t + bx[1] * t * t + bx[2] * t + bx[3];
        X[1] = by[0] * t * t * t + by[1] * t * t + by[2] * t + by[3];

        /*above is intersection point assuming infinitely long line segment,
          make sure we are also in bounds of the line*/
        float s;
        if ((s_b.x - s_a.x) != 0) { s = (X[0] - s_a.x) / (s_b.x - s_a.x); }          /*if not vertical line*/
        else { s = (X[1] - s_a.y) / (s_b.y - s_a.y); }

        /*in bounds?*/
        if (t > 0 && t < 1.0 && s > 0 && s < 1.0) {
            return true;
        }
    }
    return false;
}

Visualizer::Visualizer()
{
    //TODO RANDSEED 
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    setup();

    
    Box b1 = Box(glm::vec3(-400.0f, 75.0f, 0.0f), glm::vec2(300.0f,150.0f), "1E1022");
    Box b2 = Box(glm::vec3(400.0f, -80.0f, 0.0f), glm::vec2(300.0f,150.0f), "0E2556");

    b1.id = i1;
    b2.id = i2;

    box_map.insert(std::make_pair(i1, b1));
    box_map.insert(std::make_pair(i2, b2));

    link_box(box_map.at(i1), box_map.at(i2));
    link_box(box_map.at(i2), box_map.at(i1));

    clear_color = hex_to_rgb("#4d5f83");
}

Visualizer::Ptr const& Visualizer::get()
{
    static Ptr const singleton(new Visualizer());
    return singleton;
}

Visualizer::~Visualizer()
{

    Box::box_batch.clear();
    arrow_map.clear();
    box_map.clear();
}

void Visualizer::run()
{
    SSS::GL::Context const context(window);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_DEPTH_TEST);
    glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0f);

    // Main loop
    while (!window->shouldClose()) {
        SSS::GL::pollEverything();
        glfwGetCursorPos(window->getGLFWwindow(), &c_x, &c_y);
        input();
        
        
        glClear(GL_COLOR_BUFFER_BIT);


        //Gen batch trough the frustrum
        //FRUSTRUM 
        frustrum_test();

        //Collision test
        drag_boxes();
        line_drag_link();
        //multi_select_drag();

        //Sort every frame for now, until I have a real frustrum calling that sort before selecting
        std::sort(Box::box_batch.begin(), Box::box_batch.end(), sort_box);


        SSS::GL::Window::Objects const& objects = window->getObjects();

        //TEST CURSOR DRAG
        //UPDATE MVP MATRIX
        glm::mat4 mvp = objects.cameras.at(0)->getVP();

        //LINE RENDERER
        {
            auto const& shader = objects.shaders.at(line_shader_id);
            shader->use();
            shader->setUniformMat4fv("u_MVP", 1, GL_FALSE, &mvp[0][0]);
            Line_Batch::render();
        }

        //BOX RENDERER
        {
            // Batch info
            struct Batch_ {
                GLuint offset{ 0 };
                GLuint count{ 0 };
                std::vector<uint32_t> tex_ids;
            };
            std::queue<Batch_> queue;
            Batch_* batch = &queue.emplace();

            // Process batch queue
            for (GLuint i = 0; i < Box::box_batch.size(); ++i) {
                testBox& box = Box::box_batch[i];
                uint32_t const tex_id = box._sss_tex_id;
                // Skip if no texture needed
                if (objects.textures.count(tex_id) == 0) {
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
            vao->bind();
            if (Box::box_batch.size() > 0) {
                particles_vbo->edit(sizeof(testBox) * Box::box_batch.size(),
                    Box::box_batch.data(), GL_DYNAMIC_DRAW);
            }
            
            // Setup shader
            auto const& shader = objects.shaders.at(box_shader_id);
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
                Batch_ const& batch = queue.front();
                shader->setUniform1iv("u_Textures", batch.count, &texture_IDs[0]);
                // Bind needed textures
                for (uint32_t i = 0; i < batch.tex_ids.size(); ++i) {
                    glActiveTexture(GL_TEXTURE0 + i);
                    objects.textures.at(batch.tex_ids[i])->bind();
                }
                // Draw batch
                glDrawElementsInstancedBaseInstance(GL_TRIANGLES, 6, GL_UNSIGNED_INT,
                    nullptr, batch.count, batch.offset);
            }

            vao->unbind();
        }

        //DEBUG RENDERER
        {
            if (debug.debugmode) {
                glm::vec3 cam_pos = window->getObjects().cameras.at(0)->getPosition();
                debug.rectangle(cam_pos.x - w_w / 2 + 1, cam_pos.y + w_h / 2, w_w - 1, w_h - 1);
            }
            for (auto it = box_map.begin(); it != box_map.end(); it++) {
                debug.debug_box(it->second);
            }
            float cursor_size = 5;
            //ORIGIN CURSOR
            debug.cross(0, 0, 0, cursor_size);
            debug.circle(0, 0, 0, cursor_size);
            debug.vbo->edit(
                debug.debug_batch.size() * sizeof(debug_Vertex),
                debug.debug_batch.data(),
                GL_STATIC_DRAW);

            auto const& shader = objects.shaders.at(debug.shader_id);
            shader->use();
            shader->setUniformMat4fv("u_MVP", 1, GL_FALSE, &mvp[0][0]);
            debug.debug_show();
        }



        window->drawObjects();
        window->printFrame();
        debug.debug_batch.clear();
        Box::box_batch.clear();
    }

}


void Visualizer::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //INPUTS BOX
    if (key == GLFW_KEY_KP_ADD && action == GLFW_PRESS) {
        Visualizer::get()->push_box(rand_color());
    }

    //TEST SUPPRESSION
    if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS) {
        Visualizer::get()->pop_box(Visualizer::get()->last_selected_ID);
    }

    if (key == GLFW_KEY_KP_0 || key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(Visualizer::get()->window->getGLFWwindow(), true);
    }

}

void Visualizer::resize_callback(GLFWwindow* win, int w, int h)
{
    Ptr const& visu = get();
    visu->w_w = static_cast<float>(w);
    visu->w_h = static_cast<float>(h);
}

void Visualizer::setup()
{
    SSS::GL::Window::CreateArgs args;
    args.title = "VISUALIZER";
    args.w = static_cast<int>(w_w);
    args.h = static_cast<int>(w_h);
    window = SSS::GL::Window::create(args);
    if (!window) {
        SSS::throw_exc("Couldn't create a window");
    }

    SSS::GL::Context const context(window);

    window->setVSYNC(true);
    window->setCallback(glfwSetWindowSizeCallback, resize_callback);
    window->setCallback(glfwSetKeyCallback, key_callback);

    // SSS/GL objects
    {
        SSS::TR::Area::Ptr const& area = SSS::TR::Area::create(300, 300);
        SSS::TR::Format fmt = area->getFormat();
        fmt.style.charsize = 50;
        area->setFormat(fmt);
        area->parseString("Lorem ipsum dolor sit amet.");

        auto const& texture = SSS::GL::Texture::create();
        auto const& camera = SSS::GL::Camera::create();
        auto const& plane = SSS::GL::Plane::create();
        auto const& renderer = SSS::GL::Plane::Renderer::create();

        texture->setTextAreaID(area->getID());
        texture->setType(SSS::GL::Texture::Type::Text);

        camera->setPosition({ 0, 0, 3 });
        camera->setProjectionType(SSS::GL::Camera::Projection::OrthoFixed);

        plane->setTextureID(texture->getID());
        plane->scale(glm::vec3(300));

        renderer->chunks.emplace_back();
        renderer->chunks[0].reset_depth_before = true;
        renderer->chunks[0].objects.push_back(0);
    }

    // Shaders
    {
        auto const& line_shader = window->createShaders();
        line_shader->loadFromFiles("glsl/line.vert", "glsl/line.frag");
        line_shader_id = line_shader->getID();
    
        auto const& box_shader = window->createShaders();
        box_shader->loadFromFiles("glsl/instance.vert", "glsl/instance.frag");
        box_shader_id = box_shader->getID();
    
        auto const& debug_shader = window->createShaders();
        debug_shader->loadFromFiles("glsl/triangle.vert", "glsl/triangle.frag");
        debug.shader_id = debug_shader->getID();
    }

    // VAO
    {
        vao.reset(new SSS::GL::Basic::VAO(window));
        billboard_vbo.reset(new SSS::GL::Basic::VBO(window));
        billboard_ibo.reset(new SSS::GL::Basic::IBO(window));
        particles_vbo.reset(new SSS::GL::Basic::VBO(window));

        vao->bind();

        // Static vertices
        {
            constexpr GLfloat vertices[] = {
                // Position         // Texture UV
                0.f,  0.f, 0.f,     0.f, 1.f - 1.f, // Top left
                0.f, -1.f, 0.f,     0.f, 1.f - 0.f, // Bottom left
                1.f, -1.f, 0.f,     1.f, 1.f - 0.f, // Bottom right
                1.f,  0.f, 0.f,     1.f, 1.f - 1.f  // Top right
            };
            billboard_vbo->edit(sizeof(vertices), vertices, GL_STATIC_DRAW);
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
            billboard_ibo->edit(sizeof(indices), indices, GL_STATIC_DRAW);
        }

        // Particles
        {
            particles_vbo->bind();

            // Size (width / height)
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                sizeof(testBox), (void*)(sizeof(glm::vec3)));
            glVertexAttribDivisor(1, 1);

            // Color
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE,
                sizeof(testBox), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2))
            );
            glVertexAttribDivisor(2, 1);

            // Position
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
                sizeof(testBox), (void*)0);
            glVertexAttribDivisor(3, 1);

            // Texture unit
            glEnableVertexAttribArray(5);
            glVertexAttribIPointer(5, 1, GL_UNSIGNED_INT,
                sizeof(testBox), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2) + sizeof(glm::vec4)));
            glVertexAttribDivisor(5, 1);
        }

        vao->unbind();
    }

    debug.vbo.reset(new SSS::GL::Basic::VBO(window));
}

void Visualizer::input()
{
    SSS::GL::Window::KeyInputs const& inputs = window->getKeyInputs();
    //INPUT CAMERA
    constexpr float speed = 10.0f;

    auto const& camera = window->getObjects().cameras.at(0);

    if (inputs[GLFW_KEY_DOWN]) {
        camera->move(glm::vec3(0.0f, -speed, 0.0f));
    }

    if (inputs[GLFW_KEY_RIGHT]) {
        camera->move(glm::vec3(speed, 0.0f, 0.0f));
    }

    if (inputs[GLFW_KEY_UP]) {
        camera->move(glm::vec3(0.0f, speed, 0.0f));
    }

    if (inputs[GLFW_KEY_LEFT]) {
        camera->move(glm::vec3(-speed, 0.0f, 0.0f));
    }




    ////INPUTS BOX
    ////TEST AJOUT
    //if (inputs[GLFW_KEY_KP_ADD]) {
    //    push_box(rand_color());
    //}

    ////TEST SUPPRESSION
    //if (inputs[GLFW_KEY_KP_SUBTRACT]) {
    //    pop_box(last_selected_ID);
    //}
}



void Visualizer::link_box(Box& a, Box& b)
{

    glm::vec3 offset{ 0.f, 400.f, 0.f };
    //Create a bezier curve to link the two boxes
    Gradient<glm::vec4> Col_grdt;
    Col_grdt.push(std::make_pair(0.f, glm::vec4(a._color)));
    Col_grdt.push(std::make_pair(1.f, glm::vec4(b._color)));

    Gradient<float> Thk_grdt;
    Thk_grdt.push(std::make_pair(0.f, 25.f));
    Thk_grdt.push(std::make_pair(1.f, 25.f));

    auto seg = Polyline::Bezier(
        a.center(), a.center() - offset,
        b.center() + offset, b.center(),
        Thk_grdt, Col_grdt,
        JointType::BEVEL, TermType::SQUARE
    );

    if (arrow_map.count(a.id + b.id)) {
        arrow_map.at(a.id + b.id) = seg;
        return;
    }

    //Add the dst box to the 'link to' list of the src box
    //And add the src box to the 'link from' list of the dst box
    a.link_to.emplace_back(b.id);
    b.link_from.emplace_back(a.id);

    //The arrow ID is the cat of the two boxes ID as it keeps the order
    arrow_map.insert(std::make_pair(a.id + b.id, seg));

}

void Visualizer::link_box(Box& a)
{
    for (std::string lt : a.link_to) {
        link_box(a, box_map.at(lt));
    }
    for (std::string lf : a.link_from) {
        link_box(box_map.at(lf), a);
    }
}

void Visualizer::link_box_to_cursor(Box& b)
{
    glm::vec3 c_pos = cursor_map_coordinates();

    //Create a bezier curve to link the two boxes
    Gradient<glm::vec4> Col_grdt;
    Col_grdt.push(std::make_pair(0.f, glm::vec4(b._color)));
    Col_grdt.push(std::make_pair(1.f, glm::vec4(0.f, 0.f, 0.f, 1.f)));

    Gradient<float> Thk_grdt;
    Thk_grdt.push(std::make_pair(0.f, 25.f));
    Thk_grdt.push(std::make_pair(1.f, 25.f));

    auto seg = Polyline::Bezier(
        b.center(), b.center() - glm::vec3(0, 800, 0),
        c_pos + glm::vec3(0, 0, 0), c_pos,
        Thk_grdt, Col_grdt,
        JointType::BEVEL, TermType::SQUARE
    );

    if (arrow_map.count(first_link_ID)) {
        arrow_map.at(b.id) = seg;
        return;
    }


    //The arrow ID is the cat of the two boxes ID as it keeps the order
    arrow_map.insert(std::make_pair(b.id, seg));

}

void Visualizer::pop_link(Box& a, Box& b)
{
    arrow_map.erase(a.id + b.id);
    a.link_to.remove(b.id);  
    b.link_from.remove(a.id);
}



void Visualizer::push_box(std::string boxID)
{
    glm::vec3 position = cursor_map_coordinates();
    box_map.insert(std::make_pair(boxID, Box(position, glm::vec2{150,75}, boxID)));
    box_map.at(boxID).id = boxID;
}

void Visualizer::pop_box(std::string ID)
{
    if (!last_selected_ID.empty() ){
        //Clear the connected arrows and remove the ID from the ID lists 

        //Erase the arrows connected to the box
        //Erase the ID from their 'Link to' list
        for (std::string f_ID : box_map.at(ID).link_from) {
            arrow_map.erase(f_ID + ID);
            box_map.at(f_ID).link_to.remove(ID);
        }

        //Clear all the arrows connected to other boxes

        //Erase the arrows that connect to other boxes
        //Erase the ID from their 'link from' list
        for (std::string l_ID : box_map.at(ID).link_to) {
            arrow_map.erase(ID + l_ID);
            box_map.at(l_ID).link_from.remove(ID);
        }

        //Clear the box from the map
        box_map.erase(last_selected_ID);
    }
   
    //Erase the selection
    last_selected_ID.clear();
    current_selected_ID.clear();
}


bool Visualizer::check_frustrum_render(Box &b)
{
    //CHECK IF A BOX IS IN THE RENDERED WINDOW TROUGH THE SELECTED CAMERA
    glm::vec3 const cam_pos = window->getObjects().cameras.at(0)->getPosition();
    float const dx = glm::abs(cam_pos.x - b._pos.x);
    float const dxmax = (b._size.x + w_w) * 0.5f;    
    float const dy = glm::abs(cam_pos.y - b._pos.y);
    float const dymax = (b._size.y + w_h) * 0.5f;


    if ((dx < dxmax) && (dy < dymax)) {
        return true;
    }
    return false;
}

glm::vec3 Visualizer::cursor_map_coordinates()
{
    glm::vec3 const cam_pos = window->getObjects().cameras.at(0)->getPosition();
    return glm::vec3{ (c_x - w_w / 2) + cam_pos.x, (w_h / 2 - c_y) + cam_pos.y, 0.0 };
}

void Visualizer::frustrum_test()
{
    

    //for (int i = 0; i < boxes._size(); i++) {
    //    Box::box_batch.insert(Box::box_batch.end(), boxes[i].model.begin(), boxes[i].model.end());
    //    debug_box(boxes[i]);
    //    //if (check_frustrum_render(boxes[i])) {
    //    //    debug_box(boxes[i]);
    //    //    Box::box_batch.insert(Box::box_batch.end(), boxes[i].model.begin(), boxes[i].model.end());
    //    //}
    //}

    for (auto it = box_map.begin(); it != box_map.end(); it++) {
        Box::box_batch.insert(Box::box_batch.end(), it->second.model.begin(), it->second.model.end());
        //if (check_frustrum_render(boxes[i])) {
        //    debug_box(boxes[i]);
        //    Box::box_batch.insert(Box::box_batch.end(), boxes[i].model.begin(), boxes[i].model.end());
        //}
    }
}

void Visualizer::drag_boxes()
{
    static glm::vec3 cur_pos;
    
    //CHECK THE MAP FOR A COLLISION WITH A BOX 
    if (current_selected_ID.empty() && glfwGetMouseButton(window->getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

        //Retrieve the ID of the current selected box
        clicked_box_ID(current_selected_ID);

        if (!last_selected_ID.empty() && (last_selected_ID != current_selected_ID)) {
            //Reset the Z offset for priority 
            box_map.at(last_selected_ID)._pos.z = rand_float();
            box_map.at(last_selected_ID).update();
        }

        if (!current_selected_ID.empty()) {
            box_map.at(current_selected_ID)._clicked = true;
            box_map.at(current_selected_ID)._pos.z = 2;
            box_map.at(current_selected_ID).update();
        }
        //DELTA DE POSITION A CALCULER
        cur_pos = cursor_map_coordinates();
        //update the front selected box
        last_selected_ID = current_selected_ID;

    }

    if (!current_selected_ID.empty()) {
        //DRAG BOX
        static glm::vec3 delta;
        if (box_map.at(current_selected_ID)._clicked) {
            glm::vec3 new_pos = cursor_map_coordinates();
            delta = new_pos - cur_pos;
            //Update only if the box has moved
            if (new_pos != cur_pos) {
                box_map.at(current_selected_ID)._pos += glm::vec3{ delta.x, delta.y, 0 };
                box_map.at(current_selected_ID).update();
                link_box(box_map.at(current_selected_ID));
            }
            cur_pos = new_pos; //update the position
        }

        if (glfwGetMouseButton(window->getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE
            && box_map.at(current_selected_ID)._clicked) {
            
            box_map.at(current_selected_ID)._clicked = false;
            current_selected_ID.clear();
        }
    }
}

void Visualizer::line_drag_link()
{

    static glm::vec3 first_cursor_pos;
    //CHECK THE MAP FOR A COLLISION WITH A BOX 
    if (_states == V_STATES::DEFAULT) {
        if (first_link_ID.empty() && glfwGetMouseButton(window->getGLFWwindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            clicked_box_ID(first_link_ID);

            //CUTLINE OPTION
            if (first_link_ID.empty()) {
                _states = V_STATES::CUTLINE;

                //Begin the cut line 
                first_cursor_pos = cursor_map_coordinates();
                arrow_map.insert(std::make_pair("CUTLINE", Polyline::Segment(first_cursor_pos, first_cursor_pos)));

            }
        }
    }



    //switch (_states) {
    //case V_STATES::DEFAULT: break;
    //case V_STATES::CUTLINE: break;
    //case V_STATES::MULTI_SELECT: break;
    //

    //}

    if (_states == V_STATES::CUTLINE) {
        glm::vec3 second_cursor_pos = cursor_map_coordinates();
        arrow_map.at("CUTLINE") = Polyline::Segment(first_cursor_pos, second_cursor_pos);

        if (glfwGetMouseButton(window->getGLFWwindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
            arrow_map.erase("CUTLINE");
            static std::vector<std::pair<std::string, std::string>> cut_lines_selection;

            for (auto it = box_map.begin(); it != box_map.end(); it++) {
                Box *b1 = &it->second;
                glm::vec3 offset{ 0, 400, 0 }; //TODO

                for (std::string s : b1->link_to) {
                    Box *b2 = &box_map.at(s);
                    if (cubic_bezier_segment_intersection(b1->center(), b1->center() - offset,
                        b2->center() + offset, b2->center(),
                        first_cursor_pos, second_cursor_pos)) {
                        cut_lines_selection.emplace_back(std::make_pair(b1->id, b2->id));
                    }  
                }
            }

            for (size_t i = 0; i < cut_lines_selection.size(); i++) {
                pop_link(box_map.at(cut_lines_selection[i].first), box_map.at(cut_lines_selection[i].second));
            }

            cut_lines_selection.clear();

            _states = V_STATES::DEFAULT;
        }        
    }


    if (_states == V_STATES::DEFAULT) {
        if (!first_link_ID.empty()) {
            link_box_to_cursor(box_map.at(first_link_ID));

            if (glfwGetMouseButton(window->getGLFWwindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
                if ((first_link_ID != clicked_box_ID(second_link_ID)) && !second_link_ID.empty()) {

                    //First look out if the link between boxes already exists
                    auto it = std::find(box_map.at(first_link_ID).link_to.begin(), box_map.at(first_link_ID).link_to.end(), second_link_ID);

                    if (it != box_map.at(first_link_ID).link_to.end()) {
                        //If it already exists delete it
                        //Erase the arrows connected to the box
                        //Erase the ID from their 'Link to' and 'Link from' list
                        arrow_map.erase(first_link_ID + second_link_ID);
                        box_map.at(first_link_ID).link_to.remove(second_link_ID);
                        box_map.at(second_link_ID).link_from.remove(first_link_ID);
                    }
                    else {
                        //If it doesn't, create the link between the two boxes
                        link_box(box_map.at(first_link_ID), box_map.at(second_link_ID));
                    }
                }

                arrow_map.erase(first_link_ID);
                first_link_ID.clear();
                second_link_ID.clear();
                return;
            }
        }
    }

}

void Visualizer::multi_select_drag()
{
    static glm::vec3 cur_pos;

    //CHECK THE MAP FOR A COLLISION WITH A BOX 
    if (current_selected_ID.empty() && glfwGetMouseButton(window->getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

        //Retrieve the ID of the current selected box
        clicked_box_ID(current_selected_ID);

        if (!last_selected_ID.empty() && (last_selected_ID != current_selected_ID)) {
            //Reset the Z offset for priority 
            box_map.at(last_selected_ID)._pos.z = rand_float();
            box_map.at(last_selected_ID).update();
        }

        if (!current_selected_ID.empty()) {
            box_map.at(current_selected_ID)._clicked = true;
            box_map.at(current_selected_ID)._pos.z = 2;
            box_map.at(current_selected_ID).update();
        }
        //DELTA DE POSITION A CALCULER
        cur_pos = cursor_map_coordinates();
        //update the front selected box
        last_selected_ID = current_selected_ID;

    }

    if (!current_selected_ID.empty()) {
        //DRAG BOX
        static glm::vec3 delta;
        if (box_map.at(current_selected_ID)._clicked) {
            glm::vec3 new_pos = cursor_map_coordinates();
            delta = new_pos - cur_pos;
            //Update only if the box has moved
            if (new_pos != cur_pos) {
                box_map.at(current_selected_ID)._pos += glm::vec3{ delta.x, delta.y, 0 };
                box_map.at(current_selected_ID).update();
                link_box(box_map.at(current_selected_ID));
            }

            cur_pos = new_pos; //update the position
        }

        if (glfwGetMouseButton(window->getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE
            && box_map.at(current_selected_ID)._clicked) {

            box_map.at(current_selected_ID)._clicked = false;
            current_selected_ID.clear();
        }
    }
}

std::string Visualizer::clicked_box_ID(std::string& ID)
{
    for (auto it = box_map.begin(); it != box_map.end(); it++) {
        if (it->second.check_collision(cursor_map_coordinates()))
        {
            std::string on_top_box_ID = it->first;

            //Priority test for the box that is already on top for the collision test
            if (ID.empty()) {
                ID = on_top_box_ID;
            }

            if (!ID.empty() && (box_map.at(ID)._pos.z < box_map.at(on_top_box_ID)._pos.z)) {
                //Check if the current Box is on top of the already other selected box
                ID = on_top_box_ID;
            }
        }
    }
    return ID;
}
