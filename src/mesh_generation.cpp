//#include "type_name.hpp"
#include "mesh_generation.h"
#include "fractahedron.h"
#include <random>
#include "containerOverload.h"
#include "make_unique_cpp11.h"

//###################################################################
//######################## mesh generator  ##########################
//###                                                             ###

gfg::mesh_generator::mesh_generator(gfg::fractal_octahedron& fract, po::variables_map& vm):
    target_(fract),
    radius_(vm["radius"].as<double>()),
    coloration_(coloration_strategy::create(vm)),
    repartition_(repartition_strategy::create(vm)),
    elevation_(elevation_strategy::create(vm)),
    sea_level_modifier_(vm["sea_level"].as<double>()),
    surface_waters_(false)
{
    std::string opt = vm["post_process"].as<std::string>();
    if(opt == "surface_waters")
        surface_waters_ = true;
    else if(opt != "none")
        throw gfg::parameter_exception(
            "--post_process",
            {"surface_waters", "none"},
            opt);
    
    generate_elevation();
//    coloration_ = coloration_strategy::create(*this, vm);
}

gfg::mesh_generator::mesh_generator(gfg::fractal_octahedron& fract):
    target_(fract),
    radius_(1),
    coloration_(std::make_unique<rasterised_coloration>()),
    repartition_(std::make_unique<polar_repartition>()),
    elevation_(std::make_unique<no_elevation>())
{
    generate_elevation();
}

gfg::mesh_generator::mesh_generator(const gfg::mesh_generator& that):
    target_(that.target_),
    coloration_(that.coloration_->clone()),
    repartition_(that.repartition_->clone()),
    elevation_(that.elevation_->clone())
{}

const gfg::mesh_generator& gfg::mesh_generator::operator=( const gfg::mesh_generator & that)
{
    target_ = that.target_;
    coloration_ = that.coloration_->clone();
    repartition_ = that.repartition_->clone();
    elevation_ = that.elevation_->clone();
    return *this;
}

std::vector<gfg::color> gfg::mesh_generator::colors() const
{
    return coloration_->generate(*this);
}

std::vector<gfg::position> gfg::mesh_generator::positions() const
{
    auto result = repartition_->generate(*this);
    if(surface_waters_)
        for(auto& el : result)
            if(glm::length(el) < sea_level_)
                el = glm::normalize(el) * static_cast<float>(sea_level_);
    return result;
}

std::tuple<
    std::vector<gfg::position>,
    std::vector<gfg::normal>
    > gfg::mesh_generator::positions_and_normals() const
{
    auto pos = positions();
    std::vector<gfg::normal> norms(gfg::point::numberAtStage(target_.rank()));
    std::vector<gfg::cascade_node> neighbours;
    
    for(auto it = target_.nodeBegin(), end = target_.nodeEnd();
        it != end; ++it)
    {
        it->neighbours(neighbours);
        unsigned int ind = it->index();
        std::cout << "triangle normal for " << ind << std::endl;
        norms[ind] = glm::tvec3<gfg::composante>(0, 0, 0);
        for(auto neIt = neighbours.begin(), neEnd = neighbours.end();
            neIt != neEnd -1; ++neIt)
        {
            norms[ind] += glm::cross(
                pos[neIt->index()] - pos[ind],
                pos[neIt[1].index()] - pos[ind] );
/*            std::cout << "\t" << glm::cross(
                pos[neIt->index()] - pos[ind],
                pos[neIt[1].index()] - pos[ind] )
                << " (" << neIt->index() << ", " << neIt[1].index() << ")" <<  std::endl;*/
        }
        norms[ind] += glm::cross(
            pos[neighbours.back().index()] - pos[ind],
            pos[neighbours.front().index()] - pos[ind] );
/*        std::cout << "\t" << glm::cross(
            pos[neighbours.back().index()] - pos[ind],
            pos[neighbours.front().index()] - pos[ind] )
                  << " (" << neighbours.back().index() << ", " << neighbours.front().index() << ")" <<  std::endl;
                  std::cout << "raw=" << norms[ind] << " | normalised=" << glm::normalize(norms[ind]) << std::endl;*/
        norms[ind] = glm::normalize(norms[ind]);
    }
    return std::make_tuple(pos, norms);
}

void gfg::mesh_generator::generate_elevation()
{
    elevation_->generate(*this);
    auto minax = std::minmax_element(
        target_.elevations_.begin(),
        target_.elevations_.end() );

    lowest_altitude_ = *minax.first;
    highest_altitude_ = *minax.second;
    sea_level_ = (*minax.first + *minax.second) / ( 2 - sea_level_modifier_ );
}

