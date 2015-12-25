#include "Precompiled.h"
#include "bind_vfs.h"
#include "File/VirtualFileSystem.h"
#include "Plugin/IPluginObject.h"

namespace
{
    using namespace eg;

    VirtualFileSystem *vfs = 0;

} // namespace
namespace eg {namespace editor {


void bind_vfs(const Environment *environment)
{
    vfs = environment ? environment->get<VirtualFileSystem *>("vfs") : 0;
    //triggerFile
}

}}


extern "C" __declspec(dllexport) void eg_vfs_FileChanged(const char *fileName)
{
    if (vfs)
    {
        vfs->triggerFile(fileName);
    }
}
