#ifndef MOOSS_META_UTILS_HPP
#define MOOSS_META_UTILS_HPP

template<typename T>
struct stepped_value
{
    stepped_value(T val, T ste, T m, T M):
        value_(val),
        step_(ste),
        min_(m),
        max_(M)
    {}

    stepped_value<T> operator++()
    {
        if( value_ + step_ < max_)
            value_ += step_;
        return *this;
    }

    stepped_value<T> operator--()
    {
        if( value_ - step_ > min_)
            value_ -= step_;
        return *this;
    }

    const T& value() const { return value_; }
    const T& set() const { return step_; }
    const T& min() const { return min_; }
    const T& max() const { return max_; }
    
  private:
    T value_;
    T step_;
    T min_;
    T max_;
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



template<typename T>
struct flat_coordinates
{
    flat_coordinates(T x_coord, T y_coord):
        x(x_coord),
        y(y_coord)
    {}

    flat_coordinates() {}
    T x;
    T y;

    flat_coordinates& operator+=(const flat_coordinates& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
    
    flat_coordinates& operator-=(const flat_coordinates& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    flat_coordinates operator+(flat_coordinates rhs) const
    {
        rhs += *this;
        return rhs;
    }
    
    flat_coordinates operator-(flat_coordinates rhs) const
    {
        rhs -= *this;
        return rhs;
    }

    void reset()
    {
        x = 0;
        y = 0;
    }
};//struct flat_coordinates
//todo: find a generic way to deal with n-dimentional scalar coordinates in order to ensure future interoperability

#endif//MOOSS_META_UTILS_HPP
