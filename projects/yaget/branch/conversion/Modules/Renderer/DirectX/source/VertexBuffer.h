/////////////////////////////////////////////////////////////////////////
// VertexBuffer.h
//
//  Copyright 5/30/2009 Edgar Glowacki.
//
// NOTES:
//      This handles creation and management of vertex buffer
//      and proivides facility to fill actuall data using boost::function
//
//
// #include "VertexBuffer.h"
//
/////////////////////////////////////////////////////////////////////////
//! \file

#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H
#pragma once

#include "Base.h"
#include <atlbase.h>
#include <D3dx9core.h>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace eg
{

    class VertexBuffer
    {
        // no copy semantics
        VertexBuffer(const VertexBuffer&);
        VertexBuffer& operator=(const VertexBuffer&);

    public:
        //! Create valid vertex buffer
        //!
        //! \param device D3D valid devicd
        //! \param size how big this buffer in bytes
        VertexBuffer(CComPtr<IDirect3DDevice9> device, size_t size);
        ~VertexBuffer();

        typedef boost::function<size_t (uint8_t * /*data*/)> Generator_t;
        //! If Generator_t function is called with data = 0, then user should return
        //! size of needed memory.
        //! \return unique identifier for this generator


        //! Thsi will triiger callback to fill in VertexBuffer with data.
        //! It will handle all appropriate locking, unlocking and memory management.
        //!
        //! \param generator callback function to exceute when filling vertex buffer with user data
        //! \param vertexSize vertex structure size in bytes
        //! \param type D3DPRIMITIVETYPE DX9 type
        //! \param layout pointer to DX9 D3DVERTEXELEMENT9
        //!
        //! \return uint32_t if error, it will return (uint32_t)-1, otherwise any other number is valid
        uint32_t Generate(Generator_t generator, size_t vertexSize, D3DPRIMITIVETYPE type, D3DVERTEXELEMENT9 *layout);

        //! Trigger specific generator.
        void Generate(uint32_t token);

        //! Render this vertex buffer with all appropriate shaders and materials
        void Render(uint32_t token) const;

    private:
        struct Block
        {
            Generator_t generator;
            size_t next;
            size_t vertexSize;
            size_t numPrims;
            D3DPRIMITIVETYPE type;
            CComPtr<IDirect3DVertexDeclaration9> declaration;
        };

        std::vector<Block> mBlocks;

        CComPtr<IDirect3DDevice9> mDevice;
        CComPtr<IDirect3DVertexBuffer9> mBuffer;
        size_t mBufferSize;
        uint32_t mNextBufferIndex;
    };

    typedef boost::shared_ptr<VertexBuffer> VertexBuffer_t;


} // namespace eg

#endif // VERTEX_BUFFER_H

