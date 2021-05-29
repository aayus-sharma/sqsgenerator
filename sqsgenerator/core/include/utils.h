//
// Created by dominik on 29.05.21.
//

#ifndef SQSGENERATOR_UTILS_H
#define SQSGENERATOR_UTILS_H

namespace boost{
    template<class MultiArray>
    void extentTo(MultiArray & ma, const MultiArray & other){ //this function is adapted from
        auto& otherShape = reinterpret_cast<boost::array<size_t, MultiArray::dimensionality> const&>(*other.shape());
        ma.resize(otherShape);
    }
}
#endif //SQSGENERATOR_UTILS_H
