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
//  #include "DXErrorHandler.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef DX_ERROR_HANDLER_H
#define DX_ERROR_HANDLER_H
#pragma once

#include "Base.h"
#include <DXErr9.h>
#include <boost/format.hpp>

namespace eg
{
    namespace dx
    {
        inline std::string parse(HRESULT result)
        {
            if (FAILED(result))
            {
                const char *error = DXGetErrorString9(result);
                const char *desription = DXGetErrorDescription9(result);
                std::string errorTxt = boost::str(boost::format("Direct3D call failed. Error '%1%', Desription '%2%'") % error % desription);
                return  errorTxt;
            }
            return "";
        }
    } // namespace dx


    #define HR_CHECK_VALID(result) {if (FAILED(result)) {log_error << eg::dx::parse(result);}}
    #define HR_PRINT(result) log_error << eg::dx::parse(result)

} // namespace eg

#endif // DX_ERROR_HANDLER_H

