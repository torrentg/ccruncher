/*
 * Copyright 1999-2004 The Apache Software Foundation.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * $Log: XMLRefInfo.hpp,v $
 * Revision 1.9  2004/09/08 13:55:59  peiyongz
 * Apache License Version 2.0
 *
 * Revision 1.8  2003/09/30 18:14:34  peiyongz
 * Implementation of Serialization/Deserialization
 *
 * Revision 1.7  2003/05/16 21:36:55  knoaman
 * Memory manager implementation: Modify constructors to pass in the memory manager.
 *
 * Revision 1.6  2003/05/15 18:26:07  knoaman
 * Partial implementation of the configurable memory manager.
 *
 * Revision 1.5  2003/04/21 20:46:01  knoaman
 * Use XMLString::release to prepare for configurable memory manager.
 *
 * Revision 1.4  2003/03/07 18:08:10  tng
 * Return a reference instead of void for operator=
 *
 * Revision 1.3  2002/12/04 02:32:43  knoaman
 * #include cleanup.
 *
 * Revision 1.2  2002/11/04 15:00:21  tng
 * C++ Namespace Support.
 *
 * Revision 1.1.1.1  2002/02/01 22:21:52  peiyongz
 * sane_include
 *
 * Revision 1.5  2000/07/07 22:23:38  jpolast
 * remove useless getKey() functions.
 *
 * Revision 1.4  2000/02/24 20:00:23  abagchi
 * Swat for removing Log from API docs
 *
 * Revision 1.3  2000/02/15 01:21:31  roddey
 * Some initial documentation improvements. More to come...
 *
 * Revision 1.2  2000/02/06 07:47:49  rahulj
 * Year 2K copyright swat.
 *
 * Revision 1.1.1.1  1999/11/09 01:08:37  twl
 * Initial checkin
 *
 * Revision 1.2  1999/11/08 20:44:40  rahul
 * Swat for adding in Product name and CVS comment log variable.
 *
 */


#if !defined(XMLIDREFINFO_HPP)
#define XMLIDREFINFO_HPP

#include <xercesc/util/XMemory.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

#include <xercesc/internal/XSerializable.hpp>

XERCES_CPP_NAMESPACE_BEGIN

/**
 *  This class provides a simple means to track ID Ref usage. Since id/idref
 *  semamatics are part of XML 1.0, any validator will likely to be able to
 *  track them. Instances of this class represent a reference and two markers,
 *  one for its being declared and another for its being used. When the
 *  document is done, one can look at each instance and, if used but not
 *  declared, its an error.
 *
 *  The getKey() method allows it to support keyed collection semantics. It
 *  returns the referenced name, so these objects will be stored via the hash
 *  of the name. This name will either be a standard QName if namespaces are
 *  not enabled/supported by the validator, or it will be in the form
 *  {url}name if namespace processing is enabled.
 */
class XMLPARSER_EXPORT XMLRefInfo : public XSerializable, public XMemory
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------

    /** @name Constructor */
    //@{
    XMLRefInfo
    (
        const   XMLCh* const   refName
        , const bool           fDeclared = false
        , const bool           fUsed = false
        , MemoryManager* const manager = XMLPlatformUtils::fgMemoryManager
    );
    //@}

    /** @name Destructor */
    //@{
    ~XMLRefInfo();
    //@}


    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    bool getDeclared() const;
    const XMLCh* getRefName() const;
    bool getUsed() const;


    // -----------------------------------------------------------------------
    //  Setter methods
    // -----------------------------------------------------------------------
    void setDeclared(const bool newValue);
    void setUsed(const bool newValue);

    /***
     * Support for Serialization/De-serialization
     ***/
    DECL_XSERIALIZABLE(XMLRefInfo)

    XMLRefInfo
    (
      MemoryManager* const manager = XMLPlatformUtils::fgMemoryManager
    );

private :
    // -----------------------------------------------------------------------
    //  Unimplemented constructors and operators
    // -----------------------------------------------------------------------
    XMLRefInfo(const XMLRefInfo&);
    XMLRefInfo& operator=(XMLRefInfo&);


    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fDeclared
    //      The name was declared somewhere as an ID attribute.
    //
    //  fRefName
    //      The name of the ref that this object represents. This is not a
    //      name of the attribute, but of the value of an ID or IDREF attr
    //      in content.
    //
    //  fUsed
    //      The name was used somewhere in an IDREF/IDREFS attribute. If this
    //      is true, but fDeclared is false, then the ref does not refer to
    //      a declared ID.
    // -----------------------------------------------------------------------
    bool        fDeclared;
    bool        fUsed;
    XMLCh*      fRefName;
    MemoryManager* fMemoryManager;
};


// ---------------------------------------------------------------------------
//  XMLRefInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
inline XMLRefInfo::XMLRefInfo( const XMLCh* const   refName
                             , const bool           declared
                             , const bool           used
                             , MemoryManager* const manager) :
    fDeclared(declared)
    , fUsed(used)
    , fRefName(0)
    , fMemoryManager(manager)
{
    fRefName = XMLString::replicate(refName, fMemoryManager);
}

inline XMLRefInfo::~XMLRefInfo()
{
    fMemoryManager->deallocate(fRefName);
}


// ---------------------------------------------------------------------------
//  XMLRefInfo: Getter methods
// ---------------------------------------------------------------------------
inline bool XMLRefInfo::getDeclared() const
{
    return fDeclared;
}

inline const XMLCh* XMLRefInfo::getRefName() const
{
    return fRefName;
}

inline bool XMLRefInfo::getUsed() const
{
    return fUsed;
}


// ---------------------------------------------------------------------------
//  XMLRefInfo: Setter methods
// ---------------------------------------------------------------------------
inline void XMLRefInfo::setDeclared(const bool newValue)
{
    fDeclared = newValue;
}

inline void XMLRefInfo::setUsed(const bool newValue)
{
    fUsed = newValue;
}

XERCES_CPP_NAMESPACE_END

#endif
