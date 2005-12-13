/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   pschwartau@netscape.com
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */
/*
 * Date: 2001-06-24
 *
 * SUMMARY: Negative test: a class method may not change the value of this.
 */
//-------------------------------------------------------------------------------------------------
var bug = '(none)';
var summary = 'Negative test: a class method may not change the value of this';
var self = this; // capture a reference to the global object


class A 
{
}

class B 
{
  function changeThis() 
  {
    this = new Object();
  }
}

class C 
{
  function changeThis() 
  {
    this = self;
  }
}

class CC extends C 
{
  var m = changeThis;
}


class D
{
  function changeThis() 
  {
    this = objA;
  }
}

class E
{
  function E (obj:B)
  {
    this = obj;
  }
}


var objA = new A;
var objB = new B;
var objC = new C;
var objCC = new CC;
var objD = new D;
var objE = new E(objB);


objB.changeThis();
objC.changeThis();
objCC.m();
objD.changeThis();



//-------------------------------------------------------------------------------------------------
test();
//-------------------------------------------------------------------------------------------------



function test()
{
  enterFunc ('test');
  printBugNumber (bug);
  printStatus (summary);
  exitFunc ('test');
}
