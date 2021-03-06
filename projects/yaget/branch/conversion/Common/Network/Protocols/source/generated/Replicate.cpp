// p:\Yaget\branch\main\Common\Network\Protocols\include\Replicate.cpp - generated by OIDL Sat Dec 27 04:09:19 2008


#include "Replicate.h"
IReplicateRequestProxy::IReplicateRequestProxy(omega::Object::ConnectionID c) : ObjectProxy(c), m_Direct(0), m_Reply(0)
{
  if (IsLocal())
    {
      m_Direct = dynamic_cast<IReplicateRequest *> (GetDestinationObject());

      m_Reply = dynamic_cast<IReplicateReply *> (GetOwnerObject());

    }
};

void IReplicateRequestProxy::SetConnection(omega::Object::ConnectionID c)
{
  ObjectProxy::SetConnection(c);
  if (IsLocal())
    {
      m_Direct = dynamic_cast<IReplicateRequest *> (GetDestinationObject());

      m_Reply = dynamic_cast<IReplicateReply *> (GetOwnerObject());

    }
};

void IReplicateRequestProxy::AttachToObserver (
	omega::uint64 observerId)
{
  if (!IsValid()) return;

  if (m_Direct)
    {
    IReplicateReplyProxy reply(m_connection, m_Reply, m_Direct);
    m_Direct->AttachToObserver(reply, observerId);
    return;
  }

  omega::PDUOStream stream;
  omega::Frame& dataFrame = ObjectProxy::PrepareMessage(stream, 0x75dcfa38);
  dataFrame << observerId;
  ObjectProxy::SendMessage(stream);
}

IReplicateReplyProxy::IReplicateReplyProxy(omega::Object::ConnectionID c) : ObjectProxy(c), m_Direct(0), m_Reply(0)
{
  if (IsLocal())
    {
      m_Direct = dynamic_cast<IReplicateReply *> (GetDestinationObject());

      m_Reply = dynamic_cast<IReplicateRequest *> (GetOwnerObject());

    }
};

void IReplicateReplyProxy::SetConnection(omega::Object::ConnectionID c)
{
  ObjectProxy::SetConnection(c);
  if (IsLocal())
    {
      m_Direct = dynamic_cast<IReplicateReply *> (GetDestinationObject());

      m_Reply = dynamic_cast<IReplicateRequest *> (GetOwnerObject());

    }
};

void IReplicateReplyProxy::Replicate (
	omega::uint64 oId,
	omega::uint32 cId,
	const omega::OString& propName,
	bool value)
{
  if (!IsValid()) return;

  if (m_Direct)
    {
    IReplicateRequestProxy reply(m_connection, m_Reply, m_Direct);
    m_Direct->Replicate(reply, oId, cId, propName, value);
    return;
  }

  omega::PDUOStream stream;
  omega::Frame& dataFrame = ObjectProxy::PrepareMessage(stream, 0xfe6c9474);
  dataFrame << oId;
  dataFrame << cId;
  dataFrame << propName;
  dataFrame << value;
  ObjectProxy::SendMessage(stream);
}

void IReplicateReplyProxy::Replicate (
	omega::uint64 oId,
	omega::uint32 cId,
	const omega::OString& propName,
	omega::float32 value)
{
  if (!IsValid()) return;

  if (m_Direct)
    {
    IReplicateRequestProxy reply(m_connection, m_Reply, m_Direct);
    m_Direct->Replicate(reply, oId, cId, propName, value);
    return;
  }

  omega::PDUOStream stream;
  omega::Frame& dataFrame = ObjectProxy::PrepareMessage(stream, 0x82298879);
  dataFrame << oId;
  dataFrame << cId;
  dataFrame << propName;
  dataFrame << value;
  ObjectProxy::SendMessage(stream);
}

