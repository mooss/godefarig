#ifndef MOOSS_META_UTILS_HPP
#define MOOSS_META_UTILS_HPP

template<typename T>
struct stepped_value
{
    stepped_value(T val, T ste, T m, T M):
        value(val),
        step(ste),
        min(m),
        max(M)
    {}

    stepped_value<T> operator++()
    {
        if( value + step < max)
            value += step;
        return *this;
    }

    stepped_value<T> operator--()
    {
        if( value - step > min)
            value -= step;
        return *this;
    }
    
    T value;
    T step;
    T min;
    T max;
};

template<typename T>
class value_bounder
{
  public:
    value_bounder(T min, T max):
        min_(min),
        max_(max)
    {}

    void enforce(T& value) const
    {
        if(value > max_)
            value = max_;
        else if(value < min_)
            value = min_;
    }

    bool check(T& value) const
    {
        return (value <= max_ && value >= min_);
    }
    
  private:
    T min_;
    T max_;
};



#endif//MOOSS_META_UTILS_HPP
