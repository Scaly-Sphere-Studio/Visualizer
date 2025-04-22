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
        float S = SSS::Math::signum(R + std::sqrt(D)) * std::pow(std::abs(R + std::sqrt(D)), (1.0f / 3.0f));
        float T = SSS::Math::signum(R - std::sqrt(D)) * std::pow(std::abs(R - std::sqrt(D)), (1.0f / 3.0f));

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

    //TODO Check if the data exists
    parse_info_data_visualizer_from_json("save.json");
    setup();

    // FIRST SETUP OPERATION
    // Fill the languages ISO code map 
    std::string iso_file = "iso_codes/iso.json";
    if (check_folder_exists(iso_file)) {
        iso_map = retrieve_iso_codes(iso_file);
        SSS::log_msg("ISO File found");
    }

    start = std::chrono::steady_clock::now();
}

Visualizer& Visualizer::get()
{
    static auto const singleton(new Visualizer());
    return *singleton;
}

Visualizer::~Visualizer()
{
    arrow_map.clear();
    _proj.box_map.clear();
    line_renderer.reset();
    box_renderer.reset();
    debug_renderer.reset();
    auto win = SSS::GL::Window::get(glfwwindow);
    if (win)
        win->close();
}

void Visualizer::run()
{
    //load
    load();

    SSS::GL::Window* window = SSS::GL::Window::get(glfwwindow);
    SSS::ImGuiH::setContext(glfwwindow);



    refresh();

    clear_color = hex_to_rgb("#4d5f83");

    glEnable(GL_DEPTH_TEST);
    glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0f);

    // Main loop
    while (!window->shouldClose()) {
        SSS::GL::pollEverything();
        glfwGetCursorPos(glfwwindow, &c_x, &c_y);
        input();


        //Collision test
        switch (_states) {
        case V_STATES::DRAG_BOX: {
            drag_boxes();
            break;
        }
        case V_STATES::MULTI_SELECT: {
            multi_select();
            break;
        }
        case V_STATES::CUTLINE: {
            cut_link_line();
            break;
        }
        case V_STATES::CONNECT_LINE: {
            connect_drag_line();
            break;
        }
        case V_STATES::DRAG_SCREEN: {
            drag_screen();
            break;
        }
        }

        window->drawObjects();
        menu_bar();

        window->printFrame();
    }

}


void Visualizer::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Visualizer& visu = get();

    if (key == GLFW_KEY_KP_0 || key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, true);
    }

    if (mods == GLFW_MOD_CONTROL && key == GLFW_KEY_S && action == GLFW_PRESS) {
        visu.save();
    }

    if (mods == GLFW_MOD_CONTROL && key == GLFW_KEY_Q && action == GLFW_PRESS) {
        //Select all
        visu._selectedBoxes.clear();
        for (auto [id, b] : visu._proj.box_map) {
            visu._selectedBoxes.emplace(b);
        }
    }


}

void Visualizer::mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
}

void Visualizer::scroll_callback(GLFWwindow* window, double x, double y)
{
    SSS::GL::Camera::Shared camera = get().camera;
    float zoom = camera->getZoom();
    float const coeff = 1.f + std::abs(static_cast<float>(y)) * 0.05f;
    if (y > 0.f)
        zoom *= coeff;
    else
        zoom /= coeff;
    if (std::abs(zoom - 1.f) < 0.045f)
        zoom = 1.f;
    camera->setZoom(zoom);
}

void Visualizer::resize_callback(GLFWwindow* win, int w, int h)
{
    get()._info._w = static_cast<float>(w);
    get()._info._h = static_cast<float>(h);
}

