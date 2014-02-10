#ifndef _OSMAND_CORE_I_QUERY_FILTER_H_
#define _OSMAND_CORE_I_QUERY_FILTER_H_

#include <OsmAndCore/stdlib_common.h>

#include <OsmAndCore/QtExtensions.h>

#include <OsmAndCore.h>
#include <OsmAndCore/CommonTypes.h>

namespace OsmAnd
{
    class OSMAND_CORE_API IQueryFilter
    {
        Q_DISABLE_COPY(IQueryFilter);
    private:
    protected:
        IQueryFilter();
    public:
        virtual ~IQueryFilter();
    
        virtual bool acceptsZoom(ZoomLevel zoom) = 0;
        virtual bool acceptsArea(const AreaI& area) = 0;
        virtual bool acceptsPoint(const PointI& point) = 0;
    };
} // namespace OsmAnd

#endif // !defined(_OSMAND_CORE_I_QUERY_FILTER_H_)
