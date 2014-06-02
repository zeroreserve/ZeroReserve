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

/* Source code for RpcSettings.hpp.  */

#include "RpcSettings.hpp"
#include "JsonRpc.hpp"



#include <cstdlib>
#include <fstream>
#include <sstream>
#include <algorithm>


const static int defaultTestnetPort = 18332;
const static int defaultPort = 8332;

namespace nmcrpc
{

/**
 * Try to read the given input file and update settings when corresponding
 * ones are found there.
 * @param filename The input file's name.
 */

void RpcSettings::readConfig (const std::string& filename)
{
    port = 0;
    std::ifstream in(filename.c_str ());
    if( !in.is_open() ){
        std::string e = std::string( "Cannot open file ") + filename;
        throw JsonRpc::Exception ( e );
    }

    while (in){
        std::string line;
        std::getline (in, line);

        // strip whitespaces
        std::string::iterator end_pos = std::remove( line.begin(), line.end(), ' ' );
        line.erase(end_pos, line.end());

        if( line.empty() || line[0] == '#' ) // ignore comments and empty lines
            continue;

        const std::string::size_type equalPos = line.find ('=');
        if (equalPos != std::string::npos)
        {
            const std::string before(line, 0, equalPos);
            const std::string after(line, equalPos + 1);

            if (before == "rpcport")
            {
                std::istringstream numIn(after);
                numIn >> port;
            }
            else if (before == "rpcuser")
                username = after;
            else if (before == "rpcpassword")
                password = after;
            else if ( before == "testnet" ){
                if( after == "0" ){
                    if( port == 0 ){ // do not override if already set before
                        port = defaultPort;
                    }
                }
                else{
                    if( port == 0 ){ // do not override if already set before
                        port = defaultTestnetPort;
                    }
                }
            }
        }
    }
    if( port == 0 ){
        port = defaultPort;
    }
}

/**
 * Try to read the default namecoin.conf config file and update settings.
 */
void
RpcSettings::readDefaultConfig ()
{
  /* FIXME: Make work also for OSX and Windows!  */

  const char* home = getenv ("HOME");
  if (!home)
    return;

  std::ostringstream filename;
  filename << home << "/.namecoin/namecoin.conf";
  readConfig (filename.str ());
}

} // namespace nmcrpc
