//
// Created by dominik on 29.05.21.
//

#ifndef SQSGENERATOR_UTILS_H
#define SQSGENERATOR_UTILS_H

#include <stdexcept>
#include <boost/multi_array.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/storage.hpp>

using namespace boost;
using namespace boost::numeric::ublas;

namespace boost{
    template<class MultiArray>
    void extentTo(MultiArray & ma, const MultiArray & other){ //this function is adapted from
        auto& otherShape = reinterpret_cast<boost::array<size_t, MultiArray::dimensionality> const&>(*other.shape());
        ma.resize(otherShape);
    }

    template <typename T, typename F=row_major>
    matrix<T, F> matrix_from_vector(size_t m, size_t n, const std::vector<T> & v)
    {
        if(m*n!=v.size()) {
            throw std::invalid_argument("Matrix size does not match storage size");
        }
        unbounded_array<T> storage(m*n);
        std::copy(v.begin(), v.end(), storage.begin());
        return matrix<T>(m, n, storage);
    }

    template <typename T>
    matrix<T> matrix_from_multi_array(multi_array<T, 2> &ref) {
        std::vector<size_t> shape(ref.shape(), ref.shape()+2);
        matrix<T> result(shape[0], shape[1]);
        for (size_t i = 0; i < shape[0]; i++) {
            for (size_t j = 0; j < shape[1]; j++) {
                result(i,j) = ref[i][j];
            }
        }
        return result;
    }

    template<typename T, size_t NDims>
    std::vector<size_t> shape_from_multi_array(const multi_array<T, NDims> &a) {
        return std::vector<size_t>(a.shape(), a.shape() + NDims);
    }

    template<typename T>
    int get_index(std::vector<T> v, T el)
    {
        auto it = std::find(v.begin(), v.end(), el);
        return  it != v.end() ? it - v.begin() : -1;
    }
}

namespace sqsgenerator::utils {

        template<typename R, typename P>
        inline R factorial(P p) {
            R r {1}, c {1};
            for (; c <= p; c += 1 ) r *= c;
            return r;
        }

        template<typename T>
        T identity(T &t) {
            return t;
        }

        template<typename T>
        T round_nplaces(T value, uint8_t to)
        {
            T places = std::pow(10.0, to);
            return std::round(value * places) / places;
        }
}

#endif //SQSGENERATOR_UTILS_H
