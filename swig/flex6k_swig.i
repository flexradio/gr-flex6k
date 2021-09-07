/* -*- c++ -*- */

#define FLEX6K_API

%include "gnuradio.i"           // the common stuff

//load generated python docstrings
%include "flex6k_swig_doc.i"

%{
#include "flex6k/flex6k_sink.h"
#include "flex6k/flex6k_source.h"
#include "flex6k/flex6k_command.h"
#include "flex6k/flex6k_config.h"
%}


%include "flex6k/flex6k_sink.h"
GR_SWIG_BLOCK_MAGIC2(flex6k, flex6k_sink);
%include "flex6k/flex6k_source.h"
GR_SWIG_BLOCK_MAGIC2(flex6k, flex6k_source);
%include "flex6k/flex6k_command.h"
GR_SWIG_BLOCK_MAGIC2(flex6k, flex6k_command);

%include "flex6k/flex6k_config.h"
GR_SWIG_BLOCK_MAGIC2(flex6k, flex6k_config);
