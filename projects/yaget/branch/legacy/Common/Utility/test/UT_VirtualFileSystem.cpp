// Unit test for VirtualFileSystem object
#pragma warning(push)
#pragma warning (disable : 4512)  // '' : assignment operator could not be generated
#pragma warning (disable : 4244)  // '' : assignment operator could not be generated
#include "File/VirtualFileSystem.h"
#include "Message/Dispatcher.h"
#pragma warning(pop)
#include "File/MemoryFileFactory.h"
#include "File/DiskFileFactory.h"
#include "StringHelper.h"
#include "Config/ConfigHelper.h"
#include "Registrate.h"
#include "Logger/Log.h"
#include <wx/filename.h>
#include <boost/filesystem.hpp>
#include <UnitTest++.h>



//! This can be used to load configuration from file
//! deprecated,
namespace eg
{
    namespace bfs = boost::filesystem;

    // Search for a file with the name 'filename' starting in directory 'dir_path',
    // copy the path of the file in 'pfound' if found, and return true.
    // Else return false.
    bool find_file(const bfs::path& dir_path, const std::string& file_name, bfs::path& pfound)
    {
        if (!exists(dir_path) || !is_directory(dir_path))
            return false;

        bfs::directory_iterator it(dir_path), end_iter;

        for (; it!= end_iter; ++it)
        {
            if (bfs::is_directory(*it))
            {
                if (find_file(*it, file_name, pfound))
                {
                    return true;
                }
            }

            else if ((*it).leaf() == file_name)
            {
                pfound = *it;
                return true;
            }
        }

        return false;
    }

    namespace config
    {

        //! Helper function to add new factory to VirtualFileSystem
        template <typename T>
        inline void AddVirtualFile(VirtualFileSystem& vfs, const VirtualFileSetting& settings)
        {
            VirtualFileSystem::Factory_t factory(new T(settings.type, settings.priority, settings.folders));
            vfs.RegisterFactory(settings.type, factory);
        }

    } // namespace config
} // namespace eg


using namespace eg;

/*
      const string_type & string() const         { return m_path; }
      const string_type file_string() const;
      const string_type directory_string() const { return file_string(); }

      const external_string_type external_file_string() const { return Traits::to_external( *this, file_string() ); }
      const external_string_type external_directory_string() const { return Traits::to_external( *this, directory_string() ); }

      basic_path   root_path() const;
      string_type  root_name() const;
      string_type  root_directory() const;
      basic_path   relative_path() const;
      basic_path   parent_path() const;
      string_type  filename() const;
      string_type  stem() const;
      string_type  extension() const;
      */

namespace file
{
    struct name
    {
        name(const std::string& full_path)
        : path(full_path)
        {
        }

        std::string operator()()
        {
            return path.filename();
        }

        boost::filesystem::path path;
    };

    std::string Name(const std::string& full_path)
    {
        return name(full_path)();
    }

} // namespace file