void Visualizer::setup()
{
    SSS::GL::Window::CreateArgs args;
    args.title = "VISUALIZER";
    args.w = static_cast<int>(_info._w);
    args.h = static_cast<int>(_info._h);
    SSS::GL::Window& window = SSS::GL::Window::create(args);
    glfwwindow = window.getGLFWwindow();

    GUI_Layout layout;
    //ID FORMAT
    layout._fmt.charsize = 13;
    layout._fmt.text_color = 0x000000;
    layout._fmt.line_spacing = 1.f;
    layout._marginh = 4;
    layout._marginv = 5;
    Box::layout_map.insert(std::make_pair("ID", layout));
    //TEXT FORMAT
    layout._fmt.line_spacing = 1.5f;
    layout._fmt.charsize = 19;
    layout._fmt.text_color = 0x111111;
    layout._marginh = 5;
    layout._marginv = 10;
    Box::layout_map.insert(std::make_pair("TEXT", layout));
    //TEXT FORMAT
    layout._fmt.charsize = 16;
    layout._fmt.line_spacing = 1.3f;
    layout._fmt.font = "Ariali.ttf";
    layout._fmt.text_color = 0x333333;
    Box::layout_map.insert(std::make_pair("COMMENT", layout));

    window.setVSYNC(true);
    window.setCallback(glfwSetWindowSizeCallback, resize_callback);
    window.setCallback(glfwSetKeyCallback, key_callback);
    window.setCallback(glfwSetMouseButtonCallback, mouse_callback);
    window.setCallback(glfwSetScrollCallback, scroll_callback);

    camera = SSS::GL::Camera::create();
    camera->setPosition({ 0, 0, 20.f });
    camera->setZFar(40.f);
    camera->setProjectionType(SSS::GL::Camera::Projection::OrthoFixed);

    auto texture = SSS::GL::Texture::create();
    texture->setColor(SSS::RGBA32(200, 220, 240, 80));
    Selection_box = SSS::GL::Plane::create(texture);

    line_renderer = SSS::GL::LineRenderer::create();
    line_renderer->camera = camera;

    box_renderer = SSS::GL::PlaneRenderer::create();
    box_renderer->camera = camera;

    selection_renderer = SSS::GL::PlaneRenderer::create();
    selection_renderer->camera = camera;
    selection_renderer->addPlane(Selection_box);
    selection_renderer->setActivity(false);

    debug_renderer = Debugger::create();
    debug_renderer->setShaders(SSS::GL::Shaders::create("glsl/triangle.vert", "glsl/triangle.frag"));
    debug_renderer->camera = camera;
    // Enable or disable debugger
    debug_renderer->setActivity(false);

    window.setRenderers({ box_renderer, line_renderer, selection_renderer, debug_renderer });
}

