
/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * The contents of this file are subject to the Netscape Public License
 * Version 1.0 (the "NPL"); you may not use this file except in
 * compliance with the NPL.  You may obtain a copy of the NPL at
 * http://www.mozilla.org/NPL/
 *
 * Software distributed under the NPL is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the NPL
 * for the specific language governing rights and limitations under the
 * NPL.
 *
 * The Initial Developer of this code under the NPL is Netscape
 * Communications Corporation.  Portions created by Netscape are
 * Copyright (C) 1998 Netscape Communications Corporation.  All Rights
 * Reserved.
 */
#ifndef nsCollationWin_h__
#define nsCollationWin_h__


#include "nsICollation.h"
#include "nsCollation.h"  // static library



class nsCollationWin : public nsICollation {

public: 
  NS_DECL_ISUPPORTS
    
  // compare two strings
  // result is same as strcmp
  NS_IMETHOD CompareString(const nsCollationStrength strength, 
                           const nsString& string1, const nsString& string2, PRInt32* result) 
                           {return mCollation->CompareString(this, strength, string1, string2, result);};

  // get a length (of character) of a sort key to be generated by an input string
  // length is character length not byte length 
  NS_IMETHOD GetSortKeyLen(const nsCollationStrength strength, 
                           const nsString& stringIn, PRUint32* outLen);

  // create sort key from input string
  // length is character length not byte length, caller to allocate a memory for a key
  NS_IMETHOD CreateSortKey(const nsCollationStrength strength, 
                           const nsString& stringIn, PRUint8* key, PRUint32* outLen);

  // compare two sort keys
  // length is character length not byte length, result is same as strcmp
  NS_IMETHOD CompareSortKey(const PRUint8* key1, const PRUint32 len1, 
                            const PRUint8* key2, const PRUint32 len2, 
                            PRInt32* result) 
                            {*result = mCollation->CompareSortKey(key1, len1, key2, len2);return NS_OK;};

  // init this interface to a specified locale (should only be called by collation factory)
  //
  NS_IMETHOD Initialize(nsILocale* locale);

  nsCollationWin();

  virtual ~nsCollationWin(); 

protected:
  nsCollation   *mCollation;
};

#endif  /* nsCollationWin_h__ */
