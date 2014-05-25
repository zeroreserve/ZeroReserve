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

/* Source code for JsonRpc.hpp.  */

#include "JsonRpc.hpp"

#include <json/reader.h>
#include <json/writer.h>

#include <curl/curl.h>

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <sstream>

namespace nmcrpc
{

/* ************************************************************************** */
/* Internal cURL wrapper.  */

/**
 * This class is a quick utility wrapper around cURL to perform the POST
 * HTTP requests we need.
 */
class CurlPost
{

private:

  /** The CURL handle.  */
  CURL* handle;

  /** List of headers to send.  */
  struct curl_slist* headers;

  /** Data to be posted.  */
  std::string data;
  
  /** Store response body here.  */
  std::string response;

  // Disable copying.
#ifndef CXX_11
  CurlPost (const CurlPost& o);
  CurlPost& operator= (const CurlPost& o);
#endif /* !CXX_11  */

  /**
   * Write function for cURL.
   * @param buf Buffer containing data.
   * @param size Buffer size in elements.
   * @param nmemb Size of an element in bytes.
   * @param userdata User data, which is a pointer to the object in this case.
   * @return Number of processed bytes.
   */
  static size_t writeHandler (char* buf, size_t size, size_t nmemb,
                              void* userdata);

public:

  /**
   * Construct it, which will not yet intialise the handle.
   */
  inline CurlPost ()
    : handle(nullptr), headers(nullptr), data("")
  {
    // Nothing else to do.
  }

  // No copying.
#ifdef CXX_11
  CurlPost (const CurlPost& o) = delete;
  CurlPost& operator= (const CurlPost& o) = delete;
#endif /* CXX_11?  */

  /**
   * Destory, which cleans up the cURL connection.
   */
  ~CurlPost ();

  /**
   * Initialise the cURL handle.
   * @throws JsonRpc::Exception in case this fails.
   */
  void init ();

  /**
   * Add an HTTP header to be posted.
   * @param header The header's name.
   * @param value The header's value.
   */
  void addHeader (const std::string& header, const std::string& value);

  /**
   * Set the data to post.
   * @param d The data to be posted.
   */
  inline void
  setData (const std::string& d)
  {
    data = d;
  }

  /**
   * Perform the request and return the response body text.
   * @param host The host to connect to.
   * @param port The port to connect to.
   * @param user The username to use.
   * @param password The password to use.
   */
  void perform (const std::string& host, unsigned port,
                const std::string& user, const std::string& password);

  /**
   * Return the response body text after performing the request.
   * @return The response body text.
   */
  const std::string& getResponseBody () const
  {
    return response;
  }

