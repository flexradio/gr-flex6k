// SPDX-License-Identifier: LGPL-3.0-or-later
/// @file flex_status_tree.cc $ 
/// @brief base interface for flex radio 
/// @author nlong
///
/// $Revision: LPI-LPD-Development_nlong/8 $
/// $Date: 2021/08/03 15:34:31 $
/// $LastEditedBy: nlong $
///
/// @copyright: (c) 2018-2021 by Southwest Research Institute®
///
/// This program is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Lesser General Public License as published by
/// the Free Software Foundation, version 3.
///
/// This program is distributed in the hope that it will be useful, but
/// WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
/// Lesser General Public License for more details.
///
/// You should have received a copy of the GNU Lesser General Public License
/// along with this program. If not, see <http://www.gnu.org/licenses/>.
///


/**
 * $Log: flex_status_tree.cc $
 * Revision LPI-LPD-Development_nlong/4 2021/08/03 15:34:31 nlong
 *    added/fixed copyrights
 * 
 * 
 **************************************************************************/

#include "flex_status_tree.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/optional.hpp>
#include <vector>
#include <iostream>
#include <boost/lexical_cast.hpp>

flex::status_tree::status_tree()
{
    // std::cout<<"status tree created: "<<this<<std::endl;
}

flex::status_tree::~status_tree()
{
    // std::cout<<"status tree destroyed: "<<this<<std::endl;
}

int flex::status_tree::statusParser(unsigned int argc, char *argv[])
{
    //iterate through the list of args, for each item check if it is key=val
    //if it is place it at the node indicated by the first few keys (without pairs)
    
    std::lock_guard<std::mutex> guard(m_lock);
    // std::cout<<"storing tree"<<std::endl;
    std::vector<std::string> parts;

    parts.reserve(2);

    std::string path;
    bool firstTime = true;
    for (size_t i = 0; i < argc; i++)
    {
        if (boost::algorithm::starts_with(path, "meter"))
        {
            //it is one long string with spaces being separate commands...
            std::string meter_str;
            bool first = 1;
            for (size_t ii = i; ii < argc; ii++)
            {
                std::string s(argv[ii]);
                if (first)
                {
                    meter_str = s;
                    first = 0;
                }
                else
                {
                    meter_str = meter_str + " " + s;
                }
            }
            if (boost::ends_with(meter_str," removed"))
            {
                // example string:
                //27 removed
                // std::cout<<meter_str<<std::endl;
                boost::split(parts, meter_str, boost::is_any_of(" "));
                
                if (auto child = m_tree.get_child_optional(path + parts[0])){
                    child.value().clear();
                }

            }else{
            // example string:
            // 27.src=SLC#27.num=0#27.nam=AGC+#27.low=-150.0#27.hi=0.0#27.desc=Signal strength after AGC#27.unit=dBFS#27.fps=10#
            boost::split(parts, meter_str, boost::is_any_of("."));
            std::string meter_idx = parts[0];
            boost::split_regex(parts, meter_str.substr(meter_idx.size() + 1, meter_str.size() - 2 - meter_idx.size()), boost::regex("#" + meter_idx + "."));
            // std::cout << "got config info for meter "<<meter_idx << std::endl;
            std::vector<std::string> subparts;
            for (auto part:parts){
                boost::split(subparts, part, boost::is_any_of("="));

                m_tree.put(path + meter_idx +"." +subparts[0], subparts[1]);
            }
            }


            break;
        }

        //convert to string
        parts.clear();
        std::string tmpStr = std::string(argv[i]);
        boost::split(parts, tmpStr, boost::is_any_of("="));

        if (parts.size() == 1)
        {
            path.append(parts[0]);
            path.append(".");
        }
        else
        {
            // if (firstTime){
            //     // auto child = m_tree.get_child_optional("slice.0");
            //     if (auto child = m_tree.get_child_optional(path)){
            //         child.value().clear();
            //     }
            //     firstTime=false;
            // }
            m_tree.put(path + parts[0], parts[1]);
        }
    }
    return 0;
}


void flex::status_tree::send_local_command(flex::waveformlib_ptrdef::waveform_response_cb_t cb,
                                           void *arg, const std::string &command)
{
    //maybe later i will return something now, i just check if it says print tree

    if (boost::algorithm::starts_with(command, "local tree"))
    {
        printTree();
    }

    if (boost::algorithm::starts_with(command, "local get slicenum "))
    {
        std::vector<std::string> parts;
        boost::algorithm::split(parts, command, boost::is_any_of(" "));

        std::vector<std::string> vec;
        getSliceNum(parts[3], vec);
        std::cout << parts[3] << " is active on slice";
        for (auto &it : vec)
        {
            std::cout << " " << it;
        }
        std::cout << std::endl;
    }
}

void flex::status_tree::printTree()
{
    // std::cout<<"printing tree"<<std::endl;
    std::lock_guard<std::mutex> guard(m_lock);
    std::ostringstream out;
    boost::property_tree::info_parser::write_info(out, m_tree);
    std::cout << "\n***  PRINTING TREE ***\n"
              << out.str() << "\n*** DONE PRINTING TREE ***\n"
              << std::endl;
}

int flex::status_tree::getSliceNum(const std::string shortName, std::vector<std::string> &sliceVec)
{
    //iterate through children:
    // std::cout<<"getting slice num"<<std::endl;
    sliceVec.clear();
    std::lock_guard<std::mutex> guard(m_lock);
    if (auto child = m_tree.get_child_optional("slice"))
    {   
        // std::cout<<"found slice child"<<std::endl;
        //slice parameter was found, iterate through slices...
        for (auto &it : child.value())
        {
            // std::cout<<"found slice = "<<it.first<<std::endl;
            // check if slice is active
            if (it.second.get<bool>("in_use"))
            {
                if (it.second.get<std::string>("mode") == shortName)
                {
                    sliceVec.push_back(boost::lexical_cast<std::string>(it.first));
                }
            }

            // std::cout<<"asd"<<std::endl;
        }
        if (sliceVec.size() > 0)
        {
            return 0;
        }
    }

    //failure:
    return -1;
}
