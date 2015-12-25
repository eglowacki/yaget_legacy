#include "Precompiled.h"
//#include "../Script.h"
//#include "BindPolicy.h"
//#include <luabind/error.hpp>
//#include <luabind/operator.hpp>
#pragma warning(push)
#pragma warning (disable : 4100)  // '' : unreferenced formal parameter
#pragma warning (disable : 4512)  // '' : assignment operator could not be generated
#include "ComponentBinds.h"
#pragma warning(pop)
#include "AutoBinders.h"
#include "BindHelpers.h"
#include "IRenderer.h"
#include "ObjectID.h"
#include "Math/Vector.h"
#include "Math/Ray.h"
#include "Registrate.h"
#include "IObjectManager.h"
#include "../ComponentScriptLua.h"
#include "../Script.h"
#include "BindPolicy.h"
#include <luabind/error.hpp>
#include <luabind/operator.hpp>

namespace eg {
namespace script {

void intitMathModule(lua_State *L)
{
    using namespace luabind;

    module(L, "math")
        [
            class_<Vector2>("Vector2")
            .def(constructor<>())
            .def(constructor<float, float>())
            .def_readwrite("x", &Vector2::x)
            .def_readwrite("y", &Vector2::y)
            .def("isZero", &Vector2::isZero)
            .def("normalize", &Vector2::normalize)
            .def(self + other<Vector2>())
            .def(self - other<Vector2>())
            .def(self * other<Vector2>())
            .def(self / float())
            .def(self * float())
            .def(float() * self)
            .def(tostring(self))
        ];

    module(L, "math")
        [
            class_<Vector3>("Vector3")
            .def(constructor<>())
            .def(constructor<float, float, float>())
            .def_readwrite("x", &Vector3::x)
            .def_readwrite("y", &Vector3::y)
            .def_readwrite("z", &Vector3::z)
            .def("isZero", &Vector3::isZero)
            .def("normalize", &Vector3::normalize)
            .def(self + other<Vector3>())
            .def(self - other<Vector3>())
            .def(self * other<Vector3>())
            .def(self / float())
            .def(self * float())
            .def(float() * self)
            .def(tostring(self))
        ];

    module(L, "math")
        [
            class_<Vector4>("Vector4")
            .def(constructor<>())
            .def(constructor<float, float, float, float>())
            .def_readwrite("x", &Vector4::x)
            .def_readwrite("y", &Vector4::y)
            .def_readwrite("z", &Vector4::z)
            .def_readwrite("w", &Vector4::w)
            .def("isZero", &Vector4::isZero)
            .def("normalize", &Vector4::normalize)
            .def(self + other<Vector4>())
            .def(self - other<Vector4>())
            .def(self * other<Vector4>())
            .def(self / float())
            .def(self * float())
            .def(float() * self)
            .def(tostring(self))
        ];

    module(L, "math")
        [
            class_<Quaternion>("Quaternion")
            .def(constructor<>())
            .def(constructor<float, float, float, float>())
            .def("setEulerRad", &Quaternion::SetEulerRad)
            .def("setEulerDeg", &Quaternion::SetEulerDeg)
            .def_readwrite("x", &Quaternion::x)
            .def_readwrite("y", &Quaternion::y)
            .def_readwrite("z", &Quaternion::z)
            .def_readwrite("w", &Quaternion::w)
            .def(tostring(self))
        ];

    module(L, "math")
        [
            class_<Matrix33>("Matrix33")
            .def(constructor<>())
            .def(const_self * other<Vector3>())
            .def(other<Vector3>() * const_self)
        ];

    module(L, "math")
        [
            class_<Matrix44>("Matrix44")
            .def(constructor<>())
            .def(const_self * other<Vector3>())
            .def(const_self * other<Vector4>())
            .def(other<Vector3>() * const_self)
            .def(other<Vector4>() * const_self)
        ];

    module(L, "math")
        [
            def("crossProduct", (Vector3(*)(const Vector3&, const Vector3&))&CrossProduct),

            def("RotateRadMatrix33", (Matrix33(*)(char, float))&RotateRadMatrix33),
            def("RotateDegMatrix33", (Matrix33(*)(char, float))&RotateDegMatrix33),
            def("RotateRadMatrix33", (Matrix33(*)(const Vector3&))&RotateRadMatrix33),
            def("RotateDegMatrix33", (Matrix33(*)(const Vector3&))&RotateDegMatrix33),
            def("RotateRadMatrix33", (Matrix33(*)(const Vector3&, float))&RotateRadMatrix33),
            def("RotateDegMatrix3", (Matrix33(*)(const Vector3&, float))&RotateDegMatrix33),

            def("CreateFromMatrix33", (Quaternion(*)(const Matrix33&))&CreateFromMatrix33),

            def("RotateRadMatrix44", (Matrix44(*)(char, float))&RotateRadMatrix44),
            def("RotateDegMatrix44", (Matrix44(*)(char, float))&RotateDegMatrix44),
            def("RotateRadMatrix44", (Matrix44(*)(const Vector3&))&RotateRadMatrix44),
            def("RotateDegMatrix44", (Matrix44(*)(const Vector3&))&RotateDegMatrix44),
            def("RotateRadMatrix44", (Matrix44(*)(const Vector3&, float))&RotateRadMatrix44),
            def("RotateDegMatrix44", (Matrix44(*)(const Vector3&, float))&RotateDegMatrix44),

            def("CreateFromMatrix44", (Quaternion(*)(const Matrix44))&CreateFromMatrix44)

        ];

    module(L, "math")
        [
            class_<Ray>("Ray")
            .def(constructor<>())
            .def(constructor<const Vector3&, const Vector3&>())
            .def_readwrite("Origin", &Ray::Origin)
            .def_readwrite("Dir", &Ray::Dir)
        ];

    //wxLogError("Helolo the.");
}



} // namespace script
} // namespace eg


namespace
{
    bool result = eg::script::addRegScriptRegistrate(eg::script::intitMathModule);
} // namespace
