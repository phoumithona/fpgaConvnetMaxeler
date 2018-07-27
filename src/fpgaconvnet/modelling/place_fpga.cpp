#include <iostream>

#include "assert.h"

#include <fpgaconvnet/modelling/resource_model.h>
#include <fpgaconvnet/modelling/place_fpga.h>


namespace fpgaconvnet {
namespace modelling {

void PositionFpga::search_recur(std::vector<int> v)
{
    assert (v.size() <= reference_network.layer_size());
    assert (v.size() > 0);

    if (v.size() == reference_network.layer_size()) {
        auto network = reference_network;
        auto resource = fpgaconvnet::resource_model::project(
                fpgaconvnet::insert_fpga_positions(network, v));

        considered_solutions++;
        if (fpgaconvnet::resource_model::meets_resource_constraints(resource)) {
            solutions.push_back(v);
            accepted_solutions++;
        }
        return;
    }

    v.push_back(v.back());
    if (true) {
        // TODO(fyq14): check resource constraints here to prune
        //              search space here
        search_recur(v);
    }
    v.pop_back();

    // TODO: Allow more than the availble FPGA, ie: handle reconfigurations.
    if (v.back() < reference_network.num_fpga_available() - 1) {
        v.push_back(v.back() + 1);
        search_recur(v);
        v.pop_back();
    }
}

PositionFpga::PositionFpga(fpgaconvnet::protos::Network network)
    : reference_network(network)
{
    done = false;
    considered_solutions = 0;
    accepted_solutions = 0;
}

void PositionFpga::search()
{
    assert(!done);

    auto v = std::vector<int>();
    v.push_back(0);
    search_recur(v);
    done = true;
}

unsigned
PositionFpga::get_num_accepted_solutions()
{
    return accepted_solutions;
}

unsigned
PositionFpga::get_num_considered_solutions()
{
    return considered_solutions;
}

std::vector<std::vector<int>>
PositionFpga::get_solutions()
{
    return solutions;
}

}  // modelling
}  // fpgaconvnet
