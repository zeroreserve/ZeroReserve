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

#ifndef NMCRPC_NAMECOININTERFACE_HPP
#define NMCRPC_NAMECOININTERFACE_HPP

#include "JsonRpc.hpp"

#include <cassert>
#include <stdexcept>
#include <string>

namespace nmcrpc
{

/* ************************************************************************** */
/* High-level interface to Namecoin.  */

/**
 * Allow high-level interfacing to Namecoin via an underlying RPC connection.
 */
class NamecoinInterface
{

public:

  /* Exceptions.  */
  class NameNotFound;
  class NoPrivateKey;
  class UnlockFailure;

  /* Other child classes.  */
  class Address;
  class Name;
  class WalletUnlocker;

private:

  /** Underlying RPC connection.  */
  JsonRpc& rpc;

  /**
   * The number of seconds we want to temporarily unlock the wallet in case
   * we need access to a private key.
   */
  static const unsigned UNLOCK_SECONDS;

  // Disable copying and default constructor.
#ifndef CXX_11
  NamecoinInterface ();
  NamecoinInterface (const NamecoinInterface&);
  NamecoinInterface& operator= (const NamecoinInterface&);
#endif /* !CXX_11  */

public:

  /**
   * Construct it with the given RPC connection.
   * @param r The RPC connection.
   */
  explicit inline NamecoinInterface (JsonRpc& r)
    : rpc(r)
  {
    // Nothing more to be done.
  }

  // We want no default constructor or copying.
#ifdef CXX_11
  NamecoinInterface () = delete;
  NamecoinInterface (const NamecoinInterface&) = delete;
  NamecoinInterface& operator= (const NamecoinInterface&) = delete;
#endif /* CXX_11?  */

  /**
   * Run a simple test command and return whether the connection seems to
   * be up and running.  It also produces a message that can be shown, which
   * either includes the running version or an error string.
   * @param msg Set this to the message string.
   * @return True iff the connection seems to be fine.
   */
  bool testConnection (std::string& msg);

  /**
   * Run test command and discard message string.
   * @return True iff the connection seems to be fine.
   */
  inline bool
  testConnection ()
  {
    std::string msg;
    return testConnection (msg);
  }

  /**
   * Query for an address by string.  This immediately checks whether the
   * address is valid and owned by the user, so that this information can be
   * encapsulated into the returned object.
   * @param addr The address to check.
   * @return The created address object.
   */
  Address queryAddress (const std::string& addr);

  /**
   * Create a new address (as per "getnewaddress") and return it.
   * @return Newly created address.
   */
  Address createAddress ();

  /**
   * Query for a name by string.  If the name is registered, this immediately
   * queries for the name's associated data.  If the name does not yet exist,
   * this still succeeds and returns a Name object that can be used to find
   * out that fact as well as register the name.
   * @param name The name to check.
   * @return The created name object.
   */
  Name queryName (const std::string& name);

  /**
   * Query for a name by namespace and name.
   * @see queryName (const std::string&)
   * @param ns The namespace.
   * @param name The (namespace-less) name.
   * @return The created name object.
   */
  Name queryName (const std::string& ns, const std::string& name);

  /**
   * Query for the number of confirmations a transaction has.
   * @param txid The transaction id to check for.
   * @return Number of confirmations the transaction has.
   * @throws JsonRpc::RpcError if the tx is not found.
   */
  unsigned getNumberOfConfirmations (const std::string& txid);

  /**
   * Query for all user-owned names in the wallet (according to name_list but
   * filtering out names that have been sent away) and execute some call-back
   * on them.
   * @param cb Call-back routine.
   */
  template<typename T>
    void forMyNames (T cb);

  /**
   * Query for all names in the index (according to name_scan) and execute
   * some call-back on them.
   * @param cb Call-back routine.
   */
  template<typename T>
    void forAllNames (T cb);

  /**
   * Check whether the wallet needs to be unlocked or not.  This routine is
   * used to decide whether we need to ask for a passphrase or not before
   * using WalletUnlocker.
   * @return True iff we need a passphrase.
   */
  bool needWalletPassphrase ();

};

/* ************************************************************************** */
/* Address object.  */

/**
 * Encapsulate a Namecoin address.
 */
class NamecoinInterface::Address
{

private:

  friend class NamecoinInterface;

  /** The namecoin interface to be used for RPC calls (message signing).  */
  JsonRpc* rpc;

  /** The address as string.  */
  std::string addr;

  /** Whether the address is valid.  */
  bool valid;
  /** Whether the address is owned by the user.  */
  bool mine;

  /**
   * Construct the address.  This is meant to be used only
   * from inside NamecoinInterface.  Outside users should use
   * NamecoinInterface::queryAddress or other methods to obtain
   * address objects.
   * @param r The namecoin interface to use.
   * @param a The address as string.
   */
  Address (JsonRpc& r, const std::string& a);

public:

  /**
   * Default constructor, initialises to "".  This is here for convenience
   * so that variables of type Address can be declared.
   */
  inline Address ()
    : rpc(nullptr), addr(""), valid(false), mine(false)
  {
    // Nothing more to do.
  }

