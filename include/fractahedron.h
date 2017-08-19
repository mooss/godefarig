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

#ifndef MOOSS_FRACTAHEDRON_HPP
#define MOOSS_FRACTAHEDRON_HPP

#define DEBUG_MODE
#include "debug.h"

#include <iostream>
#include <vector>
#include <array>
#include "gfg_utils.h"
#include <memory>//unique_ptr, etc
#include "make_unique_cpp11.h"
#include <cmath>
#include "mesh_generation.h"
#include "definitions.h"
#include "iterators.hpp"

namespace gfg
{

class fractal_octahedron
{
  public:
    fractal_octahedron()=delete;
    fractal_octahedron(unsigned int);
    fractal_octahedron(po::variables_map&);
    ~fractal_octahedron(){}

    using index = unsigned int;
    
    unsigned int rank() const {return m_rank;}
    unsigned int node_cardinal() const { return gfg::point::numberAtStage(m_rank); }
    unsigned int face_cardinal() const { return gfg::face::numberAtStage(m_rank); }
    unsigned int edge_cardinal() const { return gfg::edge::numberAtStage(m_rank); }
    const gfg::mesh_generator& mesh() const { return mesh_; }
    const std::vector<elevation>& elevations() const { return elevations_; }
    
    std::vector< std::array<index, 3> > get_faces_index();
    std::vector< std::array<index, 3> > get_faces_index(unsigned int stage);

    class spiral//todo: trouver un moyen pour nettoyer cette horreur (creer une classe rassemblant spiral et spiral::iterator ? self-contained spiral::iterator par ex.)
    {
      public:
        ~spiral(){}

        unsigned int depth() const {return m_depth;}
        unsigned int stage() const {return m_stage;}
        unsigned int id() const {return m_id;}
        unsigned int originalStage() const {return m_originalStage;}
        unsigned int originalId() const {return m_originalId;}
        index cardinal() const;
        bool isEquator() const;
        bool isEven() const {return id() %2 == 0;}
        bool isOdd() const {return id() %2 != 0;}

        spiral const& operator=(spiral const& origin)
        {
            
            m_super = origin.m_super;
            m_stage = origin.m_stage;
            m_id = origin.m_id;
            m_originalStage = origin.m_originalStage;
            m_originalId = origin.m_originalId;
            m_depth = origin.m_depth;

            return *this;
        }

        class iterator//parcours DANS une spirale
        {
          public:
            iterator()=delete;
            ~iterator(){}
            
            index operator*();
            iterator& next();
            iterator& operator++()
            {
                return this->next();
            }

            bool operator==(iterator const&) const;
            bool operator!=(iterator const&) const;
            unsigned int sideOffsetUnsafe() const {return m_current % slice::halfwayId(m_super->stage(), m_super->id());}
            unsigned int sideOffset_safe() const
            {
                if(slice::halfwayId(m_super->stage(), m_super->id()) == 0)
                    return 0;
                else return sideOffsetUnsafe();
            }
            unsigned int sideIdUnsafe() const {return m_current / slice::halfwayId(m_super->stage(), m_super->id());}
            unsigned int sideId() const
            {
                if(slice::halfwayId(m_super->stage(), m_super->id()) == 0)
                    return 0;
                else return sideIdUnsafe();
            }
            bool currentIsAngular() const {return m_super->id() %2 == 1 && sideOffsetUnsafe() == 0;}
            
            friend std::ostream& operator<<(std::ostream& os, const fractal_octahedron::spiral::iterator& iter);
            friend class fractal_octahedron;
            //  friend class spiral;
//            friend class spiralIterator;//pas beau tout ça

          private:
            spiral* m_super;
            std::vector<index> m_offset;
            unsigned int m_current;

            iterator(spiral*, unsigned int = 0);
            iterator(unsigned int);//end ctor
            void restart() {m_current = 0;}
            void reload();
            void initDataOffset();
        };//class spiral::iterator
        iterator begin();
        iterator end();
        
        friend class fractal_octahedron;
        
      private:
        fractal_octahedron& m_super;
        unsigned int m_stage;
        unsigned int m_id;
        unsigned int m_originalStage;
        unsigned int m_originalId;
        unsigned int m_depth;

        spiral()=delete;
        spiral(fractal_octahedron& oct, unsigned int stage, unsigned int id);
        spiral(fractal_octahedron&, unsigned int id);
        
        void initialiseOriginalValues();
        void reload(unsigned int newStage, unsigned int newId);//comportement chelou pour d'éventuels spiral::Iterators préexistants
        void swap(spiral&);
    };//class spiral
    spiral initialEquator();//let's count on rvo
    spiral north();
    spiral south();

    class node_iterator
    {
        struct resource
        {
            resource(unsigned int stage);
            gfg::cascade_node target;
        };
        
      public:
        void operator++();
        const gfg::cascade_node* operator->() {return &m_res->target; }
        const gfg::cascade_node& operator*() { return m_res->target; }
        bool operator!=(const node_iterator& ) const;

