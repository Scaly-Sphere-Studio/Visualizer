#include "../inc/visualizer.h"

static const GLfloat g_vertex_buffer_data[] = {
 0.0f, -1.0f, 0.0f,
 1.0f, -1.0f, 0.0f,
 0.0f, 0.0f, 0.0f,
 1.0f, 0.0f, 0.0f,
};

Visualizer::Visualizer()
{
    //TODO RANDSEED 
    std::srand(std::time(nullptr));
    setup();

    
    Box b1 = Box(glm::vec3(0.0f, 400.0f, 0.0f), glm::vec2(300.0f,150.0f), "1E1022");
    Box b2 = Box(glm::vec3(700.0f, 0.0f, 0.0f), glm::vec2(300.0f,150.0f), "0E2556");

    b1.id = i1;
    b2.id = i2;

    box_map.insert(std::make_pair(i1, b1));
    box_map.insert(std::make_pair(i2, b2));

    link_box(box_map.at(i1), box_map.at(i2));
    link_box(box_map.at(i2), box_map.at(i1));

    clear_color = hex_to_rgb("#4d5f83");
}

Visualizer::~Visualizer()
{

    Box::box_batch.clear();
    arrow_map.clear();
    box_map.clear();
}

void Visualizer::run()
{
    

    //UPDATES THE BUFFER
    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &particles_data);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_DEPTH_TEST);
    glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0f);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glfwGetCursorPos(window, &c_x, &c_y);
        input();
        
        
        glClear(GL_COLOR_BUFFER_BIT);


        //Gen batch trough the frustrum
        //FRUSTRUM 
        frustrum_test();

        //TEST CURSOR DRAG
        //UPDATE MVP MATRIX
        view = glm::lookAt(
            cam_pos,
            glm::vec3(cam_pos.x, cam_pos.y, 0),
            glm::vec3(0, 1, 0)
        );
        mvp = projection * view;

        //Collision test
        drag_boxes();
        line_drag_link();
        //multi_select_drag();

        //Sort every frame for now, until I have a real frustrum calling that sort before selecting
        std::sort(Box::box_batch.begin(), Box::box_batch.end(), sort_box);



        //LINE RENDERER
        {
            glUseProgram(line_shader_ID);
            MatrixID = glGetUniformLocation(line_shader_ID, "u_MVP");
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
            Line_Batch::render();
        }

        //BOX RENDERER
        {
            glBindVertexArray(VertexArrayID);
            glUseProgram(Box::box_shader);
            if (Box::box_batch.size() > 0) {
                glBindBuffer(GL_ARRAY_BUFFER, particles_data);
                glBufferData(GL_ARRAY_BUFFER, sizeof(testBox) * Box::box_batch.size(), Box::box_batch.data(), GL_STATIC_DRAW);            
            }
            
            MatrixID = glGetUniformLocation(Box::box_shader, "u_MVP");
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
            
            draw();
        }

        //DEBUG RENDERER
        {
            if (debug.debugmode) { debug.rectangle(cam_pos.x - w_w / 2 + 1, cam_pos.y + w_h / 2, w_w - 1, w_h - 1); }
            for (auto it = box_map.begin(); it != box_map.end(); it++) {
                debug.debug_box(it->second);
            }
            float cursor_size = 5;
            //ORIGIN CURSOR
            debug.cross(0, 0, 0, cursor_size);
            debug.circle(0, 0, 0, cursor_size);
            glBindBuffer(GL_ARRAY_BUFFER, debug.debug_vb);
            glBufferData(GL_ARRAY_BUFFER,
                debug.debug_batch.size() * sizeof(debug_Vertex),
                debug.debug_batch.data(),
                GL_STATIC_DRAW);
            glUseProgram(debug.debugID);
            MatrixID = glGetUniformLocation(debug.debugID, "u_MVP");
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
            debug.debug_show(debug.debug_vb, debug.debug_batch.data(), debug.debug_batch.size());
        }





        glfwSwapBuffers(window);
        debug.debug_batch.clear();
        Box::box_batch.clear();
    }

}



void Visualizer::draw()
{

    //Render
    //// 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glVertexAttribPointer(
        0,
        3, GL_FLOAT, GL_FALSE,
        sizeof(float) * 3, (void*)0
    );

    //Particles
    int first = 1, second = 2, third = 3;

    glBindBuffer(GL_ARRAY_BUFFER, particles_data);



    //Size separate by width and height
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        2, GL_FLOAT, GL_FALSE,
        sizeof(testBox), (void*)(sizeof(glm::vec3))
    );

    //Color
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,
        4, GL_FLOAT, GL_FALSE,
        sizeof(testBox), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2))
    );

    //Position
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
        3,
        3, GL_FLOAT, GL_FALSE,
        sizeof(testBox), (void*)0
);

