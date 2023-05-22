/* -*- c++ -*- */
/*
 * Copyright 2023 mkaanerol.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ofdm_insert_preamble_tagged_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>
#include <sys/time.h>
#include <uhd/types/time_spec.hpp>

namespace gr {
namespace ieee802_11 {

ofdm_insert_preamble_tagged::sptr
ofdm_insert_preamble_tagged::make(int fft_length,
                                  const std::vector<std::vector<gr_complex>>& preamble)
{
    return gnuradio::make_block_sptr<ofdm_insert_preamble_tagged_impl>(fft_length,
                                                                       preamble);
}

/*
 * The private constructor
 */
ofdm_insert_preamble_tagged_impl::ofdm_insert_preamble_tagged_impl(
    int fft_length, const std::vector<std::vector<gr_complex>>& preamble)
    : gr::tagged_stream_block("ofdm_insert_preamble_tagged",
              gr::io_signature::make(1, 1, fft_length*sizeof(gr_complex)),
              gr::io_signature::make(1, 1, fft_length*sizeof(gr_complex)), "packet_len"),
	fft_len(fft_length),
	preamble_symbols(preamble)
{
    //log.open("log_OFDM_preamble.txt", std::fstream::in | std::fstream::out | std::fstream::trunc);
    set_tag_propagation_policy(TPP_DONT); 	// do not propagate tags	
    //std::cout << "hoy";
}

/*
 * Our virtual destructor.
 */
ofdm_insert_preamble_tagged_impl::~ofdm_insert_preamble_tagged_impl() {}


int ofdm_insert_preamble_tagged_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
{
    //std::cout << ninput_items[0] << "req\n";
    int noutput_items = ninput_items[0]+ preamble_symbols.size(); 
    return noutput_items ;
}

void ofdm_insert_preamble_tagged_impl::add_time_tag(const double delay_sec)
{
    // here we add time tag to output stream
    static const pmt::pmt_t srcid = pmt::string_to_symbol(alias());
    static const pmt::pmt_t time_key = pmt::string_to_symbol("tx_time");
    const uint64_t item = nitems_written(0);
    struct timeval t;
    gettimeofday(&t, NULL);
    // uhd::time_spec_t now = uhd::time_spec_t(t.tv_sec + t.tv_usec / 1000000.0)
    //	+ uhd::time_spec_t(double(delay_sec));

    const pmt::pmt_t time_value = pmt::make_tuple(
        pmt::from_uint64(t.tv_sec+2),
        pmt::from_double(t.tv_usec)
    );
    add_item_tag(0, item, time_key, time_value, srcid);
    //---------------------------------------------------

    return;
}

int ofdm_insert_preamble_tagged_impl::work(int noutput_items,
                                           gr_vector_int& ninput_items,
                                           gr_vector_const_void_star& input_items,
                                           gr_vector_void_star& output_items)
{
    auto in = static_cast<const gr_complex*>(input_items[0]);
    auto out = static_cast<gr_complex*>(output_items[0]);
   
	float output_packet_length;
    output_packet_length = ninput_items[0] + preamble_symbols.size();
	memset(out, 0, output_packet_length*fft_len*sizeof(gr_complex));
	
	//add_time_tag(0.1);

	int symbol = 0;
	for (; symbol < preamble_symbols.size(); symbol++){
		for (int i = 0; i < fft_len; i++ ){
			out[symbol*fft_len + i] = preamble_symbols[symbol][i];
			//log << out[symbol*fft_len + i] << std::endl;
		}
	}
	//log << "-----------------------------\n";
	for (int consumed_symbols = 0; consumed_symbols < ninput_items[0]; consumed_symbols++){
		for (int i = 0; i < fft_len; i++ ){
			out[symbol*fft_len + i] = in[consumed_symbols*fft_len+i];
		}
		symbol++;
	}

    // Tell runtime system how many output items we produced.
    return symbol;
}

} /* namespace ieee802_11 */
} /* namespace gr */