void Visualizer::input()
{
    auto window = SSS::GL::Window::get(glfwwindow);
    auto const& keys = window->getKeyInputs();

    //INPUT CAMERA
    constexpr float speed = 10.0f;
    if (keys[GLFW_KEY_DOWN]) {
        camera->move(glm::vec3(0.0f, -speed / camera->getZoom(), 0.0f));
    }
    if (keys[GLFW_KEY_RIGHT]) {
        camera->move(glm::vec3(speed / camera->getZoom(), 0.0f, 0.0f));
    }
    if (keys[GLFW_KEY_UP]) {
        camera->move(glm::vec3(0.0f, speed / camera->getZoom(), 0.0f));
    }
    if (keys[GLFW_KEY_LEFT]) {
        camera->move(glm::vec3(-speed / camera->getZoom(), 0.0f, 0.0f));
    }
    if (keys[GLFW_KEY_SPACE].is_pressed()) {
        camera->rotate(glm::vec2(0, 180));
        camera->setPosition(camera->getPosition() * glm::vec3(1, 1, -1));
    }

    //INPUTS BOX
    if (keys[GLFW_KEY_KP_ADD].is_pressed()) {
        push_box(rgb_to_hex(rand_pastel_color()));
    }
    //TEST SUPPRESSION
    if (keys[GLFW_KEY_KP_SUBTRACT].is_pressed() && !_selectedBoxes.empty()) {
        for (Box::Shared b : _selectedBoxes) {
            pop_box(*b);
        }
        _selectedBoxes.clear();
    }

    // TEXT
    auto area = SSS::TR::Area::getFocused();
    auto const& clicks = window->getClickInputs();
    
    // DOUBLE CLIC
    if (!area && clicks[GLFW_MOUSE_BUTTON_1].is_pressed(2)) {
        window->placeHoveredTextAreaCursor();
        area = SSS::TR::Area::getFocused();
    }

    // LEFT CLIC (NO MOD)
    if (clicks[GLFW_MOUSE_BUTTON_1].is_pressed()
        && !window->keyMod(GLFW_MOD_SHIFT | GLFW_MOD_CONTROL))
    {
        BoxPlane::Shared plane = window->getHovered<BoxPlane>();
        if (!plane) {
            _states = V_STATES::DRAG_SCREEN;
            _cur_pos = glm::vec3(-c_x, c_y, 0.f);
            _selectedBoxes.clear();

            LOG_MSG("DRAG SCREEN MODE");
        }
        else if (!area) {
            Box::Shared box = plane->getBox();
            //Create a selection or switch the two IDs
            if (_selectedBoxes.size() < 2) {
                for (Box::Shared b : _selectedBoxes) {
                    //Reset the Z offset for priority 
                    if (box != b) {
                        b->setZ(0);
                    }
                }
                _selectedBoxes.clear();
                _selectedBoxes.emplace(box);
            }
            _cur_pos = cursor_map_coordinates();
            _states = V_STATES::DRAG_BOX;
        }
        else {
            _states = V_STATES::DEFAULT;
            _selectedBoxes.clear();
        }
    }

    // RIGHT CLIC (NO MOD)
    if (clicks[GLFW_MOUSE_BUTTON_2].is_pressed()
        && !window->keyMod(GLFW_MOD_SHIFT | GLFW_MOD_CONTROL))
    {
        auto box = get_hovered_box();
        _cur_pos = cursor_map_coordinates();

        if (!box) {
            _states = V_STATES::CUTLINE;
            arrow_map.try_emplace("CUTLINE");
        }
        else {
            _states = V_STATES::CONNECT_LINE;
            first_link_ID = box->_id;
        }
    }

    // CTRL LEFT CLICK
    if (clicks[GLFW_MOUSE_BUTTON_1].is_pressed() && window->keyMod(GLFW_MOD_CONTROL)) {
        auto box = get_hovered_box();
        if (box) {
            if (_selectedBoxes.contains(box)) {
                _selectedBoxes.erase(box);
            }
            else {
                _selectedBoxes.emplace(box);
            }
        }
    }

    // SHIFT LEFT CLICK
    if (clicks[GLFW_MOUSE_BUTTON_1].is_pressed() && window->keyMod(GLFW_MOD_SHIFT)) {
        _otherpos = cursor_map_coordinates();
        selection_renderer->setActivity(true);
        _states = V_STATES::MULTI_SELECT;
    }

    // SELECTION RESET
    if (_states == V_STATES::MULTI_SELECT
        && (clicks[GLFW_MOUSE_BUTTON_1].is_released() || !window->keyMod(GLFW_MOD_SHIFT)))
    {
        selection_renderer->setActivity(false);
        _states = V_STATES::DEFAULT;
    }
}

void Visualizer::refresh()
{
    for (auto it = _proj.box_map.begin(); it != _proj.box_map.end(); it++) {
        link_box(*it->second);
    }
}



void Visualizer::link_box(Box& a, Box& b)
{

    glm::vec3 offset{ 0.f, std::abs(a.getPos().y - b.getPos().y) / 2.f, 0.f };
    //Create a bezier curve to link the two boxes
    SSS::Math::Gradient<glm::vec4> Col_grdt;


    SSS::Math::Gradient<float> Thk_grdt;
    Thk_grdt.push(std::make_pair(0.f, 25.f));
    Thk_grdt.push(std::make_pair(1.f, 25.f));

    using Line = SSS::GL::Polyline;
    Line::Shared seg;
    if (std::abs(a.center().x - b.center().x) < 5.0f) {
        Col_grdt.push(std::make_pair(0.f, a.getColor()));
        Col_grdt.push(std::make_pair(1.f, b.getColor()));
        seg = Line::Segment(a.center(), b.center(), Thk_grdt, Col_grdt);
    }
    else {
        // Couleurs inversées pour Bezier ?
        Col_grdt.push(std::make_pair(0.f, b.getColor()));
        Col_grdt.push(std::make_pair(1.f, a.getColor()));
        seg = Line::Bezier(
            a.center(), a.center() - offset,
            b.center() + offset, b.center(),
            Thk_grdt, Col_grdt,
            Line::JointType::BEVEL, Line::TermType::SQUARE
        );
    }

    //Add the dst box to the 'link to' list of the src box
    //And add the src box to the 'link from' list of the dst box
    if (!a.link_to.contains(b._id)) {
        a.link_to.emplace(b._id);
    }

    if (!b.link_from.contains(a._id)) {
        b.link_from.emplace(a._id);
    }

    //The arrow ID is the cat of the two boxes ID as it keeps the order
    arrow_map[a._id + b._id] = seg;

}

