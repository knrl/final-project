/* -*- c++ -*- */
/*
 * Copyright 2023 mkaanerol.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ofdm_demapper_impl.h"
#include <gnuradio/io_signature.h>

#include <gnuradio/expj.h>
#include <cstdio>
#include <iostream>

#define VERBOSE 0	// set this variaable to view logs

namespace gr {
namespace ieee802_11 {

ofdm_demapper::sptr ofdm_demapper::make(unsigned int occupied_carriers)
{
    return gnuradio::make_block_sptr<ofdm_demapper_impl>(occupied_carriers);
}


inline void ofdm_demapper_impl::enter_search()
{
    state = STATE_SYNC_SEARCH;
    phase = 0;
    freq = 0;
    packetlen_cnt = 0;
    packetlen = 1;
    if (VERBOSE)
        printf("search state \n");
}

inline void ofdm_demapper_impl::enter_have_sync()
{
    state = STATE_HAVE_SYNC;
    packetlen_cnt = 0;
    packetlen = 1;
    if (VERBOSE)
        printf("have_sync state\n");
}

inline void ofdm_demapper_impl::enter_have_first_symbol()
{
    state = STATE_HAVE_FIRST_SYMBOL;
    if (VERBOSE)
        printf("have_first_symbol state \n");
}

inline void ofdm_demapper_impl::enter_have_header()
{
    state = STATE_HAVE_HEADER;

    // header consists of two 16-bit shorts in network byte order
    // payload length is lower 12 bits
    // 4 upper bit is unused
    packetlen =  (header >> 16) & 0x0fff;	//packet length in bytes

    if (VERBOSE)
    printf("have_header state (raw payload_len = %d)",packetlen);

    packetlen = ceil(packetlen*8.0);		//packet length in bits
    packetlen = packetlen/ bits_per_symbol; 	//packet length in symbols
}

unsigned char ofdm_demapper_impl::slicer(const gr_complex x)
{
    unsigned int table_size = constell.size();
    unsigned int min_index = 0;
    float min_euclid_dist = norm(x - constell[0]);
    float euclid_dist = 0;
// finding minimum euclidan distance from constellation points
    for (unsigned int j = 1; j < table_size; j++){
        euclid_dist = norm(x - constell[j]);
        if (euclid_dist < min_euclid_dist){
            min_euclid_dist = euclid_dist;
            min_index = j;
        }
    }

    return min_index;
}

void ofdm_demapper_impl::demodulate_header(const gr_complex *in)
{
    gr_complex carrier;
    carrier=gr_expj(phase);
    header = 0;

    unsigned int header_offset = 3;
    gr_complex accum_error = 0.0;
    for (unsigned int i=0;i < HEADERCONSTPOINTS; i++) {
    // demodualting header
        gr_complex sigrot = in[subcarrier_map[i]]*carrier;
        unsigned char bits = slicer(sigrot);
        header |= (bits << (30- i*2));

    // calculating error
        gr_complex closest_sym = constell[bits];
        accum_error += sigrot * conj(closest_sym);
    }

    // phase, frequency, angle compensations
    float angle = arg(accum_error);
    freq = freq - freq_gain*angle;
    phase = phase + freq - phase_gain*angle;
    if (phase >= 2*M_PI) phase -= 2*M_PI;
    if (phase <0) phase += 2*M_PI;


    if(VERBOSE){
        printf("angle:%.6f freq:%.6f phase:%.6f \n", angle, freq, phase);
        //printf("SNR-estimate: %.4f dB\n",10*log10(snrest));
    }
}

bool ofdm_demapper_impl::header_ok()
{
    // confirm that two copies of header info are identical
    if (VERBOSE)
        printf( "Received header: 0x%08x \n",header);

    if(((header >> 16) ^ (header & 0xffff)) == 0 )
        return true;

    return false;
//	return true;
}

/*
 * The private constructor
 */
