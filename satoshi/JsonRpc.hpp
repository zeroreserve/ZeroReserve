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

#ifndef NMCRPC_JSONRPC_HPP
#define NMCRPC_JSONRPC_HPP

#include "RpcSettings.hpp"

#include <json/value.h>

#include <iostream>
#include <stdexcept>
#include <string>

namespace nmcrpc
{

/* ************************************************************************** */
/* The JsonRpc class itself.  */

/**
 * JSON-RPC handling class.  It does the HTTP connection to the given server
 * as well as the JSON parsing/encoding, but doesn't care about the
 * Namecoin behind.
 */
class JsonRpc
{

public:

  /* Exception classes thrown for different errors.  */
  class Exception;
  class JsonParseError;
  class HttpError;
  class RpcError;

  /** Type of JSON data returned.  */
  typedef Json::Value JsonData;

private:

  /** Environment variable name for controlling the call log file.  */
  static const std::string LOGFILE_VAR;

  /** Connection settings.  */
  RpcSettings settings;

  /** The next ID to use for JSON-RPC queries.  */
  unsigned nextId;

  /**
   * Allow to disable call logging in a "one shot" manner.  This can be
   * used to prevent passwords from being logged.
   */
  bool dontLogNextCall;

  // Disable copying.
#ifndef CXX_11
  JsonRpc ();
  JsonRpc (const JsonRpc&);
  JsonRpc& operator= (const JsonRpc&);
#endif /* !CXX_11  */

  /**
   * Perform a HTTP query with JSON data.  However, this routine does not
   * know/care about JSON, it just sends the raw string and returns the
   * response body as string or throws if an error or an unaccepted
   * HTTP response code is detected.
   * @param query Query string to send.
   * @param responseCode Set to the HTTP response code.
   * @return The response body.
   * @throws Exception if some error occurs.
   */
  std::string queryHttp (const std::string& query, unsigned& responseCode);

  /**
   * If logging of RPC calls is enabled (environment variable
   * LIBNMCRPC_LOGFILE_RPCCALLS), write the given string to the
   * log file.
   * @param str String to log.
   */
  void logRpcCall (const std::string& str);

public:

  /**
   * Construct for the given connection data.
   * @param s Settings to use for the connection.  They are copied.
   */
  explicit inline JsonRpc (const RpcSettings& s)
    : settings(s), nextId(0), dontLogNextCall(false)
  {
    // Nothing more to be done.
  }

  // We want no default constructor or copying.
#ifdef CXX_11
  JsonRpc () = delete;
  JsonRpc (const JsonRpc&) = delete;
  JsonRpc& operator= (const JsonRpc&) = delete;
#endif /* CXX_11?  */

  /**
   * Decode JSON from a string.
   * @param str JSON string.
   * @returns The parsed JSON data.
   * @throws JsonParseError in case of parsing errors.
   */
  static JsonData decodeJson (const std::string& str);

  /**
   * Decose JSON from an input stream.
   * @param in Input stream.
   * @returns the parsed JSON data.
   * @throws JsonParseError inc ase of parsing errors.
   */
  static JsonData readJson (std::istream& in);

  /**
   * Encode JSON to a string.
   * @param data The JSON data.
   * @return The encoded JSON as string.
   */
  static std::string encodeJson (const JsonData& data);

  /**
   * Disable logging for the next call.  This can be used to prevent passwords
   * from being logged.
   */
  inline void
  disableLoggingOneShot ()
  {
    dontLogNextCall = true;
  }

  /**
   * Perform a JSON-RPC query with arbitrary parameter list.
   * @param method The method name to call.
   * @param params Parameter list as single Json::Value containing an array.
   * @return Result of the query.
   * @throws Exception in case of error.
   * @throws RpcError if the RPC call returns an error.
   */
  JsonData executeRpcArray (const std::string& method, const JsonData& params);

  /**
   * Perform a JSON-RPC query with arbitrary parameter list.
   * @param method The method name to call.
   * @param params Iterable list of parameters to pass.
   * @return Result of the query.
   * @throws Exception in case of error.
   * @throws RpcError if the RPC call returns an error.
   */
  template<typename L>
    JsonData executeRpcList (const std::string& method, const L& params);

  /* Utility methods to call RPC methods with small number of parameters.  */

  inline JsonData
  executeRpc (const std::string& method)
  {
    std::vector<JsonData> params;
    return executeRpcList (method, params);
  }

  template<typename T>
    inline JsonData
    executeRpc (const std::string& method, const T& p1)
  {
    std::vector<JsonData> params;
    params.push_back (JsonData(p1));
    return executeRpcList (method, params);
  }

