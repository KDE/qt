/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"

#include "JSDocumentFragment.h"

#include <wtf/GetPtr.h>

#include "DocumentFragment.h"
#include "Element.h"
#include "JSElement.h"
#include "JSNodeList.h"
#include "NameNodeList.h"
#include "NodeList.h"

#include <runtime/Error.h>
#include <runtime/JSNumberCell.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSDocumentFragment)

/* Hash table */

static const HashTableValue JSDocumentFragmentTableValues[2] =
{
    { "constructor", DontEnum|ReadOnly, (intptr_t)jsDocumentFragmentConstructor, (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static const HashTable JSDocumentFragmentTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSDocumentFragmentTableValues, 0 };
#else
    { 2, 1, JSDocumentFragmentTableValues, 0 };
#endif

/* Hash table for constructor */

static const HashTableValue JSDocumentFragmentConstructorTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static const HashTable JSDocumentFragmentConstructorTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSDocumentFragmentConstructorTableValues, 0 };
#else
    { 1, 0, JSDocumentFragmentConstructorTableValues, 0 };
#endif

class JSDocumentFragmentConstructor : public DOMObject {
public:
    JSDocumentFragmentConstructor(ExecState* exec)
        : DOMObject(JSDocumentFragmentConstructor::createStructure(exec->lexicalGlobalObject()->objectPrototype()))
    {
        putDirect(exec->propertyNames().prototype, JSDocumentFragmentPrototype::self(exec, exec->lexicalGlobalObject()), None);
    }
    virtual bool getOwnPropertySlot(ExecState*, const Identifier&, PropertySlot&);
    virtual const ClassInfo* classInfo() const { return &s_info; }
    static const ClassInfo s_info;

    static PassRefPtr<Structure> createStructure(JSValuePtr proto) 
    { 
        return Structure::create(proto, TypeInfo(ObjectType, ImplementsHasInstance)); 
    }
};

const ClassInfo JSDocumentFragmentConstructor::s_info = { "DocumentFragmentConstructor", 0, &JSDocumentFragmentConstructorTable, 0 };

bool JSDocumentFragmentConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSDocumentFragmentConstructor, DOMObject>(exec, &JSDocumentFragmentConstructorTable, this, propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSDocumentFragmentPrototypeTableValues[3] =
{
    { "querySelector", DontDelete|Function, (intptr_t)jsDocumentFragmentPrototypeFunctionQuerySelector, (intptr_t)1 },
    { "querySelectorAll", DontDelete|Function, (intptr_t)jsDocumentFragmentPrototypeFunctionQuerySelectorAll, (intptr_t)1 },
    { 0, 0, 0, 0 }
};

static const HashTable JSDocumentFragmentPrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 1, JSDocumentFragmentPrototypeTableValues, 0 };
#else
    { 4, 3, JSDocumentFragmentPrototypeTableValues, 0 };
#endif

const ClassInfo JSDocumentFragmentPrototype::s_info = { "DocumentFragmentPrototype", 0, &JSDocumentFragmentPrototypeTable, 0 };

JSObject* JSDocumentFragmentPrototype::self(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSDocumentFragment>(exec, globalObject);
}

bool JSDocumentFragmentPrototype::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticFunctionSlot<JSObject>(exec, &JSDocumentFragmentPrototypeTable, this, propertyName, slot);
}

const ClassInfo JSDocumentFragment::s_info = { "DocumentFragment", &JSEventTargetNode::s_info, &JSDocumentFragmentTable, 0 };

JSDocumentFragment::JSDocumentFragment(PassRefPtr<Structure> structure, PassRefPtr<DocumentFragment> impl)
    : JSEventTargetNode(structure, impl)
{
}

JSObject* JSDocumentFragment::createPrototype(ExecState* exec, JSGlobalObject* globalObject)
{
    return new (exec) JSDocumentFragmentPrototype(JSDocumentFragmentPrototype::createStructure(JSEventTargetNodePrototype::self(exec, globalObject)));
}

bool JSDocumentFragment::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSDocumentFragment, Base>(exec, &JSDocumentFragmentTable, this, propertyName, slot);
}

JSValuePtr jsDocumentFragmentConstructor(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    return static_cast<JSDocumentFragment*>(asObject(slot.slotBase()))->getConstructor(exec);
}
JSValuePtr JSDocumentFragment::getConstructor(ExecState* exec)
{
    return getDOMConstructor<JSDocumentFragmentConstructor>(exec);
}

JSValuePtr jsDocumentFragmentPrototypeFunctionQuerySelector(ExecState* exec, JSObject*, JSValuePtr thisValue, const ArgList& args)
{
    if (!thisValue->isObject(&JSDocumentFragment::s_info))
        return throwError(exec, TypeError);
    JSDocumentFragment* castedThisObj = static_cast<JSDocumentFragment*>(asObject(thisValue));
    DocumentFragment* imp = static_cast<DocumentFragment*>(castedThisObj->impl());
    ExceptionCode ec = 0;
    const UString& selectors = valueToStringWithUndefinedOrNullCheck(exec, args.at(exec, 0));


    JSC::JSValuePtr result = toJS(exec, WTF::getPtr(imp->querySelector(selectors, ec)));
    setDOMException(exec, ec);
    return result;
}

JSValuePtr jsDocumentFragmentPrototypeFunctionQuerySelectorAll(ExecState* exec, JSObject*, JSValuePtr thisValue, const ArgList& args)
{
    if (!thisValue->isObject(&JSDocumentFragment::s_info))
        return throwError(exec, TypeError);
    JSDocumentFragment* castedThisObj = static_cast<JSDocumentFragment*>(asObject(thisValue));
    DocumentFragment* imp = static_cast<DocumentFragment*>(castedThisObj->impl());
    ExceptionCode ec = 0;
    const UString& selectors = valueToStringWithUndefinedOrNullCheck(exec, args.at(exec, 0));


    JSC::JSValuePtr result = toJS(exec, WTF::getPtr(imp->querySelectorAll(selectors, ec)));
    setDOMException(exec, ec);
    return result;
}


}
