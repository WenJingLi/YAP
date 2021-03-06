#pragma once

#include "..\Interface\YapInterfaces.h"
#include <complex>
#include <vector>
// #include "Localization.h"
#include "..\interface\SmartPtr.h"
#include <assert.h>

namespace Yap
{
	struct Dimension
	{
		DimensionType type;
		unsigned int start_index;
		unsigned int length;
		Dimension();
		Dimension(DimensionType type, unsigned start_index, unsigned int length);
	};

	class CDimensionsImp : public IDimensions
	{
	public:
		CDimensionsImp();
		CDimensionsImp(unsigned int dimension_count);
		CDimensionsImp(const CDimensionsImp& source);
		CDimensionsImp(IDimensions * source);

		bool ModifyDimension(DimensionType type, unsigned int length, unsigned int start_index = 0);
		unsigned int GetLength(unsigned int dimension_index);

		virtual unsigned int GetDimensionCount();
		virtual bool GetDimensionInfo(unsigned int dimension_index, DimensionType & dimension_type, unsigned int& start_index, unsigned int& length);
		virtual bool SetDimensionInfo(unsigned int dimension_index, DimensionType dimension_type, 
			unsigned int start_index, unsigned int length);

		virtual IDimensions * Clone();
		virtual void Release();
		CDimensionsImp & operator() (DimensionType type, unsigned int index, unsigned int length);
	private:
		std::vector<Dimension> _dimension_info;
	};

	template<typename T, int DATA_TYPE>
	class CDataImp : public IData, public IMemory
	{
	public:
		CDataImp(T* data, const CDimensionsImp& dimensions, IMemory * parent = nullptr, bool own_data = false) :
			_data(data), _own_memory(own_data), _use_count(0), _parent(parent)
		{
			assert(data != nullptr);
			_dimensions = dimensions;
		}
		
		CDataImp(T* data, IDimensions * dimensions, IMemory * parent = nullptr, bool own_data = false) : 
			_data(data), _own_memory(own_data), _dimensions(dimensions), _use_count(0), _parent(parent)
		{
			assert(data != nullptr);
		}

		CDataImp(IDimensions * dimensions) : _own_memory(true), _dimensions(dimensions), _use_count(0)
		{
			unsigned int total_size = 1;

			for (unsigned int i = 0; i < _dimensions.GetDimensionCount(); ++i)
			{
				total_size *= _dimensions.GetLength(i);
			}

			_data = new T[total_size];
		}


// 		void SetLocalization(CLocalization& localization)
// 		{
// 			_localization = localization;
// 		}
// 
// 		void SetSliceLocalization(IParams* param, unsigned int image_index) // indexΪimage_index, �������
// 		{
// 			_localization = C2dLocalization(param, image_index);
// 		}

// 		void SetSlabLocalization(IParams* param, unsigned int slab_index)
// 		{
// 			_localization = C3dLocalization(param, slab_index);
// 		}
// 
// 		virtual ILocalization * GetLocalization()
// 		{
// 			return &_localization;
// 		}

		virtual IDimensions * GetDimensions() override
		{
			return &_dimensions;
		}

		virtual void * GetData()
		{
			return _data;
		}

		virtual void Lock()
		{
			++_use_count;
			if (_parent)
			{
				_parent->Lock();
			}
		}

		virtual void Release()
		{
			assert(_use_count > 0);
			--_use_count;

			if (_parent)
			{
				_parent->Release();
			}

			if (_use_count == 0)
			{
				delete this;
			}
		}

	private:
		~CDataImp()
		{
			if (_own_memory)
			{
				delete []_data;
			}
		}

		virtual int GetDataType() override
		{
			return DATA_TYPE;
		}

		T * _data;
		CDimensionsImp _dimensions;
//		CLocalization _localization;

		unsigned int _use_count;
		bool _own_memory;
		CSmartPtr<IMemory> _parent;
	};

	typedef CDataImp<double, DataTypeDouble> CDoubleData;
	typedef CDataImp<float, DataTypeFloat> CFloatData;
	typedef CDataImp<std::complex<double>, DataTypeComplexDouble> CComplexDoubleData;
	typedef CDataImp<std::complex<float>, DataTypeComplexFloat> CComplexFloatData;
};

