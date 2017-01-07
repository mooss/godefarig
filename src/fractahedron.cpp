#include "slice.h"
#include "fractahedron.h"
#include "containerOverload.h"
#include <cmath>
#include <random>

using namespace std;
using namespace gfg;//todo: not "using" that namespace

fractal_octahedron::fractal_octahedron(unsigned int stage):
    m_rank(stage),
    elevations_(point::numberAtStage(stage)),
    mesh_(*this)
{}

fractal_octahedron::fractal_octahedron(po::variables_map& vm):
    m_rank(vm["stage"].as<unsigned int>()),
    elevations_(point::numberAtStage(rank())),
    mesh_(*this, vm)
{}

vector< array<gfg::index, 3> > fractal_octahedron::getFacesIndex()
{
    vector< array<gfg::index, 3> > result(face::numberAtStage(rank()));
    unsigned int indice = 0;
    for(auto faceIt = faceBegin(); faceIt != faceEnd(); faceIt.next())
    {
        result[indice] = *faceIt;
        ++indice;
    }
    return result;//pas besoin de move theoriquement (?)
}

//functions which return a nested class

fractal_octahedron::spiral fractal_octahedron::initialEquator()
{
    return spiral(*this, 0, slice::equatorId(0));
}

fractal_octahedron::spiral fractal_octahedron::north()
{
    return spiral(*this, m_rank, slice::numberAtStage(m_rank) -1);
}

fractal_octahedron::spiral fractal_octahedron::south()
{
    return spiral(*this, m_rank, 0);
}

fractal_octahedron::faceIterator fractal_octahedron::faceBegin(unsigned int stage)
{
    return faceIterator(this, stage);
}

fractal_octahedron::faceIterator fractal_octahedron::faceEnd(unsigned int stage)
{
    return faceIterator(face::numberAtStage(stage));
}

fractal_octahedron::faceIterator fractal_octahedron::faceBegin()
{
    return faceBegin(rank());
}

fractal_octahedron::faceIterator fractal_octahedron::faceEnd()
{
    return faceEnd(rank());
}


fractal_octahedron::edgeIterator fractal_octahedron::edgeBegin(unsigned int stage)
{
    return edgeIterator(this, stage);
}

fractal_octahedron::edgeIterator fractal_octahedron::edgeEnd(unsigned int stage)
{
    return edgeIterator(edge::numberAtStage(stage));
}

fractal_octahedron::edgeIterator fractal_octahedron::edgeBegin()
{
    return edgeBegin(rank());
}

fractal_octahedron::edgeIterator fractal_octahedron::edgeEnd()
{
    return edgeEnd(rank());
}

gfg::fractal_octahedron::node_iterator fractal_octahedron::nodeBegin(unsigned int stage)
{
    return node_iterator(stage);
}

gfg::fractal_octahedron::node_iterator fractal_octahedron::nodeEnd(unsigned int stage)
{
    return node_iterator();
}

gfg::fractal_octahedron::node_iterator fractal_octahedron::nodeBegin()
{
    return nodeBegin(m_rank);
}

gfg::fractal_octahedron::node_iterator fractal_octahedron::nodeEnd()
{
    return nodeEnd(m_rank);
}

gfg::fractal_octahedron::same_stage_iterator fractal_octahedron::stageBegin(unsigned int stage)
{
    return same_stage_iterator(stage);
}

gfg::fractal_octahedron::same_stage_iterator fractal_octahedron::stageEnd(unsigned int stage)
{
    return same_stage_iterator();
}

gfg::fractal_octahedron::same_stage_iterator fractal_octahedron::stageBegin()
{
    return stageBegin(m_rank);
}

gfg::fractal_octahedron::same_stage_iterator fractal_octahedron::stageEnd()
{
    return stageEnd(m_rank);
}


//#########################################################
//#########                spiral                ##########

fractal_octahedron::spiral::spiral(fractal_octahedron& oct, unsigned int stage, unsigned int id):
    m_super(oct),
    m_stage(stage),
    m_id(id)
{
    initialiseOriginalValues();
}

fractal_octahedron::spiral::spiral(fractal_octahedron& oct, unsigned int id):
    spiral(oct, oct.rank(), id)
{}

gfg::index fractal_octahedron::spiral::cardinal() const
{
    if(m_id == 0 || m_id == slice::numberAtStage(m_stage)-1)
        return 1;
    return slice::halfwayCardinal(slice::halfwayId(m_originalStage, m_originalId)) * pow(2, depth());
}

bool fractal_octahedron::spiral::isEquator() const
{
    return id() == slice::equatorId(stage());
}

fractal_octahedron::spiral::iterator fractal_octahedron::spiral::begin()
{
    return iterator(this);
}

fractal_octahedron::spiral::iterator fractal_octahedron::spiral::end()
{
    return iterator(cardinal());
}

void fractal_octahedron::spiral::initialiseOriginalValues()
{
    m_originalStage = m_stage;
    m_originalId = m_id;
    while(m_originalStage > 0 && m_originalId %2 == 0)
    {
        --m_originalStage;
        m_originalId/=2;
    }
    m_depth = m_stage - m_originalStage;//move in another function
}

