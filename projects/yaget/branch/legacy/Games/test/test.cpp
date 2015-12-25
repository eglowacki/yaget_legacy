// test.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "test.h"


extern "C" {
int __declspec(dllexport) globalJuk()
{
    return 10;
}
}


ExampleExport::ExampleExport()
{

}


ExampleExport::~ExampleExport()
{

}


int ExampleExport::get()
{
    return 10;
}
