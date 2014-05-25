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

/* Template implementation for NamecoinInterface.hpp.  */

/* ************************************************************************** */
/* High-level interface to Namecoin.  */

/**
 * Query for all user-owned names in the wallet (according to name_list but
 * filtering out names that have been sent away) and execute some call-back
 * on them.
 * @param cb Call-back routine.
 */
template<typename T>
  void
  NamecoinInterface::forMyNames (T cb)
{
  const JsonRpc::JsonData res = rpc.executeRpc ("name_list");
  assert (res.isArray ());
#ifdef CXX_11
  for (const JsonRpc::JsonData& val : res)
#else /* CXX_11?  */
  for (JsonRpc::JsonData::const_iterator i = res.begin (); i != res.end (); ++i)
#endif /* CXX_11?  */
    {
#ifndef CXX_11
      const JsonRpc::JsonData& val = *i;
#endif /* !CXX_11?  */
      NamecoinInterface::Name nm = queryName (val["name"].asString ());
      if (nm.getAddress ().isMine ())
        cb (nm);
    }
}

/**
 * Query for all names in the index (according to name_scan) and execute
 * some call-back on them.
 * @param cb Call-back routine.
 */
template<typename T>
  void
  NamecoinInterface::forAllNames (T cb)
{
  static const unsigned CNT = 5000;

  std::string last;
  bool haveLast = false;
  while (true)
    {
      JsonRpc::JsonData res;
      Json::ArrayIndex firstInd;
      if (haveLast)
        {
          res = rpc.executeRpc ("name_scan", last, CNT);
          assert (res.isArray () && res.size () > 0);
          assert (res[0].isObject ());
          assert (res[0u]["name"].asString () == last);

          /* FIXME: There are some unicode names in the blockchain, for which
             the above assertion fails if they are hit as last names in a scan.
             As a work-around, choosing CNT large enough helps for now, but
             of course this is only a very crude fix!  */

          firstInd = 1;
        }
      else
        {
          res = rpc.executeRpc ("name_scan");
          assert (res.isArray ());
          firstInd = 0;
        }

      assert (res.size () >= firstInd);
      if (firstInd == res.size ())
        break;

      for (Json::ArrayIndex i = firstInd; i < res.size (); ++i)
        {
          assert (res[i].isObject ());
          const std::string name = res[i]["name"].asString ();

          /* FIXME: Update call-back interface.  */
          cb (name);
        }

      last = res[res.size () - 1]["name"].asString ();
      haveLast = true;
    }
}
