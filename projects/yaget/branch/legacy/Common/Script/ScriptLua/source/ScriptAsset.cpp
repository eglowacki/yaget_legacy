// ScriptAsset.cpp
#include "Precompiled.h"
#pragma warning(push)
#pragma warning (disable : 4127) // conditional expression is constant
#pragma warning (disable : 4389) // '!=' : signed/unsigned mismatch
#pragma warning (disable : 4512) // '' : assignment operator could not be generated
#pragma warning (disable : 4244) // '' : conversion from '' to '', possible loss of data
#include "ScriptAsset.h"
//#pragma warning (default : 4389)
#include "IdGameCache.h"
#include "StringHelper.h"
#include <boost/bind.hpp>
//#pragma warning (default : 4127)
#pragma warning(pop)
#include <boost/format.hpp>



namespace eg
{

ScriptAsset::ScriptAsset(const std::string& name)
: eg::MediaAsset<ScriptAsset>(name)
{
    sigLoadFinish.connect(0, boost::bind(&ScriptAsset::onFinishedLoading, this, _1, _2));
}

ScriptAsset::~ScriptAsset()
{
    // remove script object from managed code
    //script::DeleteCompiler(GetId());
}


std::string ScriptAsset::makePackName() const
{
    std::string packName = Name();
    if (!packName.empty())
    {
        // strip all text from first occurrence of '*' or '?'
        size_t i = packName.find_first_of("?*");
        if (i != std::string::npos)
        {
            packName = packName.erase(i);
        }
        if (*packName.rbegin() == '/' || *packName.rbegin() == '\\')
        {
            packName.erase(packName.size()-1);
        }

        boost::replace_all(packName, std::string("/"), std::string("."));
        boost::replace_all(packName, std::string("\\"), std::string("."));
    }
    else
    {
        packName = boost::str(boost::format("NoName.0x%016X") % GetId());
    }

    return packName;
}

 void ScriptAsset::onFinishedLoading(ScriptAsset::Asset_t /*asset*/, const std::string& /*name*/)
 {
     DataLocker lock(*this);
     mPackage = mPackageLoader;
     mPackageLoader.clear();
 }


void ScriptAsset::SaveBlob(uint8_t *&/*pData*/, size_t& /*size*/) const
{
    wxASSERT_MSG(0, "Saving script package in source format, not supported.");
    /*
    DataLocker lock(*this);

    size = mScriptData.size();
    pData = new uint8_t[size];
    memcpy(pData, mScriptData.c_str(), size);
    */
}


void ScriptAsset::LoadBlob(const uint8_t *pData, size_t size, const std::string& streamName)
{
    DataLocker lock(*this);

    mPackageLoader.mSources.push_back(std::string(reinterpret_cast<const char *>(pData), size));
    std::string fqn = Name();
    if (VirtualFileSystem *vfs = registrate::p_cast<VirtualFileSystem>("VirtualFileSystem"))
    {
        fqn = vfs->GetFqn(streamName);
    }
    mPackageLoader.mFqns.push_back(fqn);
}


ScriptAsset::const_iterator ScriptAsset::begin() const
{
    //DataLocker lock(*this);
    return const_iterator(mPackage.mSources, mPackage.mFqns, false);
}


ScriptAsset::const_iterator ScriptAsset::end() const
{
    //DataLocker lock(*this);
    return const_iterator(mPackage.mSources, mPackage.mFqns, true);
}


bool ScriptAsset::empty() const
{
    return mPackage.mSources.empty();
}


ScriptAsset::const_iterator& ScriptAsset::const_iterator::operator++()
{
    ++mSourceIt;
    ++mFqnIt;
    return *this;
}


ScriptAsset::const_iterator ScriptAsset::const_iterator::operator++(int)
{
    const_iterator tmp(*this);
    ++(*this);
    return tmp;
}


bool operator ==(const ScriptAsset::const_iterator& lhs, const ScriptAsset::const_iterator& rhs)
{
    return lhs.mSourceIt == rhs.mSourceIt &&  lhs.mFqnIt == rhs.mFqnIt;
}


bool operator !=(const ScriptAsset::const_iterator& lhs, const ScriptAsset::const_iterator& rhs)
{
    return !(lhs == rhs);
}


ScriptAsset::Data_t ScriptAsset::const_iterator::operator *() const
{
    Data_t it(*mSourceIt, *mFqnIt);
    return it;
}


ScriptAsset::const_iterator::const_iterator(const std::vector<std::string>& sources, const std::vector<std::string>& fqns, bool bEnd)
: mSourceIt(bEnd ? sources.end() : sources.begin())
, mFqnIt(bEnd ? fqns.end() : fqns.begin())
{
}


/*
void ScriptAsset::NewCommand(uint64_t cmdId, const std::string& cmdName, uint64_t oId)
{
    //script::NewCommand(GetId(), cmdId, cmdName, oId);
}
*/

} // namespace eg

