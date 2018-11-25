#pragma once

#include <dlib/array2d.h>
#include <dlib/pixel.h>


#include "cinder/Cinder.h"
#include "cinder/Surface.h"
#include "cinder/ImageIo.h"

using namespace dlib;

namespace cinder {

    inline void toDlib( const Surface8uRef &r, array2d<rgb_pixel> &pixels_out){
        uint8_t* data = r->getData();
        pixels_out = array2d<rgb_pixel>(r->getWidth(), r->getHeight());
        uint i = 0, j = 0;
        while(i < r->getWidth() * r->getHeight()){
            pixels_out[j%r->getWidth()][j/r->getWidth()] = rgb_pixel(data[i], data[i+1], data[i+2]);            
            i+=3;
            j++;
        }
    }
    
    //{
    //    return cv::Rect( r.x1, r.y1, r.getWidth(), r.getHeight() );
    //}
//
//class ImageTargetCvMat : public ImageTarget {
//  public:
//    static std::shared_ptr<ImageTargetCvMat> createRef( cv::Mat *mat ) { return std::shared_ptr<ImageTargetCvMat>( new ImageTargetCvMat( mat ) ); }
//
//    virtual bool hasAlpha() const { return mMat->channels() == 4; }
//    virtual void*    getRowPointer( int32_t row ) { return reinterpret_cast<void*>( reinterpret_cast<uint8_t*>(mMat->data) + row * mMat->step ); }
//
//  protected:
//    ImageTargetCvMat( cv::Mat *mat );
//
//    cv::Mat        *mMat;
//};
//
//template<typename pixel_type>
//class ImageSourceDlib : public ci::ImageSource {
//    public:
//
//    typedef typename dlib::pixel_traits<pixel_type>::basic_pixel_type basic_pixel_type;
//
//    ImageSourceDlib(const dlib::array2d<pixel_type>& in) : ci::ImageSource()
//    {
//        mWidth = dlib::num_columns(in);
//        mHeight = dlib::num_rows(in);
//        setColorModel(getDlibColorModel<pixel_type>());
//        setChannelOrder(getDlibChannelOrder<pixel_type>());
//        setDataType(getDlibDataType<pixel_type>());
//        mRowBytes = (int32_t)dlib::width_step(in);
//        mData = reinterpret_cast<const basic_pixel_type*>(dlib::image_data(in));
//    }
//
//    ImageSourceDlib(const dlib::matrix<pixel_type>& in) : ci::ImageSource()
//    {
//        mWidth = dlib::num_columns(in);
//        mHeight = dlib::num_rows(in);
//        setColorModel(getDlibColorModel<pixel_type>());
//        setChannelOrder(getDlibChannelOrder<pixel_type>());
//        setDataType(getDlibDataType<pixel_type>());
//        mRowBytes = (int32_t)dlib::width_step(in);
//        mData = reinterpret_cast<const basic_pixel_type*>(dlib::image_data(in));
//    }
//
//    const basic_pixel_type*        mData;
//    int32_t                     mRowBytes;
//
//};
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// ImageTargetCvMat
//inline ImageTargetCvMat::ImageTargetCvMat( cv::Mat *mat )
//    : ImageTarget(), mMat( mat )
//{
//    switch( mat->depth() ) {
//        case CV_8U: setDataType( ImageIo::UINT8 ); break;
//        case CV_16U: setDataType( ImageIo::UINT16 ); break;
//        case CV_32F: setDataType( ImageIo::FLOAT32 ); break;
//        default:
//            throw ImageIoExceptionIllegalDataType();
//    }
//
//    switch( mat->channels() ) {
//        case 1:
//            setColorModel( ImageIo::CM_GRAY );
//            setChannelOrder( ImageIo::Y );
//        break;
//        case 3:
//            setColorModel( ImageIo::CM_RGB );
//            setChannelOrder( ImageIo::BGR );
//        break;
//        case 4:
//            setColorModel( ImageIo::CM_RGB );
//            setChannelOrder( ImageIo::BGRA );
//        break;
//        default:
//            throw ImageIoExceptionIllegalColorModel();
//        break;
//    }
//}
//
//inline cv::Mat toOcv( ci::ImageSourceRef sourceRef, int type = -1 )
//{
//    if( type == -1 ) {
//        int depth = CV_8U;
//        if( sourceRef->getDataType() == ImageIo::UINT16 )
//            depth = CV_16U;
//        else if( ( sourceRef->getDataType() == ImageIo::FLOAT32 ) || ( sourceRef->getDataType() == ImageIo::FLOAT16 ) )
//            depth = CV_32F;
//        int channels = ImageIo::channelOrderNumChannels( sourceRef->getChannelOrder() );
//        type = CV_MAKETYPE( depth, channels );
//    }
//
//    cv::Mat result( sourceRef->getHeight(), sourceRef->getWidth(), type );
//    ImageTargetRef target = ImageTargetCvMat::createRef( &result );
//    sourceRef->load( target );
//    return result;
//}
//
//inline cv::Mat toOcvRef( Channel8u &channel )
//{
//    return cv::Mat( channel.getHeight(), channel.getWidth(), CV_MAKETYPE( CV_8U, 1 ), channel.getData(), channel.getRowBytes() );
//}
//
//inline cv::Mat toOcvRef( Channel16u &channel )
//{
//    return cv::Mat( channel.getHeight(), channel.getWidth(), CV_MAKETYPE( CV_16U, 1 ), channel.getData(), channel.getRowBytes() );
//}
//
//inline cv::Mat toOcvRef( Channel32f &channel )
//{
//    return cv::Mat( channel.getHeight(), channel.getWidth(), CV_MAKETYPE( CV_32F, 1 ), channel.getData(), channel.getRowBytes() );
//}
//
//inline cv::Mat toOcvRef( Surface8u &surface )
//{
//    return cv::Mat( surface.getHeight(), surface.getWidth(), CV_MAKETYPE( CV_8U, surface.hasAlpha()?4:3), surface.getData(), surface.getRowBytes() );
//}
//
//inline cv::Mat toOcvRef( Surface16u &surface )
//{
//    return cv::Mat( surface.getHeight(), surface.getWidth(), CV_MAKETYPE( CV_16U, surface.hasAlpha()?4:3), surface.getData(), surface.getRowBytes() );
//}
//
//inline cv::Mat toOcvRef( Surface32f &surface )
//{
//    return cv::Mat( surface.getHeight(), surface.getWidth(), CV_MAKETYPE( CV_32F, surface.hasAlpha()?4:3), surface.getData(), surface.getRowBytes() );
//}
//
//inline ImageSourceRef fromOcv( cv::Mat &mat )
//{
//    return ImageSourceRef( new ImageSourceCvMat( mat ) );
//}
//
//inline ImageSourceRef fromOcv( cv::UMat &umat )
//{
//    return ImageSourceRef( new ImageSourceCvMat( umat.getMat( cv::ACCESS_READ ) ) );
//}
//
//inline cv::Scalar toOcv( const Color &color )
//{
//    return CV_RGB( color.r * 255, color.g * 255, color.b * 255 );
//}
//
//inline vec2 fromOcv( const cv::Point2f &point )
//{
//    return vec2( point.x, point.y );
//}
//
//inline cv::Point2f toOcv( const vec2 &point )
//{
//    return cv::Point2f( point.x, point.y );
//}
//
//inline ivec2 fromOcv( const cv::Point &point )
//{
//    return ivec2( point.x, point.y );
//}
//
//inline cv::Point toOcv( const ivec2 &point )
//{
//    return cv::Point( point.x, point.y );
//}
//
//inline cv::Rect toOcv( const ci::Area &r )
//{
//    return cv::Rect( r.x1, r.y1, r.getWidth(), r.getHeight() );
//}
//
//inline ci::Area fromOcv( const cv::Rect &r )
//{
//    return Area( r.x, r.y, r.x + r.width, r.y + r.height );
//}

} // namespace cinder
