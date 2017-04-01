//////////////////////////////////////////////////////////////////////////
// godefarig, (kind of) spherical terrain generator                     //
// Copyright (C) 2016-2017 mooss <mooss@protonmail.com>                 //
// This file is part of godefarig.                                      //
//                                                                      //
// godefarig is free software: you can redistribute it and/or modify    //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// godefarig is distributed in the hope that it will be useful,         //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with godefarig.  If not, see <http://www.gnu.org/licenses/>.   //
//////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "display.h"
#include "graphics/drawable.h"
//#include "interaction.h"
#include "input/input_manager.h"
#include "input/controllers.h"
#include "input/glfw_interaction.h"
#include "glToggle.h"
//#define DEBUG_MODE
#include "debug.h"
#include "graphics/shader.h"
#include "fractahedron.h"
#include <boost/program_options.hpp>
#include <fstream>

#include "graphics/shading_unit.h"

using namespace std;

int main(int argc, char** argv)
{
//########################################################
//###################### arguments #######################
//####                                                ####

    namespace po = boost::program_options;

    string config_file;

    po::options_description base_options("generic options");
    base_options.add_options()
        ("stage", po::value<unsigned int>()->default_value(7), "set the level of recursion")
        ("draw_stage", po::value<unsigned int>(), "set the stage at which the planet is initially drawn");
    
    po::options_description camera_options("camera options");
    camera_options.add_options()
        ("camera", po::value<string>()->default_value("south-north"), "select the camera")
        ("spawn", po::value<string>()->default_value("standard"), "set the spawn location (standard, eagle)");

    po::options_description display_options("display options");
    display_options.add_options()
        ("width", po::value<unsigned int>(), "width of the window")
        ("height", po::value<unsigned int>(), "height of the window");

    po::options_description hidden_options;
    hidden_options.add_options()
        ("radius", po::value<double>()->default_value(1), "radius of the pseudo-sphere")
        ("got_mode", "enable game of thrones mode for height coloration");
    

    po::options_description generation_options("generation strategies");
    generation_options.add_options()
        ("coloration", po::value<string>()->default_value("height"), "specifies a coloration strategy (height, rasterised)")
        ("repartition", po::value<string>()->default_value("polar"), "specifies a repartition strategy (polar, octahedral)")
        ("elevation", po::value<string>()->default_value("midpoint"), "specifies an elevation strategy (midpoint, diamond, off)")
        ("seed", po::value<unsigned long>(), "seed for midpoint or diamond elevation (leave empty for random seed)")
        ("range", po::value<double>(), "height range")
        ("first", po::value<unsigned int>()->default_value(3), "first stage to which the elevation algorithm applies")
        ("sea_level", po::value<double>()->default_value(0), "adjust the sea level for height coloration (relative to the median elevation)")
        ("post_process", po::value<string>()->default_value("none"), "set post-processing operations (surface_waters)");

    po::options_description command_line_specific("command line only options");
    command_line_specific.add_options()
        ("config", po::value<string>(&config_file)->default_value("res/default.gfg"), "name of the configuration file")
        ("help,h", "print this help message");

    po::options_description generic_options;//options that are visible and configurable in a file
    generic_options
        .add(base_options)
        .add(camera_options)
        .add(generation_options)
        .add(display_options);

    po::options_description config_file_options;
    config_file_options.add(generic_options).add(hidden_options);

    po::options_description visible_options;
    visible_options.add(generic_options).add(command_line_specific);

    po::options_description command_line_options;
    command_line_options.add(visible_options).add(hidden_options);

    po::variables_map varmap;
    try
    {
        po::store(po::parse_command_line(argc, argv, command_line_options), varmap);
        //command line take precedence over config file
        //cout << generation;
        po::notify(varmap);

        ifstream ifs(config_file.c_str());
        if(!ifs)
        {
            cerr << "unable to open config file: " << config_file << endl << "pursuing anyway" << endl;
        }
        else
        {
            po::store(po::parse_config_file(ifs, config_file_options), varmap);
            po::notify(varmap);
        }
    }
    catch(po::error& e)
    {
        cerr << "error: " << e.what() << endl;
        return 0;
    }

    if(varmap.count("help"))
    {
        cout << visible_options << "\n" << endl;
        return 0;
    }
        
//#############################################
//################## opengl ###################
//####                                     ####
    using namespace gfg;
    
    gfg::simple_chrono mesure;
    std::cout << "octal creation...";
    mesure.start();
    gfg::fractal_octahedron octa(varmap);
    mesure.stop();
    std::cout << " it took " << mesure.elapsed_time() << " seconds" << std::endl;

    Display display("godefarig");
    display.create();
    
    gfg::Shader lamp_shader("res/lamp_shader");
    //gfg::Shader planet_shader("res/planet_shader_phong");

    glm::vec3 lightPosition(6.0f, 0.0f, 0.0f);
    gfg::cube lightCube(0.5);
    Model light_model(glm::translate(glm::mat4(), lightPosition));

    unsigned int draw_stage;
    if(varmap.count("draw_stage"))
        draw_stage = varmap["draw_stage"].as<unsigned int>();
    else
        draw_stage = varmap["stage"].as<unsigned int>();

    /////////////////////////////////
    // shading_unit initialisation //
    /////////////////////////////////
    auto fractal_planet( std::make_shared<drawable_octal>(octa, draw_stage) );
    std::shared_ptr<gfg::camera> camera = gfg::camera::factory(varmap);
    auto projection_matrix( std::make_shared<Projection>(display.width(), display.height()) );
    graphics::shading_unit planet("res/planet_shader_phong", camera, projection_matrix);

    auto planet_model( std::make_shared<Model>() );

    auto planet_drawer( std::make_shared<graphics::drawer_single>( fractal_planet, planet_model) );

    planet.add_drawer(planet_drawer);
    planet.add_uniform( "light_color", glm::vec3(0.6, 1.0, 1.0) );
    planet.add_uniform( "light_position", -lightPosition );
    planet.add_uniform( "normal_model", glm::transpose(glm::inverse(glm::mat3( planet_model->matrix()))));

    lamp_shader.bind();
    UniformMat4f
        lampModel( lamp_shader.program(), "model", light_model.ptr() ),
        lampView( lamp_shader.program(), "view", camera->ptr() ),
        lampProjection( lamp_shader.program(), "projection", projection_matrix->ptr());

    //todo: LOWERCASE THIS SHIT

//#####################################################################
//######################## input management ###########################
//#####                                                           #####
    gfg::input::glSettings gfg_gl_settings;
    using namespace gfg::input;
    using namespace gfg::control;
    input_manager inputs;
    init_glfw_interaction(display.window(), &inputs);
    
    camera_controller cam_control(
        *camera,
        inputs,
        key::w,
        key::a,
        key::s,
        key::d
        );

    gl_options_controller opt_control(
        gfg_gl_settings,
        inputs,
        key::p,
        key::o,
        key::i,
        key::u,
        key::l,
        key::k
        );

    draw_stage_controller stg_control(
        *fractal_planet,
        inputs,
        key::x,
        key::z
        );

    fov_controller fov_control(
        inputs,
        *projection_matrix,
        planet.projection(),
        planet.shader()
        );//todo: une classe englobant Shader, UniformMat4f et Projection
    
    unsigned int nbDraw=0;
    GLfloat nbSecInit = glfwGetTime();
    //glClearColor(0.15, 0.15, 0.15, 0);//dark gray
    //glClearColor(0.4, 0.007843137, 0.235294118, 0);//tyrian purple
                
    // Game loop
    while (!glfwWindowShouldClose(display.window()))
    {
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        inputs.update_deltas();
        inputs.check();
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // interface.check();
        camera->update();//todo: automatically update via observer

        //drawing octaworld
        // planet.shader().bind();
        //planet.view().update();
//        model.update();//currently only one model
        //fractal_planet.draw();
        planet.draw();

        //drawing lamp
        lamp_shader.bind();
        lampView.update();
        lightCube.draw();
        
        ++nbDraw;
        
        // Swap the screen buffers
        glfwSwapBuffers(display.window());
    }
    GLfloat nbSecFinal = glfwGetTime() - nbSecInit;
    cout << nbDraw << " dessins en " << nbSecFinal << " secondes ; soit " << nbDraw / nbSecFinal << " dessins à la seconde" << endl;
    return EXIT_SUCCESS;
}
