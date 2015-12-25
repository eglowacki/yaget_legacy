//////////////////////////////////////////////////////////////////////
// PropertyTypes.h
//
//  Copyright 2/10/2007 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Variuos property typpes, like Vector3, Color3, etc
//
//
//  #include "Managed/PropertyTypes.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef MANAGED_PROPERTY_TYPES_H
#define MANAGED_PROPERTY_TYPES_H
#pragma once

#include <Base.h>

namespace eg
{
    namespace managed
    {
        //! Wrapper around eg::Vector3
        public value class Vector3
        {
        public:
            float x;
            float y;
            float z;

            Vector3(float v);
            Vector3(String^ stringValue);
            virtual String^ ToString() override;

            static operator Vector3(float v)
            {
                Vector3 vector3(v);
                return vector3;
            }
            static operator Vector3(String^ stringValue)
            {
                Vector3 vector3(stringValue);
                return vector3;
            }
        };

        //! Wrapper around eg::Vector4
        public value class Vector4
        {
        public:
            float x;
            float y;
            float z;
            float w;

            Vector4(float v);
            Vector4(String^ stringValue);
            virtual String^ ToString() override;

            static operator Vector4(float v)
            {
                Vector4 vector4(v);
                return vector4;
            }
            static operator Vector4(String^ stringValue)
            {
                Vector4 vector4(stringValue);
                return vector4;
            }
        };

        //! Provides color selection/value handling
        public value class Color3_Tag
        {
        public:
            static Color Convert(Vector3 vector3)
            {
                Color color = Color::FromArgb(255, vector3.x * 255.0f, vector3.y * 255.0f, vector3.z * 255.0f);
                return color;
            }

            static Vector3 Convert(Color color)
            {
                Vector3 value(0.0f);
                value.x = color.R/255.0f;
                value.y = color.G/255.0f;
                value.z = color.B/255.0f;
                return value;
            }
        };

        //! Provides color selection/value handling
        public value class Color4_Tag
        {
        public:
            static Color Convert(Vector4 vector4)
            {
                Color color = Color::FromArgb(vector4.w * 255.0f, vector4.x * 255.0f, vector4.y * 255.0f, vector4.z * 255.0f);
                return color;
            }

            static Vector4 Convert(Color color)
            {
                Vector4 value(0.0f);
                value.x = color.R/255.0f;
                value.y = color.G/255.0f;
                value.z = color.B/255.0f;
                value.w = color.A/255.0f;
                return value;
            }
        };

        //! Provides custom labels handling for ui properties
        public value class CustomLabels_Tag {};

        //! Provides similar handling for custom labels, but uses bit filed flags combination
        public value class Flags_Tag
        {
        public:
            //! Convert value (bit field) into string, where each bit is separated by '.'
            //! if labels present, then use them to construct string
            static String^ Convert(unsigned int value, List<String^>^ labels);

            //! Convert textValue into bit filed number using labels
            static unsigned int Convert(String^ textValue, List<String^>^ labels);

            //! Return TRUE if bit in value at index is on, otherwise FALSE
            static bool IsOn(unsigned int value, int index);
        };


        //! Provides custom labels handling for enum  used in ui properties
        public value class Enum_Tag {};

    } // namespace managed

} // namespace eg

#endif // MANAGED_PROPERTY_TYPES_H

