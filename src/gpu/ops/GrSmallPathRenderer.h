/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrSmallPathRenderer_DEFINED
#define GrSmallPathRenderer_DEFINED

#include "src/gpu/GrDrawOpAtlas.h"
#include "src/gpu/GrOnFlushResourceProvider.h"
#include "src/gpu/GrPathRenderer.h"
#include "src/gpu/geometry/GrRect.h"
#include "src/gpu/geometry/GrStyledShape.h"

#include "src/core/SkTDynamicHash.h"

class GrRecordingContext;
class GrSmallPathShapeData;
class GrSmallPathShapeDataKey;

class GrSmallPathRenderer : public GrPathRenderer,
                            public GrOnFlushCallbackObject,
                            public GrDrawOpAtlas::EvictionCallback,
                            public GrDrawOpAtlas::GenerationCounter {
public:
    GrSmallPathRenderer();
    ~GrSmallPathRenderer() override;

    const char* name() const final { return "Small"; }

    // GrOnFlushCallbackObject overrides
    //
    // Note: because this class is associated with a path renderer we want it to be removed from
    // the list of active OnFlushBackkbackObjects in an freeGpuResources call (i.e., we accept the
    // default retainOnFreeGpuResources implementation).

    void preFlush(GrOnFlushResourceProvider* onFlushRP, const uint32_t*, int) override {
        if (fAtlas) {
            fAtlas->instantiate(onFlushRP);
        }
    }

    void postFlush(GrDeferredUploadToken startTokenForNextFlush,
                   const uint32_t* /*opsTaskIDs*/, int /*numOpsTaskIDs*/) override {
        if (fAtlas) {
            fAtlas->compact(startTokenForNextFlush);
        }
    }

    using ShapeCache = SkTDynamicHash<GrSmallPathShapeData, GrSmallPathShapeDataKey>;
    typedef SkTInternalLList<GrSmallPathShapeData> ShapeDataList;

    static std::unique_ptr<GrDrawOp> createOp_TestingOnly(GrRecordingContext*,
                                                          GrPaint&&,
                                                          const GrStyledShape&,
                                                          const SkMatrix& viewMatrix,
                                                          GrDrawOpAtlas* atlas,
                                                          ShapeCache*,
                                                          ShapeDataList*,
                                                          bool gammaCorrect,
                                                          const GrUserStencilSettings*);
    struct PathTestStruct;

private:
    class SmallPathOp;

    StencilSupport onGetStencilSupport(const GrStyledShape&) const override {
        return GrPathRenderer::kNoSupport_StencilSupport;
    }

    CanDrawPath onCanDrawPath(const CanDrawPathArgs&) const override;

    bool onDrawPath(const DrawPathArgs&) override;

    void evict(GrDrawOpAtlas::PlotLocator) override;

    std::unique_ptr<GrDrawOpAtlas> fAtlas;
    ShapeCache fShapeCache;
    ShapeDataList fShapeList;

    typedef GrPathRenderer INHERITED;
};

#endif