ofdm_demapper_impl::ofdm_demapper_impl(unsigned int occupied_carriers)
    : gr::block("ofdm_demapper",
        gr::io_signature::make2(2,2,sizeof(gr_complex)*occupied_carriers,sizeof(char)),
        gr::io_signature::make2(2,2,sizeof(gr_complex),sizeof(char))),
        d_occupied_carriers(occupied_carriers)
{
    log.open("log_OFDM_demapper.txt", std::fstream::in | std::fstream::out | std::fstream::trunc);

    phase = 0;
    freq = 0;
    // sensitivity to offsets
    phase_gain = 0.25;
    freq_gain = 0.25*0.25/4;
    eq_gain = 0.05;

    for (int i = d_occupied_carriers/2 ; i > 2 ; i--){
        subcarrier_map.push_back(d_occupied_carriers/2 - i);
    }

    for (int i = 2 ; i < d_occupied_carriers/2 ; i++){
        subcarrier_map.push_back(d_occupied_carriers/2 + i );
    }

    constell.push_back(gr_complex(1,1));
    constell.push_back(gr_complex(-1,1));
    constell.push_back(gr_complex(-1,-1));
    constell.push_back(gr_complex(1,-1));

    bits_per_symbol = (unsigned long)ceil(log10(constell.size()) / log10(2.0));
    enter_search(); // set initial state

    set_output_multiple(d_occupied_carriers);	
    set_relative_rate(d_occupied_carriers);
}

/*
 * Our virtual destructor.
 */
ofdm_demapper_impl::~ofdm_demapper_impl() {}

void ofdm_demapper_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
	// we prcess inouts one by one (it's easier to write algorithm and handle states)
  	int input_required = 1;
  	unsigned ninputs = ninput_items_required.size();
  	for (unsigned int i = 0; i < ninputs; i++) {
    		ninput_items_required[i] = input_required;
  	}
}

int ofdm_demapper_impl::general_work(int noutput_items,
                                     gr_vector_int& ninput_items,
                                     gr_vector_const_void_star& input_items,
                                     gr_vector_void_star& output_items)
{
    auto in       = static_cast<const gr_complex*>(input_items[0]);
    auto in_sync  = static_cast<const char*>(input_items[1]);
    auto out      = static_cast<gr_complex*>(output_items[0]);
    auto out_sync = static_cast<char*>(output_items[1]);

    unsigned int subcarrier_index = 0;

    int optr = 0;

    switch( state){ 
    case STATE_SYNC_SEARCH: // Search for a new incoming symbol
        if(in_sync[0]){ // Found the sync pulse from decoder block (originaly from sampler block)
    // sync pulse is at the begining of 1st preamble (see decoder block code)
    // discard the 1st preamble
        enter_have_sync();
    }
    break;

    case STATE_HAVE_SYNC:
        // discard the 2nd preamble
    enter_have_first_symbol();
    break;

    case STATE_HAVE_FIRST_SYMBOL: // We have the first symbol
    if(in_sync[0]){
        enter_have_sync();
        consume_each(1);
        return optr;
    }

    // Find out how many constellation points that are going to be extracted
demodulate_header(&in[0]);

    // Check if we have a correct header
    if (header_ok()){
            enter_have_header();

            subcarrier_index = HEADERCONSTPOINTS; 

        // adding packet length tag
        const uint64_t nwritten = nitems_written(0);
        const pmt::pmt_t key   = pmt::string_to_symbol(std::string("packet_len"));
        const pmt::pmt_t value = pmt::from_long(long(packetlen));
        add_item_tag(0, nwritten,key,value);

            // extract constellation point from the 1st symbol
            while((subcarrier_index <  subcarrier_map.size() &&  packetlen_cnt <  packetlen)){
            out_sync[optr] = 0;  
            out[optr] = in[ subcarrier_map[subcarrier_index]];
            subcarrier_index++;
            optr++; 
            packetlen_cnt++;
            }
            out_sync[0] = 1;  	//out sync signal , at the begining of packet
    }
    else{ // The header was bad
            if (VERBOSE)
            printf("Bad header: 0x%08x \n", header);
            enter_search();
    }

    break;

    case STATE_HAVE_HEADER:
        if(in_sync[0] == 1){ // Found the sync pulse
            // discard the 1st preamble
            enter_have_sync();
            consume_each(1);
            return optr;
        }

        while((subcarrier_index <  subcarrier_map.size() &&  packetlen_cnt <  packetlen)){
            // Fixme do we have to compensate for a phase drift?
            out_sync[optr] = 0;
        out[optr] = in[ subcarrier_map[subcarrier_index]];
        subcarrier_index++;
        optr++; 
        packetlen_cnt++;
    }
    break;

    default:
    throw std::runtime_error("ofdm_demapper_impl entered an unknown state in work");
    break;
    } 

    // We are done with this packet
    if ( packetlen_cnt ==  packetlen && optr > 0){
        enter_search();
    }


    assert (optr <= noutput_items);
    consume_each(1);
    return optr;
}

} /* namespace ieee802_11 */
} /* namespace gr */
