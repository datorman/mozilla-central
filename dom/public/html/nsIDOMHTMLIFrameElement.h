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
/* AUTO-GENERATED. DO NOT EDIT!!! */

#ifndef nsIDOMHTMLIFrameElement_h__
#define nsIDOMHTMLIFrameElement_h__

#include "nsISupports.h"
#include "nsString.h"
#include "nsIScriptContext.h"
#include "nsIDOMHTMLElement.h"

class nsIDOMHTMLIFrameElement;

#define NS_IDOMHTMLIFRAMEELEMENT_IID \
{ 0x6f765305,  0xee43, 0x11d1, \
 { 0x9b, 0xc3, 0x00, 0x60, 0x08, 0x8c, 0xa6, 0xb3 } } 

class nsIDOMHTMLIFrameElement : public nsIDOMHTMLElement {
public:

  NS_IMETHOD    GetAlign(nsString& aAlign)=0;
  NS_IMETHOD    SetAlign(const nsString& aAlign)=0;

  NS_IMETHOD    GetFrameBorder(nsString& aFrameBorder)=0;
  NS_IMETHOD    SetFrameBorder(const nsString& aFrameBorder)=0;

  NS_IMETHOD    GetHeight(nsString& aHeight)=0;
  NS_IMETHOD    SetHeight(const nsString& aHeight)=0;

  NS_IMETHOD    GetLongDesc(nsString& aLongDesc)=0;
  NS_IMETHOD    SetLongDesc(const nsString& aLongDesc)=0;

  NS_IMETHOD    GetMarginHeight(nsString& aMarginHeight)=0;
  NS_IMETHOD    SetMarginHeight(const nsString& aMarginHeight)=0;

  NS_IMETHOD    GetMarginWidth(nsString& aMarginWidth)=0;
  NS_IMETHOD    SetMarginWidth(const nsString& aMarginWidth)=0;

  NS_IMETHOD    GetName(nsString& aName)=0;
  NS_IMETHOD    SetName(const nsString& aName)=0;

  NS_IMETHOD    GetScrolling(nsString& aScrolling)=0;
  NS_IMETHOD    SetScrolling(const nsString& aScrolling)=0;

  NS_IMETHOD    GetSrc(nsString& aSrc)=0;
  NS_IMETHOD    SetSrc(const nsString& aSrc)=0;

  NS_IMETHOD    GetWidth(nsString& aWidth)=0;
  NS_IMETHOD    SetWidth(const nsString& aWidth)=0;
};

extern nsresult NS_InitHTMLIFrameElementClass(nsIScriptContext *aContext, void **aPrototype);

extern "C" NS_DOM nsresult NS_NewScriptHTMLIFrameElement(nsIScriptContext *aContext, nsIDOMHTMLIFrameElement *aSupports, nsISupports *aParent, void **aReturn);

#endif // nsIDOMHTMLIFrameElement_h__
