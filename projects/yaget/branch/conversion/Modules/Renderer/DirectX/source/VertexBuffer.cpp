//#include "Precompiled.h"
#include "VertexBuffer.h"
#include "Exception/Exception.h"
#include "DXErrorHandler.h"
#include <boost/foreach.hpp>


namespace eg {

VertexBuffer::VertexBuffer(CComPtr<IDirect3DDevice9> device, size_t size)
: mDevice(device)
, mBufferSize(size)
, mNextBufferIndex(0)
{
    HRESULT hr = mDevice->CreateVertexBuffer(mBufferSize, D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &mBuffer, 0);
    if (FAILED(hr))
    {
        HR_PRINT(hr);

        using namespace boost;
        throw ex::bad_init(str(format("Could not create VertexBuffer of %1% size. %2%") % size % dx::parse(hr)).c_str());
    }
}


VertexBuffer::~VertexBuffer()
{
}


uint32_t VertexBuffer::Generate(Generator_t generator, size_t vertexSize, D3DPRIMITIVETYPE type, D3DVERTEXELEMENT9 *layout)
{
    size_t dataSize = generator(0);

    CComPtr<IDirect3DVertexDeclaration9> declaration;
    HRESULT hr = mDevice->CreateVertexDeclaration(layout, &declaration);
    if (FAILED(hr))
    {
        HR_PRINT(hr);
        return (uint32_t)-1;
    }

    DWORD lockFlags = D3DLOCK_NOOVERWRITE;
    if (mNextBufferIndex > mBufferSize - dataSize)
    {
        lockFlags = D3DLOCK_DISCARD;
        mNextBufferIndex = 0;
    }

    uint8_t *pBytes;
    hr = mBuffer->Lock(mNextBufferIndex, dataSize, (void **)&pBytes, lockFlags);
    if (FAILED(hr))
    {
        HR_PRINT(hr);
        return (uint32_t)-1;
    }

    dataSize = generator(pBytes);
    hr = mBuffer->Unlock();
    HR_REPORT_INVALID(hr);

    Block block;
    block.generator = generator;
    block.next = mNextBufferIndex;
    block.vertexSize = vertexSize;
    block.type = type;
    block.numPrims = dataSize/vertexSize;
    block.declaration = declaration;
    mBlocks.push_back(block);

    mNextBufferIndex += dataSize;
    return mBlocks.size() - 1;
}


void VertexBuffer::Generate(uint32_t token)
{
    if (token >= 0 && token < mBlocks.size())
    {
        Block& block = mBlocks[token];

        size_t dataSize = block.generator(0);
        DWORD lockFlags = D3DLOCK_NOOVERWRITE;
        if (mNextBufferIndex > mBufferSize - dataSize)
        {
            lockFlags = D3DLOCK_DISCARD;
            mNextBufferIndex = 0;
        }

        uint8_t *pBytes;
        HRESULT hr = mBuffer->Lock(mNextBufferIndex, dataSize, (void **)&pBytes, lockFlags);
        if (FAILED(hr))
        {
            HR_PRINT(hr);
            return;
        }

        dataSize = block.generator(pBytes);
        hr = mBuffer->Unlock();
        HR_REPORT_INVALID(hr);

        block.numPrims = dataSize/block.vertexSize;
        block.next = mNextBufferIndex;

        mNextBufferIndex += dataSize;
    }
}


void VertexBuffer::Render(uint32_t token) const
{
    if (token >= 0 && token < mBlocks.size())
    {
        const Block& block = mBlocks[token];

        HRESULT hr = mDevice->SetVertexDeclaration(block.declaration);
        if (FAILED(hr))
        {
            HR_PRINT(hr);
            return;
        }
        hr = mDevice->SetStreamSource(0, mBuffer, block.next, block.vertexSize);
        if (FAILED(hr))
        {
            HR_PRINT(hr);
            return;
        }

        hr = mDevice->DrawPrimitive(block.type, 0, static_cast<UINT>(block.numPrims));
        if (FAILED(hr))
        {
            HR_PRINT(hr);
            return;
        }
    }
}

} // namespace eg

