#include "Precompiled.h"
#include "IdGameCache.h"
#include "Config/Console.h"
#include "Property/PropertyDataConversion.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>


namespace
{
    const uint64_t kBurnableMinRange = 1000;
    const uint64_t kBurnableMaxRange = 1000000;

    std::string konsole_getId(const std::vector<std::string>& parameters, eg::config::KosoleResult& outResult)
    {
        assert(boost::iequals(parameters[0], std::string("/getId")));
        if (parameters.size() > 1)
        {
            if (boost::algorithm::iequals(parameters[1], std::string("help")))
            {
                std::string resultString = "Valid subcommands for '" + parameters[0] + "' are:";
                resultString += "\n\tgetId -b [-s] = (Get next available burnable id. If -s option then just show number in dec)";
                resultString += "\n\tgetId -p [-s] = (Get next available persistant id. If -s option then just show number in dec)";

                outResult = eg::config::krHelp;
                return resultString;
            }
            else if (boost::algorithm::iequals(parameters[1], std::string("-b")))
            {
                outResult = eg::config::krOk;
                uint64_t id = 0;//eg::idspace::get_burnable();
                if (parameters.size() > 2 && boost::algorithm::iequals(parameters[2], std::string("-s")))
                {
                    return eg::ConvertProperty(id);
                }
                else
                {
                    std::string resultString = "Next valid burnable id: Dec = '" + eg::ConvertProperty(id) + "', Hex = '" + boost::str(boost::format("0x%016X") % id) + "'.";
                    return resultString;
                }
            }
            else if (boost::algorithm::iequals(parameters[1], std::string("-p")))
            {
                outResult = eg::config::krOk;
                uint64_t id = 0;//eg::idspace::get_persistent();
                if (parameters.size() > 2 && boost::algorithm::iequals(parameters[2], std::string("-s")))
                {
                    return eg::ConvertProperty(id);
                }
                else
                {
                    std::string resultString = "Next valid persistant id: Dec = '" + eg::ConvertProperty(id) + "', Hex = '" + boost::str(boost::format("0x%016X") % id) + "'.";
                    return resultString;
                }
            }
        }

        outResult = eg::config::krError;
        return "Command '" + parameters[0] + "' does not have valid syntax/parameters.";
    }

} // namespace

namespace eg
{

IdGameCache::IdGameCache(config::Konsole& konsole)
: mBurnableRange(kBurnableMinRange, kBurnableMaxRange)
, mNextBurnableId(kBurnableMinRange)
, mNextPersistentId(kBurnableMaxRange)
{
    konsole.AddItem("getId", konsole_getId);
}


IdGameCache::IdGameCache()
: mBurnableRange(kBurnableMinRange, kBurnableMaxRange)
, mNextBurnableId(kBurnableMinRange)
, mNextPersistentId(kBurnableMaxRange)
{
}


IdGameCache::~IdGameCache()
{
}


uint64_t IdGameCache::GetId(IdType idType)
{
    uint64_t result = 0;
    if (idType == itBurnable)
    {
        if (mNextBurnableId >= mBurnableRange.second)
        {
            // we run out of burnable id's
            assert(0);
        }
        else
        {
            result = mNextBurnableId++;
        }
    }
    else if (idType == itPersistent)
    {
        result = mNextPersistentId++;
    }

    return result;
}


uint64_t idspace::get_burnable(IdGameCache& idCache)
{
    return idCache.GetId(IdGameCache::itBurnable);
}


uint64_t idspace::get_persistent(IdGameCache& idCache)
{
    return idCache.GetId(IdGameCache::itPersistent);
}


} // namespace eg

