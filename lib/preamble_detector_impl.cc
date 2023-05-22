/* -*- c++ -*- */
/*
 * Copyright 2023 mkaanerol.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "preamble_detector_impl.h"
#include <gnuradio/io_signature.h>
#include <fstream>
#include <stdio.h>

namespace gr {
namespace ieee802_11 {

preamble_detector::sptr preamble_detector::make()
{
    return gnuradio::make_block_sptr<preamble_detector_impl>();
}


/*
 * The private constructor
 */
preamble_detector_impl::preamble_detector_impl()
    : gr::block("preamble_detector",
        gr::io_signature::make3(3, 3, sizeof(gr_complex),sizeof(gr_complex),sizeof(float)),
        gr::io_signature::make(1, 1, sizeof(gr_complex)))
{
    preamble_length = 64; 		//in symbols
    packet_length = 2*128 + 4*512; 	//in symbols : pilot + payload
    num_of_packets = 32;		//between consequent preambles
    samp_per_symb = 1;

    preamble_mode = preamble_length?true:false;	//state 
    samples_sent = 0;
    transient_counter = 0;
    preambles_received = 0;
    history = 0;			// symbol history
    log.open("log_preamble.txt", std::fstream::in | std::fstream::out | std::fstream::trunc);
    set_history(1 + history); 
    
}

/*
 * Our virtual destructor.
 */
preamble_detector_impl::~preamble_detector_impl() 
{
    log.close();
}

void preamble_detector_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] = noutput_items;
	ninput_items_required[1] = noutput_items;
	ninput_items_required[2] = noutput_items;
}

int preamble_detector_impl::general_work(int noutput_items,
                                         gr_vector_int& ninput_items,
                                         gr_vector_const_void_star& input_items,
                                         gr_vector_void_star& output_items)
{
    auto in   = static_cast<const gr_complex*>(input_items[0]);
    auto corr = static_cast<const gr_complex*>(input_items[1]);
    auto tr   = static_cast<const float*>(input_items[2]);
    auto out  = static_cast<gr_complex*>(output_items[0]);

	int consumed_inputs = history; 
	int generated_outputs = 0;

	// as long as we have items in input buffer we process symbols
	for(;consumed_inputs < noutput_items;consumed_inputs++){
		// discarding transient (non-valid) inputs
		if (transient_counter < (preamble_length-1)*samp_per_symb){
			transient_counter++;
			continue;
		}
		if (preamble_mode){
			// calculating norm of autocorrealtion
			gr_complex norm = corr[consumed_inputs]*conj(corr[consumed_inputs]);
			float treshold = tr[consumed_inputs];

			// comparing the norm with treshold
			if(norm.real() > treshold){
				//beigining of preamble is detected
				preambles_received = 1;
				preamble_mode = false;
			}

		} else{
			// discarding preambles
			if (preambles_received < preamble_length*samp_per_symb){
				preambles_received++;
				continue;
			}
			//simply forwarding samples
			out[generated_outputs] = in[consumed_inputs];
			samples_sent ++;
			generated_outputs ++;

			// check if we have reached the end of the packet
			if(num_of_packets != -1 && samples_sent == num_of_packets*packet_length*samp_per_symb){
				preamble_mode = true;
				preambles_received = 0;
				samples_sent = 0 ;
			}
		}
	}
    // Tell runtime system how many input items we consumed on
    // each input stream.
    consume_each (consumed_inputs-history);

    // Tell runtime system how many output items we produced.
    return generated_outputs;
}

} /* namespace ieee802_11 */
} /* namespace gr */