  // Copying is ok.
#ifdef CXX_11
  Address (const Address&) = default;
  Address& operator= (const Address&) = default;
#endif /* CXX_11?  */

  /**
   * Get the address as string.
   * @return The address as string.
   */
  inline const std::string&
  getAddress () const
  {
    return addr;
  }

  /**
   * Get whether or not the address is valid.
   * @return True iff this is a valid address.
   */
  inline bool
  isValid () const
  {
    return valid;
  }

  /**
   * Get whether or not the address is owned by the user.
   * @return True iff the user has the address' private key.
   */
  inline bool
  isMine () const
  {
    return mine;
  }

  /**
   * Check a message signature against this address.  If this address
   * is invalid, false is returned.
   * @param msg The message that should be signed.
   * @param sig The message's signature.
   * @return True iff the signature matches the message.
   */
  bool verifySignature (const std::string& msg, const std::string& sig) const;

  /**
   * Sign a message with this address.  This may throw if the address
   * is invalid, the wallet locked or the private key is not owned.
   * @param msg The message that should be signed.
   * @return The message's signature.
   * @throws NoPrivateKey if this address is not owned.
   * @throws std::runtime_error if the address is invalid or the wallet locked.
   */
  std::string signMessage (const std::string& msg) const;

};

/* ************************************************************************** */
/* Name object.  */

/**
 * Encapsulate a Namecoin name.
 */
class NamecoinInterface::Name
{

private:

  friend class NamecoinInterface;

  /**
   * Whether or not this is a default-constructed object.  Those can't be
   * used for anything.
   */
  bool initialised;

  /** The name's string.  */
  std::string name;

  /** Whether or not the name is already registered.  */
  bool ex;

  /** The address holding the name.  */
  Address addr;

  /** The name's JSON data, which name_show returns.  */
  JsonRpc::JsonData data;

  /**
   * Construct the name.  This is meant to be used only
   * from inside NamecoinInterface.  Outside users should use
   * NamecoinInterface::queryName or other methods to obtain
   * name objects.
   * @param n The name's string.
   * @param nc NamecoinInterface object.
   * @param rpc The RPC object to use for finding info about the name.
   */
  Name (const std::string& n, NamecoinInterface& nc, JsonRpc& rpc);

  /**
   * Ensure that this object is initialised and not default-constructed.
   * @throws std::runtime_error if it is default-constructed.
   */
  inline void
  ensureInitialised () const
  {
    if (!initialised)
      throw std::runtime_error ("Name is not yet initialised.");
  }

  /**
   * Ensure that this object has status "exists".
   * @throws NameNotFound if the name doesn't yet exist.
   */
  void ensureExists () const;

public:

  /**
   * Default constructor, marks object as "invalid".  This is here for
   * convenience so that variables of type Name can be declared, but they
   * can't be used for anything until they have been assigned to.
   */
  inline Name ()
    : initialised(false)
  {
    // Nothing more to do.
  }

  // Copying is ok.
#ifdef CXX_11
  Name (const Name&) = default;
  Name& operator= (const Name&) = default;
#endif /* CXX_11?  */

  /**
   * Get the name as string.
   * @return The name as string.
   */
  inline const std::string&
  getName () const
  {
    ensureInitialised ();
    return name;
  }

  /**
   * Get the address holding the name.
   * @return The name's address.
   * @throws NameNotFound if the name doesn't yet exist.
   */
  inline const Address&
  getAddress () const
  {
    ensureExists ();
    return addr;
  }

  /**
   * Get whether or not the name exists.
   * @return True iff this name already exists.
   */
  inline bool
  exists () const
  {
    ensureInitialised ();
    return ex;
  }

  /**
   * Get the name's full JSON info as per name_show.
   * @return This name's full JSON info.
   * @throws NameNotFound if the name doesn't yet exist.
   */
  inline const JsonRpc::JsonData&
  getFullData () const
  {
    ensureExists ();
    return data;
  }

  /**
   * Get the name's value as string.
   * @return This name's value as string.
   * @throws NameNotFound if the name doesn't yet exist.
   */
  inline const std::string
  getStringValue () const
  {
    ensureExists ();
    return data["value"].asString ();
  }

  /**
   * Get the name's value as JSON object.
   * @return This name's value as JSON object.
   * @throws NameNotFound if the name doesn't yet exist.
   * @throws JsonRpc::JsonParseError if JSON parsing fails.
   */
  inline JsonRpc::JsonData
  getJsonValue () const
  {
    return JsonRpc::decodeJson (getStringValue ());
  }

  /**
   * Return whether the name is expired.
   * @return True iff this name exists but is expired.
   * @throws NameNotFound if the name doesn't yet exist.
   */
  inline bool
  isExpired () const
  {
    ensureExists ();
    return (data["expired"].isInt () && (data["expired"].asInt () != 0));
  }

  /**
   * Return number of blocks until the name expires.
   * @return The number of blocks until the name expires.  Might be negative.
   * @throws NameNotFound if the name doesn't yet exist.
   */
  inline int
  getExpireCounter () const
  {
    ensureExists ();
    return data["expires_in"].asInt ();
  }

