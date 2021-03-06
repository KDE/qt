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

#include "JSHTMLIsIndexElement.h"

#include <wtf/GetPtr.h>

#include "HTMLFormElement.h"
#include "HTMLIsIndexElement.h"
#include "JSHTMLFormElement.h"
#include "KURL.h"

#include <runtime/JSNumberCell.h>
#include <runtime/JSString.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSHTMLIsIndexElement)

/* Hash table */

static const HashTableValue JSHTMLIsIndexElementTableValues[4] =
{
    { "form", DontDelete|ReadOnly, (intptr_t)jsHTMLIsIndexElementForm, (intptr_t)0 },
    { "prompt", DontDelete, (intptr_t)jsHTMLIsIndexElementPrompt, (intptr_t)setJSHTMLIsIndexElementPrompt },
    { "constructor", DontEnum|ReadOnly, (intptr_t)jsHTMLIsIndexElementConstructor, (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static const HashTable JSHTMLIsIndexElementTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 7, JSHTMLIsIndexElementTableValues, 0 };
#else
    { 8, 7, JSHTMLIsIndexElementTableValues, 0 };
#endif

/* Hash table for constructor */

static const HashTableValue JSHTMLIsIndexElementConstructorTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static const HashTable JSHTMLIsIndexElementConstructorTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSHTMLIsIndexElementConstructorTableValues, 0 };
#else
    { 1, 0, JSHTMLIsIndexElementConstructorTableValues, 0 };
#endif

class JSHTMLIsIndexElementConstructor : public DOMObject {
public:
    JSHTMLIsIndexElementConstructor(ExecState* exec)
        : DOMObject(JSHTMLIsIndexElementConstructor::createStructure(exec->lexicalGlobalObject()->objectPrototype()))
    {
        putDirect(exec->propertyNames().prototype, JSHTMLIsIndexElementPrototype::self(exec, exec->lexicalGlobalObject()), None);
    }
    virtual bool getOwnPropertySlot(ExecState*, const Identifier&, PropertySlot&);
    virtual const ClassInfo* classInfo() const { return &s_info; }
    static const ClassInfo s_info;

    static PassRefPtr<Structure> createStructure(JSValuePtr proto) 
    { 
        return Structure::create(proto, TypeInfo(ObjectType, ImplementsHasInstance)); 
    }
};

const ClassInfo JSHTMLIsIndexElementConstructor::s_info = { "HTMLIsIndexElementConstructor", 0, &JSHTMLIsIndexElementConstructorTable, 0 };

bool JSHTMLIsIndexElementConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSHTMLIsIndexElementConstructor, DOMObject>(exec, &JSHTMLIsIndexElementConstructorTable, this, propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSHTMLIsIndexElementPrototypeTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static const HashTable JSHTMLIsIndexElementPrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSHTMLIsIndexElementPrototypeTableValues, 0 };
#else
    { 1, 0, JSHTMLIsIndexElementPrototypeTableValues, 0 };
#endif

const ClassInfo JSHTMLIsIndexElementPrototype::s_info = { "HTMLIsIndexElementPrototype", 0, &JSHTMLIsIndexElementPrototypeTable, 0 };

JSObject* JSHTMLIsIndexElementPrototype::self(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSHTMLIsIndexElement>(exec, globalObject);
}

const ClassInfo JSHTMLIsIndexElement::s_info = { "HTMLIsIndexElement", &JSHTMLInputElement::s_info, &JSHTMLIsIndexElementTable, 0 };

JSHTMLIsIndexElement::JSHTMLIsIndexElement(PassRefPtr<Structure> structure, PassRefPtr<HTMLIsIndexElement> impl)
    : JSHTMLInputElement(structure, impl)
{
}

JSObject* JSHTMLIsIndexElement::createPrototype(ExecState* exec, JSGlobalObject* globalObject)
{
    return new (exec) JSHTMLIsIndexElementPrototype(JSHTMLIsIndexElementPrototype::createStructure(JSHTMLInputElementPrototype::self(exec, globalObject)));
}

bool JSHTMLIsIndexElement::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSHTMLIsIndexElement, Base>(exec, &JSHTMLIsIndexElementTable, this, propertyName, slot);
}

JSValuePtr jsHTMLIsIndexElementForm(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    HTMLIsIndexElement* imp = static_cast<HTMLIsIndexElement*>(static_cast<JSHTMLIsIndexElement*>(asObject(slot.slotBase()))->impl());
    return toJS(exec, WTF::getPtr(imp->form()));
}

JSValuePtr jsHTMLIsIndexElementPrompt(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    HTMLIsIndexElement* imp = static_cast<HTMLIsIndexElement*>(static_cast<JSHTMLIsIndexElement*>(asObject(slot.slotBase()))->impl());
    return jsString(exec, imp->prompt());
}

JSValuePtr jsHTMLIsIndexElementConstructor(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    return static_cast<JSHTMLIsIndexElement*>(asObject(slot.slotBase()))->getConstructor(exec);
}
void JSHTMLIsIndexElement::put(ExecState* exec, const Identifier& propertyName, JSValuePtr value, PutPropertySlot& slot)
{
    lookupPut<JSHTMLIsIndexElement, Base>(exec, propertyName, value, &JSHTMLIsIndexElementTable, this, slot);
}

void setJSHTMLIsIndexElementPrompt(ExecState* exec, JSObject* thisObject, JSValuePtr value)
{
    HTMLIsIndexElement* imp = static_cast<HTMLIsIndexElement*>(static_cast<JSHTMLIsIndexElement*>(thisObject)->impl());
    imp->setPrompt(valueToStringWithNullCheck(exec, value));
}

JSValuePtr JSHTMLIsIndexElement::getConstructor(ExecState* exec)
{
    return getDOMConstructor<JSHTMLIsIndexElementConstructor>(exec);
}


}
