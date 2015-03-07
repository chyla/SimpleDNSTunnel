/*
 * Copyright 2014-2015 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <cerrno>
#include <cstring>
#include <boost/log/trivial.hpp>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>

#include "TunTap.h"
#include "InterfaceException.h"

using namespace std;


namespace Interfaces
{


TunTap::~TunTap()
{
  if (!close_executed && fd != -1)
    BOOST_LOG_TRIVIAL(warning) << "TunTap::Close() not called! Possibly a bug.";
}


unique_ptr<TunTap>
TunTap::Create(TunTap::InterfaceType type)
{
  const char * const tun_tap_device = "/dev/net/tun"; 
  unique_ptr<TunTap> tt(new TunTap(type));

  BOOST_LOG_TRIVIAL(info) << "Opening device file: " << tun_tap_device;
  tt->fd = open(tun_tap_device, O_RDWR);
  if (tt->fd < 0)
    throw InterfaceException(strerror(errno));

  struct ifreq ifr;
  memset(&ifr, 0, sizeof(struct ifreq));
  if (type == InterfaceType::TUN)
    ifr.ifr_flags = IFF_TUN;
  else if (type == InterfaceType::TAP)
    ifr.ifr_flags = IFF_TAP;

  BOOST_LOG_TRIVIAL(info) << "Creating TUN/TAP device...";
  int err = ioctl(tt->fd, TUNSETIFF, (void *)&ifr);
  if (err < 0)
    throw InterfaceException(strerror(errno));

  tt->name = ifr.ifr_name;

  BOOST_LOG_TRIVIAL(info) << "Created device: " << tt->name;

  return tt;
}


TunTap::InterfaceType
TunTap::GetType() const
{
  return type;
}


std::string
TunTap::GetName() const
{
  return name;
}


size_t
TunTap::Read(void *destination, const size_t &bufferLength)
{
  ssize_t n = read(fd, destination, bufferLength);

  if (n < 0)
    throw InterfaceException(strerror(errno));

  return n;
}


void
TunTap::Write(const void *source, const size_t &bufferLength)
{
  ssize_t n = write(fd, source, bufferLength);

  if (n < 0)
    throw InterfaceException(strerror(errno));
}


bool
TunTap::IsReadyToRead() const
{
  fd_set fds;
  timeval tv = {0, 0};
  FD_ZERO(&fds);
  FD_SET(fd, &fds);

  int err = select(fd + 1, &fds, 0, 0, &tv);
  if (err < 0)
    throw InterfaceException(strerror(errno));

  return err;
}


void
TunTap::Close()
{
  BOOST_LOG_TRIVIAL(info) << "Closing TunTap descriptor: " << fd;
  int err = close(fd);
  if (err < 0)
    throw InterfaceException(strerror(errno));

  BOOST_LOG_TRIVIAL(info) << "TunTap descriptor " << fd << " closed.";
  close_executed = true;
}


TunTap::TunTap(TunTap::InterfaceType type) :
  type(type),
  fd(-1),
  close_executed(false)
{
}

}