void Visualizer::link_box(Box& a)
{
    for (std::string lt : a.link_to) {
        if (Box::Shared to = _proj.box_map[lt]; to)
            link_box(a, *to);
    }
    for (std::string lf : a.link_from) {
        if (Box::Shared from = _proj.box_map[lf]; from)
            link_box(*from, a);
    }
}

void Visualizer::link_box_to_cursor(Box& b)
{
    glm::vec3 c_pos = cursor_map_coordinates() + glm::vec3(0, 0, 5);

    //Create a bezier curve to link the two boxes
    SSS::Math::Gradient<glm::vec4> Col_grdt;
    Col_grdt.push(std::make_pair(0.f, glm::vec4(0.f, 0.f, 0.f, 1.f)));
    Col_grdt.push(std::make_pair(1.f, b.getColor()));

    SSS::Math::Gradient<float> Thk_grdt;
    Thk_grdt.push(std::make_pair(0.f, 25.f));
    Thk_grdt.push(std::make_pair(1.f, 25.f));

    auto seg = SSS::GL::Polyline::Bezier(
        b.center() + glm::vec3(0, 0, 5), b.center() + glm::vec3(0, -400, 5),
        c_pos, c_pos,
        Thk_grdt, Col_grdt,
        SSS::GL::Polyline::JointType::BEVEL, SSS::GL::Polyline::TermType::SQUARE
    );

    if (arrow_map.contains(first_link_ID)) {
        arrow_map.at(b._id) = seg;
        return;
    }

    //The arrow ID is the cat of the two boxes ID as it keeps the order
    arrow_map.insert(std::make_pair(b._id, seg));

}

void Visualizer::pop_link(Box& a, Box& b)
{
    arrow_map.erase(a._id + b._id);
    a.link_to.erase(b._id);
    b.link_from.erase(a._id);
}



void Visualizer::push_box(std::string boxID)
{
    glm::vec3 position = cursor_map_coordinates();
    Box::Shared b = Box::create();
    _proj.box_map[boxID] = b;
    b->setPos(position);
    b->setColor(boxID);
    b->_id = boxID;
    b->_td.text_ID = boxID;
    b->create_box();
}

void Visualizer::push_box(glm::vec3 pos, const Text_data& td)
{
    Box::Shared b = Box::create();
    _proj.box_map[td.text_ID] = b;
    b->setPos(pos);
    b->setColor(rand_color());
    b->_id = td.text_ID;
    b->set_text_data(td);
    b->create_box();
}

void Visualizer::pop_box(std::string ID)
{
    if (!_selectedBoxes.empty()) {
        //Clear the connected arrows and remove the ID from the ID lists 

        //Erase the arrows connected to the box
        //Erase the ID from their 'Link to' list
        for (std::string f_ID : _proj.box_map.at(ID)->link_from) {
            arrow_map.erase(f_ID + ID);
            _proj.box_map.at(f_ID)->link_to.erase(ID);
        }

        //Clear all the arrows connected to other boxes

        //Erase the arrows that connect to other boxes
        //Erase the ID from their 'link from' list
        for (std::string l_ID : _proj.box_map.at(ID)->link_to) {
            arrow_map.erase(ID + l_ID);
            _proj.box_map.at(l_ID)->link_from.erase(ID);
        }

        //Clear the box from the map
        box_renderer->removePlanes(_proj.box_map.at(ID)->model);
        _proj.box_map.erase(ID);
    }

    //Erase the selection
    //last_selected_ID.clear();
    //current_selected_ID.clear();
}


