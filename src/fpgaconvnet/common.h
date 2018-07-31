#ifndef FPGACONVNET_COMMON_H
#define FPGACONVNET_COMMON_H

#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "fpgaconvnet/protos/parameters.pb.h"


namespace fpgaconvnet
{

typedef uint16_t fixed_point_t;
protos::Network load_network_proto(const std::string & filename);


class Exception : public std::exception {
private:
    std::string message;
public:
    Exception(const std::string & message);
    virtual ~Exception() throw();
    virtual const char* what() const throw();
};


namespace logging {

const int DEBUG = 0;
const int INFO = 1;
const int WARNING = 2;
const int ERROR = 3;

std::ostream& stdout(int level = INFO);
void indent();
void dedent();
void log_prefix(const std::string & prefix);
void set_level(int level);

class Indentation {
public:
    Indentation();
    ~Indentation();
};

}  // logging

namespace math {

double rng(const double lo, const double hi);
uint64_t gcd(uint64_t a, uint64_t b);
uint64_t lcm(uint64_t a, uint64_t b);
uint64_t div_ceil(uint64_t a, uint64_t b);


}  // math


namespace calculation
{

// In bytes per second
const double PCIE_BANDWIDTH    = 4e9;  // 2GB/s in each direction, so 4GB total
const double LMEM_BANDWIDTH    = 38e9; // 38GB/s in TOTAL
const double MAXRING_BANDWIDTH = 5e9;

/* Throughput calculation in terms of images a second.
 *
 * Bitstream, in the comment below, refers to the maxfiles that are used
 * to configured A PIPELINE OF FPGAs that runs simultaneously. (It is
 * useful to think of a bistream as a set of maxfiles that will be
 * reconfigured simultaneously).
 *
 *  The different kinds of throughput means different things:
 *
 *    - [pipeline throughput]: The throughput of a bitstream when compiled
 *                             to several FPGAs arranged in a single
 *                             (massive) pipeline of devices. This is
 *                             the primary evaluation metric when measuring
 *                             the performance of a pipeline that doesn't
 *                             permit reconfiguration. This is the primary
 *                             evaluation where latency is still somewhat
 *                             sensitive, but not critical (eg: game-playing
 *                             bots which needs <10ms latency) but
 *                             throughput is still very important. (so
 *                             that we have good FPS).
 *           
 *    - [effective throuhgput]: The throughput of a bitstream when
 *                              considering several parallel similarly
 *                              configured pipelines running simultaneously.
 *                              This throughput is not useful for overall
 *                              evaluation, but useful for indentifying
 *                              performance bottleneck of bistreams.
 *
 *    - [real throughput]: The real throughput that utilises as much
 *                         resources as required from the given resources.
 *                         This never simply returns the pipeline throughput,
 *                         even when it is possible to get higher pipeline
 *                         throughput whilist sacrificing effective
 *                         throughput. This is the primary evaluation
 *                         metric for latency-insensitive batch processing.
 *                         
 */
double pipeline_throughput(
        const protos::Network & network, const unsigned bitstream_id);
double effective_throughput(
        const protos::Network & network, const unsigned bitstream_id);
void explain_throughput(const protos::Network & network);
double real_throughput(const protos::Network & network);
double min_num_fpga_real_throughput(const protos::Network & network);
unsigned min_num_fpga_needed(const protos::Network & network);

/* The total number of network operations in the network. */
double ops(const protos::Network & network);

uint64_t total_multipliers(const protos::LayerParameter & layer);

/* The number of iterations to perform a pixel calculation. */
uint64_t kernel_iterations(const protos::LayerParameter & layer);
uint64_t convolution_iterations(const protos::LayerParameter & layer);
uint64_t scheduler_iterations(const protos::LayerParameter & layer);
uint64_t total_iterations(const protos::LayerParameter &layer);

/* Weight initialization */
uint64_t total_kernel_weights(const protos::LayerParameter & layer);
uint64_t conv_in_size(const protos::Network & network);
uint64_t total_rom_size(const protos::LayerParameter & layer);
uint64_t weights_vector_size(const protos::LayerParameter & layer);
bool is_layer_cpu_initialized(const protos::LayerParameter & layer);

/* Stream size for inputs */
uint64_t bias_stream_size(const protos::LayerParameter & layer);
uint64_t cpu_weights_stream_size(const protos::LayerParameter & layer);


}  // calculation

protos::Network insert_fpga_positions(protos::Network, std::vector<int>);
std::vector<protos::Network> split_by_bitstreams(protos::Network);

}  // fpgaconvnet

#endif
