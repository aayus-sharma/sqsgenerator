//
// Created by dominik on 14.07.21.
//

#ifndef SQSGENERATOR_DATA_HPP
#define SQSGENERATOR_DATA_HPP
#include "types.hpp"
#include "helpers.hpp"
#include "atomistics.hpp"
#include "containers.hpp"
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>

namespace py = boost::python;
namespace np = boost::python::numpy;
namespace atomistics = sqsgenerator::utils::atomistics;

namespace sqsgenerator::python {

    class SQSResultPythonWrapper {
    private:
        const SQSResult& m_handle;
    public:

        explicit SQSResultPythonWrapper(const SQSResult &other);
        double objective();
        cpp_int rank();
        np::ndarray configuration();
        np::ndarray parameters(py::tuple const &shape);
        const SQSResult& handle();
    };

    class StructurePythonWrapper {
    private:
        atomistics::Structure m_handle;
    public:
        StructurePythonWrapper(np::ndarray lattice, np::ndarray frac_coords, py::object symbols, py::tuple pbc);
        np::ndarray lattice();
        np::ndarray frac_coords();
        py::list species();
        py::tuple pbc();
        np::ndarray distance_vecs();
        np::ndarray distance_matrix();
        np::ndarray shell_matrix(uint8_t prec = 5);
        size_t num_atoms();
        atomistics::Structure& handle();
    };

    typedef std::vector<SQSResultPythonWrapper> SQSResultCollection;
}

#endif //SQSGENERATOR_DATA_HPP
