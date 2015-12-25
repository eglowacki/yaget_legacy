

class __declspec(dllexport) ExampleExport
{
public:
    ExampleExport();
    ~ExampleExport();
    int get();
};

extern "C" {
    int __declspec(dllexport) globalJuk();
}