void fractal_octahedron::spiral::reload(unsigned int newStage, unsigned int newId)
{
    m_stage = newStage;
    m_id = newId;
    initialiseOriginalValues();
}


//#######################################
//######## spiral::iterator #############


fractal_octahedron::spiral::iterator::iterator(spiral* sp, unsigned int el):
    m_super(sp),
    m_offset(m_super->depth() + 1),
    m_current(el)
{
    initDataOffset();
}

fractal_octahedron::spiral::iterator::iterator(unsigned int el):
    m_super(nullptr),
    m_offset(0),
    m_current(el)
{
    
}

gfg::index fractal_octahedron::spiral::iterator::operator*()
{
    unsigned int localCurrent=m_current, localStage=m_super->depth();
    while(localCurrent %2 == 0 && localStage >= 1)
    {
        localCurrent/=2;
        --localStage;
    }
    if(localCurrent %2 != 0 && localStage >= 1 && m_super->isEven())//evenCorrection
    {
        --localCurrent;
        localCurrent/=2;
    }
    
//    DEBUG("localCurrent=" << localCurrent << " ;; localStage=" << localStage << " ;; m_current=" << m_current << " supId=" << m_super->id());
    return m_offset[localStage] + localCurrent;
}

fractal_octahedron::spiral::iterator& fractal_octahedron::spiral::iterator::next()//might go beyond spiral.end(). deal with it.
{
    ++m_current;
    return *this;
}

bool fractal_octahedron::spiral::iterator::operator==(const fractal_octahedron::spiral::iterator& comp) const
{
    return m_current == comp.m_current;
}

bool fractal_octahedron::spiral::iterator::operator!=(const iterator& comp) const
{
    return !(*this == comp);
}

void fractal_octahedron::spiral::iterator::reload()
{
    restart();
    initDataOffset();
}

void fractal_octahedron::spiral::iterator::initDataOffset()
{
    if(m_super->id() == 0)
    {
        m_offset.resize(1);
        m_offset[0] = 0;
        return;
    }
    if(m_super->id() == slice::numberAtStage(m_super->stage())-1)
    {
        m_offset.resize(1);
        m_offset[0] = 5;
        return;
    }
    
    if(m_offset.size() != m_super->depth()+1)//perhaps <= check ?
        m_offset.resize(m_super->depth()+1);
    
    for(unsigned int i = 0; i <= m_super->depth(); ++i)
    {
        m_offset[i] = slice::correctDataOffset(m_super->originalStage() + i, m_super->originalId() * pow(2, i));
        if(m_super->originalStage() + i > 0)
            m_offset[i] += point::numberAtStage(m_super->originalStage() +i -1);
    }
}

//####################################################################
//########################## node_iterator ###########################
//###                                                              ###

gfg::fractal_octahedron::node_iterator::resource::resource(unsigned int stage):
    target(stage, 0, 0)
{}

gfg::fractal_octahedron::node_iterator::node_iterator():
    m_res(nullptr)
{}

gfg::fractal_octahedron::node_iterator::node_iterator(unsigned int stage):
    m_res(std::make_unique<gfg::fractal_octahedron::node_iterator::resource>(stage))
{}

bool gfg::fractal_octahedron::node_iterator::operator!=(gfg::fractal_octahedron::node_iterator const& that) const//ne prends en compte que une comparaison avec end
{
    return m_res && !m_res->target.is_beyond_north();
}

//gfg::fractal_octahedron::node_iterator&
void gfg::fractal_octahedron::node_iterator::operator++()
{
    m_res->target.next();
}

//########################################################################
//######################### same_stage_iterator ##########################
//###                                                                  ###

gfg::fractal_octahedron::same_stage_iterator::resource::resource(unsigned int stage):
    current( (stage != 0)? gfg::point::numberAtStage(stage-1) : 0),
    last(gfg::point::numberAtStage(stage) -1 ),
    target(stage, (stage != 0)? 1 : 0, 0)
{}

gfg::fractal_octahedron::same_stage_iterator::same_stage_iterator(unsigned int stage):
    m_res(std::make_unique<resource>(stage))
{}

gfg::fractal_octahedron::same_stage_iterator::same_stage_iterator():
    m_res(nullptr)
{}

gfg::fractal_octahedron::same_stage_iterator& gfg::fractal_octahedron::same_stage_iterator::operator++()
{
    m_res->target.last_stage_next();
    ++m_res->current;
    return *this;
}

bool gfg::fractal_octahedron::same_stage_iterator::operator!=(const same_stage_iterator& that) const //ne prends en compte que les comparaisons avec le end iterator
{
    return m_res && m_res->current <= m_res->last;
}


//###################################################
//#########         flux overloads         ##########



ostream& operator<<(ostream& os, const fractal_octahedron& oct)
{
    os << "stage " << oct.rank();
    return os;
}



ostream& operator<<(ostream& os, const fractal_octahedron::spiral& spir)
{
    os << "stage=" << spir.stage() << ", id=" << spir.id() << " ;; originalStage=" << spir.originalStage() << ", originalId=" << spir.originalId() << " ;; cardinal=" << spir.cardinal();
    return os;
}
