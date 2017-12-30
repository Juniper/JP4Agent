//
/// @file   AftStringTable.h
/// @brief  AftStringTable is used as the basis for all string-to-integer mappings
//
//  Copyright (c) Juniper Networks, Inc., [2017].
//
//  All rights reserved.
//
//
//  Third-Party Code: This code may depend on other components under separate
//  copyright notice and license terms.  Your use of the source code for those
//  components is subject to the terms and conditions of the respective license
//  as noted in the Third-Party source code file.
//
//

#ifndef __AftStringTable__
#define __AftStringTable__

///
/// @defgroup  AftString
/// @brief Classes used as the core of string lookups for Aft
/// @{
///

typedef std::vector<std::string> AftStringTableVector;

///
/// @class   AftStringTable
/// @brief   Base class for managing token/name mappings
///
template <typename TableType, typename EntryType>
class AftStringTable {
protected:
    std::map<std::string, EntryType> _entries;
    
    ///
    /// @brief Create and insert a new name entry
    /// @param [in]    entryName  Name for entry
    /// @param [in]    entry      Value to bind to name
    ///
    bool _insert (const std::string &entryName, EntryType entry) {
        //
        // If neither type or name is specified, just return
        //
        if (entryName.length() == 0)
            return false;
        
        //
        // Remember our token
        //
        _entries[entryName] = entry;
        
        return true;
    };
    
    ///
    /// @brief Remove a name entry
    /// @param [in]    entryName    Name of entry to remove
    ///
    void _remove (const std::string &entryName) {
        _entries.erase(entryName);
    };
    
    ///
    /// @brief Look for an entry by name
    /// @param [in]    entryName    Name to look for
    /// @param [out]   entry        Value of entry for the name if it's found in table
    /// @retval True   Sucessfully found name for entry
    /// @retval False  Didn't find name in table
    ///
    bool _find (const std::string &entryName, EntryType &entry) {
        auto it = _entries.find(entryName);
        if (it != _entries.end()) {
            entry = it->second;
            return true;
        }
        
        return false;
    };
    
    ///
    /// @brief Look for a node names by regular expression
    /// @param [in]    regexMatch  Regular expression for name match
    /// @param [out]   names       Reference to vector of names that match (if any)
    /// @retval True   Sucessfully found name for regex match
    /// @retval False  Didn't find anything matching regex in table
    ///
    bool _match (const std::regex &regexMatch, AftStringTableVector &names) {
        std::string entryName;
        
        for (auto it = _entries.begin(); it != _entries.end(); ++it) {
            entryName = it->first;
            if (std::regex_match(entryName, regexMatch) > 0) {
                names.push_back(entryName);
            }
        }
        
        return (names.size() > 0);
    };

public:
    ///
    /// Template share pointer alias
    ///
    using Ptr = std::shared_ptr<TableType>;
    
    //
    // Class for the pair of name and entry
    //
    class Pair {
    protected:
        std::string  _name;
        EntryType    _entry;
        
    public:
        friend TableType;
        
        Pair(std::string newName, EntryType newEntry): _name(newName), _entry(newEntry) {};
        
        const std::string &name() const { return _name; };
        EntryType entry() const { return _entry; };

        friend std::ostream &operator<< (std::ostream &os, const Pair &pair) {
            os << "name=" << pair.name() << " entry=" << pair.entry();
            return os;
        }
    };
    
    //
    // Factory constructor
    //
    static std::shared_ptr<TableType> create () {
        return std::make_shared<TableType>();
    }

    //
    // Default construction
    //
    AftStringTable() {};
    AftStringTable(const TableType &table) {
        for (auto it: table._entries) {
            std::string entryName = it.first;
            EntryType   entry     = it.second;
            
            insert(entryName, entry);
        }
    }
    
    //
    // Iterator access
    //
    typename std::map<std::string, EntryType>::iterator begin() { return _entries.begin(); };
    typename std::map<std::string, EntryType>::iterator end() { return _entries.end(); };
    
