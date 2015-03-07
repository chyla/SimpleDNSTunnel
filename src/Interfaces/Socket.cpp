/*
 * Copyright 2014-2015 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "Socket.h"
#include "InterfaceException.h"

#include <boost/log/trivial.hpp>
#include <cstring>
#include <cerrno>
#include <vector>
#include <stdexcept>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>

using namespace std;


namespace Interfaces
{

Socket::~Socket()
{
  if (!close_executed && socket_fd != -1)
    BOOST_LOG_TRIVIAL(warning) << "Socket::Close() not called! Possibly a bug.";
}


unique_ptr<Socket>
Socket::Create(DomainType domain, SocketType type)
{
  int d = ToUnixType(domain);
  int t = ToUnixType(type);

  BOOST_LOG_TRIVIAL(info) << "Creating socket...";
  int fd = socket(d, t, 0);
  if (fd < 0)
    throw InterfaceException(strerror(errno));

  BOOST_LOG_TRIVIAL(info) << "New socket descriptor: " << fd;

  unique_ptr<Socket> sock(new Socket(domain, type, fd));
  return sock;
}


void
Socket::Bind(const int &port, const std::string &address)
{
  int err;
  vector<ByteOfStructSockaddr> v = ToBinaryForm(domain_type, address, port);

  BOOST_LOG_TRIVIAL(info) << "Binding socket " << socket_fd
                          << " to " << address << ":" << port << ".";
  err = bind(socket_fd, reinterpret_cast<sockaddr*>(v.data()), v.size());
  if (err < 0)
    throw InterfaceException(strerror(errno));
}


void
Socket::Connect(const std::string &address, const int &port)
{
  vector<ByteOfStructSockaddr> v = ToBinaryForm(domain_type, address, port);

  BOOST_LOG_TRIVIAL(info) << "Connecting socket " << socket_fd
                          << " to: " << address << ":" << port;
  int err = connect(socket_fd, reinterpret_cast<sockaddr*>(v.data()), v.size());
  if (err < 0)
    throw InterfaceException(strerror(errno));

  remote_address = address;
  remote_port = port;
  is_connected = true;
}


bool
Socket::IsConnected() const
{
  return is_connected;
}


void
Socket::Listen(const int &backlog)
{
  BOOST_LOG_TRIVIAL(info) << "Listening socket " << socket_fd
                          << ", backlog: " << backlog;
  int err = listen(socket_fd, backlog);
  if (err < 0)
    throw InterfaceException(strerror(errno));
}


unique_ptr<Socket>
Socket::Accept()
{
  vector<ByteOfStructSockaddr> v;
  int new_socket_fd;
  in_port_t *port;
  socklen_t n;
  void *address;

  BOOST_LOG_TRIVIAL(info) << "Accept new client.";
  if (domain_type == DomainType::INET)
  {
    n = sizeof(sockaddr_in);
    v.resize(n, 0);

    sockaddr_in *p = reinterpret_cast<sockaddr_in*>(v.data());
    port = &p->sin_port;
    address = &p->sin_addr;
  }
  else if (domain_type == DomainType::INET6)
  {
    n = sizeof(sockaddr_in6);
    v.resize(n, 0);

    sockaddr_in6 *p = reinterpret_cast<sockaddr_in6*>(v.data());
    port = &p->sin6_port;
    address = &p->sin6_addr;
  }

  new_socket_fd = accept(socket_fd, reinterpret_cast<sockaddr*>(v.data()), &n);
  if (new_socket_fd < 0)
    throw InterfaceException(strerror(errno));

  unique_ptr<Socket> s(new Socket(domain_type,
				  socket_type,
				  new_socket_fd,
				  InAddrBinaryAddressToTextForm(domain_type, address),
				  ntohs(*port)));
  return s;
}


Socket::DomainType
Socket::GetDomainType() const
{
  return domain_type;
}


Socket::SocketType
Socket::GetSocketType() const
{
  return socket_type;
}


std::string
Socket::GetRemoteAddress() const
{
  if (remote_address == "")
    throw InterfaceException("Remote address not set.");

  return remote_address;
}


int
Socket::GetRemotePort() const
{
  if (remote_port == -1)
    throw InterfaceException("Remote port not set.");

  return remote_port;
}


size_t
Socket::Read(void *destination, const size_t &bufferLength)
{
  return Recv(destination, bufferLength);
}


void
Socket::Write(const void *source, const size_t &bufferLength)
{
  Send(source, bufferLength);
}


size_t
Socket::Recv(void *destination, const size_t &bufferLength, const int &flags)
{
  ssize_t r = recv(socket_fd, destination, bufferLength, flags);
  if (r < 0)
    throw InterfaceException(strerror(errno));

  return r;
}


void
Socket::Send(const void *source, const size_t &bufferLength, const int &flags)
{
  ssize_t r = send(socket_fd, source, bufferLength, flags);
  if (r < 0)
    throw InterfaceException(strerror(errno));
}


size_t
Socket::RecvFrom(void *destination, const size_t &bufferLength, string &address, int &port, const int &flags)
{
  vector<ByteOfStructSockaddr> v;
  if (domain_type == DomainType::INET)
    v.resize(sizeof(sockaddr_in), 0);  
  else if (domain_type == DomainType::INET6)
    v.resize(sizeof(sockaddr_in6), 0);

  sockaddr *s = reinterpret_cast<sockaddr*>(v.data());
  socklen_t n = v.size();

  ssize_t r = recvfrom(socket_fd, destination, bufferLength, flags, s, &n);
  if (r < 0)
    throw InterfaceException(strerror(errno));
  if (v.size() < n)
    throw InterfaceException("Address is truncated!");

  tuple<string, int> address_port = FromBinaryForm(domain_type, s);
  address = get<0>(address_port);
  port = get<1>(address_port);

  return r;
}


void
Socket::SendTo(const void *source, const size_t &bufferLength, const std::string &address, const int &port, const int &flags)
{
  vector<ByteOfStructSockaddr> v = ToBinaryForm(domain_type, address, port);
  ssize_t r = sendto(socket_fd, source, bufferLength, flags,
		     reinterpret_cast<sockaddr*>(v.data()), v.size());
  if (r < 0)
    throw InterfaceException(strerror(errno));
}


void
Socket::Close()
{
  BOOST_LOG_TRIVIAL(info) << "Closing socket: " << socket_fd << "...";
  int err = close(socket_fd);
  if (err < 0)
    throw InterfaceException(strerror(errno));

  BOOST_LOG_TRIVIAL(info) << "Closed socket descriptor: " << socket_fd;

  socket_fd = -1;
  close_executed = true;
}



bool
Socket::IsReadyToRead() const
{
  fd_set fds;
  timeval tv = {0, 0};
  FD_ZERO(&fds);
  FD_SET(socket_fd, &fds);

  int err = select(socket_fd + 1, &fds, 0, 0, &tv);
  if (err < 0)
    throw InterfaceException(strerror(errno));

  return err;
}


Socket::Socket(Socket::DomainType domain,
	       Socket::SocketType type,
	       int socket_fd,
	       const std::string &remote_address,
	       const int &remote_port) :
  socket_fd(socket_fd),
  domain_type(domain),
  socket_type(type),
  remote_address(remote_address),
  remote_port(remote_port),
  is_connected(false)
{
}


int
Socket::ToUnixType(DomainType domain)
{
  switch (domain)
  {
  case DomainType::INET:
    return PF_INET;

  case DomainType::INET6:
    return PF_INET6;
  }

  throw domain_error("Can't convert DomainType to unix type.");
}


int
Socket::ToUnixType(SocketType type)
{
  switch (type)
  {
  case SocketType::DGRAM:
    return SOCK_DGRAM;

  case SocketType::STREAM:
    return SOCK_STREAM;
  }

  throw domain_error("Can't convert SocketType to unix type.");
}


string
Socket::InAddrBinaryAddressToTextForm(DomainType domain_type, const void *in46_addr)
{
  char dst[255] = {0};
  const char *ret;

  BOOST_LOG_TRIVIAL(info) << "Converting to text form.";
  ret = inet_ntop(ToUnixType(domain_type), in46_addr, dst, sizeof(dst));
  if (ret != dst)
    throw InterfaceException(strerror(errno));

  return string(dst);
}


vector<Socket::ByteOfStructSockaddr>
Socket::ToBinaryForm(DomainType domain_type, const string &address, const int &port)
{
  vector<ByteOfStructSockaddr> v;
  int n;

  BOOST_LOG_TRIVIAL(info) << "Converting " + address + " to binary form.";
  if (domain_type == DomainType::INET)
  {
    n = sizeof(sockaddr_in);
    v.resize(n, 0);

    sockaddr_in *s = reinterpret_cast<sockaddr_in*>(v.data());
    s->sin_family = AF_INET;
    s->sin_port = htons(port);

    const int err = inet_aton(address.c_str(), &s->sin_addr);
    if (err == 0)
      throw InterfaceException(strerror(errno));
  }
  else if (domain_type == DomainType::INET6)
  {
    n = sizeof(sockaddr_in6);
    v.resize(n, 0);

    sockaddr_in6 *s = reinterpret_cast<sockaddr_in6*>(v.data());
    s->sin6_family = AF_INET6;
    s->sin6_port = htons(port);

    const int err = inet_pton(AF_INET6, address.c_str(), s->sin6_addr.s6_addr);
    if (err == 0)
      throw InterfaceException("Invalid IPv6 network address: " + address);
    else if (err < 0)
      throw InterfaceException(strerror(errno));
  }

  return v;
}


tuple<string, int>
Socket::FromBinaryForm(DomainType domain_type, const sockaddr *addr)
{
  string address;
  int port;
  
  if (domain_type == DomainType::INET)
  {
    auto p = reinterpret_cast<const sockaddr_in*>(addr);
    address = InAddrBinaryAddressToTextForm(domain_type, reinterpret_cast<const void*>(&p->sin_addr));
    port = ntohs(p->sin_port);
  }
  else if (domain_type == DomainType::INET6)
  {
    auto p = reinterpret_cast<const sockaddr_in6*>(addr);
    address = InAddrBinaryAddressToTextForm(domain_type, reinterpret_cast<const void*>(&p->sin6_addr));
    port = ntohs(p->sin6_port);
  }
  
  return make_tuple(address, port);
}

}