      private:
        std::unique_ptr<resource> m_res;
        node_iterator();//end cstr
        node_iterator(unsigned int);//begin cstr
        friend class fractal_octahedron;
    };

    class same_stage_iterator
    {
        struct resource
        {
            resource(unsigned int stage);
            unsigned int current;
            unsigned int last;
            gfg::cascade_node target;
        };
        
      public:
        same_stage_iterator& operator++();
        const gfg::cascade_node* operator->() { return &m_res->target; }
        const gfg::cascade_node& operator*() { return m_res->target; }
        bool operator!=(const same_stage_iterator&) const;

      private:
        std::unique_ptr<resource> m_res;
        same_stage_iterator(unsigned int);//begin cstr
        same_stage_iterator();//end cstr
        friend class fractal_octahedron;        
    };


    template<unsigned int> struct geometryIterationResource{};

    template<unsigned int size>
    class geometryIterator//todo: remplacer m_super par m_res->super, réfléchir à deep copy dans le cas de end_iterator et des autres iterateurs
    {
      public:
        geometryIterator()=delete;
        //geometryIterator(const geometryIterator &);//might want to fix that later (unique_ptr, something like that)
        const geometryIterator& operator=(const geometryIterator&)=delete;
        ~geometryIterator(){ delete m_res;}

        std::array<index, size>& operator*() {return m_res->content;}
        
        bool operator==(const geometryIterator& cmp) const {return m_current == cmp.m_current;}
        bool operator!=(const geometryIterator& cmp) const {return !(*this == cmp);}
        
        index& operator[](size_t el) {return m_res->content[el];}
        const index& operator[](size_t el) const {return m_res->content[el];}
        geometryIterator& operator++()
        {
            return this->next();
        }
        
        geometryIterator& next()
        {
            ++m_current;
            m_res->next();
            return *this;
        }

        friend class fractal_octahedron;

      private:
        fractal_octahedron* m_super;
        geometryIterationResource<size>* m_res;
        unsigned int m_current;

        geometryIterator(fractal_octahedron* oct, unsigned int stage):
            m_super(oct),
            m_res(new geometryIterationResource<size>(*m_super, stage)),
            m_current(0)
        {}
        geometryIterator(unsigned int end):
            m_super(nullptr),
            m_res(nullptr),
            m_current(end)
        {}//end cstr
    };//geometry iterator

    using edgeIterator = geometryIterator<2>;
    using faceIterator = geometryIterator<3>;

    

    faceIterator faceBegin(unsigned int stage);
    faceIterator faceEnd(unsigned int stage);
    faceIterator faceBegin();
    faceIterator faceEnd();

    edgeIterator edgeBegin(unsigned int stage);
    edgeIterator edgeEnd(unsigned int stage);
    edgeIterator edgeBegin();
    edgeIterator edgeEnd();

    node_iterator nodeBegin(unsigned int stage);
    node_iterator nodeEnd(unsigned int stage);
    node_iterator nodeBegin();
    node_iterator nodeEnd();
    
    same_stage_iterator stageBegin(unsigned int stage);
    same_stage_iterator stageEnd(unsigned int stage);
    same_stage_iterator stageBegin();
    same_stage_iterator stageEnd();
    

  private:
    unsigned int m_rank;
    std::vector<gfg::elevation> elevations_;
    gfg::mesh_generator mesh_;

    friend class gfg::mesh_generator;
};//class fractal_octahedron
    
template<>
struct fractal_octahedron::geometryIterationResource<2>//edge
{
    spiral inf;
    spiral sup;
    spiral::iterator infIter;
    spiral::iterator supIter;
    std::array<index, 2> content;
    
    geometryIterationResource(fractal_octahedron& oct, unsigned int stage):
        inf(oct, stage, 0),
        sup(oct, stage, 1),
        infIter(&inf),
        supIter(&sup),
        content{*infIter, *supIter}
    {}
    
    ~geometryIterationResource(){}

    void next()
    {
        if(evenStage())//spirale paire (ajout de point)
        {
            infIter.next();
            supIter.next();
            fixEvenEnd();
        }
        else//spirale impaire (creation de point)
        {
        
            if(supIter.currentIsAngular())//triangle method of utnmul
            {
                //DEBUG("ANGULAR");
                supIter.next();
                fixSupEnd();
            }
            else if(supIter.sideOffsetUnsafe() > infIter.sideOffsetUnsafe() && infIter.sideIdUnsafe() == supIter.sideIdUnsafe())
            {
                //DEBUG("ssO=" << supIter.sideOffsetUnsafe() << " ; isO=" << infIter.sideOffsetUnsafe() << " ;; ssI=" << supIter.sideIdUnsafe() << " ; isI=" << infIter.sideIdUnsafe());
                infIter.next();
                fixInfEnd();
            } else//move sup
            {
                //DEBUG("ssO=" << supIter.sideOffsetUnsafe() << " ; isO=" << infIter.sideOffsetUnsafe() << " ;; ssI=" << supIter.sideIdUnsafe() << " ; isI=" << infIter.sideIdUnsafe());
                supIter.next();
                fixSupEnd();
            }
        }
        reloadEdge();
    }
        
