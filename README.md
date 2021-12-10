#GNU Radio Blocks for the FLEX-6000 Waveform API

This module implements various blocks to talk to the FlexRadio Systems FLEX-6000 series of radios via the Waveform API.  The successfully build and run this software, you will need to compile and install the Waveform API at [https://github.com/flexradio/waveform-sdk](https://github.com/flexradio/waveform-sdk).

Note that these blocks use the Waveform API, which is intended for the implementation of new modulation modes in the radio.  This is a slightly different model than the "DAX" and "DAX IQ" interfaces on the radio that have different characteristics and capabilities.  Notably, unlike DAX IQ, the Waveform interface is restricted to a 24ksps sample rate.  This should be adequate for most amatuer applications, but may provide issues for commercial wideband applications.  This may change in the future.

We do not recommend targeting these blocks as an "on-radio" waveform solution, since there are limited resources on the radio to run GNURadio in all its glory.  Implementors should count on running this waveform external to the radio.

It would be beneficial for a user of these blocks to review the Waveform API README.md that will explain some details about the lifecycle and general information about a waveform.

There is a wiki with information on the FLEX-6000 TCP command API at [http://wiki.flexradio.com](http://wiki.flexradio.com) that implementors may find useful.  Note that some of this documentation is old and does not reflect the current state of the art.  We are working on updates to bring it current.

For assistance, please feel free to file issues and pull requests on GitHub.  Members of the FlexRadio software group do monitor these and deal with them as appropriate.