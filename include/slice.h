#ifndef MOOSS_SLICE_H
#define MOOSS_SLICE_H
#include "gfg_utils.h"
#include <vector>
#include <ostream>
#define DEBUG_MODE
#include "debug.h"

namespace gfg
{
unsigned int binary_degree(unsigned int range, unsigned int value);//nombre de division successives qu'il est possible de faire sur value (max=range)
gfg::index node_position(unsigned int rank, unsigned int id, unsigned int offset);
unsigned int ring_plus(unsigned int card, unsigned int a, unsigned int b);
unsigned int ring_minus(unsigned int card, unsigned int a, unsigned int b);

class slice
{
  public:
    slice(unsigned int stage, unsigned int id);
    slice(unsigned int stage);
    slice()=delete;
    virtual ~slice();
    
    unsigned int stage() const {return m_stage;}
    unsigned int id() const {return m_id;}
    
    unsigned int cardinal() const {return slice::all_stages_cardinal(m_stage, m_id);}
    unsigned int depth() const {return binary_degree(m_stage, m_id);}//virtual ?
    bool is_south() const { return m_id == 0; }
    bool is_north() const { return m_id == meta_cardinal()-1;}
    bool is_polar() const { return is_north() || is_polar(); }
    bool is_even() const {return m_id%2 == 0;}
    bool is_odd() const {return !is_even();}
    bool is_equator() const {return m_id == equator_id();}
    bool southern_hemisphere() const { return m_id < equator_id();}
    bool northern_hemisphere() const { return m_id > equator_id();}

    virtual void reset() { m_id = 0; }

    virtual bool up()
    {
        if(m_id + 1 < meta_cardinal())
        {
            ++m_id;
            return true;
        }
        return false;
    }

    virtual bool down()
    {
        if(m_id > 0)
        {
            --m_id;
            return true;
        }
        return false;
    }

    virtual void prev_stage();
    virtual void next_stage();
    
    virtual unsigned int equator_id() const=0;
    virtual unsigned int meta_cardinal() const=0;

    //operator overloads
    bool operator==(slice const&) const ;
    bool operator!=(slice const& comp) const { return !(*this == comp); }

    //static methods
    static gfg::index numberAtStage(unsigned int stageNum);//stageNum as short ?
    static gfg::index equatorId(unsigned int stageNum);
//static gfg::index polarDataOffset(unsigned int stageNum, gfg::index spiralId);
    static gfg::index correctDataOffset(unsigned int stageNum, gfg::index spiralId);
    static gfg::index incorrectDataOffset(unsigned int stageNum , gfg::index spiralId);
    static gfg::index halfwayId(unsigned int stageNum, gfg::index spiralId);
    static gfg::index last_stage_cardinal(unsigned int stageNum, gfg::index spiralId);
    static gfg::index all_stages_cardinal(unsigned int stageNum, gfg::index spiralId);
    static gfg::index halfwayDataOffset(gfg::index spiralId);
    static gfg::index halfwayCardinal(gfg::index spiralId);
    static gfg::index symmetricalSum(unsigned int middle, unsigned int range);
    static unsigned int equatorCardinal(unsigned int);

  protected:
    unsigned int m_stage;
    unsigned int m_id;
};

class light_slice : public slice
{
  public:
    light_slice(unsigned int stage, unsigned int id);
    light_slice(unsigned int stage);
    light_slice()=delete;
    
    unsigned int equator_id() const override { return slice::equatorId(m_stage);}
    unsigned int meta_cardinal() const override { return slice::numberAtStage(m_stage);}

};

class heavy_slice : public slice
{
  public:
    heavy_slice(unsigned int stage, unsigned int id);
    heavy_slice(unsigned int stage);
    heavy_slice()=delete;

    void reload() { m_depth = binary_degree(m_stage, m_id);}
    void rectify();
    void reset() override;

    bool up() override
    {
        if(slice::up())
        {
            reload();
            return true;
        }
        return false;
    }
    bool down() override
    {
        if(slice::down())
        {
            reload();
            return true;
        }
        return false;
    }

    void prev_stage() override;
    void next_stage() override;

    //slice redefinitions
    unsigned int equator_id() const override { return m_equator_id; }
    unsigned int meta_cardinal() const override { return m_meta_cardinal; }
    
    unsigned int depth() const {return m_depth;}

  private:
    unsigned int m_depth;
    unsigned int m_equator_id;
    unsigned int m_meta_cardinal;
};

}//namespace gfg

std::ostream& operator<<(std::ostream& os, gfg::slice const& sl);

#endif//MOOSS_SLICE_H
