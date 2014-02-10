#ifndef _OSMAND_CORE_OFFLINE_MAP_RASTER_TILE_PROVIDER_SOFTWARE_P_H_
#define _OSMAND_CORE_OFFLINE_MAP_RASTER_TILE_PROVIDER_SOFTWARE_P_H_

#include <OsmAndCore/stdlib_common.h>
#include <functional>
#include <array>

#include <OsmAndCore/QtExtensions.h>

#include <OsmAndCore.h>
#include <CommonTypes.h>
#include <Concurrent.h>
#include <TilesCollection.h>
#include <IMapBitmapTileProvider.h>
#include <IRetainableResource.h>

class SkBitmap;

namespace OsmAnd {

    class OfflineMapDataTile;

    class OfflineMapRasterTileProvider_Software;
    class OfflineMapRasterTileProvider_Software_P
    {
    private:
    protected:
        OfflineMapRasterTileProvider_Software_P(OfflineMapRasterTileProvider_Software* owner, const uint32_t outputTileSize, const float density);

        STRONG_ENUM(TileState)
        {
            // Tile is not in any determined state (tile entry did not exist)
            Unknown = 0,

            // Tile is requested
            Requested,

            // Tile is processing
            Processing,
        };
        class TileEntry : public TilesCollectionEntryWithState<TileEntry, TileState, TileState::Unknown>
        {
        private:
        protected:
        public:
            TileEntry(const TilesCollection<TileEntry>& collection, const TileId tileId, const ZoomLevel zoom)
                : TilesCollectionEntryWithState(collection, tileId, zoom)
            {}

            virtual ~TileEntry()
            {
                safeUnlink();
            }
        };

        class Tile : public MapBitmapTile, public IRetainableResource
        {
            Q_DISABLE_COPY(Tile);
        private:
            const std::shared_ptr<const OfflineMapDataTile> _dataTile;
        protected:
        public:
            Tile(SkBitmap* bitmap, const std::shared_ptr<const OfflineMapDataTile>& dataTile);
            virtual ~Tile();

            const std::shared_ptr<const OfflineMapDataTile>& dataTile;

            virtual void releaseNonRetainedData();
        };

        OfflineMapRasterTileProvider_Software* const owner;
        const uint32_t outputTileSize;
        const float density;

        const Concurrent::TaskHost::Bridge _taskHostBridge;
        TilesCollection<TileEntry> _tiles;

        bool obtainTile(const TileId tileId, const ZoomLevel zoom, std::shared_ptr<const MapTile>& outTile);
    public:
        virtual ~OfflineMapRasterTileProvider_Software_P();

    friend class OsmAnd::OfflineMapRasterTileProvider_Software;
    };

}

#endif // !defined(_OSMAND_CORE_OFFLINE_MAP_RASTER_TILE_PROVIDER_SOFTWARE_P_H_)
