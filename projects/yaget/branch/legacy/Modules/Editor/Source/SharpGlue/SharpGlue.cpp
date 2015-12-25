#include <Windows.h>

extern int sharp_Loader(const char * /*name*/);
extern int sharp_Unloader(const char * /*name*/);
extern int sharp_Lister(const char * /*name*/, char * /*validNames*/, int * /*size*/);

extern "C" __declspec(dllexport) void load(const char *name)
{
    int v = sharp_Loader(name);
}


extern "C" __declspec(dllexport) void unload(const char *name)
{
    int v = sharp_Unloader(name);
}


extern "C" __declspec(dllexport) void lister(const char *name, char *validNames, int *size)
{
    int v = sharp_Lister(name, validNames, size);
}