  /**
   * Get the response HTTP code.
   * @return The response HTTP code.
   */
  unsigned getResponseCode () const;

};

CurlPost::~CurlPost ()
{
  if (headers)
    curl_slist_free_all (headers);
  if (handle)
    curl_easy_cleanup (handle);
}

size_t
CurlPost::writeHandler (char* buf, size_t size, size_t nmemb,
                        void* userdata)
{
  CurlPost& me = *reinterpret_cast<CurlPost*> (userdata);
  const size_t realSize = size * nmemb;

  me.response.append (buf, realSize);
  return realSize;
}

void
CurlPost::init ()
{
  assert (!handle);
  handle = curl_easy_init ();
  if (!handle)
    throw JsonRpc::Exception ("Initialisation of cURL failed.");
}
  
void
CurlPost::addHeader (const std::string& header, const std::string& value)
{
  assert (handle);

  std::ostringstream out;
  out << header << ": " << value;
  headers = curl_slist_append (headers, out.str ().c_str ());
}

void
CurlPost::perform (const std::string& host, unsigned port,
                   const std::string& user, const std::string& password)
{
  assert (handle);

  curl_easy_setopt (handle, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt (handle, CURLOPT_POST, 1);
  curl_easy_setopt (handle, CURLOPT_POSTFIELDS, data.c_str ());
  curl_easy_setopt (handle, CURLOPT_USERAGENT, "libnmcrpc");
  curl_easy_setopt (handle, CURLOPT_HTTPAUTH, CURLAUTH_ANY);

  std::ostringstream url;
  url << "http://" << user << ":" << password << "@" << host << ":" << port;
  curl_easy_setopt (handle, CURLOPT_URL, url.str ().c_str ());

  curl_easy_setopt (handle, CURLOPT_WRITEFUNCTION, &writeHandler);
  curl_easy_setopt (handle, CURLOPT_WRITEDATA, this);

  const CURLcode res = curl_easy_perform (handle);
  if (res != CURLE_OK)
    {
      std::ostringstream msg;
      msg << "Error in cURL: " << curl_easy_strerror (res);
      throw JsonRpc::Exception (msg.str ());
    }
}

unsigned
CurlPost::getResponseCode () const
{
  assert (handle);

  long res;
  curl_easy_getinfo (handle, CURLINFO_RESPONSE_CODE, &res);

  return res;
}

/* ************************************************************************** */
/* The JsonRpc class itself.  */

/** Environment variable name for controlling the call log file.  */
const std::string JsonRpc::LOGFILE_VAR = "LIBNMCRPC_LOGFILE_RPCCALLS";

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
std::string
JsonRpc::queryHttp (const std::string& query, unsigned& responseCode)
{
  CurlPost poster;
  poster.init ();

  poster.setData (query);
  poster.addHeader ("Content-Type", "application/json");
  poster.addHeader ("Accept", "application/json");

  poster.perform (settings.getHost (), settings.getPort (),
                  settings.getUsername (), settings.getPassword ());

  responseCode = poster.getResponseCode ();
  return poster.getResponseBody ();
}

/**
 * If logging of RPC calls is enabled (environment variable
 * LIBNMCRPC_LOGFILE_RPCCALLS), write the given string to the
 * log file.
 * @param str String to log.
 */
void
JsonRpc::logRpcCall (const std::string& str)
{
  const char* file = std::getenv (LOGFILE_VAR.c_str ());
  if (!file)
    return;

  std::ofstream out(file, std::ios::app);
  out << str;
  out.close ();
}

/**
 * Decode JSON from a string.
 * @param str JSON string.
 * @returns The parsed JSON data.
 * @throws JsonParseError in case of parsing errors.
 */
JsonRpc::JsonData
JsonRpc::decodeJson (const std::string& str)
{
  std::istringstream in(str);
  return readJson (in);
}

/**
 * Decose JSON from an input stream.
 * @param in Input stream.
 * @returns the parsed JSON data.
 * @throws JsonParseError inc ase of parsing errors.
 */
JsonRpc::JsonData
JsonRpc::readJson (std::istream& in)
{
  Json::Reader parser;
  Json::Value root;

  const bool success = parser.parse (in, root);
  if (!success)
    throw JsonParseError ("Error decoding the JSON value.");

  return root;
}

/**
 * Encode JSON to a string.
 * @param data The JSON data.
 * @return The encoded JSON as string.
 */
std::string
JsonRpc::encodeJson (const JsonData& data)
{
  Json::FastWriter writer;
  return writer.write (data);
}

/**
 * Perform a JSON-RPC query with arbitrary parameter list.
 * @param method The method name to call.
 * @param params Parameter list as single Json::Value containing an array.
 * @return Result of the query.
 * @throws Exception in case of error.
 * @throws RpcError if the RPC call returns an error.
 */
JsonRpc::JsonData
JsonRpc::executeRpcArray (const std::string& method, const JsonData& params)
{
  const bool logging = !dontLogNextCall;
  dontLogNextCall = false;

  JsonData query(Json::objectValue);
  const int id = nextId++;

  query["id"] = id;
  query["method"] = method;
  query["params"] = params;
  const std::string queryStr = encodeJson (query);

  if (logging)
    {
      std::ostringstream msg;
      msg << "namecoind " << method;
      for (Json::Value::const_iterator i = params.begin ();
           i != params.end (); ++i)
        msg << " " << encodeJson (*i);
      msg << std::endl;
      logRpcCall (msg.str ());
    }
  else
    logRpcCall ("<logging disabled for one RPC call>\n\n");

  unsigned respCode;
  const std::string responseStr = queryHttp (queryStr, respCode);

  if (logging)
    {
      std::ostringstream msg;
      msg << "  -> " << responseStr << std::endl;
      logRpcCall (msg.str ());
    }

  switch (respCode)
    {
    case 200:
    case 404:
    case 500:
      break;

    case 401:
      throw HttpError ("Login credentials not accepted.", respCode);

    default:
      throw HttpError ("Invalid HTTP status code returned.", respCode);
    }

  const JsonData response = decodeJson (responseStr);
  if (response["id"].asInt () != id)
    throw Exception ("IDs don't match for JSON-RPC response.");

  const JsonData& result = response["result"];
  const JsonData& error = response["error"];
  if (!error.isNull ())
    throw RpcError (error);
  return result;
}

} // namespace nmcrpc
