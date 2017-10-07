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
#include "slice.h"
#include "fractahedron.h"
#include "gfg_utils.h"
#include "container_overload.h"
#include "node.h"
#include "color_heatmap.hpp"
#include <algorithm>

using namespace std;

void display_test_result(bool cond, gfg::simple_chrono const& chr)
{
    cout << "it took " << chr.elapsed_time() << " seconds" << endl;
    if(cond)
        cout << "no incoherences were found" << endl;
    else
        cout << "incoherences were found" << endl;
}

int main(int argc, char** argv)
{
    unsigned int stage(1);
    if(argc > 1)
        stage = atoi(argv[1]);
    unsigned int nb_nodes(gfg::point::numberAtStage(stage));

    gfg::simple_chrono mesure;
    bool ok;

    cout << "octal creation... ";
    mesure.start();
    gfg::fractal_octahedron frtest(stage);
    mesure.stop();
    cout << "it took " << mesure.elapsed_time() << " seconds" << endl;

    cout << "\nspeed test... " << endl;
    gfg::cascade_node nodeTest(stage, 0, 0);

    cout << "\tgfg::cascade_node::next() : ";
    mesure.start();
    for(unsigned int i = 0; i < nb_nodes; ++i, nodeTest.next())
    {}
    mesure.stop();
    cout << " it took " << mesure.elapsed_time() << " seconds" << endl;

    cout << "\tnode_iterator : ";
    mesure.start();
    for(auto it = frtest.nodeBegin(), end = frtest.nodeEnd(); it != end; ++it)
    {}
    mesure.stop();
    cout << " it took " << mesure.elapsed_time() << " seconds" << endl;

    // cout << "\ntesting the coherence bw node_iter and cascade_node::next" << endl;
    // nodeTest.reset();
    // mesure.start();
    // ok = true;
    // unsigned int loops = 0;
    // for(auto it = frtest.nodeBegin(); it != frtest.nodeEnd(); nodeTest.next(), ++it, ++loops)
    // {
    //     if(*it != nodeTest)
    //     {
    //         cout << "incoherence found bw " << nodeTest << " and " << *it << endl;
    //         ok = false;
    //     }
    // }
    // mesure.stop();
    // if(ok)
    //     cout << "no incoherences were found. ";
    // cout << "it took " << mesure.elapsed_time() << " seconds" << endl
    //      << "\nthe end node was : " << nodeTest << endl
    //      << loops << " loops were performed" << endl;
    // nodeTest.next();
    // cout << "end + 1 node : " << nodeTest << endl;
    
    
    cout << "\nnaive neighbours test... ";
    nodeTest.reset();


    mesure.start();
    
    vector<unsigned int> nb_neighbours(nb_nodes);
    for(unsigned int i = 0; i < nb_nodes; ++i, nodeTest.next())
    {
        vector<gfg::cascade_node> neighbs;
        nodeTest.neighbours(neighbs);
        for( auto it = neighbs.begin(); it != neighbs.end(); ++it)
        {
            ++nb_neighbours[it->index()];
        }
    }
    mesure.stop();

    cout << "it took " << mesure.elapsed_time() << endl;
    
    for(unsigned int i = 0; i < nb_nodes; ++i)
    {
        if(nb_neighbours[i] != 6)
            cout << "\t" << i << " has " << nb_neighbours[i] << " neighbours" << endl;
    }

    cout << "\nbetter neighbour test... ";

    nodeTest.reset();
    vector<unsigned int> incoherences(nb_nodes);
    
    mesure.start();
    
    for(unsigned int i = 0; i < nb_nodes; ++i, nodeTest.next())
    {
        vector<gfg::cascade_node> ref;
        nodeTest.neighbours(ref);
        for(auto it = ref.begin(); it != ref.end(); ++it)
        {
            vector<gfg::cascade_node> verif;
            it->neighbours(verif);
            if(find(verif.begin(), verif.end(), *it) != verif.end())
            {
                ++incoherences[it->index()];
                ++incoherences[nodeTest.index()];
            }
        }
    }

    mesure.stop();

    cout << "it took " << mesure.elapsed_time() << endl;
    
    ok = true;
    for(unsigned int i = 0; i < nb_nodes; ++i)
    {
        if(incoherences[i] > 0)
        {
            cout << "\tthe position " << i << " has " << incoherences[i] << " incoherences" << endl;
            ok = false;
        }
    }
    if(ok)
        cout << "no incoherences were found" << endl;
    else
        cout << "incoherences were found" << endl;

    // cout << "affichage des voisins" << endl;
    // nodeTest.reset();
    // for(unsigned int i = 0; i < nb_nodes; ++i, nodeTest.next())
    // {
    //     vector<gfg::cascade_node> neigh;
    //     nodeTest.neighbours(neigh);
    //     cout << nodeTest.index() << " : { ";
    //     for(auto it = neigh.begin(); it != neigh.end(); ++it)
    //     {
    //         cout << it->index() << "; ";
    //     }
    //     cout << " }" << endl;
        
    // }

    cout << "\nstage iterator test" << endl;
    unsigned int last_node = gfg::point::numberAtStage(stage-1) -1;
    mesure.start();
    ok = true;
    for(auto stageIt = frtest.stageBegin(); stageIt != frtest.stageEnd(); ++stageIt)
    {
        if(stageIt->slice().is_even() && !stageIt->is_spontaneous())
        {
            cout << "incoherence : " << *stageIt << endl;
            ok = false;
        }
        if(stageIt->index() != last_node +1)
        {
            cout << "this node was supposed to be " << last_node + 1 << " but it was instead " << stageIt->index() << endl;
            ok = false;
        }
        last_node = stageIt->index();
    }
    mesure.stop();
    if(ok)
        cout << "no incoherences were found" << endl;
    cout << "it took " << mesure.elapsed_time() << " seconds" << endl;

    cout << "\nnaive generators test... ";
    ok = true;
    mesure.start();
    for(auto stageIt = frtest.stageBegin(); stageIt != frtest.stageEnd(); ++stageIt)
    {
        gfg::t_edge<gfg::cascade_node> gen = stageIt->generators();
        std::vector<gfg::cascade_node> nei;
        stageIt->neighbours(nei);
        if(std::find_if(nei.begin(), nei.end(),
                        [gen](const gfg::cascade_node& cas) -> bool {return cas.index() == gen[0].index();}  ) == nei.end()
           || std::find_if(nei.begin(), nei.end(),
                           [gen](const gfg::cascade_node& cas) -> bool {return cas.index() == gen[1].index();} ) == nei.end()
           || stageIt->creation_stage() <= gen[0].creation_stage()
           || stageIt->creation_stage() <= gen[1].creation_stage()
            )
            ok = false;
    }
    mesure.stop();
    display_test_result(ok, mesure);
    if(stage>1)
    {
        cout << "\nbetter generators test... ";
        unsigned int current = gfg::point::numberAtStage(stage-1);
        auto stageIt = frtest.stageBegin();
        mesure.start();
        for(auto it = frtest.edgeBegin(stage-1) ; it != frtest.edgeEnd(stage-1); it.next(), ++stageIt, ++current)
        {
            auto gen = stageIt->generators();
            if(it[0] != gen[0].index() || it[1] != gen[1].index())
            {
                if(it[0] != gen[1].index() || it[1] != gen[0].index())
                    ok = false;
                else if(current != stageIt->index())
                    ok = false;
                    
            }
        }
        mesure.stop();
        display_test_result(ok, mesure);
    }

    gfg::color_heatmap<float> scale({std::tuple<float, gfg::color>(1, glm::vec3(0, 0, 1))});
    scale.add(std::tuple<float, gfg::color>(2, glm::vec3(0, 1, 0)));
    cout << "according to color_heatmap, the middle of (0, 0, 1) and (0, 1, 0) is " << scale.get_color_of(1.5) << endl;

    cout << scale << endl;
    scale.sort();
    cout << scale << endl;

    // vector<gfg::normal> normales;
    // vector<gfg::position> positions;
    // std::tie(positions, normales) = frtest.mesh().positions_and_normals();
    // cout << "affichage des normales" << endl;
    // for(unsigned int i = 0; i < nb_nodes; ++i)
    // {
    //     cout << "\t" << normales[i] << " VS " << positions[i] << endl;
    // }

    // cout << "diamond neighbour test" << endl;

    // for(auto it = frtest.stageBegin(), end = frtest.stageEnd();
    //     it != end; ++it)
    // {
    //     auto diamond = it->diamond_neighbours();
    //     cout << "diamond neighbours of " << it->index() << " :\n\t"
    //          << diamond[0].index() << " ; "
    //          << diamond[1].index() << " ; "
    //          << diamond[2].index() << " ; "
    //          << diamond[3].index() << endl;
    // }

    std::vector<int> initials{0, 1, 2, 3, 4, 5};

    gfg::clean_node_iterator<int, std::vector<int>, gfg::cascade_node>
        clean(0,&initials),
        clean_end(0);

    
    cout << "clean test" << endl;
    for(; clean != clean_end; ++clean)
    {
        cout << "\t" << *clean << endl;;
    }
    
    return 0;
}
