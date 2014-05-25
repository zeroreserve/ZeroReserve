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

/* Source code for NamecoinInterface.hpp.  */

#include "NamecoinInterface.hpp"

#include <ctime>
#include <sstream>

namespace nmcrpc
{

/* ************************************************************************** */
/* High-level interface to Namecoin.  */

const unsigned NamecoinInterface::UNLOCK_SECONDS = 3600;

/**
 * Run a simple test command and return whether the connection seems to
 * be up and running.  It also produces a message that can be shown, which
 * either includes the running version or an error string.
 * @param msg Set this to the message string.
 * @return True iff the connection seems to be fine.
 */
bool
NamecoinInterface::testConnection (std::string& msg)
{
  try
    {
      const JsonRpc::JsonData res = rpc.executeRpc ("getinfo");
      int version = res["version"].asInt ();

      unsigned v3 = version % 100;
      version /= 100;
      unsigned v2 = version % 100;
      version /= 100;

      std::ostringstream msgOut;
      msgOut << "Success!  Namecoind version "
             << "0." << version << "." << v2;
      if (v3 > 0)
        msgOut << "." << v3;
      msgOut << " running.";
      msg = msgOut.str ();

      return true;
    }
  catch (const JsonRpc::HttpError& exc)
    {
      std::ostringstream res;
      res << "HTTP-Error (" << exc.getResponseCode () << "): "
          << exc.what ();
      msg = res.str ();
    }
  catch (const JsonRpc::Exception& exc)
    {
      msg = exc.what ();
    }

  return false;
}

/**
 * Query for an address by string.  This immediately checks whether the
 * address is valid and owned by the user, so that this information can be
 * encapsulated into the returned object.
 * @param addr The address to check.
 * @return The created address object.
 */
NamecoinInterface::Address
NamecoinInterface::queryAddress (const std::string& addr)
{
  return Address (rpc, addr);
}

/**
 * Create a new address (as per "getnewaddress") and return it.
 * @return Newly created address.
 */
NamecoinInterface::Address
NamecoinInterface::createAddress ()
{
  const JsonRpc::JsonData addr = rpc.executeRpc ("getnewaddress");
  return Address (rpc, addr.asString ());
}

/**
 * Query for a name by string.  If the name is registered, this immediately
 * queries for the name's associated data.  If the name does not yet exist,
 * this still succeeds and returns a Name object that can be used to find
 * out that fact as well as register the name.
 * @param name The name to check.
 * @return The created name object.
 */
NamecoinInterface::Name
NamecoinInterface::queryName (const std::string& name)
{
  return Name (name, *this, rpc);
}

/**
 * Query for a name by namespace and name.
 * @see queryName (const std::string&)
 * @param ns The namespace.
 * @param name The (namespace-less) name.
 * @return The created name object.
 */
NamecoinInterface::Name
NamecoinInterface::queryName (const std::string& ns, const std::string& name)
{
  std::ostringstream full;
  full << ns << "/" << name;
  return queryName (full.str ());
}

/**
 * Query for the number of confirmations a transaction has.
 * @param txid The transaction id to check for.
 * @return Number of confirmations the transaction has.
 * @throws JsonRpc::RpcError if the tx is not found.
 */
unsigned
NamecoinInterface::getNumberOfConfirmations (const std::string& txid)
{
  const JsonRpc::JsonData res = rpc.executeRpc ("gettransaction", txid);
  assert (res.isObject ());

  return res["confirmations"].asInt ();
}

/**
 * Check whether the wallet needs to be unlocked or not.  This routine is
 * used to decide whether we need to ask for a passphrase or not before
 * using WalletUnlocker.
 * @return True iff we need a passphrase.
 */
bool
NamecoinInterface::needWalletPassphrase ()
{
  const JsonRpc::JsonData res = rpc.executeRpc ("getinfo");

  if (res["unlocked_until"].isNull ())
    return false;

  const int until = res["unlocked_until"].asInt ();
  return (until < std::time (nullptr) + UNLOCK_SECONDS);
}

/* ************************************************************************** */
/* Address object.  */

/**
 * Construct the address.  This is meant to be used only
 * from inside NamecoinInterface.  Outside users should use
 * NamecoinInterface::queryAddress or other methods to obtain
 * address objects.
 * @param r The namecoin interface to use.
 * @param a The address as string.
 */
NamecoinInterface::Address::Address (JsonRpc& r, const std::string& a)
  : rpc(&r), addr(a)
{
  const JsonRpc::JsonData res = rpc->executeRpc ("validateaddress", addr);
  valid = res["isvalid"].asBool ();
  mine = false;
  if (valid)
    mine = res["ismine"].asBool ();
}

/**
 * Check a message signature against this address.  If this address
 * is invalid, false is returned.
 * @param msg The message that should be signed.
 * @param sig The message's signature.
 * @return True iff the signature matches the message.
 */
bool
NamecoinInterface::Address::verifySignature (const std::string& msg,
                                             const std::string& sig) const
{
  if (!valid)
    return false;

  try
    {
      JsonRpc::JsonData res = rpc->executeRpc ("verifymessage", addr, sig, msg);
      return (res.isBool () && res.asBool ());
    }
  catch (const JsonRpc::RpcError& exc)
    {
      // Malformed base64?
      if (exc.getErrorCode () == -5)
        return false;
      throw exc;
    }
}

/**
 * Sign a message with this address.  This may throw if the address
 * is invalid, the wallet locked or the private key is not owned.
 * @param msg The message that should be signed.
 * @return The message's signature.
 * @throws NoPrivateKey if this address is not owned.
 * @throws std::runtime_error if the address is invalid or the wallet locked.
 */
std::string
NamecoinInterface::Address::signMessage (const std::string& msg) const
{
  if (!valid)
    throw std::runtime_error ("Can't sign with invalid address.");

  try
    {
      const JsonRpc::JsonData res = rpc->executeRpc ("signmessage", addr, msg);
      return res.asString ();
    }
  catch (const JsonRpc::RpcError& exc)
    {
      switch (exc.getErrorCode ())
        {
        case -13:
          throw std::runtime_error ("Need to unlock the wallet first.");

        case -3:
          {
            std::ostringstream msg;
            msg << "You don't have the private key of " << addr << " in order"
                << " to sign messages with that address.";
            throw NoPrivateKey (msg.str ());
          }

        default:
          throw exc;
        }
    }
}

/* ************************************************************************** */
/* Name object.  */

/**
 * Construct the name.  This is meant to be used only
 * from inside NamecoinInterface.  Outside users should use
 * NamecoinInterface::queryName or other methods to obtain
 * name objects.
 * @param n The name's string.
 * @param nc NamecoinInterface object.
 * @param rpc The RPC object to use for finding info about the name.
 */
NamecoinInterface::Name::Name (const std::string& n, NamecoinInterface& nc,
                               JsonRpc& rpc)
  : initialised(true), name(n)
{
  try
    {
      data = rpc.executeRpc ("name_show", n);
      ex = true;
      addr = nc.queryAddress (data["address"].asString ());
    }
  catch (const JsonRpc::RpcError& exc)
    {
      if (exc.getErrorCode () == -4)
        {
          ex = false;
          return;
        }
      throw exc;
    }
}

/**
 * Ensure that this object has status "exists".
 * @throws NameNotFound if the name doesn't yet exist.
 */
void
NamecoinInterface::Name::ensureExists () const
{
  if (!ex)
    throw NameNotFound (name);
}

/**
 * Utility routine to split a name into namespace and trimmed parts.
 * @param name The full name.
 * @param ns Set to namespace part.
 * @param trimmed Set to trimmed part.
 * @return True if splitting was successful, false if there's no namespace.
 */
bool
NamecoinInterface::Name::split (const std::string& name,
                                std::string& ns, std::string& trimmed)
{
  const std::string::size_type pos = name.find ('/');
  if (pos == std::string::npos)
    return false;

  ns = name.substr (0, pos);
  trimmed = name.substr (pos + 1);

  return true;
}

/* ************************************************************************** */
/* Wallet unlocker.  */

/**
 * Construct it, not yet unlocking.
 * @param n The NamecoinInterface to use.
 */
NamecoinInterface::WalletUnlocker::WalletUnlocker (NamecoinInterface& n)
  : rpc(n.rpc), nc(n), unlocked(false)
{
  // Nothing else to do.
}

/**
 * Lock the wallet on destruct.
 */
NamecoinInterface::WalletUnlocker::~WalletUnlocker ()
{
  if (unlocked)
    rpc.executeRpc ("walletlock");
}

/**
 * Perform the unlock (if necessary).  The passphrase must be correct if the
 * wallet is actually locked, and can be anything else.
 * @param passphrase Passphrase to use for unlocking.
 * @throws UnlockFailure if the passphrase is wrong.
 */
void
NamecoinInterface::WalletUnlocker::unlock (const std::string& passphrase)
{
  if (unlocked)
    throw std::runtime_error ("Wallet is already unlocked!");
  
  const bool needPwd = nc.needWalletPassphrase ();
  if (needPwd)
    {
      /* Empty password is a special case, because it is not handled correctly
         by the walletpassphrase RPC call.  Thus catch this case right now
         and fail as if the passphrase would have been wrong.  */
      if (passphrase.empty ())
        throw UnlockFailure ("Wallet passphrase cannot be empty.");

      /* Ensure the wallet is indeed locked before we send the passphrase.
         It could be the case that it is unlocked although for too short
         a time, then lock it now.  */
      rpc.executeRpc ("walletlock");

      try
        {
          rpc.disableLoggingOneShot ();
          rpc.executeRpc ("walletpassphrase", passphrase, UNLOCK_SECONDS);
          unlocked = true;
        }
      catch (const JsonRpc::RpcError& exc)
        {
          if (exc.getErrorCode () == -14)
            throw UnlockFailure ("Wrong wallet passphrase.");
          throw exc;
        }
    }
}

} // namespace nmcrpc
