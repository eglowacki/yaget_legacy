//////////////////////////////////////////////////////////////////////
// ErrorHandler.h
//
//  Copyright 12/27/2007 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//    Provides common and unified way to deal with D3D errors.
//
//  #include "ErrorHandler.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H
#pragma once

namespace eg
{

    inline bool IsValid(HRESULT result, const char *pfileName, int32_t lineNumber)
    {
        if (FAILED(result))
        {
            const char *error = DXGetErrorString9(result);
            const char *desription = DXGetErrorDescription9(result);
            wxLogError("Direct3D call failed. Error: %s, Desription: %s. FileName: '%s', LineNumber: %d", error, desription, pfileName, lineNumber);


            return false;
        }

        return true;
    }

    #define HR_IS_VALID(result) IsValid(result, __FILE__, __LINE__)


} // namespace eg

#endif // ERROR_HANDLER_H