//###################################################################
//###################### factory methods ############################
//###                                                             ###

std::unique_ptr<gfg::mesh_generator::coloration_strategy> gfg::mesh_generator::coloration_strategy::create(const po::variables_map& vm)
{
    std::string optval = vm["coloration"].as<std::string>();
    if(optval == "height")
    {
        
        std::unique_ptr<coloration_strategy> result =
            std::make_unique<height_coloration>( 0.003 );
        if(vm.count("got_mode"))
            static_cast<height_coloration*>(result.get())->reverse_color_scale();
        
        return result;
    }
    else if(optval == "rasterised")
        return std::make_unique<rasterised_coloration>();
    
    throw gfg::parameter_exception("--coloration", {"height", "rasterised"}, optval);
}

std::unique_ptr<gfg::mesh_generator::repartition_strategy> gfg::mesh_generator::repartition_strategy::create(po::variables_map const& vm)
{
    std::string optval = vm["repartition"].as<std::string>();
    if(optval == "polar")
    {
        return std::make_unique<polar_repartition>();
    }
    else if(optval == "octahedral")
    {
        return std::make_unique<octahedral_repartition>();
    }

    throw gfg::parameter_exception("--repartition", {"polar", "octahedral"}, optval);
}

std::unique_ptr<gfg::mesh_generator::elevation_strategy> gfg::mesh_generator::elevation_strategy::create(po::variables_map const& vm)
{
    std::string optval = vm["elevation"].as<std::string>();

    if(optval == "midpoint" || optval == "diamond")
    {
        unsigned int seed, first_stage;
        if(vm.count("seed"))
            seed = vm["seed"].as<unsigned int>();
        else
            seed = (std::random_device())();

        first_stage = vm["first"].as<unsigned int>();
            
        float height_range = 0.2 / (2 * first_stage);
        if(vm.count("range"))
            height_range = vm["range"].as<double>();

        if(optval == "midpoint")
            return std::make_unique<midpoint_elevation>
                (height_range, first_stage, seed);
        else
            return std::make_unique<diamond_elevation>
                (height_range, first_stage, seed);
    }
    else if(optval == "off")
    {
        return std::make_unique<no_elevation>();
    }

    throw gfg::parameter_exception("--elevation", {"midpoint", "off"}, optval);
    
}

//###################################################################
//###################### coloration strategies ######################
//###                                                             ###

gfg::mesh_generator::rasterised_coloration::rasterised_coloration(const std::array<gfg::color, 6>& init):
    initial_colors_(init)
{}

gfg::mesh_generator::rasterised_coloration::rasterised_coloration():
    rasterised_coloration({ glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1), glm::vec3(1, 0, 1), glm::vec3(1, 1, 0), glm::vec3(0, 1, 1)})
{}

std::vector<gfg::color> gfg::mesh_generator::rasterised_coloration::generate(const mesh_generator& mesh) const
{
    std::vector<gfg::color> result(gfg::point::numberAtStage(mesh.target_.rank()));

    for(unsigned int i = 0; i < gfg::point::numberAtStage(initial_stages_); ++i)
        result[i] = initial_colors_[i];
    
    for(unsigned int stage = initial_stages_; stage <= mesh.target_.rank(); ++stage)//interesting when not coloring the last stage (< instead of <=)
        for(auto it = mesh.target_.stageBegin(stage); it != mesh.target_.stageEnd(stage); ++it)
        {
            auto gen = it->generators();
            setAsMiddleOf(result[it->index()],
                          result[gen[0].index()],
                          result[gen[1].index()]);
        }
    
    return result;
}

std::unique_ptr<gfg::mesh_generator::coloration_strategy> gfg::mesh_generator::rasterised_coloration::clone() const
{
    return std::make_unique<rasterised_coloration>(*this);
}

gfg::mesh_generator::height_coloration::height_coloration(float step):
    water_(std::vector< std::tuple<float, gfg::color> >{
        //std::make_tuple(700, gfg::color(0, 0, 0)),//bullshit
        std::make_tuple(0, gfg::color(0, 0, 0)),
        std::make_tuple(15, gfg::color(0, 0, 0.07)),//abyss
        std::make_tuple(73, gfg::color(0, 0.05, 0.3)),//deep waters
//        std::make_tuple(60, gfg::color(0, 0.3, 0.5)),//shallow waters
        std::make_tuple(90, gfg::color(0, 0.6, 0.9))//surface waters
    }),
    land_{{
        //std::make_tuple(5, gfg::color(0, 0, 0)),//sand
        std::make_tuple(12, gfg::color(0.0, 0.4, 0.05)),//light green
        std::make_tuple(20, gfg::color(0.1, 0.28, 0.1)),
        std::make_tuple(35, gfg::color(0.05, 0.2, 0.05)),//dark green
        std::make_tuple(49, gfg::color(0.02, 0.15, 0)),//darker green
        std::make_tuple(60, gfg::color(0.4, 0.15, 0.)),//mountain (dirt)
        std::make_tuple(85, gfg::color(0.4, 0.4, 0.4)),//mountain (rock)
        std::make_tuple(100, gfg::color(1, 1, 1))//snow
    }},
    shore_step_(step)
{}

