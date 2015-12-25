//#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Hash/Hash.h"
#include "Hash/ReverseHashLookup.h"
#include "Config/Console.h"
#include "Config/ConfigHelper.h"
//#include "App/MainConsole.h"
//#include "Registrate.h"
#include "IdGameCache.h"
#include "MessageInterface.h"
#include "Message/Dispatcher.h"
#include "Property/PropertyData.h"
#include "Math/InterpolationType.h"
#include "Logger/Log.h"
#include <boost/format.hpp>
//#include <wx/filename.h>
//#include <boost/any.hpp>
//#include <boost/regex.hpp>
//#include "MemoryManager/Stackwalker.h"
#include "vld.h"
#include <UnitTest++.h>

using namespace eg;

const guid_t kDummyMessage = 0x1647beef;

TEST(Dispatcher)
{
    Dispatcher disp;

    struct TestDispatch
    {
        int counter;
        void operator()(Message& /*msg*/) {counter++;}
    };

    TestDispatch testDispatch;
    testDispatch.counter = 0;
    boost::signals::connection c = disp[kDummyMessage].connect(boost::ref(testDispatch));

    Message msg(kDummyMessage);
    disp[kDummyMessage](msg);
    CHECK_EQUAL(1, testDispatch.counter);

    //disp[kDummyMessage].disconnect(boost::ref(testDispatch));
    c.disconnect();
    disp[kDummyMessage](msg);
    CHECK_EQUAL(1, testDispatch.counter);
}


TEST(IdCache)
{
    config::Konsole konsole;
    IdGameCache idServer(konsole);

    for (int i = 0; i < 100; i++)
    {
        uint64_t id = idServer.GetId(IdGameCache::itBurnable);
        CHECK(id);
    }

    for (int i = 0; i < 100; i++)
    {
        uint64_t id = idServer.GetId(IdGameCache::itPersistent);
        CHECK(id);
    }
}




TEST(Hash)
{
    Hash hash;
    CHECK_EQUAL(false, hash.IsValid());
    hash = Hash("Entity");
    CHECK_EQUAL(true, hash.IsValid());
    hash = Hash((uint32_t)0);
    CHECK_EQUAL(true, hash.IsValid());

    Hash hash1 = 101;
    Hash hash2 = 102;
    std::ostringstream os;
    os << hash1;
    os << hash2;
    std::string str = os.str();

    Hash testHash1;
    Hash testHash2;
    std::istringstream iss(str);
    iss >> testHash1;
    iss >> testHash2;
    CHECK_EQUAL(hash1, testHash1);
    CHECK_EQUAL(hash2, testHash2);
}


TEST(ReverseHash)
{
    ReverseHashLookup<Hash> reverseLookup;
    Hash hash("Entity");
    reverseLookup.Add("Entity", hash);
    reverseLookup.Add("AnotherDummyName", Hash("AnotherDummyName"));

    wxString name = reverseLookup.GetName(hash);
    CHECK_EQUAL("Entity", name);
}


