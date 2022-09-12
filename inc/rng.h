#include <iostream>
#include <string>
#include <map>
#include <random>
#include <iomanip>
#include <SSS/Math/Gradient.hpp>

#include <format>

template<class T>
struct Weighted_Element {
    Weighted_Element();
    Weighted_Element(int w, T elem);
    int _weight = 1;
    T _elem;
};


class Weighted_RNG {
public:
    static std::unique_ptr<Weighted_RNG> const& get();
    //Evaluate the value between the nearest pair in the array

    //Fill an array with the probabilities in percentage from a list of weights
    std::vector<float> probabilities(const std::vector<int>& weights);
    //Chose an element from a weight array, return the ID in the array
    size_t pick_element_weighted(const std::vector<int>& weights);

    ~Weighted_RNG();
protected:
    Weighted_RNG();
    std::random_device rd;
    std::mt19937 gen{ rd() };
};


template<class T>
Weighted_Element<T>::Weighted_Element()
{
};

template<class T>
Weighted_Element<T>::Weighted_Element(int w, T elem) :
    _weight(w), _elem(elem)
{

};
