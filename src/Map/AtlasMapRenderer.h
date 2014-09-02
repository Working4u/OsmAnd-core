#ifndef _OSMAND_CORE_ATLAS_MAP_RENDERER_H_
#define _OSMAND_CORE_ATLAS_MAP_RENDERER_H_

#include "stdlib_common.h"

#include "QtExtensions.h"

#include "OsmAndCore.h"
#include "CommonTypes.h"
#include "MapRenderer.h"
#include "IAtlasMapRenderer.h"
#include "MapRendererResourcesManager.h"
#include "QuadTree.h"

namespace OsmAnd
{
    class AtlasMapRendererSkyStage;
    class AtlasMapRendererRasterMapStage;
    class AtlasMapRendererSymbolsStage;
    class AtlasMapRendererDebugStage;

    class AtlasMapRenderer
        : public MapRenderer
        , public IAtlasMapRenderer
    {
        Q_DISABLE_COPY_AND_MOVE(AtlasMapRenderer);

    private:
        // General:
        QSet<TileId> _uniqueTiles;
    protected:
        AtlasMapRenderer(
            GPUAPI* const gpuAPI,
            const std::unique_ptr<const MapRendererConfiguration>& baseConfiguration,
            const std::unique_ptr<const MapRendererDebugSettings>& baseDebugSettings);

        // Configuration-related:
        enum class ConfigurationChange
        {
            ReferenceTileSize = static_cast<int>(MapRenderer::ConfigurationChange::__LAST),

            __LAST
        };
        enum {
            RegisteredConfigurationChangesCount = static_cast<unsigned int>(ConfigurationChange::__LAST)
        };
        virtual uint32_t getConfigurationChangeMask(
            const std::shared_ptr<const MapRendererConfiguration>& current,
            const std::shared_ptr<const MapRendererConfiguration>& updated) const;
        virtual void invalidateCurrentConfiguration(const uint32_t changesMask);

        // State-related:
        virtual bool updateInternalState(
            MapRendererInternalState& outInternalState,
            const MapRendererState& state,
            const MapRendererConfiguration& configuration) const;

        // Debug-related:

        // Customization points:
        virtual bool preInitializeRendering();
        virtual bool doInitializeRendering();

        virtual bool prePrepareFrame();
        virtual bool postPrepareFrame();

        virtual bool doReleaseRendering();

        // Stages:
        std::shared_ptr<AtlasMapRendererSkyStage> _skyStage;
        virtual AtlasMapRendererSkyStage* createSkyStage() = 0;
        std::shared_ptr<AtlasMapRendererRasterMapStage> _rasterMapStage;
        virtual AtlasMapRendererRasterMapStage* createRasterMapStage() = 0;
        std::shared_ptr<AtlasMapRendererSymbolsStage> _symbolsStage;
        virtual AtlasMapRendererSymbolsStage* createSymbolsStage() = 0;
        std::shared_ptr<AtlasMapRendererDebugStage> _debugStage;
        virtual AtlasMapRendererDebugStage* createDebugStage() = 0;
    public:
        virtual ~AtlasMapRenderer();

        // Stages:
        const std::shared_ptr<AtlasMapRendererSkyStage>& skyStage;
        const std::shared_ptr<AtlasMapRendererRasterMapStage>& rasterMapStage;
        const std::shared_ptr<AtlasMapRendererSymbolsStage>& symbolsStage;
        const std::shared_ptr<AtlasMapRendererDebugStage>& debugStage;

        virtual QList<TileId> getVisibleTiles() const;
        virtual unsigned int getVisibleTilesCount() const;

        // Symbols-related
        virtual QList< std::shared_ptr<const MapSymbol> > getSymbolsAt(const PointI& screenPoint) const;
    };
}

#endif // !defined(_OSMAND_CORE_ATLAS_MAP_RENDERER_H_)