//
// Created by dominik on 08.07.21.
//

#include "utils.hpp"
#include "settings.hpp"
#include <stdexcept>
#include <utility>

using namespace sqsgenerator::utils;

namespace sqsgenerator {
/*        Structure &m_structure;
        uint8_t m_prec;
        int m_niterations;
        int m_noutput_configurations;
        size_t m_nspecies;
        configuration_t m_configuration_packing_indices;
        configuration_t m_packed_configuration;
        pair_shell_weights_t m_shell_weights;
        iteration_mode m_mode;
        array_2d_t m_parameter_weights;
        array_3d_t m_target_objective;
        array_3d_t m_parameter_prefactors;*/

    typedef array_3d_t::index index_t;


    IterationSettings::IterationSettings(Structure &structure, const_array_3d_ref_t target_objective, const_array_2d_ref_t parameter_weights,  const pair_shell_weights_t &shell_weights, int iterations, int output_configurations, iteration_mode mode, uint8_t prec) :
        m_structure(structure),
        m_prec(prec),
        m_niterations(iterations),
        m_noutput_configurations(output_configurations),
        m_nspecies(unique_species(structure.configuration()).size()),
        m_mode(mode),
        m_parameter_weights(parameter_weights),
        m_target_objective(target_objective),
        m_parameter_prefactors(boost::extents[static_cast<index_t>(shell_weights.size())][static_cast<index_t>(m_nspecies)][static_cast<index_t>(m_nspecies)])
    {
        auto shell_m(shell_matrix());
        auto num_elements {num_atoms()*num_atoms()};
        std::set<shell_t> unique(shell_m.data(), shell_m.data() + num_elements);
        m_available_shells = std::vector<shell_t>(unique.begin(), unique.end());
        std::sort(m_available_shells.begin(), m_available_shells.end());
        m_available_shells.erase(m_available_shells.begin());
        for (const auto &s : m_available_shells) {
            if (shell_weights.count(s)) m_shell_weights.emplace(std::make_pair(s, shell_weights.at(s)));
        }
        if (!m_shell_weights.size()) throw std::invalid_argument("None of the shells you have specified are available");
        std::tie(m_configuration_packing_indices, m_packed_configuration) = pack_configuration(structure.configuration());
        init_prefactors();
    }

    [[nodiscard]] std::vector<shell_t> IterationSettings::available_shells() const {
        return m_available_shells;
    }

    void IterationSettings::init_prefactors() {
        auto nshells {static_cast<index_t>(num_shells())};
        auto nspecies {static_cast<index_t>(num_species())};
        auto natoms {static_cast<index_t>(num_atoms())};
        auto natoms_d {static_cast<double>(num_atoms())};

        std::vector<shell_t> shells = std::get<0>(shell_indices_and_weights());
        std::map<shell_t, size_t> neighbor_count;
        for (const auto &shell: shells) neighbor_count.emplace(std::make_pair(shell, 0));
        auto shell_mat = shell_matrix();
        for (index_t i = 1; i < natoms; i++) {
            auto neighbor_shell {shell_mat[0][i]};
            if (neighbor_count.count(neighbor_shell)) neighbor_count[neighbor_shell]++;
        }
        auto hist = configuration_histogram(m_packed_configuration);
        for (index_t i = 0; i < nshells; i++) {
            double M_i {static_cast<double>(neighbor_count[shells[i]])};
            for (index_t a = 0; a < nspecies; a++) {
                double x_a {static_cast<double>(hist[a])/natoms_d};
                for (index_t b = a; b < nspecies; b++) {
                    double x_b {static_cast<double>(hist[b])/natoms_d};
                    double prefactor {1.0/(M_i*x_a*x_b*natoms_d)};
                    m_parameter_prefactors[i][a][b] = prefactor;
                    m_parameter_prefactors[i][b][a] = prefactor;
                }
            }
        }
    }


    const_pair_shell_matrix_ref_t IterationSettings::shell_matrix() {
        return m_structure.shell_matrix(m_prec);
    }

    [[nodiscard]] std::vector<AtomPair> IterationSettings::pair_list() const {
        return m_structure.create_pair_list(m_shell_weights);
    }

    [[nodiscard]] const Structure& IterationSettings::structure() const {
        return m_structure;
    }

    [[nodiscard]] size_t IterationSettings::num_atoms() const {
        return m_structure.num_atoms();
    }

    [[nodiscard]] int IterationSettings::num_iterations() const {
        return m_niterations;
    }

    [[nodiscard]] size_t IterationSettings::num_species() const {
        return m_nspecies;
    }

    [[nodiscard]] size_t IterationSettings::num_shells() const {
        return m_shell_weights.size();
    }

    [[nodiscard]] int IterationSettings::num_output_configurations() const {
        return m_noutput_configurations;
    }

    [[nodiscard]] const_array_3d_ref_t IterationSettings::target_objective() const {
        return m_target_objective;
    }

    pair_shell_weights_t IterationSettings::shell_weights() const {
        return m_shell_weights;
    }

    [[nodiscard]] configuration_t IterationSettings::packed_configuraton() const {
        return m_packed_configuration;
    }

    [[nodiscard]] const_array_2d_ref_t IterationSettings::parameter_weights() const {
        return m_parameter_weights;
    }

    iteration_mode IterationSettings::mode() const {
        return m_mode;
    }

    [[nodiscard]] std::tuple<std::vector<shell_t>, std::vector<double>> IterationSettings::shell_indices_and_weights() const {
        std::vector<shell_t> shells;
        std::vector<double> sorted_shell_weights;
        for (const auto &weight: m_shell_weights) shells.push_back(weight.first);
        std::sort(shells.begin(), shells.end());
        for (const auto &shell: shells) sorted_shell_weights.push_back(m_shell_weights.at(shell));
        return std::make_tuple(shells, sorted_shell_weights);
    }

    [[nodiscard]] const_array_3d_ref_t IterationSettings::parameter_prefactors() const {
        return m_parameter_prefactors;
    }

    [[nodiscard]] configuration_t IterationSettings::unpack_configuration(const configuration_t &conf) const{
        return utils::unpack_configuration(m_configuration_packing_indices, conf);
    }

}