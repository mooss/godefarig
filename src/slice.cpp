#include "slice.h"
#define DEBUG_MODE
#include "debug.h"
#include <cmath>

//###############################################################
//#######################      slice      #######################
//####                                                       ####

gfg::slice::slice(unsigned int stage, unsigned int id):
    m_stage(stage),
    m_id(id)
{}

gfg::slice::slice(unsigned int stage):
    slice(stage, 0)
{}

gfg::slice::~slice()
{}

bool gfg::slice::operator==(gfg::slice const& comp) const
{
    return stage() == comp.stage()
        && id() == comp.id();
}

void gfg::slice::next_stage()
{
    ++m_stage;
    m_id <<= 1;
}

void gfg::slice::prev_stage()
{
    --m_stage;
    m_id >>=1;
}

//###############################################################
//#######################   light_slice   #######################
//####                                                       ####

gfg::light_slice::light_slice(unsigned int stage, unsigned int id):
    slice(stage, id)
{}

gfg::light_slice::light_slice(unsigned int stage):
    slice(stage)
{}

//###############################################################
//#######################   heavy_slice   #######################
//####                                                       ####

gfg::heavy_slice::heavy_slice(unsigned int stage, unsigned int id):
    slice(stage, id),
    m_depth(binary_degree(m_stage, m_id)),
    m_equator_id(slice::equatorId(m_stage)),
    m_meta_cardinal(slice::numberAtStage(m_stage))
{}

gfg::heavy_slice::heavy_slice(unsigned int stage):
    heavy_slice(stage, 0)
{}

void gfg::heavy_slice::rectify()
{
    m_depth = binary_degree(m_stage, m_id);
    m_equator_id = slice::equatorId(m_stage);
    m_meta_cardinal = slice::numberAtStage(m_stage);
}

void gfg::heavy_slice::reset()
{
    slice::reset();
    rectify();
}

void gfg::heavy_slice::next_stage()
{
    slice::next_stage();
    rectify();
}

void gfg::heavy_slice::prev_stage()
{
    slice::prev_stage();
    rectify();
}

unsigned int gfg::binary_degree(unsigned int range, unsigned int value)//optimisable
{
    for(; range > 0; --range)
    {
        if(value % (2 << (range-1)) == 0)
            break;
    }
    return range;
}

gfg::index gfg::node_position(unsigned int stage, unsigned int id, unsigned int offset)
{
    //DEBUG(">>");
    unsigned int spiralDepth = binary_degree(stage, id);
    //DEBUG("spiral depth " << spiralDepth);
    unsigned int nodeDepth = binary_degree(spiralDepth, offset);
    //unsigned int spiralCreationStage = stage - spiralDepth;
    unsigned int nodeCreationStage = stage - nodeDepth;
    //DEBUG("node creation stage " << nodeCreationStage << " node depth " << nodeDepth);
    unsigned int offsetOnSlice;//offset among same nodeDepth

    offsetOnSlice = offset >> nodeDepth;
    if(nodeDepth != spiralDepth)
    {
        --offsetOnSlice;
        offsetOnSlice >>=1;
    }
    //DEBUG("offset on slice " << offsetOnSlice);
    unsigned int dataOffset = gfg::slice::correctDataOffset(nodeCreationStage, id >> nodeDepth);
    unsigned int pointsFromPrevStages = gfg::point::numberAtStage(nodeCreationStage -1);//incorrect si nodeCreationStage == 0
    //DEBUG("data offset " << dataOffset << "\tprev points " << pointsFromPrevStages);
    //DEBUG("res " << offsetOnSlice + dataOffset + pointsFromPrevStages);
    //DEBUG("<<");
    return offsetOnSlice + dataOffset + pointsFromPrevStages;
}

std::ostream& operator<<(std::ostream& os, gfg::slice const& sl)
{
    os << "slice(" << sl.stage() << ", " << sl.id() << ")";
    return os;
}