  /**
   * Utility routine to split a name into namespace and trimmed parts.
   * @param name The full name.
   * @param ns Set to namespace part.
   * @param trimmed Set to trimmed part.
   * @return True if splitting was successful, false if there's no namespace.
   */
  static bool split (const std::string& name,
                     std::string& ns, std::string& trimmed);

};

/* ************************************************************************** */
/* Wallet unlocker.  */

/**
 * Unlock the wallet temporarily (during the scope of the object).  RAII
 * ensures that the wallet is locked (if it was indeed unlocked) again
 * at the latest when we go out of scope.
 */
class NamecoinInterface::WalletUnlocker
{

private:

  friend class NamecoinInterface;

  /** The RPC object to use.  */
  JsonRpc& rpc;
  /** High-level interface to use.  */
  NamecoinInterface& nc;

  /** Whether we actually unlocked the wallet.  */
  bool unlocked;

  // Disable copying and default constructor.
#ifndef CXX_11
  WalletUnlocker ();
  WalletUnlocker (const WalletUnlocker&);
  WalletUnlocker& operator= (const WalletUnlocker&);
#endif /* !CXX_11  */

public:

  /**
   * Construct it, not yet unlocking.
   * @param n The NamecoinInterface to use.
   */
  explicit WalletUnlocker (NamecoinInterface& n);

  // No default constructor or copying.
#ifdef CXX_11
  WalletUnlocker () = delete;
  WalletUnlocker (const WalletUnlocker&) = delete;
  WalletUnlocker& operator= (const WalletUnlocker&) = delete;
#endif /* CXX_11?  */

  /**
   * Lock the wallet on destruct.
   */
  ~WalletUnlocker ();

  /**
   * Perform the unlock (if necessary).  The passphrase must be correct if the
   * wallet is actually locked, and can be anything else.
   * @param passphrase Passphrase to use for unlocking.
   * @throws UnlockFailure if the passphrase is wrong.
   */
  void unlock (const std::string& passphrase);

};

/* ************************************************************************** */
/* Exception classes.  */

/**
 * Thrown when a name that is not yet registered is asked for.
 */
class NamecoinInterface::NameNotFound : public std::runtime_error
{

private:

  /** The name that was not found.  */
  std::string name;

  // Disable default constructor.
#ifndef CXX_11
  NameNotFound ();
#endif /* !CXX_11  */

public:

  /**
   * Construct it given the name that is not found.
   * @param n The undefined name.
   */
  explicit inline NameNotFound (const std::string& n)
    : std::runtime_error("Name not found: " + n), name(n)
  {
    // Nothing else to do.
  }

  /* No default constructor, but copying ok.  */
#ifdef CXX_11
  NameNotFound () = delete;
  NameNotFound (const NameNotFound&) = default;
  NameNotFound& operator= (const NameNotFound&) = default;
#endif /* CXX_11?  */

  // Specify throw() explicitly.
#ifndef CXX_11
  inline ~NameNotFound () throw ()
  {}
#endif /* !CXX_11  */

  /**
   * Get the name of this error.
   * @return The name that was not found.
   */
  inline const std::string&
  getName () const
  {
    return name;
  }

};

/**
 * Thrown when an action is to be performed for which the private key
 * is missing from the wallet.
 */
class NamecoinInterface::NoPrivateKey : public std::runtime_error
{

private:

  // Disable default constructor.
#ifndef CXX_11
  NoPrivateKey ();
#endif /* !CXX_11  */

public:

  /**
   * Construct it given the error message.
   * @param msg The error message.
   */
  explicit inline NoPrivateKey (const std::string& msg)
    : std::runtime_error(msg)
  {
    // Nothing else to do.
  }

  /* No default constructor, but copying ok.  */
#ifdef CXX_11
  NoPrivateKey () = delete;
  NoPrivateKey (const NoPrivateKey&) = default;
  NoPrivateKey& operator= (const NoPrivateKey&) = default;
#endif /* CXX_11?  */

};

/**
 * Thrown in case of failure to unlock the wallet (because of a wrong
 * passphrase, presumably).
 */
class NamecoinInterface::UnlockFailure : public std::runtime_error
{

private:

  // Disable default constructor.
#ifndef CXX_11
  UnlockFailure ();
#endif /* !CXX_11  */

public:

  /**
   * Construct it given the error message.
   * @param msg The error message.
   */
  explicit inline UnlockFailure (const std::string& msg)
    : std::runtime_error(msg)
  {
    // Nothing else to do.
  }

  /* No default constructor, but copying ok.  */
#ifdef CXX_11
  UnlockFailure () = delete;
  UnlockFailure (const UnlockFailure&) = default;
  UnlockFailure& operator= (const UnlockFailure&) = default;
#endif /* CXX_11?  */

};

/* ************************************************************************** */

/* Include template implementations.  */
#include "NamecoinInterface.tpp"

} // namespace nmcrpc

#endif /* Header guard.  */