bool Visualizer::check_frustrum_render(Box& b)
{
    //CHECK IF A BOX IS IN THE RENDERED WINDOW TROUGH THE SELECTED CAMERA
    glm::vec3 const cam_pos = camera->getPosition();
    float const dx = glm::abs(cam_pos.x - b.getPos().x);
    float const dxmax = (b.getSize().x + _info._w) * 0.5f;
    float const dy = glm::abs(cam_pos.y - b.getPos().y);
    float const dymax = (b.getSize().y + _info._h) * 0.5f;


    if ((dx < dxmax) && (dy < dymax)) {
        return true;
    }
    return false;
}

// TODO: exporter dans GL::Window
glm::vec3 Visualizer::cursor_map_coordinates()
{
    float const zoom = camera->getZoom();
    return glm::vec3(glm::vec2(glm::inverse(camera->getView()) *
        glm::vec4((c_x - _info._w / 2.f) / zoom, (_info._h / 2.f - c_y) / zoom, 0, 1)
    ), 0);
}

void Visualizer::drag_boxes()
{
    //SI LA SELECTION EST DE 1 LE METTRE EN PRIO
    if (_selectedBoxes.size() == 1) {
        (*_selectedBoxes.cbegin())->setZ(2);
    }
    //CHECK THE MAP FOR A COLLISION WITH A BOX 

    //DRAG BOX
    static glm::vec3 delta;

    glm::vec3 const new_pos = cursor_map_coordinates();
    delta = new_pos - _cur_pos;
    //Update only if the box has moved
    if (new_pos != _cur_pos) {
        for (Box::Shared b : _selectedBoxes) {
            b->setPos(b->getPos() + glm::vec3{ delta.x, delta.y, 0 });
            link_box(*b);
        }
        _cur_pos = new_pos;
    }

    if (glfwGetMouseButton(glfwwindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        _states = V_STATES::DEFAULT;
    }
}

void Visualizer::cut_link_line()
{
    using Line = SSS::GL::Polyline;
    if (_states == V_STATES::CUTLINE) {
        glm::vec3 second_cursor_pos = cursor_map_coordinates();
        arrow_map["CUTLINE"] = Line::Segment({ _cur_pos.x, _cur_pos.y, 5.f }, second_cursor_pos, 10.f, hex_to_rgb("#03070e"), Line::JointType::BEVEL, Line::TermType::ROUND);

        if (glfwGetMouseButton(glfwwindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
            arrow_map.erase("CUTLINE");
            static std::vector<std::pair<std::string, std::string>> cut_lines_selection;

            for (auto it = _proj.box_map.begin(); it != _proj.box_map.end(); it++) {
                Box::Shared b1 = it->second;
                glm::vec3 offset{ 0, 400, 0 }; //TODO

                for (std::string s : b1->link_to) {
                    Box::Shared b2 = _proj.box_map.at(s);
                    if (cubic_bezier_segment_intersection(b1->center(), b1->center() - offset,
                        b2->center() + offset, b2->center(),
                        _cur_pos, second_cursor_pos)) {
                        cut_lines_selection.emplace_back(std::make_pair(b1->_id, b2->_id));
                    }
                }
            }

            for (size_t i = 0; i < cut_lines_selection.size(); i++) {
                pop_link(*_proj.box_map.at(cut_lines_selection[i].first), *_proj.box_map.at(cut_lines_selection[i].second));
            }

            cut_lines_selection.clear();
            _states = V_STATES::DEFAULT;
        }
    }
}

void Visualizer::connect_drag_line()
{
    if (!first_link_ID.empty()) {
        link_box_to_cursor(*_proj.box_map.at(first_link_ID));

        if (glfwGetMouseButton(glfwwindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
            second_link_ID = get_hovered_box() ? get_hovered_box()->_id : "";
            if (first_link_ID != second_link_ID && !second_link_ID.empty()) {

                //First look out if the link between boxes already exists
                auto it = std::find(_proj.box_map.at(first_link_ID)->link_to.begin(), _proj.box_map.at(first_link_ID)->link_to.end(), second_link_ID);

                if (it != _proj.box_map.at(first_link_ID)->link_to.end()) {
                    //If it already exists delete it
                    //Erase the arrows connected to the box
                    //Erase the ID from their 'Link to' and 'Link from' list
                    arrow_map.erase(first_link_ID + second_link_ID);
                    _proj.box_map.at(first_link_ID)->link_to.erase(second_link_ID);
                    _proj.box_map.at(second_link_ID)->link_from.erase(first_link_ID);
                }
                else {
                    //If it doesn't, create the link between the two boxes
                    link_box(*_proj.box_map.at(first_link_ID), *_proj.box_map.at(second_link_ID));
                }
            }

            arrow_map.erase(first_link_ID);
            first_link_ID.clear();
            second_link_ID.clear();

            _states = V_STATES::DEFAULT;
            return;
        }
    }
}

void Visualizer::multi_select()
{
    static glm::vec2 diff(0);
    glm::vec2 const new_diff = cursor_map_coordinates() - _otherpos;
    if (diff == new_diff)
        return;
    diff = new_diff;

    //MAKE THE SELECTION PARTICLE IN FRONT
    Selection_box->setTranslation(_otherpos + glm::vec3(diff / 2, 5.f));
    Selection_box->setScaling(glm::vec3(glm::abs(diff), 1.f));

    for (auto [id, box] : _proj.box_map) {
        if (box->checkCollision(Selection_box))
            _selectedBoxes.emplace(box);
        else
            _selectedBoxes.erase(box);
    }
}

void Visualizer::drag_screen()
{
    _otherpos = glm::vec3(-c_x, c_y, 0.f);
    glm::vec3 delta = _otherpos - _cur_pos;
    //Move the camera using the cursor position
    camera->move(delta / camera->getZoom());

    if (glfwGetMouseButton(glfwwindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        _states = V_STATES::DEFAULT;
    }

    //Update the position
    _cur_pos = _otherpos;
}

Box::Shared Visualizer::get_hovered_box()
{
    if (auto window = SSS::GL::Window::get(glfwwindow); window) {
        if (auto plane = window->getHovered<BoxPlane>(); plane)
            return plane->getBox();
    }
    return nullptr;
}

void Visualizer::parse_info_data_visualizer_to_json(const std::string& path, const bool prettify)
{
    nlohmann::json dst;
    dst = this->_info;

    std::ofstream ofs(path);
    if (prettify) {
        ofs << std::setw(4) << dst << std::endl;
    }
    else {
        ofs << dst << std::endl;
    }
    ofs.close();
}

void Visualizer::parse_info_data_project_from_json(const std::string& path)
{
    std::ifstream ifs(path);
    nlohmann::json tmp;
    ifs >> tmp;
    ifs.close();

    this->_proj = tmp;
}

void Visualizer::parse_info_data_project_to_json(const std::string& path, const bool prettify)
{
    nlohmann::json dst;
    dst = this->_proj;

    std::ofstream ofs(path);
    if (prettify) {
        ofs << std::setw(4) << dst << std::endl;
    }
    else {
        ofs << dst << std::endl;
    }
    ofs.close();
}

void Visualizer::parse_info_data_visualizer_from_json(const std::string& path)
{
    std::ifstream ifs(path);
    nlohmann::json tmp;
    ifs >> tmp;
    ifs.close();

    this->_info = tmp;
}

void Visualizer::save()
{
    LOG_MSG("SAVED");
    
    std::string data_str = "data.json";
    parse_info_data_project_to_json(data_str, true);
    std::string str = "save.json";
    parse_info_data_visualizer_to_json(str, true);


    //Todo save the text data in their own file
    for (auto it = _proj.box_map.begin(); it != _proj.box_map.end(); ++it) {

    }
}

void Visualizer::load()
{
    //LOAD THE PROJECT INFORMATIONS : LANGUAGE, LAST LOADED LANGUAGE...
    _ti.parse_info_data_from_json("project/bohemian/bohemian.ini");
    _fl = _ti.fl;
    //LOAD THE TEXT DATA FROM TRANSLATOR
    _mt.parse_traduction_data_from_json("project/bohemian/bohemian_eng.json");
    //LOAD THE PROJECT DATA FOR VIZUALIZER : BOX POS...
    parse_info_data_project_from_json("data.json");
    SSS::GL::Window::get(glfwwindow)->setTitle("VIZUALIZER - " + _proj.project_name);

    //COMPARE THE TWO FILES, AND ADD MISSING BOXES INTO
    float i = 0;
    for (const Text_data& td : _mt.text_data) {
        if (!_proj.box_map.contains(td.text_ID)) {
            push_box(glm::vec3(i*5.0f,-i*5.0f, 10.f),td);
            i += 1.f;
        }
        else
            _proj.box_map[td.text_ID]->set_text_data(td);
    }


    LOG_MSG("LOADED");
}

void Visualizer::menu_bar()
{
    if (!SSS::ImGuiH::newFrame())
        return;

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_MenuBar;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoDecoration;
    window_flags |= ImGuiWindowFlags_NoBackground;


    if (ImGui::Begin("MENUBAR", NULL, window_flags))
    {
        ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
        ImGui::SetWindowSize(ImVec2(_info._w, 0), ImGuiCond_Always);

        ImGui::PushItemWidth(ImGui::GetFontSize());

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Menu"))
            {
                //SAVE CURRENT PROGRESSION
                if (ImGui::MenuItem("Save"))
                {
                    SSS::log_msg("FILE SAVED");
                    save();
                }

                ImGui::EndMenu();
            }

            language_selector();
            mode_selector();
        
            ImGui::EndMenuBar();
        }
        ImGui::End();
    }

    SSS::ImGuiH::render();
}


std::string Visualizer::lang_file_name(std::string& lang)
{
    std::string ext = project_path();
    ext += "/" + _project_name + "_";
    ext += lang;
    ext += ".json";

    return ext;
}

void to_json(nlohmann::json& j, const VISUALISER_INFO& t)
{
    j = nlohmann::json{
        {"HEIGHT", t._h},
        {"WIDTH", t._w},
    };
}

void from_json(const nlohmann::json& j, VISUALISER_INFO& t)
{
    j.at("HEIGHT").get_to(t._h);
    j.at("WIDTH").get_to(t._w);

}

void to_json(nlohmann::json& j, const PROJECT_DATA& t)
{
    j = nlohmann::json{
        {"BOX", t.box_map},
        {"PROJECT_NAME", t.project_name}
    };
}

void from_json(const nlohmann::json& j, PROJECT_DATA& t)
{
    j.at("BOX").get_to(t.box_map);
    j.at("PROJECT_NAME").get_to(t.project_name);
}

PROJECT_DATA::~PROJECT_DATA()
{
    box_map.clear();
}


void Visualizer::language_selector()
{
    ImGui::Text("        Mode ");
    float width = 150.f;
    ImGui::SetNextItemWidth(width);

    //initialize to the first item of the iso languages list
    static std::string item_current_idx = _ti.mother_language;
    //reviewed option for the first slider
    const char* combo_preview_value = iso_map[item_current_idx].c_str();
    static std::string iterator = "";

    if (ImGui::BeginCombo("##MODE SELECTOR", combo_preview_value))
    {
        for (auto& m : _ti.trad_languages)
        {
            iterator = m.first;
            const bool is_selected = (item_current_idx == iterator);

            if (ImGui::Selectable(iso_map[m.first].c_str(), is_selected))
            {

            }

            if (is_selected) {
                ImGui::SetItemDefaultFocus();
                //TODO LOAD THE SPECIFIC FILE
            }
        }

        ImGui::EndCombo();
    }    
}

void Visualizer::mode_selector()
{
    ImGui::Text("        Language ");
    float width = 150.f;
    ImGui::SetNextItemWidth(width);

    //initialize to the first item of the iso languages list
    static std::string item_current_idx = _ti.mother_language;
    //reviewed option for the first slider
    const char* combo_preview_value = iso_map[item_current_idx].c_str();
    static std::string iterator = "";

    if (ImGui::BeginCombo("##first_Language", combo_preview_value))
    {
        for (auto& m : _ti.trad_languages)
        {
            iterator = m.first;
            const bool is_selected = (item_current_idx == iterator);

            if (ImGui::Selectable(iso_map[m.first].c_str(), is_selected))
            {

            }

            if (is_selected) {
                ImGui::SetItemDefaultFocus();
                //TODO LOAD THE SPECIFIC FILE
            }
        }

        ImGui::EndCombo();
    }
}

std::string Visualizer::project_path()
{
    std::string file_path = _translation_folder_path;
    file_path += "/" + _project_name;

    return file_path;
}