    ///
    /// @brief Create and insert a new name entry
    /// @param [in]    entryName  Name for entry
    /// @param [in]    entry      Value to bind to name
    ///
    bool insert (const std::string &entryName, EntryType entry) {
        return _insert(entryName, entry);
    };
    
    ///
    /// @brief Remove a name entry
    /// @param [in]    entryName    Name of entry to remove
    ///
    void remove (const std::string &entryName) {
        _remove(entryName);
    };
    
    ///
    /// @brief Look for an entry by name
    /// @param [in]    entryName    Name to look for
    /// @param [out]   entry        Value of entry for the name if it's found in table
    /// @retval True   Sucessfully found name for entry
    /// @retval False  Didn't find name in table
    ///
    bool find (const std::string &entryName, EntryType &entry) {
        return _find(entryName, entry);
    };
    
    ///
    /// @brief Look for an entry by name and return a tuple
    /// @param [in]    entryName    Name to look for
    /// @param [out]   entryTuple   Entry tuple if
    /// @retval True   Sucessfully found name for entry
    /// @retval False  Didn't find name in table
    ///
    bool find (const std::string &entryName, Pair &&entryPair) {
        EntryType entry;
        if (_find(entryName, entry)) {
            //
            // Found it
            //
            entryPair = Pair(entryName, entry);
            
            return true;
        } else {
            //
            // Didn't find anything
            //
            return false;
        }
    };

    ///
    /// @brief Look for a node names by regular expression
    /// @param [in]    regexMatch  Regular expression for name match
    /// @param [out]   names       Reference to vector of names that match (if any)
    /// @retval True   Sucessfully found name for regex match
    /// @retval False  Didn't find anything matching regex in table
    ///
    bool match (const std::regex &regexMatch, AftStringTableVector &names) {
        return _match(regexMatch, names);
    };

    ///
    /// @brief Check that an entry name is valid
    /// @param [in]    entryName Name of entry to check
    /// @retval True   Entry is valid
    /// @retval False  Entry isn't in table
    ///
    bool isValid (std::string entryName) {
        EntryType entry;
        return (find(entryName, entry));
    };
};

///
/// @class   AftStringBitable
/// @brief   Base class for managing token/name mappings
///
template <typename TableType, typename EntryType>
class AftStringBitable {
protected:
    std::map<std::string, EntryType> _entries;
    std::map<EntryType, std::string> _indexes;
    
    ///
    /// @brief Create and insert a new name entry
    /// @param [in]    entryName  Name for entry
    /// @param [in]    entry      Value to bind to name
    ///
    bool _insert (const std::string &entryName, EntryType entry) {
        //
        // If neither type or name is specified, just return
        //
        if (entryName.length() == 0)
            return false;
        
        //
        // Remember our name and reverse
        //
        _entries[entryName] = entry;
        _indexes[entry] = entryName;
        
        return true;
    };
    
    ///
    /// @brief Remove a name entry
    /// @param [in]    entryName    Name of entry to remove
    ///
    void _remove (const std::string &entryName) {
        EntryType entry;
        
        if (find(entryName, entry)) {
            _indexes.erase(entry);
            _entries.erase(entryName);
        }
    };
    
    ///
    /// @brief Look for an entry by name
    /// @param [in]    entryName    Name to look for
    /// @param [out]   entry        Value of entry for the name if it's found in table
    /// @retval True   Sucessfully found name for entry
    /// @retval False  Didn't find name in table
    ///
    bool _find (const std::string &entryName, EntryType &entry) {
        auto it = _entries.find(entryName);
        if (it != _entries.end()) {
            entry = it->second;
            return true;
        }
        
        return false;
    };
    