void IReplicateReplyProxy::Replicate (
	omega::uint64 oId,
	omega::uint32 cId,
	const omega::OString& propName,
	omega::int32 value)
{
  if (!IsValid()) return;

  if (m_Direct)
    {
    IReplicateRequestProxy reply(m_connection, m_Reply, m_Direct);
    m_Direct->Replicate(reply, oId, cId, propName, value);
    return;
  }

  omega::PDUOStream stream;
  omega::Frame& dataFrame = ObjectProxy::PrepareMessage(stream, 0x6698627e);
  dataFrame << oId;
  dataFrame << cId;
  dataFrame << propName;
  dataFrame << value;
  ObjectProxy::SendMessage(stream);
}

void IReplicateReplyProxy::Replicate (
	omega::uint64 oId,
	omega::uint32 cId,
	const omega::OString& propName,
	omega::uint32 value)
{
  if (!IsValid()) return;

  if (m_Direct)
    {
    IReplicateRequestProxy reply(m_connection, m_Reply, m_Direct);
    m_Direct->Replicate(reply, oId, cId, propName, value);
    return;
  }

  omega::PDUOStream stream;
  omega::Frame& dataFrame = ObjectProxy::PrepareMessage(stream, 0xaa8963c2);
  dataFrame << oId;
  dataFrame << cId;
  dataFrame << propName;
  dataFrame << value;
  ObjectProxy::SendMessage(stream);
}

void IReplicateReplyProxy::Replicate (
	omega::uint64 oId,
	omega::uint32 cId,
	const omega::OString& propName,
	const eg::Vector2& value)
{
  if (!IsValid()) return;

  if (m_Direct)
    {
    IReplicateRequestProxy reply(m_connection, m_Reply, m_Direct);
    m_Direct->Replicate(reply, oId, cId, propName, value);
    return;
  }

  omega::PDUOStream stream;
  omega::Frame& dataFrame = ObjectProxy::PrepareMessage(stream, 0x6b13e085);
  dataFrame << oId;
  dataFrame << cId;
  dataFrame << propName;
  dataFrame << value;
  ObjectProxy::SendMessage(stream);
}

void IReplicateReplyProxy::Replicate (
	omega::uint64 oId,
	omega::uint32 cId,
	const omega::OString& propName,
	const eg::Vector3& value)
{
  if (!IsValid()) return;

  if (m_Direct)
    {
    IReplicateRequestProxy reply(m_connection, m_Reply, m_Direct);
    m_Direct->Replicate(reply, oId, cId, propName, value);
    return;
  }

  omega::PDUOStream stream;
  omega::Frame& dataFrame = ObjectProxy::PrepareMessage(stream, 0x6b13e185);
  dataFrame << oId;
  dataFrame << cId;
  dataFrame << propName;
  dataFrame << value;
  ObjectProxy::SendMessage(stream);
}

void IReplicateReplyProxy::Replicate (
	omega::uint64 oId,
	omega::uint32 cId,
	const omega::OString& propName,
	const eg::Vector4& value)
{
  if (!IsValid()) return;

  if (m_Direct)
    {
    IReplicateRequestProxy reply(m_connection, m_Reply, m_Direct);
    m_Direct->Replicate(reply, oId, cId, propName, value);
    return;
  }

  omega::PDUOStream stream;
  omega::Frame& dataFrame = ObjectProxy::PrepareMessage(stream, 0x6b13e285);
  dataFrame << oId;
  dataFrame << cId;
  dataFrame << propName;
  dataFrame << value;
  ObjectProxy::SendMessage(stream);
}

void IReplicateReplyProxy::Replicate (
	omega::uint64 oId,
	omega::uint32 cId,
	const omega::OString& propName,
	const omega::OString& value)
{
  if (!IsValid()) return;

  if (m_Direct)
    {
    IReplicateRequestProxy reply(m_connection, m_Reply, m_Direct);
    m_Direct->Replicate(reply, oId, cId, propName, value);
    return;
  }

  omega::PDUOStream stream;
  omega::Frame& dataFrame = ObjectProxy::PrepareMessage(stream, 0x5c64f8bf);
  dataFrame << oId;
  dataFrame << cId;
  dataFrame << propName;
  dataFrame << value;
  ObjectProxy::SendMessage(stream);
}