gfg::mesh_generator::height_coloration::height_coloration(
    gfg::color_heatmap<float> water, gfg::color_heatmap<float> land,
    float step):

    water_(water),
    land_(land),
    shore_step_(step)
{}

void gfg::mesh_generator::height_coloration::reverse_color_scale()
{
    land_.reverse();
    water_.reverse();
    std::swap(land_, water_);
}

std::unique_ptr<gfg::mesh_generator::coloration_strategy> gfg::mesh_generator::height_coloration::clone() const
{
    return std::make_unique<height_coloration>(*this);
}

std::vector<gfg::color> gfg::mesh_generator::height_coloration::generate(const gfg::mesh_generator& mesh) const
{
    auto land = land_;
    auto water = water_;

    land.translate_scale(mesh.sea_level_ + shore_step_, mesh.highest_altitude_);
    water.translate_scale(mesh.lowest_altitude_, mesh.sea_level_);

    auto final_scale = gfg::color_heatmap<float>::merge(land, water);

    std::vector<gfg::color> result(mesh.target_.node_cardinal());
    for(auto it = mesh.target_.nodeBegin(); it != mesh.target_.nodeEnd(); ++it)
        result[it->index()] = final_scale.get_color_of(mesh.target_.elevations_[it->index()]);
    return result;
}

//###################################################################
//###################### repartition strategies #####################
//###                                                             ###

std::vector<gfg::position> gfg::mesh_generator::polar_repartition::generate(const mesh_generator& mesh) const
{
    std::vector<gfg::position> result(gfg::point::numberAtStage( mesh.target_.rank()) );
    
    for(auto it = mesh.target_.nodeBegin(); it != mesh.target_.nodeEnd(); ++it)
    {
        double longitude = (2*PI / it->slice_cardinal()) * it->slice_offset();

        if(it->is_south())
            longitude = 0;
        else if(it->is_north())//todo: check if those tests are still relevant
            longitude = 2*PI;

        double latitude = (PI/ (it->slice_meta_cardinal() -1) ) * it->slice_id();
        result[it->index()] = glm::vec3(
                sin(longitude) * sin(latitude),
                cos(latitude),
                cos(longitude) * sin(latitude) ) * mesh.target_.elevations_[it->index()];        
    }//todo: create and use a fractal_octahedron::slice::iterator

    return result;
}

std::unique_ptr<gfg::mesh_generator::repartition_strategy> gfg::mesh_generator::polar_repartition::clone() const
{
    return std::make_unique<polar_repartition>(*this);
}

std::vector<gfg::position> gfg::mesh_generator::octahedral_repartition::generate(const mesh_generator& mesh) const
{
    std::vector<gfg::position> result(mesh.target_.elevations_.size());
    
    result[0] = gfg::position(0, -1, 0);
    result[1] = gfg::position(-1, 0, 0);
    result[2] = gfg::position(0, 0, -1);
    result[3] = gfg::position(1, 0, 0);
    result[4] = gfg::position(0, 0, 1);
    result[5] = gfg::position(0, 1, 0);
    
    for(unsigned int stage =1; stage <= mesh.target_.rank(); ++stage)
    {
        for(auto it = mesh.target_.stageBegin(stage), end = mesh.target_.stageEnd(stage);
            it != end; ++it)
        {
            auto gen = it->generators();
            //result[it->index()] = (result[gen[0].index()] + result[gen[1].index()])/2;
            setAsMiddleOf(result[it->index()], result[gen[0].index()], result[gen[1].index()]);
        }
    }//creating the octahedron shaped base
    
    for(auto it = mesh.target_.nodeBegin(), end = mesh.target_.nodeEnd();
        it != end; ++it)
    {
        result[it->index()] +=
            glm::normalize(result[it->index()]) *
            (mesh.target_.elevations_[it->index()] - 1);
    }//adding the height

    //todo: try with current_res *= future_length / current_length
    return result;
    
}

