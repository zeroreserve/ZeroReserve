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

/* Template implementation for JsonRpc.hpp.  */

/* ************************************************************************** */
/* The JsonRpc class itself.  */

/**
 * Perform a JSON-RPC query with arbitrary parameter list.
 * @param method The method name to call.
 * @param params Iterable list of parameters to pass.
 * @return Result of the query.
 * @throws Exception in case of error.
 * @throws RpcError if the RPC call returns an error.
 */
template<typename L>
  JsonRpc::JsonData
  JsonRpc::executeRpcList (const std::string& method, const L& params)
{
  JsonData arr(Json::arrayValue);
#ifdef CXX_11
  for (const auto& elem : params)
    arr.append (JsonData (elem));
#else /* CXX_11?  */
  for (typename L::const_iterator i = params.begin (); i != params.end (); ++i)
    arr.append (JsonData (*i));
#endif /* CXX_11?  */

  return executeRpcArray (method, arr);
}
