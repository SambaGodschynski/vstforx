#include "RemoteChannelHandler.h"


namespace processing {
//=============================================================================
// struct RemoteChannel::Buffer
//=============================================================================
//-----------------------------------------------------------------------------
void RemoteChannel::Buffer::write(float **in, size_t numSamples) {
	size_t c=0;
	int i=wrote;
	for (; i<wrote+numSamples; ++i) {
		(*this)[i] = in[0][c++];
	}
	wrote = i;
}
//----------------------------------------------------------------------------- 
void RemoteChannel::Buffer::read(float **out, size_t numSamples) const {
	if(wrote<preBuffer)
		return;
	size_t c=0;
	int start = wrote - preBuffer;
	for (int i=start; i<=start + numSamples; ++i) {
		out[0][c]   = (*this)[i];
		out[1][c++] = (*this)[i];
	}
}

} // namespace
