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


#include "nsCollationUnix.h"


NS_DEFINE_IID(kICollationIID, NS_ICOLLATION_IID);

NS_IMPL_ISUPPORTS(nsCollationUnix, kICollationIID);


nsCollationUnix::nsCollationUnix() 
{
  NS_INIT_REFCNT(); 
  mCollation = NULL;
}

nsCollationUnix::~nsCollationUnix() 
{
  if (mCollation != NULL)
    delete mCollation;
}

nsresult nsCollationUnix::Initialize(nsILocale* locale) 
{
  mCollation = new nsCollation;
  if (mCollation == NULL) {
    return NS_ERROR_OUT_OF_MEMORY;
  }

  // locale -> LCID

  return NS_OK;
};
 

nsresult nsCollationUnix::GetSortKeyLen(const nsCollationStrength strength, 
                           const nsString& stringIn, PRUint32* outLen)
{
  *outLen = stringIn.Length() * sizeof(PRUnichar);
  return NS_OK;
}

nsresult nsCollationUnix::CreateSortKey(const nsCollationStrength strength, 
                           const nsString& stringIn, PRUint8* key, PRUint32* outLen)
{
  // temporary implementation, call FE eventually
  PRUint32 byteLenIn = stringIn.Length() * sizeof(PRUnichar);

  if (byteLenIn > *outLen) {
    *outLen = 0;
  }
  else {
    if (mCollation != NULL) {
      mCollation->NormalizeString(stringIn);
    }
    if (strength != kCollationCaseSensitive) {
      nsString *stringLower = new nsString(stringIn);
      if (NULL == stringLower)
        return NS_ERROR_OUT_OF_MEMORY;

      stringLower->ToLowerCase();
      memcpy((void *) key, (void *) stringLower->GetUnicode(), byteLenIn);
      delete stringLower;
    }
   else {
      memcpy((void *) key, (void *) stringIn.GetUnicode(), byteLenIn);
   }
   *outLen = byteLenIn;
  }

  return NS_OK;
}

