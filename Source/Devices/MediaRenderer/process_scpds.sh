#!/bin/sh

set -x

# abort on any errors
set -e

# AVTransport
../../../Targets/x86-unknown-linux/Release/TextToHeader -v RDR_AVTransportSCPD -h AVTransport AVTransportSCPDFull.xml AVTransportSCPD.cpp

# Rendering Control
../../../Targets/x86-unknown-linux/Release/TextToHeader -v RDR_RenderingControlSCPD -h RenderingControl RenderingControlSCPD_Full.xml RenderingControlSCPD.cpp

# ConnectionManager
../../../Targets/x86-unknown-linux/Release/TextToHeader -v RDR_ConnectionManagerSCPD -h ConnectionManager RdrConnectionManagerSCPD.xml RdrConnectionManagerSCPD.cpp
