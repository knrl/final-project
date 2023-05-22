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
/* BINDTOOL_HEADER_FILE(my_strobe.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(82511e6f67e724526b5e834e09ce621c)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <ieee802_11/my_strobe.h>
// pydoc.h is automatically generated in the build directory
#include <my_strobe_pydoc.h>

void bind_my_strobe(py::module& m)
{

    using my_strobe = gr::ieee802_11::my_strobe;


    py::class_<my_strobe, gr::block, gr::basic_block, std::shared_ptr<my_strobe>>(
        m, "my_strobe", D(my_strobe))

        .def(py::init(&my_strobe::make),
             py::arg("msg"),
             py::arg("period_ms"),
             D(my_strobe, make))


        ;
}