/* -*- c++ -*- */

#define FLEX6000_API

%include "gnuradio.i"           // the common stuff

//load generated python docstrings
%include "Flex6000_swig_doc.i"

%{
#include "Flex6000/flex6k_sink.h"
#include "Flex6000/flex6k_source.h"
#include "Flex6000/flex6k_command.h"
#include "Flex6000/flex6k_config.h"
%}


%include "Flex6000/flex6k_sink.h"
GR_SWIG_BLOCK_MAGIC2(Flex6000, flex6k_sink);
%include "Flex6000/flex6k_source.h"
GR_SWIG_BLOCK_MAGIC2(Flex6000, flex6k_source);
%include "Flex6000/flex6k_command.h"
GR_SWIG_BLOCK_MAGIC2(Flex6000, flex6k_command);

%include "Flex6000/flex6k_config.h"
GR_SWIG_BLOCK_MAGIC2(Flex6000, flex6k_config);
