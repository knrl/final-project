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
/* BINDTOOL_HEADER_FILE(ofdm_sampler.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(e70eed86fa6180c88cda1700441a68f4)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <ieee802_11/ofdm_sampler.h>
// pydoc.h is automatically generated in the build directory
#include <ofdm_sampler_pydoc.h>

void bind_ofdm_sampler(py::module& m)
{

    using ofdm_sampler = gr::ieee802_11::ofdm_sampler;


    py::class_<ofdm_sampler, gr::block, gr::basic_block, std::shared_ptr<ofdm_sampler>>(
        m, "ofdm_sampler", D(ofdm_sampler))

        .def(py::init(&ofdm_sampler::make),
             py::arg("fft_length"),
             py::arg("symbol_length"),
             py::arg("timeout") = 1000,
             D(ofdm_sampler, make))


        ;
}