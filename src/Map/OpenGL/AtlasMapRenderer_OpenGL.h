#ifndef _OSMAND_CORE_ATLAS_MAP_RENDERER_OPENGL_H_
#define _OSMAND_CORE_ATLAS_MAP_RENDERER_OPENGL_H_

#include "stdlib_common.h"

#include <glm/glm.hpp>

#include "QtExtensions.h"

#include "OsmAndCore.h"
#include "CommonTypes.h"
#include "AtlasMapRenderer.h"
#include "AtlasMapRendererInternalState_OpenGL.h"
#include "AtlasMapRendererSkyStage_OpenGL.h"
#include "AtlasMapRendererRasterMapStage_OpenGL.h"
#include "AtlasMapRendererSymbolsStage_OpenGL.h"
#include "AtlasMapRendererDebugStage_OpenGL.h"
#include "OpenGL/GPUAPI_OpenGL.h"

namespace OsmAnd
{
    class AtlasMapRenderer_OpenGL : public AtlasMapRenderer
    {
        Q_DISABLE_COPY_AND_MOVE(AtlasMapRenderer_OpenGL);
    public:
        // Short type aliases:
        typedef AtlasMapRendererInternalState_OpenGL InternalState;
    private:
    protected:
        const static float _zNear;

        void updateFrustum(InternalState* internalState, const MapRendererState& state) const;
        void computeVisibleTileset(InternalState* internalState, const MapRendererState& state) const;
        
        // State-related:
        InternalState _internalState;
        virtual const MapRendererInternalState* getInternalStateRef() const;
        virtual MapRendererInternalState* getInternalStateRef();
        virtual const MapRendererInternalState& getInternalState() const;
        virtual MapRendererInternalState& getInternalState();
        virtual bool updateInternalState(
            MapRendererInternalState& outInternalState,
            const MapRendererState& state,
            const MapRendererConfiguration& configuration) const;

        // Resources:
        virtual void onValidateResourcesOfType(const MapRendererResourceType type);

        // Customization points:
        virtual bool doInitializeRendering();
        virtual bool doRenderFrame();

        GPUAPI_OpenGL* getGPUAPI() const;

        // Stages:
        virtual AtlasMapRendererSkyStage* createSkyStage();
        virtual AtlasMapRendererRasterMapStage* createRasterMapStage();
        virtual AtlasMapRendererSymbolsStage* createSymbolsStage();
        virtual AtlasMapRendererDebugStage* createDebugStage();
    public:
        AtlasMapRenderer_OpenGL(GPUAPI_OpenGL* const gpuAPI);
        virtual ~AtlasMapRenderer_OpenGL();

        virtual float getCurrentTileSizeOnScreenInPixels() const;
        virtual bool getLocationFromScreenPoint(const PointI& screenPoint, PointI& location31) const;
        virtual bool getLocationFromScreenPoint(const PointI& screenPoint, PointI64& location) const;
    };
}

#endif // !defined(_OSMAND_CORE_ATLAS_MAP_RENDERER_OPENGL_H_)