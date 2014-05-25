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

#include "NameRegistration.hpp"

#include <cassert>
#include <memory>
#include <sstream>

namespace nmcrpc
{

/* ************************************************************************** */
/* Handle registration of a name.  */

/**
 * Number of confirmations we want on the name_new transaction before
 * performing name_firstupdate.
 */
const unsigned NameRegistration::FIRSTUPDATE_DELAY = 12;

/**
 * Start registration of a name with issuing the corresponding name_new
 * transaction.
 * @param nm The name to register, as Name object.
 * @throws std::runtime_error if we're not in NOT_STARTED state.
 * @throws NameAlreadyReserved if the name already exists.
 */
void
NameRegistration::registerName (const NamecoinInterface::Name& nm)
{
  if (state != NOT_STARTED)
    throw std::runtime_error ("Can registerName() only in NOT_STARTED state.");
  if (nm.exists () && !nm.isExpired ())
    throw NameAlreadyReserved (nm.getName ());

  name = nm.getName ();
  const JsonRpc::JsonData res = rpc.executeRpc ("name_new", name);
  assert (res.isArray () && res.size () == 2);
  rand = res[1].asString ();
  tx = res[0].asString ();

  /* Set default value, can be changed now.  */
  value = "";

  /* Update state as last action, so that it is not changed if some action
     above throws.  */
  state = REGISTERED;
}

/**
 * Return rand value.
 * @return rand value.
 * @throws std::runtime_error if this is not in REGISTERED state.
 */
const std::string&
NameRegistration::getRand () const
{
  if (state != REGISTERED)
    throw std::runtime_error ("rand value is only available"
                              " in REGISTERED state.");

  return rand;
}

/**
 * Check whether we can already perform a firstupdate transaction.
 * @return True iff we're in REGISTERED state and enough time has passed.
 */
bool
NameRegistration::canActivate () const
{
  if (state != REGISTERED)
    return false;

  return (nc.getNumberOfConfirmations (tx) >= FIRSTUPDATE_DELAY);
}

/**
 * Activate the name, which issues the firstupdate transaction.
 * @throws std::runtime_error if this is not (yet) possible.
 */
void
NameRegistration::activate ()
{
  if (state != REGISTERED)
    throw std::runtime_error ("Can activate() only in REGISTERED state.");
  if (!canActivate ())
    throw std::runtime_error ("Can't yet activate, please wait longer.");

  JsonRpc::JsonData args(Json::arrayValue);
  args.append (name);
  args.append (rand);
  args.append (tx);
  args.append (value);
  const JsonRpc::JsonData res = rpc.executeRpcArray ("name_firstupdate", args);

  assert (res.isString ());
  txActivation = res.asString ();
  state = ACTIVATED;
}

/**
 * Check whether the registration is finished, which includes a confirmation
 * of the firstupdate transaction.
 * @return True iff the name was activated and the tx confirmed.
 */
bool
NameRegistration::isFinished () const
{
  if (state != ACTIVATED)
    return false;

  return (nc.getNumberOfConfirmations (txActivation) > 0);
}

/**
 * For registered but not yet activated registration processes, save the
 * state necessary to later perform firstupdate to a stream.
 * @param out The output stream.
 * @param obj The NameRegistration object to save.
 * @return The stream.
 * @throws std::runtime_error if the name is not REGISTERED state.
 */
std::ostream&
operator<< (std::ostream& out, const NameRegistration& obj)
{
  JsonRpc::JsonData outVal(Json::objectValue);
  outVal["type"] = "NameRegistration";
  outVal["version"] = 1;
  outVal["name"] = obj.name;

  switch (obj.state)
    {
    case NameRegistration::REGISTERED:
      outVal["state"] = "registered";
      outVal["value"] = obj.value;
      outVal["rand"] = obj.rand;
      outVal["tx"] = obj.tx;
      break;

    case NameRegistration::ACTIVATED:
      outVal["state"] = "activated";
      outVal["txActivation"] = obj.txActivation;
      break;

    default:
      throw std::runtime_error ("Wrong state for saving of NameRegistration.");
    }

  out << JsonRpc::encodeJson (outVal);
  return out;
}

/**
 * Load state of a registration process from the stream, so that we can
 * finish with firstupdate.
 * @param in The input stream.
 * @param obj The NameRegistration object to load into.
 * @return The stream.
 * @throws std::runtime_error/JsonParseError if no valid data can be found.
 */
std::istream&
operator>> (std::istream& in, NameRegistration& obj)
{
  const JsonRpc::JsonData inVal = JsonRpc::readJson (in);

  if (inVal["type"].asString () != "NameRegistration"
      || inVal["version"].asInt () != 1)
    throw std::runtime_error ("Wrong JSON object found, expected"
                              " version 1 NameRegistration.");

  obj.name = inVal["name"].asString ();

  const std::string state = inVal["state"].asString ();
  if (state == "registered")
    {
      obj.state = NameRegistration::REGISTERED;
      obj.value = inVal["value"].asString ();
      obj.rand = inVal["rand"].asString ();
      obj.tx = inVal["tx"].asString ();
    }
  else if (state == "activated")
    {
      obj.state = NameRegistration::ACTIVATED;
      obj.txActivation = inVal["txActivation"].asString ();
    }
  else
    throw std::runtime_error ("Invalid state found in the JSON data"
                              " of NameRegistration.");

  return in;
}

/* ************************************************************************** */
/* Manage multiple name registration processes.  */

/**
 * Destroy it safely.
 */
RegistrationManager::~RegistrationManager ()
{
  clear ();
}

/**
 * Clear all elements, freeing the memory properly.
 */
void
RegistrationManager::clear ()
{
#ifdef CXX_11
  for (auto ptr : names)
    delete ptr;
#else /* CXX_11  */
  for (nameListT::iterator i = names.begin (); i != names.end (); ++i)
    delete *i;
#endif /* CXX_11  */
  names.clear ();
}

/**
 * Start registration for a new name.  The process object is returned so that
 * the value can be set as desired.
 * @param nm The name to register.
 * @return The NameRegistration object created and inserted.
 * @throws NameAlreadyReserved if the name already exists.
 */
NameRegistration&
RegistrationManager::registerName (const NamecoinInterface::Name& nm)
{
#ifdef CXX_11
  std::unique_ptr<NameRegistration> ptr;
#else /* CXX_11  */
  std::auto_ptr<NameRegistration> ptr;
#endif /* CXX_11  */

  ptr.reset (new NameRegistration (rpc, nc));
  ptr->registerName (nm);

  names.push_back (ptr.release ());
  return *names.back ();
}

/**
 * Try to update all processes, which activates names where it is possible.
 */
void
RegistrationManager::update ()
{
#ifdef CXX_11
  for (auto& nm : *this)
#else /* CXX_11  */
  for (iterator i = begin (); i != end (); ++i)
#endif /* CXX_11  */
    {
#ifndef CXX_11
      NameRegistration& nm = *i;
#endif /* !CXX_11  */
      if (nm.canActivate ())
        nm.activate ();
    }
}

/**
 * Purge finished names from the list.
 * @return Number of elements purged.
 */
unsigned
RegistrationManager::cleanUp ()
{
  unsigned res = 0;

  nameListT::iterator i = names.begin ();
  while (i != names.end ())
    {
      if ((*i)->isFinished ())
        {
          i = names.erase (i);
          ++res;
        }
      else
        ++i;
    }

  return res;
}

/**
 * Write out all states to the stream.
 * @param out The output stream.
 * @param obj The RegistrationManager object to save.
 * @return The stream.
 */
std::ostream&
operator<< (std::ostream& out, const RegistrationManager& obj)
{
  JsonRpc::JsonData outVal(Json::objectValue);
  outVal["type"] = "RegistrationManager";
  outVal["version"] = 1;

  JsonRpc::JsonData arr(Json::arrayValue);
#ifdef CXX_11
  for (const auto& nm : obj)
#else /* CXX_11  */
  for (RegistrationManager::const_iterator i = obj.begin ();
       i != obj.end (); ++i)
#endif /* CXX_11  */
    {
#ifndef CXX_11
      const NameRegistration& nm = *i;
#endif /* !CXX_11  */
      std::ostringstream val;
      val << nm;
      arr.append (val.str ());
    }
  outVal["elements"] = arr;

  out << JsonRpc::encodeJson (outVal);
  return out;
}

/**
 * Load all states from the stream, replacing all objects that
 * are currently in the list (if any).
 * @param in The input stream.
 * @param obj The object to load into.
 * @return The stream.
 * @throws std::runtime_error/JsonParseError if no valid data can be found.
 */
std::istream&
operator>> (std::istream& in, RegistrationManager& obj)
{
  const JsonRpc::JsonData inVal = JsonRpc::readJson (in);

  if (inVal["type"].asString () != "RegistrationManager"
      || inVal["version"].asInt () != 1)
    throw std::runtime_error ("Wrong JSON object found, expected"
                              " version 1 RegistrationManager.");

  const JsonRpc::JsonData elements = inVal["elements"];
  if (!elements.isArray ())
    throw std::runtime_error ("Invalid JSON for RegistrationManager.");

  obj.clear ();
#ifdef CXX_11
  for (const JsonRpc::JsonData el : elements)
#else /* CXX_11  */
  for (JsonRpc::JsonData::const_iterator i = elements.begin ();
       i != elements.end (); ++i)
#endif /* CXX_11  */
    {
#ifndef CXX_11
      const JsonRpc::JsonData& el = *i;
#endif /* !CXX_11  */

      std::istringstream val(el.asString ());
#ifdef CXX_11
      std::unique_ptr<NameRegistration> ptr;
#else /* CXX_11  */
      std::auto_ptr<NameRegistration> ptr;
#endif /* CXX_11  */

      ptr.reset (new NameRegistration (obj.rpc, obj.nc));
      val >> *ptr;
      obj.names.push_back (ptr.release ());
    }

  return in;
}

/* ************************************************************************** */
/* Name updater.  */

/**
 * Create the object.  It sets the value per default to the value
 * currently held at the name.
 * @param r The RPC connection to use.
 * @param n The high-level interface to use.
 * @param nm The name to update.
 * @throws NameNotFound if the name doesn't yet exist.
 */
NameUpdate::NameUpdate (JsonRpc& r, NamecoinInterface& n,
                        const NamecoinInterface::Name& nm)
  : rpc(r), nc(n), name(nm)
{
  value = name.getStringValue ();
}

/**
 * Utility routine to perform the update in both cases with and without
 * manual address.  This handles the exception catching and things like that.
 * @param addr Address to send the name to or NULL.
 * @returns The transaction ID.
 * @throws NoPrivateKey if the name is not owned by the user.
 * @throws std::runtime_error if the wallet is locked.
 */
std::string
NameUpdate::internalExecute (const NamecoinInterface::Address* addr)
{
  try
    {
      JsonRpc::JsonData args(Json::arrayValue);
      args.append (name.getName ());
      args.append (value);
      if (addr)
        {
          if (!addr->isValid ())
            throw std::runtime_error ("Target address is invalid.");
          args.append (addr->getAddress ());
        }
      const JsonRpc::JsonData res = rpc.executeRpcArray ("name_update", args);

      return res.asString ();
    }
  catch (const JsonRpc::RpcError& exc)
    {
      switch (exc.getErrorCode ())
        {
        case -13:
          throw std::runtime_error ("Need to unlock the wallet first.");

        case -1:
          {
            std::ostringstream msg;
            msg << "You don't have the private key for the name "
                << name.getName () << " and can't update this name.";
            throw NamecoinInterface::NoPrivateKey (msg.str ());
          }

        default:
          throw exc;
        }
    }
}

} // namespace nmcrpc
