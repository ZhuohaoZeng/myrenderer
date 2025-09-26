#include <ostream>
#include <cassert>

template<int n> struct vec {
    double data[n]{0};
    double& operator[](const int i)       {assert(i >= 0 && i <= n); return data[i];}
    double  operator[](const int i) const {assert(i >= 0 && i <= n); return data[i];}
};

template<int n> std::ostream& operator<<(std::ostream& out, const vec<n>& v) {
    for (int i = 0; i < n; ++i)out << v[i];
    return out;
}

template<> struct vec<2>
{
    double x{}, y{};
    double& operator[](const int i)       {assert(i >= 0 && i <= 2); return (i == 1 ? y : x);}
    double  operator[](const int i) const {assert(i >= 0 && i <= 2); return (i == 1 ? y : x);}
};

template<> struct vec<3>
{
    double x{}, y{}, z{};
    double& operator[](const int i)       {assert(i >= 0 && i <= 3); return (i ? i == 1 ? y : z : x);}
    double  operator[](const int i) const {assert(i >= 0 && i <= 3); return (i ? i == 1 ? y : z : x);}
};

typedef vec<2> vec2;
typedef vec<3> vec3;
