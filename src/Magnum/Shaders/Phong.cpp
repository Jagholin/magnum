/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include "Phong.h"

#include <Corrade/Utility/Resource.h>

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/Shader.h"
#include "Magnum/Texture.h"

#include "Implementation/CreateCompatibilityShader.h"

namespace Magnum { namespace Shaders {

namespace {
    enum: Int {
        AmbientTextureLayer = 0,
        DiffuseTextureLayer = 1,
        SpecularTextureLayer = 2
    };
}

Phong::Phong(const Flags flags): transformationMatrixUniform(0), projectionMatrixUniform(1), normalMatrixUniform(2), lightUniform(3), diffuseColorUniform(4), ambientColorUniform(5), specularColorUniform(6), lightColorUniform(7), shininessUniform(8), _flags(flags) {
    Utility::Resource rs("MagnumShaders");

    #ifndef MAGNUM_TARGET_GLES
    const Version version = Context::current()->supportedVersion({Version::GL320, Version::GL310, Version::GL300, Version::GL210});
    #else
    const Version version = Context::current()->supportedVersion({Version::GLES300, Version::GLES200});
    #endif

    Shader vert = Implementation::createCompatibilityShader(version, Shader::Type::Vertex);
    Shader frag = Implementation::createCompatibilityShader(version, Shader::Type::Fragment);

    vert.addSource(flags ? "#define TEXTURED\n" : "")
        .addSource(rs.get("generic.glsl"))
        .addSource(rs.get("Phong.vert"));
    frag.addSource(flags & Flag::AmbientTexture ? "#define AMBIENT_TEXTURE\n" : "")
        .addSource(flags & Flag::DiffuseTexture ? "#define DIFFUSE_TEXTURE\n" : "")
        .addSource(flags & Flag::SpecularTexture ? "#define SPECULAR_TEXTURE\n" : "")
        .addSource(rs.get("Phong.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(Shader::compile({vert, frag}));

    attachShader(vert);
    attachShader(frag);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::explicit_attrib_location>(version))
    #else
    if(!Context::current()->isVersionSupported(Version::GLES300))
    #endif
    {
        bindAttributeLocation(Position::Location, "position");
        bindAttributeLocation(Normal::Location, "normal");
        if(flags) bindAttributeLocation(TextureCoordinates::Location, "textureCoordinates");
    }

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::explicit_uniform_location>(version))
    #endif
    {
        transformationMatrixUniform = uniformLocation("transformationMatrix");
        projectionMatrixUniform = uniformLocation("projectionMatrix");
        normalMatrixUniform = uniformLocation("normalMatrix");
        lightUniform = uniformLocation("light");
        if(!(flags & Flag::AmbientTexture)) ambientColorUniform = uniformLocation("ambientColor");
        if(!(flags & Flag::DiffuseTexture)) diffuseColorUniform = uniformLocation("diffuseColor");
        if(!(flags & Flag::SpecularTexture)) specularColorUniform = uniformLocation("specularColor");
        lightColorUniform = uniformLocation("lightColor");
        shininessUniform = uniformLocation("shininess");
    }

    #ifndef MAGNUM_TARGET_GLES
    if(flags && !Context::current()->isExtensionSupported<Extensions::GL::ARB::shading_language_420pack>(version))
    #endif
    {
        if(flags & Flag::AmbientTexture) setUniform(uniformLocation("ambientTexture"), AmbientTextureLayer);
        if(flags & Flag::DiffuseTexture) setUniform(uniformLocation("diffuseTexture"), DiffuseTextureLayer);
        if(flags & Flag::SpecularTexture) setUniform(uniformLocation("specularTexture"), SpecularTextureLayer);
    }

    /* Set defaults in OpenGL ES (for desktop they are set in shader code itself) */
    #ifdef MAGNUM_TARGET_GLES
    setAmbientColor({});
    setSpecularColor(Vector3(1.0f));
    setLightColor(Vector3(1.0f));
    setShininess(80.0f);
    #endif
}

Phong& Phong::setAmbientTexture(Texture2D& texture) {
    if(_flags & Flag::AmbientTexture) texture.bind(AmbientTextureLayer);
    return *this;
}

Phong& Phong::setDiffuseTexture(Texture2D& texture) {
    if(_flags & Flag::DiffuseTexture) texture.bind(DiffuseTextureLayer);
    return *this;
}

Phong& Phong::setSpecularTexture(Texture2D& texture) {
    if(_flags & Flag::SpecularTexture) texture.bind(SpecularTextureLayer);
    return *this;
}

Phong& Phong::setTextures(Texture2D* ambient, Texture2D* diffuse, Texture2D* specular) {
    AbstractTexture::bind(AmbientTextureLayer, {ambient, diffuse, specular});
    return *this;
}

}}