  template<typename S, typename T>
    inline JsonData
    executeRpc (const std::string& method, const S& p1, const T& p2)
  {
    std::vector<JsonData> params;
    params.push_back (JsonData(p1));
    params.push_back (JsonData(p2));
    return executeRpcList (method, params);
  }

  template<typename R, typename S, typename T>
    inline JsonData
    executeRpc (const std::string& method,
                const R& p1, const S& p2, const T& p3)
  {
    std::vector<JsonData> params;
    params.push_back (JsonData(p1));
    params.push_back (JsonData(p2));
    params.push_back (JsonData(p3));
    return executeRpcList (method, params);
  }

};

/* ************************************************************************** */
/* Exception classes.  */

/**
 * Unspecified exception during RPC call.
 */
class JsonRpc::Exception : public std::runtime_error
{

private:

  // Disable default constructor.
#ifndef CXX_11
  Exception ();
#endif /* !CXX_11  */

public:

  /**
   * Construct it just with an error message.
   * @param msg Error message.
   */
  explicit inline Exception (const std::string& msg)
    : std::runtime_error(msg)
  {
    // Nothing else to do.
  }

  /* No default constructor, but copying ok.  */
#ifdef CXX_11
  Exception () = delete;
  Exception (const Exception&) = default;
  Exception& operator= (const Exception&) = default;
#endif /* CXX_11?  */

};

/**
 * Error parsing JSON.
 */
class JsonRpc::JsonParseError : public JsonRpc::Exception
{

private:

  // Disable default constructor.
#ifndef CXX_11
  JsonParseError ();
#endif /* !CXX_11  */

public:

  /**
   * Construct it just with an error message.
   * @param msg Error message.
   */
  explicit inline JsonParseError (const std::string& msg)
    : Exception(msg)
  {
    // Nothing else to do.
  }

  /* No default constructor, but copying ok.  */
#ifdef CXX_11
  JsonParseError () = delete;
  JsonParseError (const JsonParseError&) = default;
  JsonParseError& operator= (const JsonParseError&) = default;
#endif /* CXX_11?  */

};

/**
 * HTTP connection error (returned with unaccepted HTTP response code).
 */
class JsonRpc::HttpError : public JsonRpc::Exception
{

private:

  /** The response code.  */
  unsigned code;

  // Disable copy constructor.
#ifndef CXX_11
  HttpError ();
#endif /* !CXX_11  */

public:

  /**
   * Construct with the given HTTP code and a default message.
   * @param msg Error message.
   * @param c HTTP error code.
   */
  inline HttpError (const std::string& msg, unsigned c)
    : Exception(msg), code(c)
  {
    // Nothing else to do.
  }

  /* Default copying, no default constructor.  */
#ifdef CXX_11
  HttpError () = delete;
  HttpError (const HttpError&) = default;
  HttpError& operator= (const HttpError&) = default;
#endif /* CXX_11?  */

  /**
   * Query HTTP response code.
   * @return The HTTP response code.
   */
  inline unsigned
  getResponseCode () const
  {
    return code;
  }

};

/**
 * Error returned by the RPC method call.
 */
class JsonRpc::RpcError : public JsonRpc::Exception
{

private:

  /** Error code.  */
  int code;

  /** Error message (from RPC).  */
  std::string message;

  // Disable default constructor.
#ifndef CXX_11
  RpcError ();
#endif /* !CXX_11  */

public:

  /**
   * Construct it given the JSON representation returned.
   * @param data JSON data returned by RPC.
   */
  explicit inline
  RpcError (const JsonData& data)
    : Exception("RPC returned an error response."),
      code(data["code"].asInt ()), message(data["message"].asString ())
  {
    // Nothing else to do.
  }

  /* No default constructor but copying allowed.  */
#ifdef CXX_11
  RpcError () = delete;
  RpcError (const RpcError&) = default;
  RpcError& operator= (const RpcError&) = default;
#endif /* CXX_11?  */

  // Set explicit throw() for destructor in C++03 mode.
#ifndef CXX_11
  inline ~RpcError () throw ()
  {}
#endif /* !CXX_11  */

  /**
   * Get the error code.
   * @return Error code.
   */
  inline int
  getErrorCode () const
  {
    return code;
  }

  /**
   * Get the error message.
   * @return The error message.
   */
  inline const std::string&
  getErrorMessage () const
  {
    return message;
  }

};

/* ************************************************************************** */

/* Include template implementations.  */
#include "JsonRpc.tpp"

} // namespace nmcrpc

#endif /* Header guard.  */