TEST(Property)
{
    PropertySet propSet;
    PropertyOwn<bool, bool> boolProp(&propSet, "Boolean", 0, 0, "");
    PropertyOwn<int, int> intProp(&propSet, "Integer", 0, 0, "");
    PropertyOwn<float, float> floatProp(&propSet, "Float", 0.0f, 0, "");
    PropertyOwn<uint64_t, uint64_t> uint64Prop(&propSet, "Uint64", 0, 0, "");
    PropertyOwn<Vector2, Vector2> Vector2Prop(&propSet, "Vector2", v2::ZERO(), 0, "");
    PropertyOwn<Vector3, Vector3> Vector3Prop(&propSet, "Vector3", v3::ZERO(), 0, "");
    PropertyOwn<Vector4, Vector4> Vector4Prop(&propSet, "Vector4", v4::ZERO(), 0, "");
    PropertyOwn<Quaternion, Quaternion> QuaternionProp(&propSet, "Quaternion", Quaternion(v4::WONE()), 0, "");
    PropertyOwn<Matrix33, Matrix33> Matrix33Prop(&propSet, "Matrix33", m33::Identity(), 0, "");
    PropertyOwn<Matrix44, Matrix44> Matrix44Prop(&propSet, "Matrix44", m44::Identity(), 0, "");
    PropertyOwn<std::vector<component::ObjectId>, std::vector<component::ObjectId> > ObjectListProp(&propSet, "ObjectList", std::vector<component::ObjectId>(), 0, "");
    PropertyOwn<std::vector<std::string>, std::vector<std::string> > StringListProp(&propSet, "StringList", std::vector<std::string>(), 0, "");
    PropertyOwn<std::string, std::string> stringProp(&propSet, "String", "", 0, "");

    bool Boolean = boolProp;
    CHECK_EQUAL(false, Boolean);

    int Integer = intProp;
    CHECK_EQUAL(0, Integer);

    float Float = floatProp;
    CHECK_EQUAL(0.0f, Float);

    uint64_t Uint64 = uint64Prop;
    CHECK_EQUAL(0, Uint64);

    Vector2 vector2 = Vector2Prop;
    CHECK_EQUAL(v2::ZERO(), vector2);

    Vector3 vector3 = Vector3Prop;
    CHECK_EQUAL(v3::ZERO(), vector3);

    Vector4 vector4 = Vector4Prop;
    CHECK_EQUAL(v4::ZERO(), vector4);

    Quaternion quaternion = QuaternionProp;
    CHECK_EQUAL(Quaternion(v4::WONE()), quaternion);

    Matrix33 matrix33 = Matrix33Prop;
    CHECK_EQUAL(m33::Identity(), matrix33);

    Matrix44 matrix44 = Matrix44Prop;
    CHECK_EQUAL(m44::Identity(), matrix44);
    //////////////////////////////////////////////////////////////////////////

    boolProp = true;
    CHECK_EQUAL(true, (bool)boolProp);

    intProp = 1;
    CHECK_EQUAL(1, (int)intProp);

    floatProp = 1.0f;
    CHECK_EQUAL(1.0f, (float)floatProp);

    uint64Prop = 1;
    CHECK_EQUAL(1, (uint64_t)uint64Prop);

    Vector2Prop = v2::ONE();
    CHECK_EQUAL(v2::ONE(), (Vector2)Vector2Prop);

    Vector3Prop = v3::ONE();
    CHECK_EQUAL(v3::ONE(), (Vector3)Vector3Prop);

    Vector4Prop = v4::ONE();
    CHECK_EQUAL(v4::ONE(), (Vector4)Vector4Prop);

    QuaternionProp = Quaternion(v4::ONE());
    CHECK_EQUAL(Quaternion(v4::ONE()), (Quaternion)QuaternionProp);

    Matrix33Prop = Matrix33(1.0f);
    CHECK_EQUAL(Matrix33(1.0f), (Matrix33)Matrix33Prop);

    Matrix44Prop = Matrix44(1.0f);
    CHECK_EQUAL(Matrix44(1.0f), (Matrix44)Matrix44Prop);
}


TEST(Interpolation)
{
    math::TimeUnit timeUnit;

    float currValue = timeUnit.Tick(0.2f);
    CHECK_CLOSE(0.2f, currValue, 0.002f);

    currValue = timeUnit.Tick(0.2f);
    CHECK_CLOSE(0.4f, currValue, 0.002f);

    currValue = timeUnit.Tick(0.2f);
    CHECK_CLOSE(0.6f, currValue, 0.002f);

    currValue = timeUnit.Tick(0.2f);
    CHECK_CLOSE(0.8f, currValue, 0.002f);

    currValue = timeUnit.Tick(0.2f);
    CHECK_CLOSE(0.0f, currValue, 0.002f);

    currValue = timeUnit.Tick(0.2f);
    CHECK_CLOSE(0.2f, currValue, 0.002f);

    currValue = timeUnit.Tick(0.2f);
    CHECK_CLOSE(0.4f, currValue, 0.002f);

    currValue = timeUnit.Tick(0.2f);
    CHECK_CLOSE(0.6f, currValue, 0.002f);

    currValue = timeUnit.Tick(0.2f);
    CHECK_CLOSE(0.8f, currValue, 0.002f);

    currValue = timeUnit.Tick(0.2f);
    CHECK_CLOSE(0.0f, currValue, 0.002f);
}

