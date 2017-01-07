#ifndef MOOSS_CAMERA_H
#define MOOSS_CAMERA_H

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "transformatrix.h"
#include <memory>
#include "make_unique_cpp11.h"
#include "definitions.h"

namespace gfg
{

template<typename distanceType>
struct rotationSettings
{
    glm::vec3 pointOfFocus;
    distanceType min, max;
};

enum movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
};

class camera : public Transformation
{
  public:
    static std::unique_ptr<camera> factory(const po::variables_map& vm);
    
    camera(glm::vec3 const& position);
    camera()=delete;
    
    camera(const camera&)=delete;
    camera& operator=(camera const&)=delete;
    
    virtual ~camera(){}

    void update() override;
    virtual void move(gfg::movement, GLfloat)=0;
    virtual void orientate(double deltaX, double deltaY)=0;
//    void spinAroundCenter(double deltaX, double deltaY);

    // static std::unique_ptr<camera> fps(glm::vec3 const& position=glm::vec3(0.0f, 0.0f, 6.0f));
    // static std::unique_ptr<camera> rotating(glm::vec3 const& position, const rotationSettings<GLfloat>& sett);
    
  protected:
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;

};

class fps_camera : public camera
{
  public:
    fps_camera(const glm::vec3& position):
        camera(position),
        m_pitch(0.0f),
        m_yaw(-90.0f)
    {}
    fps_camera()=delete;
    fps_camera(const fps_camera&)=delete;
    fps_camera& operator=(fps_camera const&)=delete;

    void move(gfg::movement, GLfloat) override;
    void orientate(double, double) override;

  private:
    void updateFront();//private ?
    GLfloat m_pitch;
    GLfloat m_yaw;

};

class spinning_camera : public camera//ajouter point of focus
{
  public:
    spinning_camera(const glm::vec3& position, const rotationSettings<GLfloat>& sett)://universal references with std::forward ?
        camera(position),
        m_latitude(0.0),
        m_longitude(0.0),
        m_rayon(glm::length(position)),
        m_settings(sett)
    {
        refresh();
    }

    spinning_camera(const spinning_camera&)=delete;
    spinning_camera& operator=(spinning_camera const&)=delete;

    void move(gfg::movement, GLfloat) override;
    void orientate(double, double) override;

  private:
    GLfloat m_latitude;
    GLfloat m_longitude;
    GLfloat m_rayon;
    rotationSettings<GLfloat> m_settings;
    
    void refresh();
};


}//namespace gfg

#endif//MOOSS_CAMERA_H
