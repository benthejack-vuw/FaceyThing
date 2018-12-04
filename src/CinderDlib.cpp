#include "CinderDlib.h"

/*template<typename pixel_type>
cinder::ImageTargetDlib<pixel_type>::ImageTargetDlib(array2d<pixel_type> * pixels)
	*/



template<typename pixel_type>
void cinder::ImageSourceDlib<pixel_type>::load(ci::ImageTargetRef target) {
	// get a pointer to the ImageSource function appropriate for handling our data configuration
	ci::ImageSource::RowFunc func = setupRowFunc(target);
	const size_t numChannels = ci::ImageIo::channelOrderNumChannels(mChannelOrder);

	// seperate the process for UINT8, UINT16 and FLOAT32 DataTypes        
	if (getDataType() == ci::ImageIo::DataType::UINT8) {
		for (int32_t row = 0; row < mHeight; ++row) {
			((*this).*func)(target, row, mData);
			mData += mRowBytes;
		}
	}
	else if (getDataType() == ci::ImageIo::DataType::FLOAT32) {
		std::vector<float> data(mWidth * mHeight * numChannels, 0);
		for (auto& val : data) {
			val = ci::lmap<float>(*mData, mSourceValueMin, mSourceValueMax, 0.0f, 1.0f);
			mData++;
		}

		for (int32_t row = 0; row < mHeight; ++row) {
			std::vector<float> rowData(data.begin() + mWidth * numChannels * row, data.begin() + mWidth * numChannels * (row + 1));
			((*this).*func)(target, row, rowData.data());
		}
	}
	else if (getDataType() == ci::ImageIo::DataType::UINT16) {
		std::vector<uint16_t> data(mWidth * mHeight * numChannels, 0);
		for (auto& val : data) {
			val = ci::lmap<uint16_t>(*mData, mSourceValueMin, mSourceValueMax, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());
			mData++;
		}

		for (int32_t row = 0; row < mHeight; ++row) {
			std::vector<uint16_t> rowData(data.begin() + mWidth * numChannels * row, data.begin() + mWidth * numChannels * (row + 1));
			((*this).*func)(target, row, rowData.data());
		}
	}
}