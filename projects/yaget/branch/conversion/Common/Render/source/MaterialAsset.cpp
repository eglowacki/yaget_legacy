// ShaderAsset.cpp
#include "Precompiled.h"
#include "ShaderAsset.h"
#include "IdGameCache.h"
#include "Interface/IRenderContext.h"
#include "StringHelper.h"
#include "Renderer.h"
#include "ErrorHandler.h"
#include "Property/PropertyDataConversion.h"
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <tinyxml.h>
//#include <xsd/cxx/xml/string.hxx>
//#include <xercesc/framework/MemBufInputSource.hpp>

//#include "generated/Material.h"

namespace
{
    using namespace eg;

    //! Find input of particular name
    bool isInputName(const MaterialAsset::Input& input, const std::string name)
    {
        return input.Name == name;
    }

    // case insensitive string comparison
    bool isEquals(const std::string& str1, const std::string& str2)
    {
        return boost::algorithm::iequals(str1, str2);
    }

    /*!
     Let's create xml loader for material file
     where each 'Input' tag has a 'name attribute
     'Value' and 'Type' as inner tags.
     For type of input supported /see MaterialAsset::Input::Type
    */
    class MaterialParser : public TiXmlVisitor
    {
    public:
        MaterialParser()
        : TiXmlVisitor()
        , mpCurrInput(0)
        , mCurrInputType(itNone)
        {
        }

        const MaterialAsset::Inputs_t& GetInputs() const
        {
            return mInputs;
        }

        virtual bool VisitEnter(const TiXmlElement& element, const TiXmlAttribute *firstAttribute)
        {
            if (isEquals(element.ValueStr(), "Input"))
            {
                if (firstAttribute)
                {
                    if (isEquals(firstAttribute->NameTStr(), "name"))
                    {
                        mStagingIputs.push_back(FlatInput());
                        // we can safely store pointer to our latest input
                        // structure addition, since this is the only
                        // place it can be resized.
                        mpCurrInput = &mStagingIputs.back();
                        mpCurrInput->Name = firstAttribute->ValueStr();
                        mpCurrInput->Value = std::string("");
                    }
                }
            }
            else if (isEquals(element.ValueStr(), "Type") && mpCurrInput)
            {
                mCurrInputType = itType;
            }
            else if (isEquals(element.ValueStr(), "Value") && mpCurrInput)
            {
                mCurrInputType = itValue;
            }
            else if (isEquals(element.ValueStr(), "Pass") && mpCurrInput)
            {
                mCurrInputType = itPass;
            }

            return true;
        }

        /// Visit an element.
        virtual bool VisitExit(const TiXmlElement& element)
        {
            if (isEquals(element.ValueStr(), "Input"))
            {
                mpCurrInput = 0;
                mCurrInputType = itNone;
            }
            else if (isEquals(element.ValueStr(), "Type"))
            {
                mCurrInputType = itNone;
            }
            else if (isEquals(element.ValueStr(), "Value"))
            {
                mCurrInputType = itNone;
            }
            else if (isEquals(element.ValueStr(), "Pass"))
            {
                mCurrInputType = itNone;
            }

            return true;
        }

        /// Visit a text node
        virtual bool Visit(const TiXmlText& text)
        {
            if (mCurrInputType == itType)
            {
                mpCurrInput->InputType = MaterialAsset::Input::Convert(text.ValueStr());
            }
            else if (mCurrInputType == itValue)
            {
                // We start with exact string of this value
                // since we can not be assured that Type will come before this
                // so,we might not know yet what kind of value this supposed to be.
                // So, after processing entire xml file we'll resolve then resolve it.
                mpCurrInput->Value = text.ValueStr();
            }
            else if (mCurrInputType == itPass)
            {
                mpCurrInput->Pass = ConvertProperty<uint32_t>(text.ValueStr());
            }

            return true;
        }

        virtual bool VisitExit(const TiXmlDocument& /*doc*/ )
        {
            resolve();
            return true;
        }

    private:
        enum InputType {itNone, itValue, itType, itPass};

        //! This provides extra member to hold which pass this input is designed for
        struct FlatInput : public MaterialAsset::Input
        {
            FlatInput() : MaterialAsset::Input(), Pass(0)
            {}

            uint32_t Pass;
        };


        void resolve()
        {
            typedef std::vector<FlatInput>::iterator it_t;
            for (it_t it = mStagingIputs.begin(); it != mStagingIputs.end(); ++it)
            {
                FlatInput& flatInput = *it;

                MaterialAsset::InputsPerPass_t& inputs = mInputs[flatInput.Pass];
                inputs.push_back(MaterialAsset::Input());
                MaterialAsset::Input& input = inputs.back();

                input.InputType = flatInput.InputType;
                input.Name = flatInput.Name;
                input.Value = flatInput.Value;

                std::string valueString;
                try
                {
                    valueString = boost::any_cast<std::string>(input.Value);
                }
                catch (const boost::bad_any_cast& er)
                {
                    wxLogDebug("%s.", er.what());
                    continue;
                }

                input.ConvertToValue(valueString);
            }
        }

        //! While we processing input block, this will be set to current one
        //! to except values from xml
        FlatInput *mpCurrInput;

        std::vector<FlatInput> mStagingIputs;
        MaterialAsset::Inputs_t mInputs;
        InputType mCurrInputType;
    };

} // namespace


