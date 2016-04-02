#include "opencvhdfs.h"

#include <iostream>
#include <stdexcept>

namespace CVHDFS
{

H5::PredType datatypeCV2H5( int cv_type )
{
	switch( CV_MAT_DEPTH( cv_type ) )
	{
		case CV_16S:
			return H5::PredType::NATIVE_INT16;
		case CV_16U:
			return H5::PredType::NATIVE_UINT16;
		case CV_32F:
			return H5::PredType::NATIVE_FLOAT;
		case CV_32S:
			return H5::PredType::NATIVE_INT32;
		case CV_64F:
			return H5::PredType::NATIVE_DOUBLE;
		case CV_8S:
			return H5::PredType::NATIVE_INT8;
		case CV_8U:
			return H5::PredType::NATIVE_UINT8;
	}
	throw std::runtime_error("Unknown OpenCV type format.");
}

void write( H5::H5File file, std::string key, cv::Mat matrix )
{
	hsize_t      dims[3]  = { matrix.rows, matrix.cols, matrix.channels()};
	H5::DataSpace mspace( 3, dims);

	H5::PredType datatype = datatypeCV2H5( matrix.type() );

	H5::DataSet dataset = file.createDataSet( key, datatype, mspace);

	H5::DataSpace fspace = dataset.getSpace ();
	hsize_t      offset[3]  = { 0, 0, 0};
	fspace.selectHyperslab( H5S_SELECT_SET, dims, offset );

	dataset.write( matrix.data, datatype, mspace );
}

void write( std::string filename, std::string key, cv::Mat matrix )
{
	H5::Exception::dontPrint(); // it would be nice if I could restore this settings, I hate globals
	H5::H5File file;
	try
	{
		file = H5::H5File( filename, H5F_ACC_RDWR );
		

	}
	catch( H5::FileIException error )
	{
		file = H5::H5File( filename, H5F_ACC_TRUNC );
		
	}
	write( file, key, matrix );
	
	
}


void read( H5::H5File file, std::string key, cv::Mat &matrix )
{
	H5::DataSet dataset = file.openDataSet( key );

	H5::DataSpace dataspace = dataset.getSpace();

	hsize_t dims_out[3];
	int rank = dataspace.getSimpleExtentDims( dims_out, NULL);
	if(  rank!=3) {
		throw std::runtime_error("CVHDFS supports only rank 3 datasets.");
	}

	int channel_count = dims_out[2];
	if( channel_count<1 || channel_count>4 )
	{
		throw std::runtime_error("unsuported number of channels.");
	}

	int cvtype;
	H5::PredType h5type = H5::PredType::NATIVE_INT8;

	H5T_class_t type_class = dataset.getTypeClass();

	if ( type_class == H5T_INTEGER )
	{
		H5::IntType intype = dataset.getIntType();
		size_t size = intype.getSize();
		bool is_signed = (intype.getSign() != H5T_SGN_NONE);
		if( size==1 && is_signed )
		{
			cvtype = CV_8SC(channel_count);
			h5type = H5::PredType::NATIVE_INT8;
		}
		else if(size==1 && !is_signed)
		{
			cvtype = CV_8UC(channel_count);
			h5type = H5::PredType::NATIVE_UINT8;
		}
		else if(size==2 && is_signed)
		{
			cvtype = CV_16SC(channel_count);
			h5type = H5::PredType::NATIVE_INT16;
		}
		else if(size==2 && !is_signed)
		{
			cvtype = CV_16UC(channel_count);
			h5type = H5::PredType::NATIVE_UINT16;
		}
		else if(size==4 && is_signed)
		{
			cvtype = CV_32SC(channel_count);
			h5type = H5::PredType::NATIVE_INT32;
		}
		else
		{
			throw std::runtime_error("unsuported integer type.");
		}

	}
	else if ( type_class == H5T_FLOAT )
	{
		H5::FloatType floattype = dataset.getFloatType();
		size_t size = floattype.getSize();
		if( size==4 )
		{
			cvtype = CV_32FC(channel_count);
			h5type = H5::PredType::NATIVE_FLOAT;
		}
		else if( size==8 )
		{
			cvtype = CV_64FC(channel_count);
			h5type = H5::PredType::NATIVE_DOUBLE;
		}
		else
		{
			throw std::runtime_error("unsuported floating type.");
		}
	}
	else
	{
		throw std::runtime_error("dataset's type class is not supported.");
	}

	matrix.create( dims_out[0], dims_out[1], cvtype );


	hsize_t      offset[3];   // hyperslab offset in the file
	offset[0] = 0;
	offset[1] = 0;
	offset[2] = 0;
	dataspace.selectHyperslab( H5S_SELECT_SET, dims_out, offset );

	H5::DataSpace memspace( rank, dims_out );

	dataset.read( matrix.data, h5type, memspace, dataspace );
}


void read( std::string filename, std::string key, cv::Mat &matrix )
{
	H5::Exception::dontPrint(); // it would be nice if I could restore this settings, I hate globals
	H5::H5File file( filename, H5F_ACC_RDONLY );
	read( file, key, matrix );
	
}

	
} // namespace CVHDFS