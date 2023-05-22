/*
 * Copyright 2023 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(alamouti_ofdm_rx.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(2b42bea529b4e5a6d0a270345453f06b)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <ieee802_11/alamouti_ofdm_rx.h>
// pydoc.h is automatically generated in the build directory
#include <alamouti_ofdm_rx_pydoc.h>

void bind_alamouti_ofdm_rx(py::module& m)
{

    using alamouti_ofdm_rx = gr::ieee802_11::alamouti_ofdm_rx;


    py::class_<alamouti_ofdm_rx,
               gr::block,
               gr::basic_block,
               std::shared_ptr<alamouti_ofdm_rx>>(
        m, "alamouti_ofdm_rx", D(alamouti_ofdm_rx))

        .def(py::init(&alamouti_ofdm_rx::make),
             py::arg("occupied_carriers"),
             py::arg("fft_length"),
             py::arg("cplen"),
             D(alamouti_ofdm_rx, make))


        ;
}