    void fixSupEnd()
    {
        //DEBUG("FIXSUPEND");
        if(supIter == sup.end())
            moveTowardEven();
    }
        
    void fixInfEnd()
    {
        //DEBUG("FIXINFEND");
        if(infIter == inf.end())
            infIter.restart();
    }
        
    void fixEvenEnd()
    {
        //DEBUG("FIXEVENEND");
        if(supIter == sup.end())
            supIter.restart();
        else if(infIter == inf.end())
            moveTowardOdd();
    }
        
    void moveTowardEven()
    {
        //DEBUG("MOVETOWARDEVEN");
        if(beyondSouth())
            sup.reload(sup.m_stage, sup.m_id +1);
        else
            inf.reload(inf.m_stage, inf.m_id +1);
        reloadIterators();
        supIter.next();
    }
        
    void moveTowardOdd()
    {
        //DEBUG("MOVETOWARDODD");
        if(beyondSouth())
            inf.reload(inf.m_stage, inf.m_id +1);
        else
            sup.reload(sup.m_stage, sup.m_id +1);
        reloadIterators();
    }

    bool beyondSouth() const
    {
        return inf.m_id >= slice::equatorId(inf.m_stage);
    }

    void reloadIterators()
    {
        infIter.reload();
        supIter.reload();
    }
            
    void reloadEdge()
    {
        content[0] = *infIter;
        content[1] = *supIter;
    }

    void moveBeyondEquator()
    {
        inf.reload(inf.m_stage, inf.m_id +1);
        reloadIterators();
    }

    bool evenStage() const
    {
        return inf.m_id == sup.m_id;
    }

};//class fractal_octahedron::geometryIterationResource<2>

template<>
struct fractal_octahedron::geometryIterationResource<3>//face
{
    geometryIterationResource(fractal_octahedron& oct, unsigned int stage):
        inf(oct, stage, 0),
        sup(oct, stage, 1),
        infIter(&inf),
        supIter(&sup),
        content{*supIter, *supIter.next(), *infIter}
    { }
    ~geometryIterationResource(){};

    spiral inf;
    spiral sup;
    spiral::iterator infIter;
    spiral::iterator supIter;
    std::array<index, 3> content;

    void next()//foret d'ifs. nettoyer cette horeur
    {
//    DEBUG("supIter=" << supIter << " ;; infIter=" << infIter);
//    DEBUG("SUPSIDEID=" << supIter.sideId());
        if(supIter.currentIsAngular() || (supIter.sideId() == 3 && infIter.m_current == 0) || supIter.sideId() < infIter.sideId() || supIter.sideOffset_safe() <= infIter.sideOffset_safe())//on avance en haut. tricky angular test in order to avoid mod by 0 from first instance of infIter.sideOffset_safe()
        {
            content[0] = *supIter;
            supIter.next();
            content[2] = *infIter;
            if(supIter != sup.end())
            {
                content[1] = *supIter;
            }
            else
            {
                supIter.restart();
                content[1] = *supIter;
                moveToNextIds();
            }
        }
        else//on avance en bas
        {
            content[0] = *infIter;
            content[1] = *supIter;
            infIter.next();
            if(infIter == inf.end())
            {
                infIter.restart();
                //DEBUG("restarted infiter : " << infIter.m_super->m_super.getIndiceOf(*infIter));
            }
            content[2] = *infIter;
        }

        if(beyondSouth())
            changeFaceOrientation();//dirty fix
    }
        
    void moveToNextIds()
    {
    
        if(inf.m_id + 1 == slice::equatorId(inf.m_stage))
        {
            inf.reload(inf.m_stage, inf.m_id +2);
            changeFaceOrientation();
        }
        else
        {
            inf.reload(inf.m_stage, inf.m_id +1);
            sup.reload(sup.m_stage, sup.m_id +1);
        }
        reloadIterators();
    }

    void changeFaceOrientation()
    {
        index tmp = content[0];
        content[0] = content[1];
        content[1] = tmp;
    }

    bool beyondSouth() const
    {
        return inf.m_id >= slice::equatorId(inf.m_stage);
    }

    void reloadIterators()
    {
        infIter.reload();
        supIter.reload();
    }
};//class fractal_octahedron::geometryIterationResource<3>

};//namespace gfg

std::ostream& operator<<(std::ostream& os, const gfg::fractal_octahedron& oct);
//std::ostream& operator<<(std::ostream& os, const std::vector<RecursionStage>& stage);
std::ostream& operator<<(std::ostream& os, const gfg::fractal_octahedron::spiral& spir);
//std::ostream& operator<<(std::ostream& os, const gfg::fractal_octahedron::spiral::iterator& iter);




#endif
