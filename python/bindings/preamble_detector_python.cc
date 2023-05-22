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
/* BINDTOOL_HEADER_FILE(preamble_detector.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(2dffc7034998294a820be31a78ddd408)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <ieee802_11/preamble_detector.h>
// pydoc.h is automatically generated in the build directory
#include <preamble_detector_pydoc.h>

void bind_preamble_detector(py::module& m)
{

    using preamble_detector = gr::ieee802_11::preamble_detector;


    py::class_<preamble_detector,
               gr::block,
               gr::basic_block,
               std::shared_ptr<preamble_detector>>(
        m, "preamble_detector", D(preamble_detector))

        .def(py::init(&preamble_detector::make), D(preamble_detector, make))


        ;
}