    ///
    /// @brief Look for a name by entry
    /// @param [out]   entry        Value of entry for the name if it's found in table
    /// @param [in]    entryName    Name to look for
    /// @retval True   Sucessfully found name for entry
    /// @retval False  Didn't find name in table
    ///
    bool _find (const EntryType &entry, std::string &entryName) {
        auto it = _indexes.find(entry);
        if (it != _indexes.end()) {
            entryName = it->second;
            return true;
        }
        
        return false;
    };

public:
    ///
    /// Template share pointer alias
    ///
    using Ptr = std::shared_ptr<TableType>;
    
    //
    // Class for the pair of name and entry
    //
    class Pair {
    protected:
        std::string  _name;
        EntryType    _entry;
        
    public:
        friend TableType;
        
        Pair(std::string newName, EntryType newEntry): _name(newName), _entry(newEntry) {};
        
        const std::string &name() const { return _name; };
        EntryType entry() const { return _entry; };

        friend std::ostream &operator<< (std::ostream &os, const Pair &pair) {
            os << "name=" << pair.name() << " entry=" << pair.entry();
            return os;
        }
    };

    static std::shared_ptr<TableType> create () {
        return std::make_shared<TableType>();
    }
    
    //
    // Default construction
    //
    AftStringBitable() {};
    AftStringBitable(const TableType &table) {
        for (auto it: table._entries) {
            std::string entryName = it.first;
            EntryType entry       = it.second;
            
            insert(entryName, entry);
        }
    }

    //
    // Iterator access
    //
    typename std::map<std::string, EntryType>::iterator begin() { return _entries.begin(); };
    typename std::map<std::string, EntryType>::iterator end() { return _entries.end(); };

    ///
    /// @brief Create and insert a new name entry
    /// @param [in]    entryName  Name for entry
    /// @param [in]    entry      Value to bind to name
    ///
    bool insert (const std::string &entryName, EntryType entry) {
        return _insert(entryName, entry);
    };
    
    ///
    /// @brief Remove a name entry
    /// @param [in]    entryName    Name of entry to remove
    ///
    void remove (const std::string &entryName) {
        _remove(entryName);
    };
    
    ///
    /// @brief Look for an entry by name
    /// @param [in]    entryName    Name to look for
    /// @param [out]   entry        Value of entry for the name if it's found in table
    /// @retval True   Sucessfully found name for entry
    /// @retval False  Didn't find name in table
    ///
    bool find (const std::string &entryName, EntryType &entry) {
        return _find(entryName, entry);
    };

    ///
    /// @brief Look for an entry by index
    /// @param [in]    entryName    Name of entry if it's found in table
    /// @param [out]   entry        Entry to look for
    /// @retval True   Sucessfully found entry
    /// @retval False  Didn't find entry in table
    ///
    bool find (const EntryType &entry, std::string &entryName) {
        return _find(entryName, entry);
    };

    ///
    /// @brief Look for an entry by name and return a pair
    /// @param [in]    entryName   Name to look for
    /// @param [out]   entryPair   Entry pair if found
    /// @retval True   Sucessfully found name for entry
    /// @retval False  Didn't find name in table
    ///
    bool find (const std::string &entryName, Pair &&entryPair) {
        EntryType entry;
        if (_find(entryName, entry)) {
            //
            // Found it
            //
            entryPair = Pair(entryName, entry);
            
            return true;
        } else {
            //
            // Didn't find anything
            //
            return false;
        }
    };

    ///
    /// @brief Check that an entry index is valid
    /// @param [in]    entryIndex Index of entry to check
    /// @retval True   Entry is valid
    /// @retval False  Entry isn't in table
    ///
    bool isValid (EntryType entry) {
        std::string entryName;
        return (find(entry, entryName) != nullptr);
    };

    ///
    /// @brief Check that an entry name is valid
    /// @param [in]    entryName Name of entry to check
    /// @retval True   Entry is valid
    /// @retval False  Entry isn't in table
    ///
    bool isValid (std::string entryName) {
        EntryType entry;
        return (find(entryName, entry));
    };
};

///
/// @}
///

#endif
