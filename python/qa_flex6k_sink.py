#!/usr/bin/env python
# SPDX-License-Identifier: LGPL-3.0-or-later
# @file qa_flex6k_sink.py $
# @author nlong
#
# $Revision: LPI-LPD-Development_nlong/8 $
# $Date: 2021/08/03 15:34:31 $
# $LastEditedBy: nlong $
#
# @copyright: (c) 2018-2021 by Southwest Research Institute®
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, version 3.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#


from gnuradio import gr, gr_unittest
from gnuradio import blocks
import flex6k_swig as flex6k

class qa_flex6k_sink(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_t(self):
        # set up fg
        self.tb.run()
        # check data


if __name__ == '__main__':
    gr_unittest.run(qa_flex6k_sink)
