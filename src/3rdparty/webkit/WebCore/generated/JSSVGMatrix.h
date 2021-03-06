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

#ifndef JSSVGMatrix_h
#define JSSVGMatrix_h


#if ENABLE(SVG)

#include "JSDOMBinding.h"
#include <runtime/JSGlobalObject.h>
#include <runtime/ObjectPrototype.h>
#include "TransformationMatrix.h"
#include "JSSVGPODTypeWrapper.h"

namespace WebCore {

class JSSVGMatrix : public DOMObject {
    typedef DOMObject Base;
public:
    JSSVGMatrix(PassRefPtr<JSC::Structure>, PassRefPtr<JSSVGPODTypeWrapper<TransformationMatrix> >, SVGElement* context);
    virtual ~JSSVGMatrix();
    static JSC::JSObject* createPrototype(JSC::ExecState*, JSC::JSGlobalObject*);
    virtual bool getOwnPropertySlot(JSC::ExecState*, const JSC::Identifier& propertyName, JSC::PropertySlot&);
    virtual void put(JSC::ExecState*, const JSC::Identifier& propertyName, JSC::JSValuePtr, JSC::PutPropertySlot&);
    virtual const JSC::ClassInfo* classInfo() const { return &s_info; }
    static const JSC::ClassInfo s_info;

    static PassRefPtr<JSC::Structure> createStructure(JSC::JSValuePtr prototype)
    {
        return JSC::Structure::create(prototype, JSC::TypeInfo(JSC::ObjectType));
    }


    // Custom functions
    JSC::JSValuePtr multiply(JSC::ExecState*, const JSC::ArgList&);
    JSC::JSValuePtr inverse(JSC::ExecState*, const JSC::ArgList&);
    JSC::JSValuePtr translate(JSC::ExecState*, const JSC::ArgList&);
    JSC::JSValuePtr scale(JSC::ExecState*, const JSC::ArgList&);
    JSC::JSValuePtr scaleNonUniform(JSC::ExecState*, const JSC::ArgList&);
    JSC::JSValuePtr rotate(JSC::ExecState*, const JSC::ArgList&);
    JSC::JSValuePtr rotateFromVector(JSC::ExecState*, const JSC::ArgList&);
    JSC::JSValuePtr flipX(JSC::ExecState*, const JSC::ArgList&);
    JSC::JSValuePtr flipY(JSC::ExecState*, const JSC::ArgList&);
    JSC::JSValuePtr skewX(JSC::ExecState*, const JSC::ArgList&);
    JSC::JSValuePtr skewY(JSC::ExecState*, const JSC::ArgList&);
    JSSVGPODTypeWrapper<TransformationMatrix>* impl() const { return m_impl.get(); }
    SVGElement* context() const { return m_context.get(); }

private:
    RefPtr<SVGElement> m_context;
    RefPtr<JSSVGPODTypeWrapper<TransformationMatrix> > m_impl;
};

JSC::JSValuePtr toJS(JSC::ExecState*, JSSVGPODTypeWrapper<TransformationMatrix>*, SVGElement* context);
TransformationMatrix toSVGMatrix(JSC::JSValuePtr);

class JSSVGMatrixPrototype : public JSC::JSObject {
public:
    static JSC::JSObject* self(JSC::ExecState*, JSC::JSGlobalObject*);
    virtual const JSC::ClassInfo* classInfo() const { return &s_info; }
    static const JSC::ClassInfo s_info;
    virtual bool getOwnPropertySlot(JSC::ExecState*, const JSC::Identifier&, JSC::PropertySlot&);
    static PassRefPtr<JSC::Structure> createStructure(JSC::JSValuePtr prototype)
    {
        return JSC::Structure::create(prototype, JSC::TypeInfo(JSC::ObjectType));
    }
    JSSVGMatrixPrototype(PassRefPtr<JSC::Structure> structure) : JSC::JSObject(structure) { }
};

// Functions

JSC::JSValuePtr jsSVGMatrixPrototypeFunctionMultiply(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr, const JSC::ArgList&);
JSC::JSValuePtr jsSVGMatrixPrototypeFunctionInverse(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr, const JSC::ArgList&);
JSC::JSValuePtr jsSVGMatrixPrototypeFunctionTranslate(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr, const JSC::ArgList&);
JSC::JSValuePtr jsSVGMatrixPrototypeFunctionScale(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr, const JSC::ArgList&);
JSC::JSValuePtr jsSVGMatrixPrototypeFunctionScaleNonUniform(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr, const JSC::ArgList&);
JSC::JSValuePtr jsSVGMatrixPrototypeFunctionRotate(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr, const JSC::ArgList&);
JSC::JSValuePtr jsSVGMatrixPrototypeFunctionRotateFromVector(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr, const JSC::ArgList&);
JSC::JSValuePtr jsSVGMatrixPrototypeFunctionFlipX(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr, const JSC::ArgList&);
JSC::JSValuePtr jsSVGMatrixPrototypeFunctionFlipY(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr, const JSC::ArgList&);
JSC::JSValuePtr jsSVGMatrixPrototypeFunctionSkewX(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr, const JSC::ArgList&);
JSC::JSValuePtr jsSVGMatrixPrototypeFunctionSkewY(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr, const JSC::ArgList&);
// Attributes

JSC::JSValuePtr jsSVGMatrixA(JSC::ExecState*, const JSC::Identifier&, const JSC::PropertySlot&);
void setJSSVGMatrixA(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr);
JSC::JSValuePtr jsSVGMatrixB(JSC::ExecState*, const JSC::Identifier&, const JSC::PropertySlot&);
void setJSSVGMatrixB(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr);
JSC::JSValuePtr jsSVGMatrixC(JSC::ExecState*, const JSC::Identifier&, const JSC::PropertySlot&);
void setJSSVGMatrixC(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr);
JSC::JSValuePtr jsSVGMatrixD(JSC::ExecState*, const JSC::Identifier&, const JSC::PropertySlot&);
void setJSSVGMatrixD(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr);
JSC::JSValuePtr jsSVGMatrixE(JSC::ExecState*, const JSC::Identifier&, const JSC::PropertySlot&);
void setJSSVGMatrixE(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr);
JSC::JSValuePtr jsSVGMatrixF(JSC::ExecState*, const JSC::Identifier&, const JSC::PropertySlot&);
void setJSSVGMatrixF(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr);

} // namespace WebCore

#endif // ENABLE(SVG)

#endif
