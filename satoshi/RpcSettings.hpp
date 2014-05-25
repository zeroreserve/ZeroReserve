/*  Namecoin RPC library.
 *  Copyright (C) 2013-2014  Daniel Kraft <d@domob.eu>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  See the distributed file COPYING for additional permissions in addition
 *  to those of the GNU Affero General Public License.
 */

#ifndef NMCRPC_RPCSETTINGS_HPP
#define NMCRPC_RPCSETTINGS_HPP

#include <string>

namespace nmcrpc
{

/**
 * Connection settings for the HTTP channel to the RPC interface.  This
 * also contains routines for trying to load the correct settings from
 * a user's namecoin.conf file.
 */
class RpcSettings
{

private:

  /** Host name.  */
  std::string host;
  /** Port to connect to.  */
  unsigned port;

  /** Username for authentication.  */
  std::string username;
  /** Password for authentication.  */
  std::string password;

public:

  /**
   * Default constructor.  This sets host and port to the default values
   * and leaves the authentication data blank.
   */
  inline RpcSettings ()
    : host("localhost"), port(8336), username(""), password("")
  {
    // Nothing else to do.
  }

  /**
   * Construct with all values given.
   * @param h The host value.
   * @param p The port value.
   * @param u The username value.
   * @param pwd The password value.
   */
  inline RpcSettings (const std::string& h, unsigned p,
                      const std::string& u, const std::string& pwd)
    : host(h), port(p), username(u), password(pwd)
  {
    // Nothing else to do.
  }

  // Allow copying.
#ifdef CXX_11
  RpcSettings (const RpcSettings&) = default;
  RpcSettings& operator= (const RpcSettings&) = default;
#endif /* CXX_11?  */

  /**
   * Try to read the given input file and update settings when corresponding
   * ones are found there.
   * @param filename The input file's name.
   */
  void readConfig (const std::string& filename);

  /**
   * Try to read the default namecoin.conf config file and update settings.
   */
  void readDefaultConfig ();

  /* Accessor methods.  */

  inline const std::string&
  getHost () const
  {
    return host;
  }
  inline void
  setHost (const std::string& h)
  {
    host = h;
  }

  inline unsigned
  getPort () const
  {
    return port;
  }
  inline void
  setPort (unsigned p)
  {
    port = p;
  }

  inline const std::string&
  getUsername () const
  {
    return username;
  }
  inline void
  setUsername (const std::string& u)
  {
    username = u;
  }

  inline const std::string&
  getPassword () const
  {
    return password;
  }
  inline void
  setPassword (const std::string& pwd)
  {
    password = pwd;
  }

};

} // namespace nmcrpc

#endif /* Header guard.  */
