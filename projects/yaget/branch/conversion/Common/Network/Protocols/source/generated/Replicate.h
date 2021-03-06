// p:\Yaget\branch\main\Common\Network\Protocols\include\Replicate.h - generated by OIDL Sat Dec 27 04:09:19 2008


#if !defined(REPLICATE_H)
#define REPLICATE_H

#include "objectsystem/object.h"
#include "objectsystem/objectproxy.h"
#include "objectsystem/objectreference.h"
#include "Math/Vector.h"
#include "Math/Vector.h"
#include "Math/Vector.h"
#include "utilities/frame.h"

class IReplicateRequestRef : public omega::ObjectReference {};

class IReplicateReplyProxy;

class IReplicateRequest;
class IReplicateReply;

class IReplicateRequest {
public:


  enum { AttachToObserver_uint64_observerIdSignature = 0x75dcfa38 };
  virtual void AttachToObserver (IReplicateReplyProxy &reply,
	omega::uint64 observerId) = 0;

};



class IReplicateRequestProxy : public omega::ObjectProxy {
private:

  IReplicateRequest *m_Direct;
  IReplicateReply *m_Reply;

public:

  IReplicateRequestProxy(omega::Object::ConnectionID c);

  IReplicateRequestProxy() : ObjectProxy(), m_Direct(0), m_Reply(0) {
  };

  IReplicateRequestProxy(const IReplicateRequestProxy &c) : ObjectProxy(c)
  {
  m_Direct = c.m_Direct;
  m_Reply = c.m_Reply;
  };

  IReplicateRequestProxy(omega::Object::ConnectionID c, IReplicateRequest *direct, IReplicateReply *reply) : ObjectProxy(c)
  {
  m_Direct = direct;
  m_Reply = reply;
  };

  virtual void SetConnection(omega::Object::ConnectionID c);

  bool IsValid() const
  {
    return (IsLocal() || IsDestinationSupportedInterface("(IReplicateRequest)"));
  };

  omega::uint32 GetInterfaceVersion()
  {
    return GetProtocolVersion("(IReplicateRequest)");
  };

  void AttachToObserver ( omega::uint64 observerId ); // signature 75dcfa38

};


class IReplicateReplyRef : public omega::ObjectReference {};

class IReplicateRequestProxy;

class IReplicateReply;
class IReplicateRequest;

class IReplicateReply {
public:


  enum { Replicate_uint64_oId_uint32_cId_string_propName_bool_valueSignature = 0xfe6c9474 };
  virtual void Replicate (IReplicateRequestProxy &reply,
	omega::uint64 oId,
	omega::uint32 cId,
	const omega::OString& propName,
	bool value) = 0;

  enum { Replicate_uint64_oId_uint32_cId_string_propName_float32_valueSignature = 0x82298879 };
  virtual void Replicate (IReplicateRequestProxy &reply,
	omega::uint64 oId,
	omega::uint32 cId,
	const omega::OString& propName,
	omega::float32 value) = 0;

  enum { Replicate_uint64_oId_uint32_cId_string_propName_int32_valueSignature = 0x6698627e };
  virtual void Replicate (IReplicateRequestProxy &reply,
	omega::uint64 oId,
	omega::uint32 cId,
	const omega::OString& propName,
	omega::int32 value) = 0;

  enum { Replicate_uint64_oId_uint32_cId_string_propName_uint32_valueSignature = 0xaa8963c2 };
  virtual void Replicate (IReplicateRequestProxy &reply,
	omega::uint64 oId,
	omega::uint32 cId,
	const omega::OString& propName,
	omega::uint32 value) = 0;

  enum { Replicate_uint64_oId_uint32_cId_string_propName_eg__Vector2_valueSignature = 0x6b13e085 };
  virtual void Replicate (IReplicateRequestProxy &reply,
	omega::uint64 oId,
	omega::uint32 cId,
	const omega::OString& propName,
	const eg::Vector2& value) = 0;

  enum { Replicate_uint64_oId_uint32_cId_string_propName_eg__Vector3_valueSignature = 0x6b13e185 };
  virtual void Replicate (IReplicateRequestProxy &reply,
	omega::uint64 oId,
	omega::uint32 cId,
	const omega::OString& propName,
	const eg::Vector3& value) = 0;

  enum { Replicate_uint64_oId_uint32_cId_string_propName_eg__Vector4_valueSignature = 0x6b13e285 };
  virtual void Replicate (IReplicateRequestProxy &reply,
	omega::uint64 oId,
	omega::uint32 cId,
	const omega::OString& propName,
	const eg::Vector4& value) = 0;

  enum { Replicate_uint64_oId_uint32_cId_string_propName_string_valueSignature = 0x5c64f8bf };
  virtual void Replicate (IReplicateRequestProxy &reply,
	omega::uint64 oId,
	omega::uint32 cId,
	const omega::OString& propName,
	const omega::OString& value) = 0;

};



class IReplicateReplyProxy : public omega::ObjectProxy {
private:

  IReplicateReply *m_Direct;
  IReplicateRequest *m_Reply;

public:

  IReplicateReplyProxy(omega::Object::ConnectionID c);

  IReplicateReplyProxy() : ObjectProxy(), m_Direct(0), m_Reply(0) {
  };

  IReplicateReplyProxy(const IReplicateReplyProxy &c) : ObjectProxy(c)
  {
  m_Direct = c.m_Direct;
  m_Reply = c.m_Reply;
  };

  IReplicateReplyProxy(omega::Object::ConnectionID c, IReplicateReply *direct, IReplicateRequest *reply) : ObjectProxy(c)
  {
  m_Direct = direct;
  m_Reply = reply;
  };

  virtual void SetConnection(omega::Object::ConnectionID c);

  bool IsValid() const
  {
    return (IsLocal() || IsDestinationSupportedInterface("(IReplicateReply)"));
  };

  omega::uint32 GetInterfaceVersion()
  {
    return GetProtocolVersion("(IReplicateReply)");
  };

  void Replicate ( omega::uint64 oId, omega::uint32 cId, const omega::OString& propName, bool value ); // signature fe6c9474
  void Replicate ( omega::uint64 oId, omega::uint32 cId, const omega::OString& propName, omega::float32 value ); // signature 82298879
  void Replicate ( omega::uint64 oId, omega::uint32 cId, const omega::OString& propName, omega::int32 value ); // signature 6698627e
  void Replicate ( omega::uint64 oId, omega::uint32 cId, const omega::OString& propName, omega::uint32 value ); // signature aa8963c2
  void Replicate ( omega::uint64 oId, omega::uint32 cId, const omega::OString& propName, const eg::Vector2& value ); // signature 6b13e085
  void Replicate ( omega::uint64 oId, omega::uint32 cId, const omega::OString& propName, const eg::Vector3& value ); // signature 6b13e185
  void Replicate ( omega::uint64 oId, omega::uint32 cId, const omega::OString& propName, const eg::Vector4& value ); // signature 6b13e285
  void Replicate ( omega::uint64 oId, omega::uint32 cId, const omega::OString& propName, const omega::OString& value ); // signature 5c64f8bf

};

#endif
