#pragma once

#include <dlib/array2d.h>
#include <dlib/pixel.h>


#include "cinder/Cinder.h"
#include "cinder/Surface.h"
#include "cinder/ImageIo.h"
#include "cinder/Log.h"

using namespace dlib;

namespace cinder {

	template<typename pixel_type>
	class ImageTargetDlib : public ImageTarget {
	public:

		typedef typename dlib::pixel_traits<pixel_type>::basic_pixel_type basic_pixel_type;

		static std::shared_ptr<ImageTargetDlib> createRef(array2d<pixel_type> *pixels) { return std::shared_ptr<ImageTargetDlib>(new ImageTargetDlib(pixels)); }

		virtual bool hasAlpha() const { return dlib::pixel_traits<pixel_type>::has_alpha; }
		virtual void*getRowPointer(int32_t row) { 
			return reinterpret_cast<void*>(
				reinterpret_cast<uint8_t*>(mPixels->begin()) + row*mPixels->width_step()
				);
		}
	
	protected:
		ImageTargetDlib(array2d<pixel_type> *pixels) : ImageTarget(), mPixels(pixels)
		{

			setDataType(ImageIo::UINT8);
			if (dlib::pixel_traits<pixel_type>::rgb ||
				dlib::pixel_traits<pixel_type>::rgb_alpha ||
				dlib::pixel_traits<pixel_type>::hsi ||
				dlib::pixel_traits<pixel_type>::lab) {
				setDataType(ImageIo::UINT8);
			}
			else if (sizeof(dlib::pixel_traits<pixel_type>::basic_pixel_type) > 1) {
				setDataType(ImageIo::FLOAT32);
			}
			else {
				throw ImageIoExceptionIllegalDataType();
			}

			switch (dlib::pixel_traits<pixel_type>::num) {
			case 1:
				setColorModel(ImageIo::CM_GRAY);
				setChannelOrder(ImageIo::Y);
				CI_LOG_D("SET TO GRAY");

				break;
			case 3:
				setColorModel(ImageIo::CM_RGB);
				setChannelOrder(ImageIo::RGB);
				CI_LOG_D("SET TO RGB");
				break;
			case 4:
				setColorModel(ImageIo::CM_RGB);
				setChannelOrder(ImageIo::RGBA);
				CI_LOG_D("SET TO RGBA");
				break;
			default:
				throw ImageIoExceptionIllegalColorModel();
				break;
			}
		};


		array2d<pixel_type> *mPixels;
	};




	template<typename pixel_type>
	class ImageSourceDlib : public ci::ImageSource {
	public:

		typedef typename dlib::pixel_traits<pixel_type>::basic_pixel_type basic_pixel_type;

		ImageSourceDlib(const dlib::array2d<pixel_type>& in) : ci::ImageSource()
		{
			mWidth = dlib::num_columns(in);
			mHeight = dlib::num_rows(in);
			setColorModel(getDlibColorModel<pixel_type>());
			setChannelOrder(getDlibChannelOrder<pixel_type>());
			setDataType(getDlibDataType<pixel_type>());
			mRowBytes = (int32_t)dlib::width_step(in);
			mData = reinterpret_cast<const basic_pixel_type*>(dlib::image_data(in));
		}

		ImageSourceDlib(const dlib::matrix<pixel_type>& in) : ci::ImageSource()
		{
			mWidth = dlib::num_columns(in);
			mHeight = dlib::num_rows(in);
			setColorModel(getDlibColorModel<pixel_type>());
			setChannelOrder(getDlibChannelOrder<pixel_type>());
			setDataType(getDlibDataType<pixel_type>());
			mRowBytes = (int32_t)dlib::width_step(in);
			mData = reinterpret_cast<const basic_pixel_type*>(dlib::image_data(in));
		}

		void load(ci::ImageTargetRef target);

		const basic_pixel_type*		mData;
		int32_t                     mRowBytes;

	};

	template<typename pixel_type>
	inline cinder::ImageIo::ColorModel getDlibColorModel() {
		if (dlib::pixel_traits<pixel_type>::grayscale) {
			return cinder::ImageIo::CM_GRAY;
		}
		else if (dlib::pixel_traits<pixel_type>::rgb || dlib::pixel_traits<pixel_type>::rgb_alpha) {
			return cinder::ImageIo::CM_RGB;
		}
		else {
			return cinder::ImageIo::ColorModel::CM_UNKNOWN;
		}
	}

	template<typename pixel_type>
	inline cinder::ImageIo::ChannelOrder getDlibChannelOrder() {
		
		if (dlib::pixel_traits<pixel_type>::basic_pixel_type == rgb_pixel) {
			return cinder::ImageIo::RGB;
		}

		if (dlib::pixel_traits<pixel_type>::basic_pixel_type == rgb_alpha_pixel) {
			return cinder::ImageIo::RGBA;
		}

		if (dlib::pixel_traits<pixel_type>::basic_pixel_type == bgr_pixel) {
			return cinder::ImageIo::BGR;
		}

		return cinder::ImageIo::RGB;

	}


	template<typename pixel_type>
	inline cinder::ImageIo::DataType getDlibDataType() {
		if (sizeof(dlib::pixel_traits<pixel_type>::basic_pixel_type) == 1) {
			return cinder::ImageIo::UINT8;
		}
		else {
			return cinder::ImageIo::FLOAT32;
		}
	}
	
	template<typename pixel_type>
	inline cinder::ImageSourceRef fromDLib(array2d<pixel_type> * data)
	{
		return ImageSourceRef(new ImageSourceDLib(data));
	}


	template<typename pixel_type>
	inline array2d<pixel_type> toDlib(cinder::ImageSourceRef sourceRef) {

		array2d<pixel_type> result(sourceRef->getHeight(), sourceRef->getWidth());
		ImageTargetRef target = ImageTargetDlib<pixel_type>::createRef(&result);
		sourceRef->load(target);
		return result;
	}





} // namespace cinder