TEST(VirtualFileSystem_File)
{
    namespace bfs = boost::filesystem;

    std::string workingDir = registrate::GetExecutablePath() + "/Assets";
    bfs::path base_asset_path(workingDir);
    bfs::path assetPath("Textures");

    bfs::path fullAssetPath = base_asset_path / assetPath;
    log_trace("ut_util") << "Folders:\n\tBase Asset Path: '" << base_asset_path << "'\n\tFull Asset Path: '" << fullAssetPath << "'.";
    bfs::create_directories(fullAssetPath);

    //bfs::path new_path = bfs::system_complete(dir_path);

    VirtualFileSystem vfs;
    config::VirtualFileSetting setting;
    setting.type = "tga";
    setting.folders.push_back(fullAssetPath.string());
    config::AddVirtualFile<DiskFileFactory>(vfs, setting);

    setting = config::VirtualFileSetting();
    setting.type = "scratch";
    config::AddVirtualFile<MemoryFileFactory>(vfs, setting);

    {
        VirtualFileSystem::ostream_t file = vfs.AttachFileStream("Stats.scratch");
        CHECK(file);

        *file << "HelloThere" << " " << 5;

        size_t fileSize = stream::size(*file);
        CHECK_EQUAL((size_t)12, fileSize);
    }

    {
        VirtualFileSystem::ostream_t file = vfs.AttachFileStream("SplashScreen.tga");
        CHECK(file);
        file = vfs.AttachFileStream("Buildings/Brick.tga");
        CHECK(file);
    }

    CHECK(vfs.IsFileStreamExist("SplashScreen.tga"));
    CHECK(vfs.IsFileStreamExist("Buildings/Brick.tga"));
    CHECK(vfs.IsFileStreamExist("Garbage.000") == false);
    CHECK(vfs.IsFileStreamExist("Stats.scratch"));

    std::string file_name = vfs.GetCleanName("SplashScreen.tga");
    CHECK("SplashScreen" == file_name);

    file_name = vfs.GetCleanName("Buildings/Brick.tga");
    CHECK("Buildings/Brick" == file_name);

    std::string texturePath = fullAssetPath.string();

    file_name = vfs.GetFqn("SplashScreen.tga");
    CHECK(texturePath + "/SplashScreen.tga" == file_name);

    file_name = vfs.GetFqn("Buildings/Brick.tga");
    CHECK(texturePath + "/Buildings/Brick.tga" == file_name);

    CHECK("tga" == vfs.GetKey(".tga"));
    CHECK("tga" == vfs.GetKey("Buildings/Brick.tga"));
    CHECK("tga" == vfs.GetKey("SplashScreen.tga"));
    CHECK("tga" == vfs.GetKey("garbage.tga"));
    CHECK("tga" == vfs.GetKey(".tga"));
    CHECK("tga" == vfs.GetKey("tga"));
    CHECK("" == vfs.GetKey("garbage.000"));
    CHECK("" == vfs.GetKey("garbage."));
    CHECK("" == vfs.GetKey("garbage"));
    CHECK("" == vfs.GetKey("."));

    VirtualFileSystem::FileTree_t fileTree = vfs.GetFileTree("");
    //VirtualFileSystem::FileTree_t::const_iterator it = fileTree.begin(), it_end = fileTree.end();
    VirtualFileSystem::FileTree_t::pre_order_iterator_type it = fileTree.pre_order_begin(), it_end = fileTree.pre_order_end();
    for (; it != it_end; ++it)
    {
        const std::string& name = *it;
        int z = 0;
    }

    std::vector<std::string> fileList = vfs.GetFileList("tga");
    for (std::vector<std::string>::const_iterator it = fileList.begin(); it != fileList.end(); ++it)
    {
        std::string fileName = file::Name(*it);
        int z = 0;
    }

    {VirtualFileSystem::istream_t file = vfs.GetFileStream("Stats.scratch");
    CHECK(file.get());

    std::string line;
    int number;
    *file >> line >> number;
    CHECK_EQUAL(std::string("HelloThere"), line);
    CHECK_EQUAL(5, number);

    size_t fileSize = stream::size(*file);
    CHECK_EQUAL((size_t)12, fileSize);}

    {VirtualFileSystem::ostream_t file = vfs.AttachFileStream("Stats.scratch");
    {stream::SaveMarker saveMarkerA(*file);
    *file << "HelloThere" << " " << 5;}
    {stream::SaveMarker saveMarkerB(*file);
    *file << "SecondPart" << " " << 20;}}

    {
    std::string line;
    int number;

    VirtualFileSystem::istream_t file = vfs.GetFileStream("Stats.scratch");
    {stream::LoadMarker loadMarkerA(*file);
    *file >> line;// >> number;
    CHECK_EQUAL(std::string("HelloThere"), line);}

    {stream::LoadMarker loadMarkerB(*file);
    *file >> line >> number;
    CHECK_EQUAL(std::string("SecondPart"), line);
    CHECK_EQUAL(20, number);}
    }

    {
    std::string line;
    int number;
    std::string dummyLine;
    int dummyNumber;

    VirtualFileSystem::istream_t file = vfs.GetFileStream("Stats.scratch");
    try
    {
        stream::LoadMarker loadMarkerA(*file);
        *file >> line >> number >> dummyLine >> dummyNumber >> dummyNumber;
        CHECK_EQUAL(std::string("HelloThere"), line);
        CHECK_EQUAL(5, number);
    }
    catch (ex::serialize& e)
    {
        log_debug << "Loading object A threw exception '" << e.what() << "'";
    }

    {stream::LoadMarker loadMarkerB(*file);
    *file >> line >> number;
    CHECK_EQUAL(std::string("SecondPart"), line);
    CHECK_EQUAL(20, number);}
    }

    bfs::remove_all(base_asset_path);
}


/*
TEST(VirtualFileSystem_Memory)
{
    VirtualFileSystem vfs;
    config::VirtualFileSetting setting;
    setting.type = "scratch";
    config::AddVirtualFile<MemoryFileFactory>(vfs, setting);

    {
        VirtualFileSystem::ostream_t file = vfs.AttachFileStream("Stats.scratch");
        CHECK(file);

        *file << "HelloThere" << " " << 5;
    }

    {
        VirtualFileSystem::istream_t file = vfs.GetFileStream("Stats.scratch");
        CHECK(file.get());

        std::string line;
        int number;
        *file >> line >> number;
        CHECK_EQUAL(std::string("HelloThere"), line);
        CHECK_EQUAL(5, number);
    }
}
*/