namespace eg
{

MaterialAsset::MaterialAsset(const std::string& name)
: eg::MediaAsset<MaterialAsset>(name)
{
    // when asset is finished loading, it will trigger signal, which will allow us to use missing material
    // if there is not loaded one.
    sigLoadFinish.connect(0, boost::bind(&MaterialAsset::onFinishedLoading, this, _1, _2));
}


MaterialAsset::~MaterialAsset()
{
    mMissingAssetConnection.disconnect();
}


void MaterialAsset::onFinishedLoading(MaterialAsset::Asset_t asset, const std::string& name)
{
    DataLocker lock(*this);
    if (mInputs.empty())
    {
        wxLogWarning("Material Asset '%s' was not loaded successfully, using '%s' as a placeholder.", name.c_str(), missingasset::Material);

        asset::AssetLocker<MaterialAsset, asset::NoSyncAssetPolicy> assetLocker(missingasset::Material);
        if (MaterialAsset *source = assetLocker)
        {
            mMissingAssetConnection = source->sigLoadFinish.connect(0, boost::bind(&MaterialAsset::onMissingAssetChanged, this, _1, _2));
            mInputs = source->mInputs;
        }
    }
}


void MaterialAsset::onMissingAssetChanged(MaterialAsset::Asset_t asset, const std::string& /*name*/)
{
    DataLocker lock(*this);
    mInputs = asset->mInputs;
}


void MaterialAsset::SaveBlob(uint8_t *&/*pData*/, size_t& /*size*/) const
{
    /*
    DataLocker lock(*this);

    size = mScriptData.size();
    pData = new uint8_t[size];
    memcpy(pData, mScriptData.c_str(), size);
    */
}


void MaterialAsset::LoadBlob(const uint8_t *pData, size_t /*size*/, const std::string& /*streamName*/)
{
    mMissingAssetConnection.disconnect();
    //using namespace yaget;
    //char dummyStuff;
    //XERCES_CPP_NAMESPACE::MemBufInputSource inputSource(pData, size, (const char* const)0);
    /*
    std::istringstream localBuffer(std::string((const char *)pData, size));
    try
    {
        std::auto_ptr<Material> matLoader(material(localBuffer, xml_schema::Flags::dont_validate));

        const Inputs& mainInputs = matLoader->inputs();

        const Inputs::InputSequence& inputs = mainInputs.input();
        for (Inputs::InputConstIterator it = inputs.begin(); it != inputs.end(); ++it)
        {
            const Inputs::InputType& inputData = *it;

            uint32_t passIndex = inputData.pass();
            InputsPerPass_t& inputs = mInputs[passIndex];
            inputs.push_back(Input());
            Input& input = inputs.back();

            //std::string dummy = (std::string)inputData.name();
            //std::string dummy(xsd::cxx::xml::transcode<char>((const XMLCh *)inputData.name().c_str()));
            //std::string dummy(xsd::cxx::xml::transcode<char>((const XMLCh *)inputData.name().get().c_str()));
            //xsd::cxx::xml::string dummy(xsd::cxx::xml::transcode<char>((const XMLCh *)inputData.name().get().c_str()));
                          //string ns(xml::transcode<char>(root->getNamespaceURI()));
            //input.Name = xsd::cxx::xml::transcode<char>(inputData.name());//inputData.name();
            input.Name = inputData.name();
            input.Value = (std::string)inputData.value();

            std::string valueString;
            try
            {
                valueString = boost::any_cast<std::string>(input.Value);
            }
            catch (const boost::bad_any_cast& er)
            {
                wxLogDebug("%s.", er.what());
                continue;
            }
            input.ConvertToValue(valueString);

            int z = 0;
        }
    }
    catch (const xml_schema::Exception& ex)
    {
        std::string what = ex.what();
        std::ostringstream errBuffer;
        errBuffer << ex << std::endl;
        std::string errorText = errBuffer.str();
        int z = 0;
    }
    */

    //Material::InputsType inputs = matLoader->inputs();

    TiXmlDocument xmlDoc;
    xmlDoc.Parse((const char *)pData);
    if (!xmlDoc.Error())
    {
        MaterialParser materialParser;
        if (xmlDoc.Accept(&materialParser))
        {
            DataLocker lock(*this);
            mInputs = materialParser.GetInputs();
        }
    }
}


const MaterialAsset::InputsPerPass_t& MaterialAsset::GetInputs(uint32_t passIndex) const
{
    wxASSERT(isLocked());
    Inputs_t::const_iterator it = mInputs.find(passIndex);
    if (it != mInputs.end())
    {
        return (*it).second;;
    }

    static InputsPerPass_t emptyCollection = InputsPerPass_t();
    return emptyCollection;

}


boost::any MaterialAsset::SetInput(const std::string& name, boost::any newValue, uint32_t passIndex)
{
    wxASSERT(isLocked());
    Inputs_t::iterator it = mInputs.find(passIndex);
    if (it != mInputs.end())
    {
        InputsPerPass_t& inputCollection = (*it).second;
        for (InputsPerPass_t::iterator iti = inputCollection.begin(); iti != inputCollection.end(); ++iti)
        {
            if ((*iti).Name == name)
            {
                // we have this input so let's get the old one and set new one
                boost::any oldValue = (*iti).Value;
                (*iti).Value = newValue;

                InputsPerPass_t::iterator testIt = std::find_if(inputCollection.begin(), inputCollection.end(), boost::bind(isInputName, _1, name));
                return oldValue;
            }
        }
    }

    return boost::any();
}


boost::any MaterialAsset::GetInput(const std::string& name, uint32_t passIndex)
{
    wxASSERT(isLocked());
    Inputs_t::iterator it = mInputs.find(passIndex);
    if (it != mInputs.end())
    {
        InputsPerPass_t& inputCollection = (*it).second;
        for (InputsPerPass_t::iterator iti = inputCollection.begin(); iti != inputCollection.end(); ++iti)
        {
            if ((*iti).Name == name)
            {
                // we have this input so return this value
                boost::any oldValue = (*iti).Value;
                return oldValue;
            }
        }
    }

    return boost::any();
}




} // namespace eg