unsigned int gfg::ring_plus(unsigned int card, unsigned int a, unsigned int b)
{
    return (a+b) % card;
}

unsigned int gfg::ring_minus(unsigned int card, unsigned int a, unsigned int b)
{
    return (a+card-b) % card;
}

//#########################################################
//################# slice static methods ##################
//###                                                   ###



gfg::index gfg::slice::numberAtStage(unsigned int stageNum)//todo: remplacer les puissances de 2 par des décalages binaires
{
    return pow(2ul, stageNum+1) + 1;
}

gfg::index gfg::slice::equatorId(unsigned int stageNum)
{
    return (gfg::slice::numberAtStage(stageNum)-1) >>1;
}

gfg::index gfg::slice::incorrectDataOffset(unsigned int stageNum, gfg::index spiralId)
{
    gfg::index middle = gfg::slice::equatorId(stageNum);
    if(spiralId > middle)
    {
        gfg::index range = spiralId - middle;
        return gfg::slice::symmetricalSum(middle, range) * 2ul + equatorCardinal(stageNum) + gfg::slice::halfwayDataOffset(middle - range - 1);
    }
    return gfg::slice::halfwayDataOffset(spiralId);
}


// gfg::index gfg::slice::polarDataOffset(unsigned int stageNum, gfg::index spiralId)
// {
//     if(spiralId == 0)
//         return 0;
//     else if(spiralId == gfg::slice::numberAtStage(stageNum) -1)
//     {
//         if(stageNum==0)//peut être pas nécessaire
//             return 5;
//         else
//             return edge::numberAtStage(stageNum-1) + 1;
//     }
//     else
//         return gfg::slice::incorrectDataOffset(stageNum, spiralId-1)+1;
// }

gfg::index gfg::slice::correctDataOffset(unsigned int stageNum, gfg::index spiralId)//worst function ever
{
    
    if(stageNum == 0)
    {
        if(spiralId == 0)
            return 0;
        else if(spiralId == 2)
            return 5;
        else
            return 1;
    }
    else
        return gfg::slice::incorrectDataOffset(stageNum, spiralId-1);
}

gfg::index gfg::slice::halfwayDataOffset(gfg::index spiralId)
{
    if(spiralId%2==0)
    {
        spiralId/=2;
        return spiralId * (6ul * spiralId + 2);
    }
    else
    {
        spiralId/=2;
        return (spiralId) * (6ul * (spiralId) + 10 ) + 4;
    }
    //return (gfg::slice::halfwayOddOffset(spiralId) + gfg::slice::halfwayEvenOffset(spiralId) ) * 4;
}

gfg::index gfg::slice::halfwayId(unsigned int stageNum, gfg::index spiralId)
{
    gfg::index equator = gfg::slice::equatorId(stageNum);
    if(spiralId > equator)
        return 2*equator - spiralId;
    else
        return spiralId;
}

gfg::index gfg::slice::halfwayCardinal(gfg::index spiralId)
{
    if(spiralId %2 == 0)
        return spiralId*2;
    else
        return spiralId*4;
}

gfg::index gfg::slice::last_stage_cardinal(unsigned int stageNum, gfg::index spiralId)//todo:change name to lastStageCardinal
{
    return gfg::slice::halfwayCardinal(gfg::slice::halfwayId(stageNum, spiralId));
}

gfg::index gfg::slice::all_stages_cardinal(unsigned int stageNum, gfg::index spiralId)
{
    return gfg::slice::halfwayId(stageNum, spiralId) * 4;
}

gfg::index gfg::slice::symmetricalSum(unsigned int middle, unsigned int range)
{
    if(range%2==0)
        return range * (3ul * middle - (range * 3ul)/2 -1);
    else
        return (2ul * middle - range - 1) * ( 3ul * (range/2) + 2);
}

unsigned int gfg::slice::equatorCardinal(unsigned int stageNum)
{
    if(stageNum == 0)
        ++stageNum;
    ++stageNum;
    return pow(2ul, stageNum);
}
