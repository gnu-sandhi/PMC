// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_PMC_PMC_HPP
#define INCLUDED_PMC_PMC_HPP

#include <PMC/Config.hpp>
#include <typeinfo>
#include <ostream>
#include <string>

/*!
 * PMC: Just another polymorphic container for C++.
 *
 * The goal of PMC is to be a polymorphic container that holds primitive types such as
 * integers, floats, complex, string, and basic data structures like arrays and dicts.
 *
 * PMC is a reference counted object. Once created with a value, the value will not be copied.
 * This makes copying PMCs very cheap, and light-weight on the allocation overhead.
 * Also, the contained object will not be deleted until all references are deleted.
 */
struct PMC_API PMCC : PMCBase
{
    //! Create a null or empty PMCC
    PMCC(void);

    virtual ~PMCC(void);

    //! Unique if caller holds the only reference count
    bool unique(void) const;

    //! Get the number of reference holders
    size_t use_count(void) const;

    /*!
     * Get the type of the item held.
     * Only call if this object is not empty.
     */
    const std::type_info &type(void) const;

    //! Check if the container type matches
    template <typename ValueType>
    bool is(void) const;

    /*!
     * Cast the item held by this object to an arbitrary type.
     * This method will return a const reference to the object.
     * Only call if this object is not empty.
     */
    template <typename ValueType>
    const ValueType &as(void) const;

    /*!
     * Create an interned PMC object.
     *
     * Object interning ensures that there is only one unique
     * memory allocation for each unique object that is interned.
     * https://en.wikipedia.org/wiki/String_interning
     *
     * Interning a PMC has a high overhead cost due to lookup;
     * the advantage is that comparison of interned objects
     * has the equivalent overhead of comparing pointers.
     *
     * The implementation details:
     * Search for a duplicate object in the intern pool.
     * If found, return the entry already in the pool.
     * Otherwise, create a new entry in the intern pool.
     *
     * This method will return a PMC from the intern pool.
     * The PMC involved in the call will not be changed!
     * Interned objects are intentionally immutable.
     * To ensure immutability, the intern() returns PMCC.
     *
     * \return an object from the intern pool
     */
    const PMCC &intern(void) const;

    //! True if this PMC object has been interned
    bool is_intern(void) const;

    /*!
     * Compare two PMC objects for equivalency.
     * The operator==() checks if the containers are idential.
     * The eq() method checks if the contents are identical.
     * If both objects are null, the result is also true.
     * \return true if they have the same contents
     */
    bool eq(const PMCC &rhs) const;

    /*!
     * Serialize a PMC object into a string.
     * The object can be serialized into several different forms:
     *  - BINARY - platform dependent, but most compact form
     *  - TEXT - ascii displayable text, portable, less compact
     *  - XML - xml representation of the data, least compact
     * \param object the PMC object to serialize
     * \param format string format "BINARY", "TEXT", "XML"
     * \return a string representing the object
     */
    static std::string serialize(const PMCC &object, const std::string &format);

    /*!
     * Deserialize a string into a PMC object.
     * \param data the serialized string data
     * \param format string format "BINARY", "TEXT", "XML"
     * \return a new PMC object representing data
     */
    static PMCC deserialize(const std::string &data, const std::string &format);
};

/*!
 * The read/write version of the PMC container.
 * Non-const references can be retrieved.
 */
struct PMC_API PMC : PMCC
{
    //! Make an empty container
    PMC(void);

    virtual ~PMC(void);

    /*!
     * Cast the item held by this object to an arbitrary type.
     * This method will return a reference to the object.
     * Use this method to read or write the contained value.
     * Only call if this object is not empty.
     */
    template <typename ValueType>
    ValueType &as(void) const;
};

//! Make a new container holding a copy of the given value
template <typename ValueType>
PMC PMC_M(const ValueType &value);

//! Special make overload to create std::string from char *
PMCC PMC_M(const char *);

//! PMCC stream operator for iostream printing
std::ostream& operator <<(std::ostream &os, const PMCC &obj);

/*!
 * Simple macro to declare an equality operator for a given type.
 * The equality operator will always return false for this type.
 * Since PMC requires that types have an equality operator
 * to implement its .eq() method, its necessary to have
 * said equality operator defined for the given type.
 * When using its macro, its understood that the user expects
 * the .eq() method to return false for the given type.
 */
#define PMC_DECL_FALSE_EQUALITY(Type) \
    inline bool operator==(const Type &, const Type &){return false;}

#include <PMC/Detail/PMCImpl.hpp>

#endif /*INCLUDED_PMC_PMC_HPP*/