glVertexAttribDivisor(0, 0);
glVertexAttribDivisor(1, 1);
glVertexAttribDivisor(2, 1);
glVertexAttribDivisor(3, 1);


// Draw the triangle !
glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, Box::box_batch.size());

glDisableVertexAttribArray(0);
glDisableVertexAttribArray(1);
glDisableVertexAttribArray(2);
glDisableVertexAttribArray(3);

}

void Visualizer::input()
{

    //INPUT CAMERA
    float speed = 10.0f;

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        cam_pos -= glm::vec3(0.0f, speed, 0.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        cam_pos += glm::vec3(speed, 0.0f, 0.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        cam_pos += glm::vec3(0.0f, speed, 0.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        cam_pos -= glm::vec3(speed, 0.0f, 0.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }


    //INPUTS BOX
    //TEST AJOUT
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
        push_box(rand_color());
    }

    //TEST SUPPRESSION
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
        pop_box(last_selected_ID);
    }



    //INPUT WINDOW
    if (glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

}



void Visualizer::link_box(Box& a, Box& b)
{

    glm::vec3 offset{ 0,400,0 };
    //Create a bezier curve to link the two boxes
    Gradient<glm::vec4> Col_grdt;
    Col_grdt.push(std::make_pair(0.0, glm::vec4(a._color)));
    Col_grdt.push(std::make_pair(1.0, glm::vec4(b._color)));

    Gradient<float> Thk_grdt;
    Thk_grdt.push(std::make_pair(0.0, 25));
    Thk_grdt.push(std::make_pair(1.0, 25));

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
    glm::vec3 c_pos = glm::vec3{ (c_x - w_w / 2), (w_h / 2 - c_y), 0.0 } + cam_pos;

    //Create a bezier curve to link the two boxes
    Gradient<glm::vec4> Col_grdt;
    Col_grdt.push(std::make_pair(0.0, glm::vec4(b._color)));
    Col_grdt.push(std::make_pair(1.0, glm::vec4(0, 0, 0, 1.0)));

    Gradient<float> Thk_grdt;
    Thk_grdt.push(std::make_pair(0.0, 25));
    Thk_grdt.push(std::make_pair(1.0, 25));

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
    glm::vec3 position = glm::vec3{ (c_x - w_w / 2) + cam_pos.x, (w_h / 2 - c_y) + cam_pos.y, 0.0f };
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

void Visualizer::setup()
{
    if (!glfwInit()) {
        std::cout << "couldn't init glfw" << std::endl;
    }

    window = glfwCreateWindow(w_w, w_h, "VISUALISEUR", NULL, NULL);
    if (!window)
    {

    }

    glfwMakeContextCurrent(window);
    glfwSetWindowPos(window, 200, 100);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    // Enable blending (transparency)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable depth test
    glDepthFunc(GL_LESS);

    //GL TRIANGLE

    VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    glGenBuffers(1, &vertexbuffer);
    glGenBuffers(1, &debug.debug_vb);

    Box::box_shader = LoadShaders("glsl/instance.vert", "glsl/instance.frag");
    line_shader_ID = LoadShaders("glsl/line.vert", "glsl/line.frag");
    debug.debugID = LoadShaders("glsl/triangle.vert", "glsl/triangle.frag");
    
    //CAMERA SETUP AND CANVAS
    projection = glm::ortho(-static_cast<float>(w_w) / 2, static_cast<float>(w_w) / 2, -static_cast<float>(w_h) / 2, static_cast<float>(w_h) / 2, 0.0f, 100.0f);
}



bool Visualizer::check_frustrum_render(Box &b)
{
    //CHECK IF A BOX IS IN THE RENDERED WINDOW TROUGH THE SELECTED CAMERA
    float dx = glm::abs(cam_pos.x - b._pos.x);
    float dxmax = (b._size.x + w_w) * 0.5;    
    float dy = glm::abs(cam_pos.y - b._pos.y);
    float dymax = (b._size.y + w_h) * 0.5;


    if ((dx < dxmax) && (dy < dymax)) {
        return true;
    }
    return false;
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
    if (current_selected_ID.empty() && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

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
        cur_pos = glm::vec3(c_x, c_y, 0);
        //update the front selected box
        last_selected_ID = current_selected_ID;

    }

    if (!current_selected_ID.empty()) {
        //DRAG BOX
        static glm::vec3 delta;
        if (box_map.at(current_selected_ID)._clicked) {
            glm::vec3 new_pos = { c_x, c_y, 0 };
            delta = new_pos - cur_pos;
            //Update only if the box has moved
            if (new_pos != cur_pos) {
                box_map.at(current_selected_ID)._pos += glm::vec3{ delta.x, -delta.y, 0 };
                box_map.at(current_selected_ID).update();
                link_box(box_map.at(current_selected_ID));
            }
            
            cur_pos = new_pos; //update the position
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE
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
        if (first_link_ID.empty() && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            clicked_box_ID(first_link_ID);

            //CUTLINE OPTION
            if (first_link_ID.empty()) {
                _states = V_STATES::CUTLINE;

                //Begin the cut line 
                first_cursor_pos = glm::vec3{ (c_x - w_w / 2), (w_h / 2 - c_y), 0.0 } + cam_pos;
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
        glm::vec3 second_cursor_pos = glm::vec3{ (c_x - w_w / 2), (w_h / 2 - c_y), 5.0 } + cam_pos;


        arrow_map.at("CUTLINE") = Polyline::Segment(first_cursor_pos, second_cursor_pos);


        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
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

            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
                if ((first_link_ID != clicked_box_ID(second_link_ID)) && !second_link_ID.empty()) {

                    //First look out if the link between boxes already exists
                    auto it = box_map.at(first_link_ID).link_to.begin();
                    std::find(box_map.at(first_link_ID).link_to.begin(), box_map.at(first_link_ID).link_to.end(), second_link_ID);

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
    if (current_selected_ID.empty() && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

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
        cur_pos = glm::vec3(c_x, c_y, 0);
        //update the front selected box
        last_selected_ID = current_selected_ID;

    }

    if (!current_selected_ID.empty()) {
        //DRAG BOX
        static glm::vec3 delta;
        if (box_map.at(current_selected_ID)._clicked) {
            glm::vec3 new_pos = { c_x, c_y, 0 };
            delta = new_pos - cur_pos;
            //Update only if the box has moved
            if (new_pos != cur_pos) {
                box_map.at(current_selected_ID)._pos += glm::vec3{ delta.x, -delta.y, 0 };
                box_map.at(current_selected_ID).update();
                link_box(box_map.at(current_selected_ID));
            }

            cur_pos = new_pos; //update the position
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE
            && box_map.at(current_selected_ID)._clicked) {

            box_map.at(current_selected_ID)._clicked = false;
            current_selected_ID.clear();
        }
    }
}

std::string Visualizer::clicked_box_ID(std::string& ID)
{
    for (auto it = box_map.begin(); it != box_map.end(); it++) {
        if (it->second.check_collision((c_x - w_w / 2.0) + cam_pos.x, (w_h / 2.0 - c_y) + cam_pos.y))
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

std::array<float, 3> Visualizer::CubicRoots(float a, float b, float c, float d)
{

    float A = b / a;
    float B = c / a;
    float C = d / a;

    float Im;

    float Q = (3.0 * B - std::pow(A, 2)) / 9.0;
    float R = (9.0 * A * B - 27.0 * C - 2.0 * std::pow(A, 3.0)) / 54.0;
    float D = std::pow(Q, 3.0f) + std::pow(R, 2.0f);    // polynomial discriminant

    std::array<float,3> t;

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
            t[1] = -1;
            t[2] = -1;
        }

    }
    else                                          // distinct real roots
    {
        float th = std::acos(R / std::sqrt(-std::pow(Q, 3)));

        t[0] = 2.0f * std::sqrt(-Q) * std::cos(th / 3.0f) - A / 3.0f;
        t[1] = 2.0f * std::sqrt(-Q) * std::cos((th + 2.0f * M_PI) / 3.0f) - A / 3.0f;
        t[2] = 2.0f * std::sqrt(-Q) * std::cos((th + 4.0f * M_PI) / 3.0f) - A / 3.0f;
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
bool Visualizer::cubic_bezier_segment_intersection(glm::vec3 b_a, glm::vec3 b_b, glm::vec3 b_c, glm::vec3 b_d,
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
        else{ s = (X[1] - s_a.y) / (s_b.y - s_a.y); }
            
        /*in bounds?*/
        if (t > 0 && t < 1.0 && s > 0 && s < 1.0) {
            return true;
        }
    }
    return false;
}

std::array<float, 4> Visualizer::BezierCoeffs(float P0, float P1, float P2, float P3)
{
    std::array<float, 4> Z;
    Z[0] = -P0 + 3.0f * P1 + -3.0f * P2 + P3;
    Z[1] = 3.0f * P0 - 6.0f * P1 + 3.0f * P2;
    Z[2] = -3.0f * P0 + 3.0f * P1;
    Z[3] = P0;

    return Z;
}