std::unique_ptr<gfg::mesh_generator::repartition_strategy> gfg::mesh_generator::octahedral_repartition::clone() const
{
    return std::make_unique<octahedral_repartition>(*this);
}


//###################################################################
//####################### elevation strategies ######################
//###                                                             ###

gfg::mesh_generator::neighbour_elevation::neighbour_elevation(float range, unsigned int first, unsigned int seed):
    range_(range),
    first_(first),
    seed_(seed)
{}

//################### midpoint ###############

gfg::mesh_generator::midpoint_elevation::midpoint_elevation(float range, unsigned int first, unsigned int seed):
    neighbour_elevation(range, first, seed)
{}

void gfg::mesh_generator::midpoint_elevation::generate(mesh_generator& mesh) const
{
    std::cout << "using the seed " << seed_ << std::endl;
    std::mt19937 engine(seed_);
    std::uniform_real_distribution<float> distrib(-range_/2, range_/2);

    //the heights of the nodes of the initial stages are assigned a random value between initial_elevation_ and initial_elevation_ + range_
    for(unsigned int stage=0; stage < first_ && stage <= mesh.target_.rank() ; ++stage)
        for(auto it = mesh.target_.stageBegin(stage); it != mesh.target_.stageEnd(stage); ++it)
            mesh.target_.elevations_[it->index()] = mesh.radius_ + distrib(engine);
    for(unsigned int stage = first_; stage <= mesh.target_.rank(); ++stage)
    {
        float div = pow(2, stage - first_);
        distrib = std::uniform_real_distribution<float>(-range_/div, range_/div);
        for(auto it = mesh.target_.stageBegin(stage); it != mesh.target_.stageEnd(stage); ++it)
        {
            auto gen = it->generators();
            mesh.target_.elevations_[it->index()] =
                (mesh.target_.elevations_[gen[0].index()] +
                 mesh.target_.elevations_[gen[1].index()]) / 2 +
                distrib(engine);
        }
    }
}

std::unique_ptr<gfg::mesh_generator::elevation_strategy> gfg::mesh_generator::midpoint_elevation::clone() const
{
    return std::make_unique<midpoint_elevation>(*this);
}

//################# diamond #################

gfg::mesh_generator::diamond_elevation::diamond_elevation(float range, unsigned int first, unsigned int seed):
    neighbour_elevation(range, first, seed)
{}

void gfg::mesh_generator::diamond_elevation::generate(mesh_generator& mesh) const
{
    
    std::cout << "using the seed " << seed_ << std::endl;
    std::mt19937 engine(seed_);
    std::uniform_real_distribution<float> distrib(-range_/2, range_/2);

    for(unsigned int stage=0; stage < first_ && stage <= mesh.target_.rank() ; ++stage)
        for(auto it = mesh.target_.stageBegin(stage); it != mesh.target_.stageEnd(stage); ++it)
            mesh.target_.elevations_[it->index()] = mesh.radius_ + distrib(engine);


    for(unsigned int stage = first_; stage <= mesh.target_.rank(); ++stage)
    {
        unsigned int param = stage - first_+1;
        float div = pow(2, param);
        distrib = std::uniform_real_distribution<float>(-range_/div, range_/div);
        std::cout << "stage " << stage << " ## range "
                  << range_/div << " ## div " << div << " ## param" << param << std::endl;
        for(auto it = mesh.target_.stageBegin(stage); it != mesh.target_.stageEnd(stage); ++it)
        {
            auto dia = it->diamond_neighbours();
            mesh.target_.elevations_[it->index()] =
                (mesh.target_.elevations_[dia[0].index()] +
                 mesh.target_.elevations_[dia[1].index()] +
                 mesh.target_.elevations_[dia[2].index()] +
                 mesh.target_.elevations_[dia[3].index()])/ 4;
            //+ (stage % 2  == 0) ? distrib(engine) : 0;
            if(stage %2 == 0)
                mesh.target_.elevations_[it->index()] += distrib(engine);
//todo: use std::sum or whatever
        }
    }
}

std::unique_ptr<gfg::mesh_generator::elevation_strategy> gfg::mesh_generator::diamond_elevation::clone() const
{
    return std::make_unique<diamond_elevation>(*this);
}

void gfg::mesh_generator::no_elevation::generate(mesh_generator& mesh) const
{
    for(auto it = mesh.target_.nodeBegin(), end = mesh.target_.nodeEnd();
        it != end; ++it)
    {
        mesh.target_.elevations_[it->index()] = mesh.radius_;
    }
}

std::unique_ptr<gfg::mesh_generator::elevation_strategy> gfg::mesh_generator::no_elevation::clone() const
{
    return std::make_unique<no_elevation>();
}
