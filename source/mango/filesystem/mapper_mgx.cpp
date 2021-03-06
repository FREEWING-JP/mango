/*
    MANGO Multimedia Development Platform
    Copyright (C) 2012-2016 Twilight Finland 3D Oy Ltd. All rights reserved.
*/
#include <map>
#include <mango/core/pointer.hpp>
#include <mango/core/string.hpp>
#include <mango/core/exception.hpp>
#include <mango/filesystem/mapper.hpp>
#include <mango/filesystem/path.hpp>

#include "../../external/lz4/lz4.h"

#define ID ".mgx mapper: "

#if 0

// TODO

namespace
{

    using namespace mango;

	struct Block
	{
		uint64 offset;
		uint64 size;
	};

	struct Archive
	{
		uint32 signature;
		uint32 version;
		Block directory;
	};

	struct Directory
	{
		uint32 signature;
	};

	struct FileHeader
	{
		Block compressed;
		uint64 uncompressed;
		uint32 checksum;
		uint32 compression;
		uint32 filenameSize;
	};

} // namespace

namespace mango
{

    // -----------------------------------------------------------------
    // MapperMGX
    // -----------------------------------------------------------------

    class MapperMGX : public Mapper
    {
    public:

        bool isfile(const std::string& filename) const override
        {
        }

        void index(FileIndex& index, const std::string& pathname) override
        {
        }

        VirtualMemory* mmap(const std::string& filename) override
        {
        }
    };

    // -----------------------------------------------------------------
    // functions
    // -----------------------------------------------------------------

    Mapper* createMapperMGX(Memory parent, const std::string& password)
    {
        Mapper* mapper = new MapperMGX(parent, password);
        return mapper;
    }

} // namespace mango

#else

    void dummy_mgx_function_to_satisfy_linker()
    {
        // TODO: remove this when .mgx support is done
    }

#endif // 0
