//--------------------------------------------------------------------------------------------------------------------//
//                                                ASSERT CUSTOM MACRO                                                 //
// 13/11/01, Mythos                                                                                                   //
//           based on item 1.12 of the Game Programming Gems book                                                     //
// features: understandable error message (YOU should provide it)                                                     //
//           breaks on the line where _ASSERT_ sits, not in assert.c                                                  //
//           "ignore always" option                                                                                   //
//           the whole dialog text is copied to the clipboard for easier bug reporting                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _MYTHOS_MYASSERT_H_
#define   _MYTHOS_MYASSERT_H_
#pragma    once

//----------------------------------------------- MACROS -------------------------------------------------------------//

#define   _ASSERT_ACTIVE_
//#undef    _ASSERT_ACTIVE_

#ifdef    _ASSERT_ACTIVE_
namespace  Mythos
  {
  extern   bool DisplayAssert(char*,char*,int,bool*);       // display message & choose to continue/abort (false) or debug (true)
  }

#define   _ASSERT_(cond,text)                                          \
  {                                                                    \
  static bool boIgnoreAlways = false;                                  \
  if(!boIgnoreAlways && !(cond))                                       \
    {                                                                  \
    if(Mythos::DisplayAssert(text,__FILE__,__LINE__,&boIgnoreAlways))  \
      {                                                                \
      _asm { int 3 }                                                   \
      }                                                                \
    }                                                                  \
  }

#else  // _ASSERT_ACTIVE_
#define   _ASSERT_(cond,text)  \
  {                            \
  if(cond) {}                  \
  }
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _MYTHOS_MYASSERT_H_
