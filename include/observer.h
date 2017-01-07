#ifndef MOOSS_OBSERVER_H
#define MOOSS_OBSERVER_H

#include <vector>
#include <functional>
#include <boost/signals2.hpp>

namespace gfg
{

// class observer
// {
//   public:
//     /**@brief method used by the subject to inform an observer that a change occured
//      */
//     virtual void update()=0;//todo: think about crtp.
//     virtual ~observer(){}
// };

// class subject
// {
//   public:
//     virtual ~subject(){}
//     /**@brief adds an observer
//      *@param obs the observer to add
//      */
//     virtual void attach(std::function<void()> obs)=0;

//     /**@brief notifies the observers of a change in the subject
//      */
//     virtual void notify()=0;
// };

class signal_subject
{
  public:

    template<typename Fun>
    boost::signals2::connection attach( Fun&& obs)
    {
        return observers_.connect(std::forward<Fun>(obs));
    }

    void notify() { observers_(); }

  private:
    boost::signals2::signal<void()> observers_;
};

}//namespace gfg

#endif//MOOSS_OBSERVER_H
