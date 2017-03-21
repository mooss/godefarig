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

#ifndef MOOSS_MESH_GENERATION_H
#define MOOSS_MESH_GENERATION_H

#include "gfg_utils.h"
#include <vector>
#include <array>
#include <functional>
#include <memory>
#include "color_heatmap.hpp"

#include "definitions.h"
namespace gfg
{

class fractal_octahedron;

class mesh_generator
{
  public:
    class coloration_strategy
    {
      public:
        virtual std::vector<gfg::color> generate(const mesh_generator&) const=0;
        virtual std::unique_ptr<coloration_strategy> clone() const=0;
        static std::unique_ptr<coloration_strategy> create(const po::variables_map&);
    };

    class repartition_strategy
    //todo: considerer utiliser des foncteurs pour representer les strategies (probablement plus souple)
    {
      public:
        virtual std::vector<gfg::position> generate(const mesh_generator&) const=0;
        virtual std::unique_ptr<repartition_strategy> clone() const=0;
        static std::unique_ptr<repartition_strategy> create(const po::variables_map&);
    };

    class elevation_strategy
    {
      public:
        virtual void generate(mesh_generator&) const=0;
        virtual std::unique_ptr<elevation_strategy> clone() const=0;
        static std::unique_ptr<elevation_strategy> create(const po::variables_map&);
    };

    /**@class elevation_generator
     * Handles a sequence of \ref elevation_strategy and stages on which to apply them.
     * Provides an abstraction for \ref mesh_generator.
     */
    class elevation_generator
    {
        /**@struct link
         * stores an link of the elevation generation sequence,
         * that is to say an \ref elevation_strategy and the last stage on which it applies
         */
        struct link
        {
            std::unique_ptr<elevation_strategy> strat;///<strategy to apply
            unsigned int closure;///<last stage on which the stategy applies
        };
        /**@brief generate the height
         *
         * generate the height starting with stage 0 up to the last stage
         * using the links stored in \ref sequence_
         */
        void generate(mesh_generator& mesh) const;
      private:
        std::vector<link> sequence_;
        double range_;
        unsigned long seed_;
    };

    class rasterised_coloration : public coloration_strategy
    {
      public:
        rasterised_coloration();
        rasterised_coloration(const std::array<gfg::color, 6>&);
        std::vector<gfg::color> generate(const mesh_generator&) const override;
        std::unique_ptr<coloration_strategy> clone() const override;
      private:
        std::array<gfg::color, 6> initial_colors_;
        const unsigned int initial_stages_=0;//todo: generalise that
    };

    class height_coloration : public coloration_strategy
    {
      public:
        height_coloration()=delete;
        height_coloration(float);
        height_coloration(gfg::color_heatmap<float>, gfg::color_heatmap<float>,
                          float);
        std::vector<gfg::color> generate(const mesh_generator&) const override;
        std::unique_ptr<coloration_strategy> clone() const override;
        void reverse_color_scale();
      private:
        gfg::color_heatmap<float> water_;
        gfg::color_heatmap<float> land_;
        float shore_step_;
    };

    // template<typename scalar>
    // class Theight_coloration : public coloration_strategy
    // {
    //     Theight_coloration()=delete;
    //     Theight_coloration(double min, double max, double sea_level_, double shore_step_);
    //   private:
    //     gfg::color_heatmap<scalar> water_;
    //     gfg::color_heatmap<scalar> land_;
    //     double min_;
    //     double max_;
    //     double sea_level_;
    //     double shore_step_;
        
    // };

    class polar_repartition : public repartition_strategy
    {
      public:
        std::vector<gfg::position> generate(const mesh_generator&) const override;
        std::unique_ptr<repartition_strategy> clone() const override;
    };

    class octahedral_repartition : public repartition_strategy
    {
      public:
        std::vector<gfg::position> generate(const mesh_generator&) const override;
        std::unique_ptr<repartition_strategy> clone() const override;
    };

    class neighbour_elevation : public elevation_strategy
    {
      public:
        neighbour_elevation()=delete;
      protected:
        float range_;
        unsigned int first_;
        unsigned int seed_;
        neighbour_elevation(float range, unsigned int first, unsigned int seed);
    };

    class midpoint_elevation : public neighbour_elevation
    {
      public:
        midpoint_elevation(float range, unsigned int first, unsigned int seed);
        midpoint_elevation()=delete;
        void generate(mesh_generator&) const override;
        std::unique_ptr<elevation_strategy> clone() const override;
    };

    class diamond_elevation : public neighbour_elevation
    {
      public:
        diamond_elevation(float range, unsigned int first, unsigned int seed);
        diamond_elevation()=delete;
        void generate(mesh_generator&) const override;
        std::unique_ptr<elevation_strategy> clone() const override;
    };

    class no_elevation : public elevation_strategy
    {
      public:
        no_elevation(){}
        void generate(mesh_generator&) const override;
        std::unique_ptr<elevation_strategy> clone() const override;
    };

    mesh_generator()=delete;
    mesh_generator(gfg::fractal_octahedron&, po::variables_map&);
    mesh_generator(gfg::fractal_octahedron&);
    mesh_generator(const mesh_generator&);
    const mesh_generator& operator=(const mesh_generator&);
    std::vector<gfg::color> colors() const;
    std::vector<gfg::position> positions() const;
    std::tuple<
        std::vector<gfg::position>,
        std::vector<gfg::normal>
        > positions_and_normals() const;
    void generate_elevation();

    bool surface_waters() const { return surface_waters_; }

  private:
    gfg::fractal_octahedron& target_;
    double radius_;
    std::unique_ptr<coloration_strategy> coloration_;
    std::unique_ptr<repartition_strategy> repartition_;
    std::unique_ptr<elevation_strategy> elevation_;
    
    double lowest_altitude_;
    double highest_altitude_;
    double sea_level_;
    double sea_level_modifier_;

    bool surface_waters_;

    //std::unique_ptr<elevation_strategy> initial_elevation_;
    
};//class mesh_generator

}//namespace gfg

#endif//MOOSS_MESH_GENERATION_H
