// -----------------------------------------------------------------------------
//
// NsFactory.h
//
// Naiad Studio factory header file.
//
// Copyright (c) 2011 Exotic Matter AB.  All rights reserved.
//
// This file is part of Open Naiad Studio.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// * Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// * Neither the name of Exotic Matter AB nor its contributors may be used to
// endorse or promote products derived from this software without specific
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// -----------------------------------------------------------------------------

#ifndef NS_FACTORY_H
#define NS_FACTORY_H

#include <QHash>
#include <QList>

// -----------------------------------------------------------------------------

// NsFactory
// ---------
//! Factory pattern implementation.

//template<class P, class K, class C, class A>
template<class K, class C>
class NsFactory
{
public:     // Types.

    typedef K KeyType;
    typedef C CreatorType;
    typedef typename C::ProductType ProductType;
    typedef typename C::ArgsType ArgsType;

public:     // Singleton interface.

    static NsFactory *instance();
    static void destroy();

public:

    NsFactory() {}      //!< CTOR.
    ~NsFactory() {}     //!< DTOR.

    void registerCreator(const K &key, const C *creator);
    int unregisterCreator(const K &key);

    ProductType *create(const K &key, const ArgsType &args) const;

    QList<K> registeredCreators() const;

private:    // Member variables.

    typedef QHash<K, const C *> _HashType;
    _HashType _registeredCreators;      //!< Associate keys with creators.

private:    // Singleton implementation.

    static NsFactory<K,C> *_instance;

private:    // Disabled copy/assign.

    NsFactory(const NsFactory<K,C> &);                  //! Disable copy.
    NsFactory<K,C>& operator=(const NsFactory<K,C> &);  //! Disable assign.
};

// -----------------------------------------------------------------------------

// instance
// --------
//! Returns pointer to factory singleton instance.

template<class K, class C>
NsFactory<K,C>*
NsFactory<K,C>::instance()
{
    if (0 == _instance) {
        // Note that the new instance will have no registered creators.

        _instance = new NsFactory<K,C>();
    }

    return _instance;   // Return singleton instance.
}


// destroy
// -------
//! Destroy the factory singleton instance.

template<class K, class C>
void
NsFactory<K,C>::destroy()
{
    delete _instance;
    _instance = 0;
}


// registerCreator
// ---------------
//! Associate a creation object with a value widget name.

template<class K, class C>
void
NsFactory<K,C>::registerCreator(const K &key, const C *creator)
{
    // If there is already a creator for this key it will be overwritten.

    _registeredCreators.insert(key, creator);
}


// unregisterCreator
// -----------------
//! Remove a registered creator.

template<class K, class C>
int
NsFactory<K,C>::unregisterCreator(const K &key)
{
    // Return number of removed keys.

    return _registeredCreators.remove(key);
}


// create
// ------
//! Create a product with type depending on the provided key.
//! Returns null if the provided key has not been registered.

template<class K, class C>
typename NsFactory<K,C>::ProductType *
NsFactory<K,C>::create(const K &key,
                       const typename NsFactory<K,C>::ArgsType &args) const
{
    const typename _HashType::const_iterator iter(
        _registeredCreators.find(key));

    if (iter != _registeredCreators.end()) {
        // Found a matching key.

        return iter.value()->create(args);   // May throw.
    }

    return 0;   // Return null if no matching key exists.
}


// registeredCreators
// ------------------
//! Returns the list of keys registered with the factory.

template<class K, class C>
QList<K>
NsFactory<K,C>::registeredCreators() const
{
    return _registeredCreators.keys();
}


// _instance
// ---------
//! Initialize singleton instance to null

template<class K, class C>
NsFactory<K,C> *NsFactory<K,C>::_instance = 0;

// -----------------------------------------------------------------------------

#endif  // NS_FACTORY_H
