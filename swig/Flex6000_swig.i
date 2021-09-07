/* -*- c++ -*- */

#define FLEX6000_API

%include "gnuradio.i"           // the common stuff

//load generated python docstrings
%include "Flex6000_swig_doc.i"

%{
#include "Flex6000/flex_radio_6000_sink.h"
#include "Flex6000/flex_radio_6000_source.h"
#include "Flex6000/flex_radio_6000_command.h"
#include "Flex6000/flex_radio_6000_configuration.h"
%}


%include "Flex6000/flex_radio_6000_sink.h"
GR_SWIG_BLOCK_MAGIC2(Flex6000, flex_radio_6000_sink);
%include "Flex6000/flex_radio_6000_source.h"
GR_SWIG_BLOCK_MAGIC2(Flex6000, flex_radio_6000_source);
%include "Flex6000/flex_radio_6000_command.h"
GR_SWIG_BLOCK_MAGIC2(Flex6000, flex_radio_6000_command);

%include "Flex6000/flex_radio_6000_configuration.h"
GR_SWIG_BLOCK_MAGIC2(Flex6000, flex_radio_6000_configuration);
