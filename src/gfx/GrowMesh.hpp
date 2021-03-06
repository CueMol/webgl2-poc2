// -*-Mode: C++;-*-
//
//  Growing Mesh Object
//
#pragma once

#include <qlib/LStream.hpp>
#include <qlib/LString.hpp>

#include "ColorTable.hpp"
#include "DisplayContext.hpp"
#include "gfx.hpp"

namespace gfx {

using qlib::LString;
using qlib::Vector4D;

///////////////////////////////////////////////////

// TODO: impl as template
using MeshColorType = qlib::quint32;
// using MeshColorType = ColorTable::elem_t;

struct GFX_API MeshVert
{
    Vector4D v, n;

    MeshColorType c;

    MeshVert() {}

    MeshVert(const Vector4D &av, const Vector4D &an, const MeshColorType &ac)
        : v(av), n(an), c(ac)
    {
    }

    bool isFinite() const
    {
        if (qlib::isFinite(v.x()) && qlib::isFinite(v.y()) && qlib::isFinite(v.z()) &&
            qlib::isFinite(n.x()) && qlib::isFinite(n.y()) && qlib::isFinite(n.z()))
            return true;
        else
            return false;
    }
};

///////////////////////////////////////////////////

/// mode of the face (for silhouette line rendering)
//  0: nomal mode
//  1: ridge triangle without silhouette line (e.g. at cylinder termini)
//  2: generated by conv_sphere
//  3: generated by conv_cyl (with termini)
static constexpr int MFMOD_MESH = 0;
static constexpr int MFMOD_NORGLN = 1;
static constexpr int MFMOD_SPHERE = 2;
static constexpr int MFMOD_CYL = 3;
static constexpr int MFMOD_MESHXX = 4;

struct GFX_API MeshFace
{
    /// index of verteces
    int iv1, iv2, iv3;

    /// mode of the face (for silhouette line rendering)
    int nmode;

    /// ctor
    MeshFace(int ai1, int ai2, int ai3, int anmode)
        : iv1(ai1), iv2(ai2), iv3(ai3), nmode(anmode)
    {
    }
};

///////////////////////////////////////////////////

using MeshFaceSet = std::deque<MeshFace>;
using MeshVertSet = std::deque<MeshVert *>;

class GFX_API GrowMesh
{
private:
    bool m_bLighting;
    MeshFaceSet m_faces;
    MeshVertSet m_verts;

    //////////

public:
    ~GrowMesh()
    {
        clear();
    }

    //////////

    int getVertexSize() const
    {
        return m_verts.size();
    }
    int getFaceSize() const
    {
        return m_faces.size();
    }

    int addVertex(const Vector4D &v, const Vector4D &n, const MeshColorType &c)
    {
        int rval = m_verts.size();
        m_verts.push_back(MB_NEW MeshVert(v, n, c));
        return rval;
    }

    int addVertex(const Vector4D &v, const Vector4D &n, const MeshColorType &c,
                  const Matrix4D &xfm)
    {
        Vector4D xv = v;
        xv.w() = 1.0;
        xfm.xform4D(xv);

        Vector4D xn = n;
        xn.w() = 0.0;
        xfm.xform4D(xn);

        return addVertex(xv, xn, c);
    }

    int addVertex(MeshVert *p)
    {
        int rval = m_verts.size();
        m_verts.push_back(p);
        return rval;
    }

    int copyVertex(MeshVert *pOrig)
    {
        return addVertex(pOrig->v, pOrig->n, pOrig->c);
    }

    void addFace(int iv0, int iv1, int iv2, int nmode = 0)
    {
        m_faces.push_back(MeshFace(iv0, iv1, iv2, nmode));
    }

    void clear()
    {
        std::for_each(m_verts.begin(), m_verts.end(), qlib::delete_ptr<MeshVert *>());
        m_verts.clear();
        m_faces.clear();
    }

