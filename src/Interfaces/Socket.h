/*
 * Copyright 2014-2015 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <boost/noncopyable.hpp>
#include <memory>
#include <vector>
#include <tuple>

#include "Interface.h"

#ifndef _SOCKET_H_
#define _SOCKET_H_

struct sockaddr;


namespace Interfaces
{

class Socket : public Interface, private boost::noncopyable
{
public:
  typedef char ByteOfStructSockaddr;

  enum class DomainType
  {
    INET,
    INET6
  };


  enum class SocketType
  {
    DGRAM,
    STREAM
  };

  virtual ~Socket();

  static std::unique_ptr<Socket> Create(DomainType domain, SocketType type);
  void Bind(const int &port, const std::string &address);

  void Connect(const std::string &address, const int &port);
  bool IsConnected() const;

  void Listen(const int &backlog);
  std::unique_ptr<Socket> Accept();

  DomainType GetDomainType() const;
  SocketType GetSocketType() const;

  std::string GetRemoteAddress() const;
  int GetRemotePort() const;

  size_t Read(void *destination, const size_t &bufferLength);
  void Write(const void *source, const size_t &bufferLength);

  size_t Recv(void *destination, const size_t &bufferLength, const int &flags = 0);
  void Send(const void *source, const size_t &bufferLength, const int &flags = 0);

  size_t RecvFrom(void *destination, const size_t &bufferLength, std::string &address, int &port, const int &flags = 0);
  void SendTo(const void *source, const size_t &bufferLength, const std::string &address, const int &port, const int &flags = 0);

  void Close();

private:
  Socket(DomainType domain,
	 SocketType type,
	 int socket_fd,
	 const std::string &remote_address = "",
	 const int &remote_port = -1);

  int socket_fd;

  DomainType domain_type;
  SocketType socket_type;

  std::string remote_address;
  int remote_port;

  bool is_connected;
  bool close_executed;

  static int ToUnixType(DomainType domain);
  static int ToUnixType(SocketType type);
  static std::string InAddrBinaryAddressToTextForm(DomainType domain_type, const void *in46_addr);
  static std::vector<ByteOfStructSockaddr> ToBinaryForm(DomainType domain,
							const std::string &address,
							const int &port);
  static std::tuple<std::string, int> FromBinaryForm(DomainType domain_type,
						     const sockaddr *addr);
};

}

#endif