TEST(MathHelpers)
{
    const char *pValue1 = "1";
    const char *pValue2 = "1, 2, ";
    const char *pValue3 = "1,2, 3";
    const char *pValue4 = " 1, 2, 3,4";
    float v1 = 0;
    Vector2 v2 = v2::ZERO();
    Vector3 v3 = v3::ZERO();
    Vector4 v4 = v4::ZERO();

    GetFloatValues(pValue1, &v1, 1);
    CHECK_EQUAL(v1, 1);

    GetFloatValues(pValue2, v2.getF(), 2);
    CHECK_EQUAL(Vector2(1, 2), v2);

    GetFloatValues(pValue3, v3.getF(), 3);
    CHECK_EQUAL(Vector3(1, 2, 3), v3);

    GetFloatValues(pValue4, v4.getF(), 4);
    CHECK_EQUAL(Vector4(1, 2, 3, 4), v4);

    GetFloatValues(pValue4, v2.getF(), 2);
    CHECK_EQUAL(Vector2(1, 2), v2);

    GetFloatValues("fg, 7f", v2.getF(), 2);
    CHECK_EQUAL(Vector2(0, 0), v2);

    GetFloatValues("4, 7f", v4.getF(), 4);
    CHECK_EQUAL(Vector4(4, 0, 0, 0), v4);
}


namespace
{
    std::vector<std::string> getTraceModules()
    {
        std::vector<std::string> traceModules;
        //traceModules.push_back(tr_util);
        traceModules.push_back("ut_util");
        return traceModules;
    }
    logs::LoggerInit loggerInit("", logs::kDest_file|logs::kDest_debug|logs::kDest_cout, 0, true, getTraceModules());

  std::string GetKey(const std::string& name)
  {
      size_t pos = name.find_first_of('#');
      if (pos != std::string::npos)
      {
          // we have tag marker
          std::string tag = name.substr(0, pos);
          return tag;
      }

      // there is no tag marker,
      // so extract extension
      pos = name.find_last_of('.');
      if (pos != std::string::npos)
      {
          // we have tag marker
          std::string tag = name.substr(pos+1);
          return tag;
      }

      return "";
  }

  std::string normalizeName(const std::string& name)
  {
      size_t pos = name.find_first_of('#');
      if (pos != std::string::npos)
      {
        // we have tag marker
        std::string normName(name);
        normName.erase(0, pos+1);
        return normName;

      }
      return name;
  }

} // namespace


int main(int argc, char* argv[])
{
    argc; argv;
    /*
    std::string key;
    key = GetKey("scratch#tree.tga");
    key = GetKey("tree.tga");
    key = GetKey("tree");
    key = GetKey("#");
    key = GetKey(".");
    key = GetKey("");

    key = normalizeName("scratch#tree.tga");
    key = normalizeName("tree.tga");
    key = normalizeName("tree");
    key = normalizeName("#");
    key = normalizeName(".");
    key = normalizeName("");

    //boost::regex e("^.+\\.");
    //boost::regex e("scratch");
    //boost::smatch what;
    //if (boost::regex_match(fileName, what, e))
    //{
    //    std::string key = what[0];
    //    int z = 0;
    //}
    */

    log_trace("ut_util") << "Starting Unit Test...";

    std::string configFile;
    configFile += "[VFS Factory/MemoryFile]\n";
    configFile +=     "scratch = Temp/";

    config::Use(configFile);

    int result = UnitTest::RunAllTests();

    uint32_t blockSize = 0;
    std::string hexNumber = boost::str(boost::format(" 0x%08X") % blockSize);
    log_trace("ut_util") << "blockSize: '" << hexNumber << "'";

    config::Shutdown();
    log_trace("ut_util") << "Finished Unit Test.";
    return result;
}