    ///////////////////
    // GL-stype construction

private:
    int m_nPivot;
    int m_nMode;

    void endTrigs()
    {
        int nVerts = getVertexSize() - m_nPivot;
        if (nVerts % 3 != 0) {
            LOG_DPRINTLN("RendIntData> Trig mesh: nVerts(%d)%%3!=0 !!", nVerts);
        }

        int nfmode = MFMOD_MESH;
        // const int nPolyMode = m_pdc->getPolygonMode();
        // if (nPolyMode == DisplayContext::POLY_FILL_NORGLN) nfmode = MFMOD_NORGLN;

        int nFaces = nVerts / 3;
        for (int i = 0; i < nFaces; i++) {
            addFace(m_nPivot + i * 3 + 0, m_nPivot + i * 3 + 1, m_nPivot + i * 3 + 2,
                    nfmode);
        }
    }

    void endTrigStrip()
    {
        int nVerts = getVertexSize() - m_nPivot;
        if (nVerts < 3) {
            // ERROR!!
            LOG_DPRINTLN("TrigStrip mesh: nVerts<3 !!");
        }

        int nfmode = MFMOD_MESH;
        // const int nPolyMode = m_pdc->getPolygonMode();
        // if (nPolyMode == DisplayContext::POLY_FILL_NORGLN ||
        //     nPolyMode == DisplayContext::POLY_FILL_XX)
        //     nfmode = MFMOD_NORGLN;

        for (int i = 2; i < nVerts; i++) {
            int imode = nfmode;

            // if (m_pVAttrAry != NULL) {
            //     MB_ASSERT(i < m_pVAttrAry->size());
            //     int nattr = m_pVAttrAry->at(i);
            //     if (nattr == DisplayContext::DVA_NOEDGE) imode = MFMOD_MESHXX;
            // }

            if (i % 2 == 0) {
                addFace(m_nPivot + i - 2, m_nPivot + i - 1, m_nPivot + i, imode);
            } else {
                addFace(m_nPivot + i, m_nPivot + i - 1, m_nPivot + i - 2, imode);
            }
        }

        // if (m_pVAttrAry != NULL) {
        //     delete m_pVAttrAry;
        //     m_pVAttrAry = NULL;
        // }
    }

    void endTrigFan()
    {
        int nVerts = getVertexSize() - m_nPivot;
        if (nVerts < 3) {
            LOG_DPRINTLN("RendIntData> TrigFan mesh: nVerts<3 !!");
        }

        int nfmode = MFMOD_MESH;
        // const int nPolyMode = m_pdc->getPolygonMode();
        // if (nPolyMode == DisplayContext::POLY_FILL_NORGLN) nfmode = MFMOD_NORGLN;

        for (int i = 2; i < nVerts; i++) {
            addFace(m_nPivot + i - 1, m_nPivot + i, m_nPivot, nfmode);
        }
    }

public:
    static constexpr int GM_TRIGS = 0;
    static constexpr int GM_TRIGSTRIP = 1;
    static constexpr int GM_TRIGFAN = 2;

    void start(int nmode)
    {
        m_nMode = nmode;
        m_nPivot = getVertexSize();
    }
    void end()
    {
        switch (m_nMode) {
            case GM_TRIGS:
                endTrigs();
                break;
            case GM_TRIGSTRIP:
                endTrigStrip();
                break;
            case GM_TRIGFAN:
                endTrigFan();
                break;
            default:
                // ERROR
                LOG_DPRINTLN("invalid mode: %d", m_nMode);
                break;
        }
    }

    ///////////////////
    // access methods

    MeshVert *getVertex(int index) const
    {
        return m_verts[index];
    }

    const MeshFace &getFace(int find) const
    {
        return m_faces[find];
    }

    const auto &getVertexData() const {
        return m_verts;
    }
    const auto &getFaceData() const {
        return m_faces;
    }

};

}  // namespace gfx
